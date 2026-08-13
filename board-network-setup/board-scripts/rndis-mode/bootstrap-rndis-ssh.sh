#!/bin/sh
# One-time UVC-to-RNDIS bootstrap. Run detached from the ACM console:
#   setsid /data/rndis-bootstrap.sh </dev/null >/data/rndis-bootstrap.log 2>&1 &
#
# The script stops UVC through usb_mode_ctl (the required safe teardown
# sequence), brings up RNDIS+ACM, starts a short-lived FTP listener, then waits
# for /data/openssh/.ssh-upload-ready. The FTP upload helper creates that marker
# only after the complete OpenSSH bundle has arrived.

OPENSSH_DIR=/data/openssh
READY_FILE="$OPENSSH_DIR/.ssh-upload-ready"
FTP_PID_FILE=/tmp/rndis-ftp.pid

mkdir -p "$OPENSSH_DIR"
rm -f "$READY_FILE"

echo "Stopping UVC gadget"
/etc/usb_mode_ctl.sh stop
sleep 2

echo "Starting RNDIS gadget"
/opt/bin/usb-ether.sh || exit 1
ifconfig usb0 192.168.1.101 || exit 1

echo "Starting temporary FTP server"
kill "$(cat "$FTP_PID_FILE" 2>/dev/null)" 2>/dev/null
/usr/bin/tcpsvd -E 0.0.0.0 21 /usr/sbin/ftpd -w / &
echo $! > "$FTP_PID_FILE"

echo "Waiting for OpenSSH upload marker"
i=0
while [ ! -f "$READY_FILE" ] && [ "$i" -lt 900 ]; do
    sleep 1
    i=$((i + 1))
done

if [ ! -f "$READY_FILE" ]; then
    echo "OpenSSH upload marker did not arrive"
    exit 1
fi

chmod +x "$OPENSSH_DIR/start_sshd.sh" "$OPENSSH_DIR/bin/"* || exit 1
sed -i 's/\r$//' "$OPENSSH_DIR/start_sshd.sh" "$OPENSSH_DIR/etc/ssh/sshd_config"

echo "Starting OpenSSH"
"$OPENSSH_DIR/start_sshd.sh" || exit 1

# The marker is uploaded last, so all FTP data has been acknowledged. Stop the
# unauthenticated bootstrap server once SSH is running.
sleep 2
kill "$(cat "$FTP_PID_FILE" 2>/dev/null)" 2>/dev/null
rm -f "$FTP_PID_FILE"
echo "RNDIS SSH bootstrap complete"
