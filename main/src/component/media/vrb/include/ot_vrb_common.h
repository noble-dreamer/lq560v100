#include "ot_eis_type.h"

#ifndef OT_VRB_COMMON_H
#define OT_VRB_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define OT_VRB_SUPP_JPEG_MASK        0x1
#define OT_VRB_SUPP_MOTION_DATA_MASK 0x2
#define OT_VRB_SUPP_DNG_MASK         0x4
#define OT_VRB_SUPP_BNR_MOT_MASK     0x8

typedef struct {
    ot_u32 supplement;
} ot_vrb_supplement_attr;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_VRB_COMMON_H */