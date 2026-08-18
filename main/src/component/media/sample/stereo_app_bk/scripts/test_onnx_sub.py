import os
import cv2
import numpy as np
import onnxruntime as ort

def _build_color_map():
    cmap = [(0, 0, 0)] * 2000
    for i in range(1, 2000):
        if i < 255: k = i + 1; b, g, r = 255, k, 0
        elif i < 400: k = int(255 - (i - 255) * 1.76 + 0.5); b, g, r = k, 255, 0
        elif i < 500: k = int((i - 399) * 2.55 + 0.5); b, g, r = 0, 255, k
        elif i < 1000: k = int(255 - (i - 500) * 0.51 + 0.5); b, g, r = 0, k, 255
        else: k = int((i - 996) * 0.254 + 0.5); b, g, r = k, 0, 255
        cmap[i] = (r, g, b)
    return cmap

CMAP = _build_color_map()

def center_crop(img, target_h, target_w):
    """Crop the center of the image to target size."""
    h, w = img.shape[:2]
    top = (h - target_h) // 2
    # left = 96
    left = (w - target_w) // 2
    return img[top:top + target_h, left:left + target_w]


def load_and_preprocess_image(img_path, target_h=480, target_w=640):
    """Load image, center crop to target size."""
    img = cv2.imread(img_path)
    if img is None:
        raise FileNotFoundError(f"Image not found: {img_path}")
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = center_crop(img, target_h, target_w)
    img = img.astype(np.float32)
    img = np.transpose(img, (2, 0, 1))  # HWC -> CHW
    img = np.expand_dims(img, axis=0)  # add batch dim
    return img


def apply_colormap(disparity, min_disp=None, max_disp=None):
    """Apply colormap to disparity map for visualization."""
    if min_disp is None:
        min_disp = disparity.min()
    if max_disp is None:
        max_disp = disparity.max()

    if max_disp - min_disp < 1e-6:
        max_disp = min_disp + 1e-6

    # Normalize to [0, 1]
    disparity_norm = (disparity - min_disp) / (max_disp - min_disp)
    disparity_norm = np.clip(disparity_norm, 0, 1)
    disparity_norm = (disparity_norm * 255).astype(np.uint8)

    # Apply colormap (JET: blue->red->yellow)
    colored = cv2.applyColorMap(disparity_norm, cv2.COLORMAP_JET)
    colored = cv2.cvtColor(colored, cv2.COLOR_BGR2RGB)
    return colored


def main():
    image_path = '/hdata/caipuguang/dataset/stereo/indoor_oye_0610/pic/'
    model_path = os.path.join("/hdata/caipuguang/code/StereoNet/pths/pytorch_ori_r1.3.0.7/",
                              "stereo_s_ori_h480_w640_96_sub_e125_sim.onnx")
    left_dir = os.path.join(image_path, "left_images")
    right_dir = os.path.join(image_path, "right_images")
    output_dir = fr"/hdata/caipuguang/code/StereoNet/test/indoor_0610_sub_0701_e125/"

    disp_path = '/hdata/caipuguang/dataset/stereo/indoor_oye_0610/pic/'
    disp_dir = os.path.join(disp_path, "disparity_images")

    left_files = sorted([f for f in os.listdir(left_dir) if f.lower().endswith(('.png', '.jpg', '.jpeg'))])

    session = ort.InferenceSession(model_path, providers=["CPUExecutionProvider"])
    total1, total2 = 0. , 0.
    sum1, sum2 = 0, 0
    for left_file in left_files:
        left_img_path = os.path.join(left_dir, left_file)
        right_file = left_file.replace("left", "right").replace('LRemap', 'RRemap').replace('LCRemap', 'RCRemap')
        right_img_path = os.path.join(right_dir, right_file)

        # Load and preprocess images
        left_input = load_and_preprocess_image(left_img_path)
        right_input = load_and_preprocess_image(right_img_path)

        base_name = os.path.splitext(left_file)[0]
        disp_file = base_name.replace('left_', 'disparity_5_').replace('LRemap', 'disparity_5').replace('LCRemap',
                                                                                                      'disparity_5')
        disp_file = disp_file + ".png"
        disp_path = os.path.join(disp_dir, disp_file)
        gt_disp = None
        gt_disp_small = None
        if os.path.exists(disp_path):
            gt = cv2.imread(disp_path, cv2.IMREAD_UNCHANGED)
            if gt is not None:
                gt_disp = np.ascontiguousarray(gt, dtype=np.float32)
                gt_disp = gt_disp / 32.0
                gt_disp = np.clip(gt_disp, a_min=0, a_max=95)
                gt_disp = center_crop(gt_disp, 480, 640)
                gt_disp_small = cv2.resize(gt_disp, (320, 240), interpolation=cv2.INTER_NEAREST).astype(np.float32)
                gt_disp_small = gt_disp_small / 2.0
                print(f"[{base_name}] GT loaded: {gt_disp.shape}, range: [{gt_disp.min():.2f}, {gt_disp.max():.2f}]")

        # Run inference
        outputs = session.run(None, {"input.1": left_input, "input.19": right_input})
        disparity = outputs[0]
        disparity = disparity[0, 0].astype(np.float32)
        disparity_resized = cv2.resize(disparity, (640, 480), interpolation=cv2.INTER_NEAREST)
        disparity_resized = disparity_resized * 2.0
        # Calculate EPE if ground truth is available
        if gt_disp is not None:
            valid_mask = (0 < gt_disp) & (gt_disp < 96)
            if valid_mask.sum() > 0:
                epe = np.abs(disparity_resized[valid_mask] - gt_disp[valid_mask]).mean()
                print(f"[{base_name}] EPE: {epe:.4f}")
                total1 += epe
                sum1 += 1
        if gt_disp_small is not None:
            valid_mask = (0 < gt_disp_small) & (gt_disp_small < 48)
            if valid_mask.sum() > 0:
                epe = np.abs(disparity[valid_mask] - gt_disp_small[valid_mask]).mean()
                print(f"[{base_name}] EPE_1_2: {epe:.4f}")
                total2 += epe
                sum2 += 1
        # Save raw disparity
        # raw_path = os.path.join(output_dir, f"{base_name}_disparity_raw.npy")
        # np.save(raw_path, disparity)
        # print(f"[{base_name}] Disparity shape: {disparity.shape}, range: [{disparity.min():.2f}, {disparity.max():.2f}]")

        # Save disparity as image
        disparity_vis = disparity_resized * 32
        disparity_vis = disparity_vis.astype(np.uint16)
        # raw_img_path = os.path.join(output_dir, f"{base_name}_disparity_raw.png")
        # cv2.imwrite(raw_img_path, disparity_vis)

        # Apply colormap and save
        
        colored_resized = apply_colormap(disparity_resized, min_disp=0, max_disp=95)
        colored_resized_1_2 = apply_colormap(disparity, min_disp=0, max_disp=48)
        # colored_resized = cv2.resize(colored_resized, (544, 480), interpolation=cv2.INTER_LINEAR)

        # Load original left image for concatenation
        left_img = cv2.imread(left_img_path)
        left_img = cv2.cvtColor(left_img, cv2.COLOR_BGR2RGB)
        left_img = center_crop(left_img, 480, 640)

        # Concatenate left image and disparity vertically (left on top, disparity below)
        combined = np.vstack([left_img, colored_resized])
        colored_gt_1_2 = apply_colormap(gt_disp_small, min_disp=0, max_disp=48)
        combined2 = np.vstack([colored_resized_1_2, colored_gt_1_2])
        if gt_disp is not None:
            colored = apply_colormap(gt_disp, min_disp=0, max_disp=95)
            combined = np.vstack([combined, colored])
        combined_path = os.path.join(output_dir, f"{base_name}_combined.png")
        combined_path2 = os.path.join(output_dir, f"{base_name}_combined_1_2.png")
        cv2.imwrite(combined_path, cv2.cvtColor(combined, cv2.COLOR_RGB2BGR))
        # cv2.imwrite(combined_path2, cv2.cvtColor(combined2, cv2.COLOR_RGB2BGR))
    
    print(f"EPE_total: {total1 / sum1:.4f}")
    print(f"EPE_total_1_2: {total2 / sum2:.4f}")
    print("Done.")


if __name__ == "__main__":
    main()
