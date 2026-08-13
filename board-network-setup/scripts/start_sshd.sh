#!/bin/sh
# =============================================================================
# start_sshd.sh - Fix accounts & start OpenSSH on SD3589C
#
# Deploy to: /data/openssh/start_sshd.sh
# Called from: /opt/user_init.sh (via user_init.sh.net)
#
# Merges fix_openssh.sh + start_sshd.sh into one idempotent script.
# =============================================================================

OPENSSH_DIR="${OPENSSH_DIR:-/data/openssh}"

# --- 1. Account setup (bind mount passwd/shadow/group) ---
mkdir -p /data/etc

cat > /data/etc/passwd <<'EOF'
root:x:0:0::/root:/bin/sh
sshd:x:74:74::/var/empty:/bin/false
EOF

cat > /data/etc/shadow <<'EOF'
root:$6$LjyMOtp0JWAdnNcE$cg8m8.vG1lyl7MfpnNPMDER9kXR4dLbo4kJi/5JrYLX1.tPx0TwBQAeDWkZnJCsUOUSUyAC/aKzw4a.P9SCRI1:20000:0:99999:7:::
sshd:*:20000:0:99999:7:::
EOF

cat > /data/etc/group <<'EOF'
root::0:
sshd::74:
EOF

mount --bind /data/etc/passwd /etc/passwd 2>/dev/null
mount --bind /data/etc/shadow /etc/shadow 2>/dev/null
mount --bind /data/etc/group  /etc/group  2>/dev/null

# --- 2. Required directories ---
mkdir -p /tmp/var/empty
mount --bind /tmp/var /var 2>/dev/null
mkdir -p /var/empty /var/run
chmod 755 /var/empty

mkdir -p /tmp/empty
chmod 755 /tmp/empty

mkdir -p "$OPENSSH_DIR/libexec"
ln -sf "$OPENSSH_DIR/bin/sshd-session" "$OPENSSH_DIR/libexec/sshd-session"
chmod +x "$OPENSSH_DIR/libexec/sshd-session" 2>/dev/null

# --- Ensure library SONAME symlinks (sshd-session links against libz.so.1 etc.) ---
ln -sf libz.so.1.2.12    "$OPENSSH_DIR/lib/libz.so.1"    2>/dev/null
ln -sf libcrypto.so.3    "$OPENSSH_DIR/lib/libcrypto.so"  2>/dev/null
ln -sf libcrypt.so.1     "$OPENSSH_DIR/lib/libcrypt.so"   2>/dev/null

# --- 3. Generate host keys if missing ---
if [ ! -f "$OPENSSH_DIR/etc/ssh/ssh_host_rsa_key" ]; then
    echo "Generating SSH host keys..."
    "$OPENSSH_DIR/bin/ssh-keygen" -t rsa -f "$OPENSSH_DIR/etc/ssh/ssh_host_rsa_key" -N "" -q
    "$OPENSSH_DIR/bin/ssh-keygen" -t ed25519 -f "$OPENSSH_DIR/etc/ssh/ssh_host_ed25519_key" -N "" -q
fi
chmod 600 "$OPENSSH_DIR/etc/ssh/ssh_host_rsa_key" "$OPENSSH_DIR/etc/ssh/ssh_host_ed25519_key"
chmod 644 "$OPENSSH_DIR/etc/ssh/"*.pub 2>/dev/null

# --- 4. Start sshd ---
export LD_LIBRARY_PATH="$OPENSSH_DIR/lib:$LD_LIBRARY_PATH"
"$OPENSSH_DIR/bin/sshd" -f "$OPENSSH_DIR/etc/ssh/sshd_config"
echo "sshd started."
