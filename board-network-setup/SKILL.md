---
name: board-network-setup
description: Diagnose and restore network connectivity between WSL2, Windows host, and embedded board (via USB RNDIS). Covers NFS mount failures, portproxy refresh, IP forwarding enablement, return route configuration, OpenSSH deployment/autostart, and serial console diagnostics. Use when the board cannot mount NFS, WSL cannot SSH/ping the board, SSH is not running after reboot, after any reboot, or when RNDIS is down and only serial is available.
---

# Board Network Setup & Troubleshooting

## File Layout

```
.qoder/skills/board-network-setup/
├── SKILL.md              # This file
├── config.env            # ★ All variable parameters (edit per environment)
├── openssh/              # Board OpenSSH bundle, deploy to /data/openssh
│   ├── bin/              # sshd, ssh-keygen, sftp-server, sshd-session, scp
│   ├── etc/ssh/          # sshd_config and host keys
│   ├── lib/              # runtime libraries
│   └── start_sshd.sh     # idempotent board-side SSH startup
├── board-scripts/        # ★ Board-side boot scripts (bundled, no backup needed)
│   ├── uvc-mode/         # UVC camera mode — USB as camera, no network
│   │   └── user_init.sh  # Boot: load modules → run.sh (UVC camera)
│   └── rndis-mode/       # RNDIS network mode — USB as ethernet+ACM
│       ├── user_init.sh  # Boot: load modules → usb-ether.sh → sshd → run.sh
│       └── usb-ether.sh  # RNDIS+ACM USB gadget bring-up (deploy to /opt/bin/)
└── scripts/
    ├── detect_env.ps1    # ★ Auto-detect per-machine vars (run FIRST on new PC)
    ├── nfs_up.ps1        # PC one-button: WSL NFS + portproxy + IP fwd
    ├── wsl_setup_nfs.sh  # WSL NFS server install & configure
    ├── nfs-mount.sh      # Board-side mount/umount tool
    ├── usb-ether.sh      # Board-side RNDIS + ACM gadget bring-up
    ├── user_init.sh.net  # Board boot script (RNDIS + return route + SSH)
    ├── start_sshd.sh     # Board SSH startup (account fix + sshd)
    ├── serial_run.ps1    # Send commands to board via serial, capture output
    ├── serial_check.ps1  # Quick serial diagnostic (thin wrapper, auto-detect port)
    └── serial_push.ps1   # Push files to board via serial (base64 chunks)
```

## Dynamic Configuration

All environment-specific values live in **`config.env`**. When switching PC or board, only edit this file:

| Parameter | Description | Example | Auto-detect? |
|-----------|-------------|---------|--------------|
| `WSL_DISTRO` | WSL distribution name | `Ubuntu-20.04` | No — manual |
| `WSL_USER` | Linux username in WSL | `xiao` | No — manual |
| `NFS_EXPORT_DIR` | NFS share path (WSL ext4) | `/home/xiao/...` | No — manual |
| `PC_RNDIS_IP` | PC's IP on RNDIS adapter | `192.168.1.3` | No — manual, usually stable |
| `BOARD_IP` | Board's USB network IP | `192.168.1.101` | No — manual, usually stable |
| `WSL_SUBNET` | WSL NAT subnet (for return route) | `172.23.224.0/20` | **⭐ YES — per-machine!** |
| `PORT_MOUNTD` | Fixed mountd port | `20048` | No — manual |
| `SERIAL_PORT` | Board serial COM port | `COM7` / `COM8` | **⭐ YES — per-machine!** |

**Auto-detected at runtime** (no need to configure):
- WSL eth0 IP → scripts auto-detect via `hostname -I`
- RNDIS adapter ifIndex → scripts find by `InterfaceDescription -like 'Remote NDIS*'`
- WSL vSwitch ifIndex → scripts find by `Name -like 'vEthernet (WSL*'`

**Per-machine variables** (starred above) are the #1 cause of silent failures when switching PCs.
Run `detect_env.ps1` on every new machine before troubleshooting.

---

## Network Topology

```
Board {BOARD_IP} ──USB RNDIS──► Windows {PC_RNDIS_IP}
                                    │
                                    ├─ portproxy 0.0.0.0:{111,2049,20048} → WSL eth0 (auto-detected)
                                    ├─ IP Forwarding (RNDIS ↔ WSL vSwitch)
                                    ▼
                               WSL2 eth0 {auto} (NAT, IP changes on restart)
                                    │
                               nfs-kernel-server
                                    │
                               {NFS_EXPORT_DIR}
```

---

## Pre-flight: Auto-detect Environment (RUN FIRST on new PC)

**On any new machine or after WSL version update**, run this BEFORE troubleshooting:

```powershell
# From skill scripts directory:
powershell -ExecutionPolicy Bypass -File .claude\skills\board-network-setup\scripts\detect_env.ps1
```

This auto-detects:
1. **WSL subnet** from `ip route show dev eth0` — compares against `config.env`
2. **Available serial ports** — finds which ones respond as boards
3. **RNDIS & WSL adapter** status and forwarding state
4. **Connectivity**: Windows→Board ping, SSH port, WSL→Board ping
5. **Board return route** (via serial) — checks route matches current WSL subnet

**If it finds mismatches**, it prints the exact fix commands. Common mismatches:

| Detection | config.env says | Actual | Impact |
|-----------|----------------|--------|--------|
| WSL_SUBNET | `172.23.224.0/20` | `172.29.192.0/20` | Board can't reply to WSL → NFS mount hangs, WSL ping fails |
| SERIAL_PORT | `COM8` | `COM7` | serial_run.ps1 fails with "port does not exist" |

After running detect_env.ps1:
1. Update `config.env` with the detected values
2. Update board `/opt/user_init.sh` with the correct `WSL_SUBNET`
3. Run `nfs_up.ps1` to apply portproxy + forwarding

### Auto-detect without scripts (quick manual check)

```powershell
# Find WSL subnet:
wsl -d Ubuntu-20.04 -- ip route show dev eth0
# Look for: 172.XX.YY.0/20 dev eth0 proto kernel

# Find available serial ports:
[System.IO.Ports.SerialPort]::GetPortNames()

# Check if board return route exists (via serial or SSH):
# On board: ip route | grep via
# Expected: 172.XX.YY.0/20 via 192.168.1.3 dev usb0
```

---

## Standard Operation

### PC Side (one command)

```powershell
# From skill scripts directory:
powershell -ExecutionPolicy Bypass -File .\.qoder\skills\board-network-setup\scripts\nfs_up.ps1
```

This auto-loads `config.env` and does:
1. Starts WSL rpcbind + nfs-kernel-server
2. Refreshes portproxy (auto-detects WSL IP)
3. Enables IP forwarding on both adapters (by description, not hardcoded index)
4. Runs connectivity sanity checks

### Board Side

```sh
/opt/bin/nfs-mount.sh          # mount NFS
/opt/bin/nfs-mount.sh u        # unmount
/opt/bin/nfs-mount.sh status   # check
```

Return route is auto-added on boot via `/opt/user_init.sh` (from `user_init.sh.net`). OpenSSH is started from `/data/openssh/start_sshd.sh` when present.

### Board OpenSSH Deployment

Use this when SSH is not installed on the board or does not survive reboot. This skill already carries the OpenSSH bundle under `openssh/`; deploy the whole directory to `/data/openssh` on the board:

```sh
# Skill-side source bundle:
./skills/board-network-setup/openssh/

# Expected board-side layout after deployment:
/data/openssh/bin/sshd
/data/openssh/bin/ssh-keygen
/data/openssh/bin/sftp-server
/data/openssh/bin/sshd-session
/data/openssh/etc/ssh/sshd_config
/data/openssh/start_sshd.sh
```

Boot integration is handled by `scripts/user_init.sh.net`:

```sh
# Return route to WSL subnet
( sleep 8; ip route add $WSL_SUBNET via $PC_RNDIS_IP 2>/dev/null ) &

# Start OpenSSH
if [ -x /data/openssh/start_sshd.sh ]; then
    /data/openssh/start_sshd.sh &
fi
```

`start_sshd.sh` is idempotent. It creates `/data/etc/{passwd,shadow,group}`, bind-mounts them to `/etc`, prepares `/var`, ensures `/data/openssh/libexec/sshd-session -> /data/openssh/bin/sshd-session`, generates host keys if missing, and starts `sshd` with `/data/openssh/etc/ssh/sshd_config`.

### Greenfield SSH Deployment (No SSH, No Network, UVC Mode)

This is the **cold-start** scenario: a fresh device that has never had SSH installed, is outputting UVC (camera) over USB, and has no network interface. Only serial console is available. Follow this sequence:

**Phase 1 — Diagnose via serial (COM port)**

```powershell
# Quick serial check — find the right COM port first
[System.IO.Ports.SerialPort]::GetPortNames()
# Then test: uname -a, ifconfig -a, ps, df -h / /data /tmp
```

Key things to check:
- USB gadget mode: `ls /sys/kernel/config/usb_gadget/` — if only `camera/` exists, the board is in UVC mode
- Disk space: `df -h / /data /tmp` — rootfs is often full (100%), use `/data` (persistent) or `/tmp` (tmpfs) for temp files
- Existing boot script: `cat /opt/user_init.sh` — back it up before replacing!

**Phase 2 — Switch USB gadget from camera/UVC to RNDIS+ACM**

The board's UDC can only serve one gadget at a time. Release the camera gadget and create an ether gadget:

```sh
# 1. Release camera UDC
echo "" > /sys/kernel/config/usb_gadget/camera/UDC
sleep 1

# 2. Create ether gadget (RNDIS + ACM)
mkdir -p /sys/kernel/config/usb_gadget/ether
cd /sys/kernel/config/usb_gadget/ether
echo "0x3ef5" > idVendor
echo "0x15b0" > idProduct
# ... (full sequence in usb-ether.sh)

# 3. Bind UDC
UDC_NAME=$(ls /sys/class/udc/ | head -1)
echo "$UDC_NAME" > UDC
sleep 1

# 4. Set IP
ifconfig usb0 192.168.1.101
```

After this, Windows should detect the RNDIS adapter. Set the PC-side RNDIS IP to `192.168.1.3/24` in Windows network settings if not auto-configured.

Verify: `ping 192.168.1.101` from Windows.

**Phase 3 — Deploy OpenSSH via FTP (fast, ~7MB in seconds)**

Once RNDIS is up, use the board's built-in `tcpsvd` + `ftpd` to receive files at network speed:

```sh
# On board (via serial):
tcpsvd -E 0.0.0.0 21 ftpd -w / &
```

Then from Windows, use the built-in FTP client:

```powershell
# Generate FTP script and run:
@"
binary
put "C:\path\to\file" "/data/openssh/bin/sshd"
put ...
quit
"@ | ftp -n 192.168.1.101
```

The board's busybox provides `tcpsvd` and `ftpd` — no extra tools needed. FTP over RNDIS transfers ~7MB in under 1 second vs. ~10 minutes over serial at 115200 baud.

**Phase 4 — Fix library SONAME symlinks (CRITICAL)**

The bundled OpenSSH binaries link against specific SONAMEs. Without symlinks, `sshd-session` fails silently on first connection attempt with `kex_exchange_identification: Connection closed by remote host`:

```sh
cd /data/openssh/lib
ln -sf libz.so.1.2.12    libz.so.1      # sshd-session needs libz.so.1
ln -sf libcrypto.so.3    libcrypto.so
ln -sf libcrypt.so.1     libcrypt.so
```

Verify: `LD_LIBRARY_PATH=/data/openssh/lib /data/openssh/bin/sshd-session --help` should print the OpenSSH version banner.

**Phase 5 — Start SSH and persist boot config**

```sh
# Fix CRLF, chmod, start
sed -i 's/\r$//' /data/openssh/start_sshd.sh /data/openssh/etc/ssh/sshd_config
chmod +x /data/openssh/bin/* /data/openssh/start_sshd.sh
/data/openssh/start_sshd.sh
ps | grep sshd   # should show listener
netstat -tlnp | grep 22
```

Then deploy persistence scripts so networking + SSH survive reboot:
- `/opt/bin/usb-ether.sh` — RNDIS+ACM gadget bring-up
- `/opt/user_init.sh` — boot script that loads modules, sets device mode, calls usb-ether.sh, starts sshd
- Back up the original `/opt/user_init.sh` first (e.g., `camera_bak`)!

**Key differences from the normal (NFS-based) flow:**
- No NFS mount needed — all files transferred via FTP or serial
- No WSL return route needed — Windows SSH connects directly over RNDIS (no WSL in the path)
- The board's original `user_init.sh` runs camera/stereo app — must back it up before replacing
- Root filesystem is often full — temp files go to `/tmp` (tmpfs), persistent files to `/data`

### UVC-Mode Recovery: Switch to RNDIS by Deploying Boot Scripts

**Simpler than the Greenfield manual UDC switch above.** The skill already bundles both UVC and RNDIS boot scripts under `board-scripts/`. To switch a UVC-mode board to RNDIS, push the two RNDIS-mode scripts via serial and reboot — the board comes up with networking automatically.

**Script inventory (bundled in skill, no backup tarball needed):**

| Mode | Script | Board path | Size | Purpose |
|------|--------|-----------|------|---------|
| UVC | `board-scripts/uvc-mode/user_init.sh` | `/opt/user_init.sh` | 26 lines | Boot camera/UVC gadget (no network) |
| RNDIS | `board-scripts/rndis-mode/user_init.sh` | `/opt/user_init.sh` | 59 lines | Boot: modules → RNDIS → sshd → app |
| RNDIS | `board-scripts/rndis-mode/usb-ether.sh` | `/opt/bin/usb-ether.sh` | 77 lines | RNDIS+ACM USB gadget bring-up |

**Key differences between modes:**

```
UVC user_init.sh:    load modules → echo device → ./run.sh (UVC camera)
RNDIS user_init.sh:  load modules → echo device → usb-ether.sh → sshd → ./run.sh
```

- UVC mode: USB presents as a camera, no `usb0` interface, no network
- RNDIS mode: USB presents as RNDIS+ACM, `usb0` at 192.168.1.101, SSH on port 22
- Both modes ultimately run `/opt/stereo/run.sh` for the stereo camera app
- The RNDIS `user_init.sh` includes a retry for `usb-ether.sh` and WSL return route

**Step 1 — Generate serial command file**

Use `serial_run.ps1 -CmdFile` with the `echo >> file` heredoc pattern. A PowerShell generator produces the command file from the bundled scripts:

```powershell
# Read bundled scripts and generate push_cmds.txt
$rndisDir = '.claude\skills\board-network-setup\board-scripts\rndis-mode'
$usbEther = [System.IO.File]::ReadAllText("$rndisDir\usb-ether.sh")
$userInit = [System.IO.File]::ReadAllText("$rndisDir\user_init.sh")
# For each script line, emit: echo '<escaped>' >> /path/file
# Follow with: sed -i "s/\r$//", chmod +x, wc -l, head -3
```

**Step 2 — Push via serial**

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\serial_run.ps1 `
    -Port COM5 -CmdFile push_cmds.txt -WaitMs 35000
```

Serial heredoc at 115200 baud reliably handles ~80-line scripts. Output display may show line wrapping but file content is correct (verify with `wc -l` and `head` in the command file itself).

**Step 3 — Verify and reboot**

The command file should verify line counts before rebooting:
- `usb-ether.sh`: 77 lines → `/opt/bin/usb-ether.sh`
- `user_init.sh`: 59 lines → `/opt/user_init.sh`

Then reboot:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\serial_run.ps1 `
    -Port COM5 -Cmd "reboot" -WaitMs 3000
```

⚠️ **Reboot risk:** If uvc_app is currently running, `reboot` may trigger a UVC driver kernel panic. Be prepared to physically power cycle (unplug/replug USB). After the first successful reboot into RNDIS mode, subsequent reboots are safe.

**Step 4 — Verify RNDIS came up**

After reboot (~10-15s), the board should appear as a Remote NDIS network adapter:

```powershell
ping 192.168.1.101
# Should respond <1ms
```

The hardware serial port (CH340/FTDI) continues to work after reboot. The USB-ACM virtual serial port (`ttyGS0`) appears as a new COM port if the RNDIS composite device enumerates successfully.

**Switching back to UVC mode:** Push `board-scripts/uvc-mode/user_init.sh` (26 lines) via the same serial heredoc method, then reboot.

**Why this is simpler than Greenfield Phase 2:**
- No manual UDC manipulation (`echo "" > UDC`, `mkdir ether`, etc.)
- No live gadget reconfiguration — the boot scripts handle everything
- Scripts persist across reboots (stored in `/opt`)
- `usb-ether.sh` is idempotent: safely handles the case where camera gadget still owns UDC
- No external backup tarball needed — scripts are bundled in the skill

### FTP-Based File Transfer (Faster Alternative to Serial)

When RNDIS networking is up but SSH/NFS are not yet available, FTP is the fastest way to move files to the board:

```sh
# Board side — start FTP server (busybox ftpd, chroot to /):
tcpsvd -E 0.0.0.0 21 ftpd -w / &

# Board side — restrict to specific directory:
tcpsvd -E 0.0.0.0 21 ftpd -w /data/openssh &
```

```powershell
# Windows side — scripted FTP upload:
$cmds = @(
    'binary'
    'put "C:\local\file" "/remote/path/file"'
    'quit'
)
$cmds -join "`r`n" | ftp -n 192.168.1.101

# Or use ftp -s:script.txt for batch transfers
```

**Prefer FTP over serial for bulk transfers** — it is ~1000× faster (network speed vs 115200 baud serial). Use serial only for:
- Commands (not file data)
- When RNDIS is completely down
- Very small files (<1KB)

### Verified Lessons From Recovery

- **WSL subnet is per-machine** — the #1 cause of "WSL can't ping board but Windows can." WSL NAT subnet varies:
  - Machine A: `172.29.192.0/20`
  - Machine B: `172.23.224.0/20`
  - After WSL version updates or `wsl --update`, the subnet may change.
  - Always compare actual `ip route show dev eth0` against `config.env` AND the board's `ip route` output.
- **WSL return route is optional for direct Windows↔Board SSH.** If you only need `ssh root@192.168.1.101` from Windows PowerShell (not from WSL), the WSL subnet return route is unnecessary. The board ↔ Windows path over RNDIS works without WSL involvement. Only add the WSL return route if you also need WSL→Board connectivity (NFS mounts, WSL-based SSH, cross-compilation from WSL). **Default: skip WSL configuration.**
- WSL IP may change after `wsl --shutdown`; always compare `hostname -I`, `portproxy`, and `WSL_SUBNET` together.
- A successful Windows ping only proves RNDIS is up; a successful WSL ping additionally proves IP forwarding AND board return route are correct.
- If WSL ping fails but Windows ping succeeds, check board return route FIRST: `ip route | grep $PC_RNDIS_IP`. The route subnet must match WSL's actual subnet.
- If RNDIS was manually recovered but `/opt/bin/usb-ether.sh` is absent on the board, reboot will break networking again.
- OpenSSH is considered reachable when port 22 is open and an SSH command reaches authentication, even if it returns `Permission denied` without a password/key.
- **`kex_exchange_identification: Connection closed by remote host` means sshd-session cannot start** — the TCP/handshake layer works but the session binary crashes. Check: (1) `LD_LIBRARY_PATH` includes `/data/openssh/lib`, (2) library SONAME symlinks exist (`libz.so.1`, `libcrypto.so`), (3) the `sshd-session` binary runs from command line with `--help`.
- **UVC/camera gadget blocks RNDIS** — the USB UDC can only bind one gadget at a time. If the board boots into camera/UVC mode, the ether gadget cannot bind until the camera gadget releases the UDC. Always check `cat /sys/kernel/config/usb_gadget/*/UDC` to see which gadget owns the controller.
- Before serial diagnostics, close MobaXterm or any terminal occupying the serial port; otherwise PowerShell reports access denied.
- If serial commands return empty output on COM7 but COM8 works: COM7=USB-ACM (appears after RNDIS), COM8=CH340 (hardware, always present). Try both.
- `wsl --shutdown` can hang if WSL processes are stuck; force kill with `taskkill /f /im wsl.exe` then restart.
- **Root filesystem may be 100% full** on embedded devices — always `df -h` before deploying. Use `/data` (persistent) or `/tmp` (tmpfs) for temp files. Never write large files to `/`.
- **`reboot` while uvc_app is running → kernel panic → board hangs forever.** The UVC driver (`usb_gadget_deactivate`) triggers a NULL pointer dereference during shutdown if the camera gadget was released before uvc_app closes its file descriptors. Board requires physical power cycle (unplug/replug USB). Once uvc_app is removed from boot scripts, future reboots are safe — but always use physical power cycle for the first reboot after removing uvc_app.
- **Busybox `mkpasswd -m sha512 <password>`** generates SHA-512 password hashes on the board. After generating a new hash, update BOTH `/data/etc/shadow` AND the embedded heredoc in `/data/openssh/start_sshd.sh` — otherwise `start_sshd.sh` overwrites the shadow file with the old hash on next boot. Use: `sed -i 's|root:.*:20000|<hash>:20000|' /data/openssh/start_sshd.sh`.
- **RNDIS adapter disappears after board reboot** — all VID_3EF5 PNP devices show `CM_PROB_PHANTOM` (device not present). The Windows RNDIS network adapter vanishes. Fix: rebind the ether gadget UDC from serial (`echo "" > UDC; sleep 1; echo "<udc_name>" > UDC`), or physically replug the USB cable. The latter is more reliable.
- **Windows USB Composite Device in Error state** blocks RNDIS from appearing as a network adapter, even though PNP shows `Remote NDIS Compatible Device` as a child device. The Error on the parent (VID_3EF5/PID_15B0) prevents all children from functioning. Rebinding UDC from the board side triggers Windows re-enumeration and usually fixes it. `Disable-PnpDevice` / `Enable-PnpDevice` alone does NOT fix this.
- **Serial port types differ — check instance IDs**: The hardware debug serial (FTDI chip, VID_0403/PID_6001 → e.g. COM26) is separate from the board's USB-ACM serial port (VID_3EF5, appears as `USB 串行设备` when the RNDIS+ACM gadget is up). Use `Get-PnpDevice | Where-Object { $_.InstanceId -match 'VID_3EF5' }` in PowerShell to identify which COM ports belong to the board vs. external debug probes. The FTDI port always works; the board's ACM port only works when the composite device is healthy.
- **`pnputil /remove-device` deletes but devices immediately re-appear** — Windows re-detects the hardware and recreates PNP entries instantly. You cannot "clear" a connected device this way. Only a physical unplug truly removes it.
- **`pnputil /enable-device` fails "设备没有连接" when PID mismatches** — if the board switched USB PID (e.g., camera PID_0EF0 vs RNDIS PID_15B0), PNP entries for the old PID are stale phantoms. The device IS connected, just with a different identity. Check `ls /sys/kernel/config/usb_gadget/*/UDC` to see which gadget owns the UDC and what PID it exports.
- **Physical USB replug → board power-cycles → camera reclaims UDC.** After unplug-replug, the board reboots and `user_init.sh` runs. If the boot script starts camera/UVC, the ether gadget is destroyed and must be rebuilt from scratch (Greenfield Phase 2: release camera UDC → create ether gadget → bind). Always check `cat /sys/kernel/config/usb_gadget/*/UDC` after replug.
- **USB Composite Device Error code 10 (CM_PROB_FAILED_START) is a driver-level failure.** Software UDC rebind, PNP disable/enable, and ghost-cleanup all failed to fix it. Physical USB replug was the only working fix. If Error code 10 persists even after replug, check if the board's USB gadget configuration has issues (e.g., missing/invalid descriptors).
- **Serial heredoc is reliable for deploying scripts** — `cat > /path/file <<'EOF'` via serial at 115200 baud works for ~60-line shell scripts. Send each line individually with ~50ms delay. The serial output display may show line truncation but the file content is correct. Always `sed -i 's/\r$//'` after writing.
- **Board has `mkpasswd` (busybox) but NOT `openssl`** — use `mkpasswd -m sha512 <password>` on the board to generate SHA-512 password hashes. Update both `/data/etc/shadow` AND the heredoc in `/data/openssh/start_sshd.sh` to survive reboot.
- **After switching from camera to RNDIS, Windows auto-detects the new PID** — a fresh Remote NDIS Compatible Device network adapter appears within seconds. No manual driver installation needed if the Microsoft RNDIS driver is present. The adapter may need IP assignment: `New-NetIPAddress -InterfaceIndex <idx> -IPAddress 192.168.1.3 -PrefixLength 24`.
- **Simplest UVC→RNDIS switch: push bundled boot scripts via serial, then reboot.** Instead of live UDC manipulation (Greenfield Phase 2) or extracting from backup tarballs, use the skill's bundled scripts: `board-scripts/rndis-mode/user_init.sh` (59 lines) and `board-scripts/rndis-mode/usb-ether.sh` (77 lines). Push via `serial_run.ps1 -CmdFile` with the `echo '...' >> file` heredoc pattern, verify line counts, then reboot. The boot scripts handle the gadget switch automatically. To revert to UVC mode, push `board-scripts/uvc-mode/user_init.sh` (26 lines).

---

## Troubleshooting Decision Tree

### Board `mount` times out

```
Board mount timeout?
├── ping {PC_RNDIS_IP} from board?
│   ├── NO → RNDIS not up / PC IP not configured
│   └── YES → NFS service or portproxy issue
│       ├── WSL NFS running?
│       │   wsl -d {WSL_DISTRO} --user root -- rpcinfo -p localhost
│       │   ├── Timeout/empty → WSL hung, do: wsl --shutdown + nfs_up.ps1
│       │   │   └── wsl --shutdown also hangs? → taskkill /f /im wsl.exe, then restart
│       │   └── Shows ports → portproxy stale, re-run nfs_up.ps1
│       └── portproxy shows rules but fails → delete+re-add (nfs_up.ps1)
```

### WSL cannot ping/SSH board

```
WSL ping {BOARD_IP} fails?
├── Windows ping {BOARD_IP}?
│   ├── NO → Board not connected
│   └── YES → IP forwarding or return route
│       ├── IP forwarding disabled? → nfs_up.ps1 (step 3)
│       └── Board missing return route? → ip route add {WSL_SUBNET} via {PC_RNDIS_IP}
```

### SSH port unreachable after reboot

```
SSH port 22 closed?
├── Windows ping {BOARD_IP}?
│   ├── NO → RNDIS did not come up; check /opt/bin/usb-ether.sh and serial logs
│   └── YES → check sshd startup
│       ├── /data/openssh/start_sshd.sh exists and executable?
│       ├── /data/openssh/libexec/sshd-session symlink exists?
│       ├── ps | grep sshd shows listener?
│       └── Test-NetConnection {BOARD_IP} -Port 22 succeeds?
```

### WSL commands hang

```
wsl ... hangs? → wsl --shutdown, wait 3s, re-run nfs_up.ps1
```

---

## Manual Steps (when scripts unavailable)

### 1. Start WSL NFS

```powershell
wsl -d {WSL_DISTRO} --user root -- bash -c "service rpcbind start; service nfs-kernel-server start"
```

### 2. Refresh portproxy (admin)

```powershell
$WslIp = (wsl -d {WSL_DISTRO} -- hostname -I).Trim().Split(' ')[0]
foreach ($p in @(111, 2049, 20048)) {
    netsh interface portproxy delete v4tov4 listenport=$p listenaddress=0.0.0.0
    netsh interface portproxy add v4tov4 listenport=$p listenaddress=0.0.0.0 connectport=$p connectaddress=$WslIp
}
```

### 3. Enable IP forwarding (admin)

```powershell
$rndis = Get-NetAdapter | Where-Object { $_.InterfaceDescription -like 'Remote NDIS*' -and $_.Status -eq 'Up' }
$wslVs = Get-NetAdapter | Where-Object { $_.Name -like 'vEthernet (WSL*' -and $_.Status -eq 'Up' }
Set-NetIPInterface -InterfaceIndex $rndis.ifIndex -Forwarding Enabled
Set-NetIPInterface -InterfaceIndex $wslVs.ifIndex -Forwarding Enabled
```

### 4. Board return route

```sh
ip route add {WSL_SUBNET} via {PC_RNDIS_IP}
```

### 5. Board NFS mount

```sh
mkdir -p /tmp/nfs
mount -t nfs -o nolock,vers=3,proto=tcp,soft,timeo=30,rsize=8192,wsize=8192 \
    {PC_RNDIS_IP}:{NFS_EXPORT_DIR} /tmp/nfs
```

### 6. Deploy / start OpenSSH

```sh
mkdir -p /data/openssh
# Copy skill bundled openssh/ to /data/openssh, for example via NFS:
# cp -a /tmp/nfs/<project>/.qoder/skills/board-network-setup/openssh/. /data/openssh/
chmod +x /data/openssh/start_sshd.sh /data/openssh/bin/*
sed -i 's/\r$//' /data/openssh/start_sshd.sh /data/openssh/etc/ssh/sshd_config
/data/openssh/start_sshd.sh
```

### 7. Verify SSH

```powershell
Test-NetConnection -ComputerName {BOARD_IP} -Port 22
wsl -d {WSL_DISTRO} -- bash -lc "ssh -o BatchMode=yes -o ConnectTimeout=3 root@{BOARD_IP} true"
```

`Permission denied` in the SSH command means the TCP/SSH handshake reached authentication, so the service is reachable.

---

## Common Pitfalls

| Pitfall | Root Cause | Fix |
|---------|-----------|-----|
| **WSL can't ping board but Windows can** | Board return route has wrong WSL subnet (per-machine!) | `ip route del <old>; ip route add <actual_subnet> via $PC_RNDIS_IP`; update `user_init.sh` |
| **NFS/rpcinfo hangs in WSL** | WSL processes deadlocked (especially after host sleep) | `wsl --shutdown`; if hangs, `taskkill /f /im wsl.exe`; then re-run `nfs_up.ps1` |
| WSL services don't auto-start | WSL2 doesn't persist services across shutdown | Run `nfs_up.ps1` after each WSL restart |
| portproxy rules present but dead | iphlpsvc socket stale after WSL network reset | Delete + re-add (nfs_up.ps1 step [2]) |
| IP forwarding lost after replug | Adapter ifIndex changes on replug/reboot | Scripts detect by name/description, not index |
| Board route lost on reboot | `ip route` is volatile | Persisted in `user_init.sh.net` — ensure `WSL_SUBNET` matches actual |
| NFS requires `vers=3,nolock` | BusyBox userspace nfsmount, NFSv3 only | Always specify in OPTS |
| mountd on random port | Ubuntu default | Fixed via RPCMOUNTDOPTS in `/etc/default/nfs-kernel-server` |
| RNDIS does not recover after reboot | `/opt/bin/usb-ether.sh` missing or not executable | Deploy fixed `usb-ether.sh` to `/opt/bin/usb-ether.sh`, `chmod +x`, LF only |
| SSH port 22 closed after reboot | OpenSSH not deployed to `/data/openssh` or `user_init.sh` does not start it | Deploy OpenSSH bundle and use `user_init.sh.net` autostart block |
| `sshd-session does not exist or is not executable` | OpenSSH expects libexec helper path | `start_sshd.sh` creates `/data/openssh/libexec/sshd-session -> /data/openssh/bin/sshd-session` |
| Serial port access denied | Serial console already opened by MobaXterm/other terminal | Close the serial session/app, then retry |
| **Board scripts have CRLF (\r\n)** | Windows editor/NFS saved with CR | `sed -i 's/\r$//' /opt/user_init.sh` — BusyBox ash cannot parse \r, all paths & commands silently fail |
| DRD stuck in host mode after reboot | user_init.sh had CRLF, `echo device` never executed | Fix CRLF; always push .sh files with LF only |
| **detect_env.ps1 shows WSL_SUBNET mismatch** | Switched to different PC or WSL updated | Update `config.env` and board `/opt/user_init.sh` to actual subnet |
| **`kex_exchange_identification: Connection closed by remote host`** after port 22 is open | `sshd-session` can't find libz.so.1 / libcrypto.so at runtime; the binary crashes silently | Create SONAME symlinks: `ln -sf libz.so.1.2.12 libz.so.1` etc. in /data/openssh/lib; verify with `sshd-session --help` |
| RNDIS won't come up even though `usb-ether.sh` runs | Camera/UVC gadget still owns the UDC, only one gadget can bind at a time | `echo "" > /sys/kernel/config/usb_gadget/camera/UDC` to release, then re-run usb-ether.sh |
| FTP upload succeeds but files are flat (no subdirectories) | `ftpd -w` sets working dir but Windows `ftp` client doesn't support remote `mkdir`; all files land in root of the FTP tree | Manually `mkdir -p` via serial first, then move files to subdirectories after upload |
| **Root filesystem `/` is 100% full** | Embedded device has minimal rootfs (e.g., 6.7MB squashfs/ro); all persistent storage is on `/data` | Always `df -h` first; deploy to `/data` (persistent) or use `/tmp` (tmpfs, 50MB+) for temp files |
| `sshd-session` symlink exists but sshd still fails to find it | `sshd` resolves `libexec/sshd-session` relative to its configured path; the symlink must be an absolute path or correctly relative | `start_sshd.sh` creates `$OPENSSH_DIR/libexec/sshd-session -> $OPENSSH_DIR/bin/sshd-session` (absolute) |
| `echo 'password' \| ssh root@...` returns `Permission denied` | SSH reads passwords from `/dev/tty`, not stdin; piping the password has no effect | Use interactive terminal login, or set up SSH key-based authentication for non-interactive (scripted) access |
| Long serial commands produce truncated or incomplete output | Serial buffer limits at 115200 baud; very long single-line commands get cut off | Break multi-step procedures into smaller commands. Deploy scripts via heredoc (`cat > file <<'EOF'`) rather than running giant one-liners |
| RNDIS adapter is Up but has no IPv4 address | Windows does not auto-assign an IP to RNDIS adapters; the adapter appears but is unconfigured | `New-NetIPAddress -InterfaceIndex <idx> -IPAddress 192.168.1.3 -PrefixLength 24` in PowerShell. Check `Get-NetAdapter` for ifIndex |
| `reboot` causes kernel panic and board hangs, requiring physical power cycle | uvc_app holds FDs to the UVC gadget; `usb_gadget_deactivate` NULL-derefs when the gadget was already released | Physical power cycle (unplug/replug USB). After removing uvc_app from `/opt/user_init.sh`, future reboots are safe — but the FIRST reboot after the change still needs a power cycle if uvc_app is currently running |
| **WSL not needed for Windows↔Board SSH** | User spends time configuring WSL return route, portproxy, NFS when they only need PowerShell `ssh root@192.168.1.101` | Skip WSL configuration entirely for direct SSH. Only set up WSL if NFS mounts or WSL-based toolchains are required. |
| **`pnputil /remove-device` deletes but device re-appears instantly** | Windows re-detects connected hardware and recreates PNP entries | Physical USB unplug is the only way to truly remove. Software deletion is cosmetic. |
| **`pnputil /enable-device` says "设备没有连接"** | Board's USB PID changed (camera vs RNDIS); the PNP entry is a stale phantom of the old identity | Check actual UDC owner via serial: `cat /sys/kernel/config/usb_gadget/*/UDC`. Switch gadget to RNDIS first. |
| **USB Composite Device Error code 10 persists through all software fixes** | Driver-level failure; UDC rebind/PNP cleanup/disable-enable all fail | Physical USB replug is the only reliable fix. Also try updating the Microsoft RNDIS driver. |
| **After physical replug, RNDIS gone → camera is back** | Board reboots and `user_init.sh` runs camera; ether gadget was never persisted | Must switch from camera to RNDIS manually (Greenfield Phase 2), THEN persist with updated `user_init.sh` including `usb-ether.sh`. |
| **Serial heredoc `cat > file <<'EOF'` for deploying scripts** | Files transferred via serial may have CRLF or display truncation | Works reliably at 115200 baud for ~60-line scripts. Always `sed -i 's/\r$//'` after writing. Content is correct even if serial output looks truncated. |
| **SSH password unknown after deployment** | SHA-512 hash in `start_sshd.sh` was generated earlier with unknown password | Use `mkpasswd -m sha512 <newpass>` on board, update both `/data/etc/shadow` AND the heredoc in `start_sshd.sh`. |

---

## Serial Console Diagnostics

When RNDIS/SSH is down and only serial console is available, use `serial_run.ps1`:

### Quick serial command

```powershell
# Inline (PowerShell direct - avoids sandbox issues with -File):
$sp = New-Object System.IO.Ports.SerialPort '{SERIAL_PORT}',{SERIAL_BAUD},'None',8,'One'
$sp.NewLine="`n"; $sp.ReadTimeout=500; $sp.DtrEnable=$true; $sp.RtsEnable=$true
$sp.Open(); Start-Sleep -Milliseconds 200; $null=$sp.ReadExisting()
$sp.Write("`r"); Start-Sleep -Milliseconds 200; $null=$sp.ReadExisting()
$sp.Write("<COMMAND>`n"); Start-Sleep -Milliseconds <WAIT_MS>
$out=$sp.ReadExisting(); $sp.Close(); Write-Output $out
```

### RNDIS Diagnostic Sequence

When ethernet is not working, run these commands via serial to diagnose:

```sh
# 1. Check if USB gadget is configured
ls /sys/kernel/config/usb_gadget/ether/
cat /sys/kernel/config/usb_gadget/ether/UDC

# 2. Check if usb0 interface exists and has IP
ifconfig usb0

# 3. Check USB DRD mode
cat /proc/10320000.usb30drd/mode

# 4. Check if usb-ether.sh ran
ps | grep usb-ether

# 5. Check dmesg for USB/dwc3 errors
dmesg | grep -i -E 'dwc3|gadget|usb0|rndis' | tail -20

# 6. Try manual restart
/opt/bin/usb-ether.sh
ifconfig usb0
```

### Push files via serial

```powershell
# Push a script to the board when NFS/SSH unavailable:
$sp = ... # same as above
# Use serial_push.ps1 logic or inline base64 transfer
```

---

## Adapting to a New Environment

When switching PC or setting up a new board:

**Step 0: Auto-detect per-machine variables (ALWAYS FIRST)**

```powershell
powershell -ExecutionPolicy Bypass -File .claude\skills\board-network-setup\scripts\detect_env.ps1
```

This tells you:
- Actual `WSL_SUBNET` and whether `config.env` needs updating
- Which serial ports are available and which responds as a board
- Whether Windows↔Board and WSL↔Board connectivity works
- Whether the board's return route is stale

**Step 1-9: Apply fixes and deploy**

1. Copy this skill directory to the new project
2. Edit `config.env` with values from `detect_env.ps1`:
   - Set `SERIAL_PORT` to the available port on this machine ★ REQUIRED
   - Set `WSL_SUBNET` to the actual detected subnet (★ ONLY if WSL→Board connectivity is needed; skip for direct Windows SSH)
   - Verify `WSL_DISTRO`, `NFS_EXPORT_DIR` (only if using NFS)
   - Verify `PC_RNDIS_IP` / `BOARD_IP` (usually stable at `192.168.1.3` / `192.168.1.101`)
3. Deploy `scripts/user_init.sh.net` to board `/opt/user_init.sh.net`, and copy it to `/opt/user_init.sh` to activate net mode
   - **IMPORTANT**: Update the `WSL_SUBNET` default in the deployed copy to match this machine's actual subnet (or remove the return route line if WSL is not used)
   - **IMPORTANT**: Back up the existing `/opt/user_init.sh` first! (e.g., `cp /opt/user_init.sh /opt/user_init.sh.camera_bak`)
4. Deploy `scripts/nfs-mount.sh` to board `/opt/bin/nfs-mount.sh` (only if using NFS)
5. Deploy `scripts/usb-ether.sh` to board `/opt/bin/usb-ether.sh`
6. Deploy bundled `openssh/` to board `/data/openssh` (via FTP if RNDIS is up: `tcpsvd -E 0.0.0.0 21 ftpd -w / &`; otherwise via serial)
7. Deploy `scripts/start_sshd.sh` to board `/data/openssh/start_sshd.sh` if you changed it separately
8. Run `scripts/nfs_up.ps1` on PC — only if using NFS (skip for direct Windows SSH)
9. Reboot the board and verify: Windows ping, port 22, SSH login. WSL ping and NFS mount only if configured.
