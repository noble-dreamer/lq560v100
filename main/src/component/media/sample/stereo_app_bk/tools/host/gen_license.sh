#!/bin/bash
# gen_license.sh — Deploy auth_gen to board, run it, capture UID, clean up.
#
# Usage: ./gen_license <board_ip> [password]
#
# Prerequisites:
#   - auth_gen binary cross-compiled and in the same directory as this script
#   - sshpass installed on the host
#   - Board reachable via SSH at <board_ip>
#
# Output: UID hex string (48 chars), printed to stdout
# Side effect: /opt/stereo/license.bin created on the board

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <board_ip> [password]"
    echo ""
    echo "  board_ip   Board IP address (e.g. 192.168.1.101)"
    echo "  password   SSH password (default: 123456)"
    exit 1
fi

BOARD_IP="$1"
BOARD_USER="root"
BOARD_PASS="${2:-123456}"

AUTH_GEN="${SCRIPT_DIR}/auth_gen"
if [ ! -f "$AUTH_GEN" ]; then
    echo "ERROR: auth_gen binary not found at $AUTH_GEN"
    echo "       Build it first with the SDK cross toolchain:"
    echo "       make -C <sdk>/main/src/component/media/sample/stereo_app auth_gen"
    exit 1
fi

SSH_OPTS="-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null"
REMOTE_PATH="/tmp/auth_gen"

echo "==> Deploying auth_gen to ${BOARD_USER}@${BOARD_IP}:${REMOTE_PATH}"
sshpass -p "$BOARD_PASS" scp $SSH_OPTS "$AUTH_GEN" "${BOARD_USER}@${BOARD_IP}:${REMOTE_PATH}"

echo "==> Running auth_gen on board..."
SSH_OUTPUT=$(sshpass -p "$BOARD_PASS" ssh $SSH_OPTS "${BOARD_USER}@${BOARD_IP}" "chmod +x ${REMOTE_PATH} && ${REMOTE_PATH}" 2>&1 || true)

echo "==> Cleaning up ${REMOTE_PATH} on board"
sshpass -p "$BOARD_PASS" ssh $SSH_OPTS "${BOARD_USER}@${BOARD_IP}" "rm -f ${REMOTE_PATH}" 2>/dev/null || true

# Extract UID from output (format: "UID:<48 hex chars>")
UID_HEX=$(echo "$SSH_OUTPUT" | grep -oP 'UID:[0-9a-f]+' | cut -d: -f2)

if [ -z "$UID_HEX" ]; then
    echo "ERROR: failed to capture UID from auth_gen output"
    echo "Raw output:"
    echo "$SSH_OUTPUT"
    exit 1
fi

# Verify license.bin was created on board
echo "==> Verifying license.bin on board..."
LICENSE_OK=$(sshpass -p "$BOARD_PASS" ssh $SSH_OPTS "${BOARD_USER}@${BOARD_IP}" \
    "test -f /opt/stereo/license.bin && wc -c < /opt/stereo/license.bin" 2>/dev/null || echo "MISSING")

if [ "$LICENSE_OK" = "MISSING" ]; then
    echo "ERROR: /opt/stereo/license.bin was not created on board"
    exit 1
fi

echo "==> license.bin: ${LICENSE_OK} bytes on board"
echo ""
echo "============================================"
echo " Device UID: $UID_HEX"
echo "============================================"
echo ""
echo "Use this UID to encrypt the model:"
echo "  ./encrypt_model <model.ortm> <model.ortm.enc> $UID_HEX"
echo ""
