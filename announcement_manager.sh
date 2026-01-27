
#!/usr/bin/env bash
#
# setup-supermon-announcements.sh
# Fully automates Supermon Announcements Manager setup
#
# Author: N5AD — January 2026
#
set -euo pipefail

# ────────────────────────────────────────────────
# CONFIG
# ────────────────────────────────────────────────
REPO_URL="https://github.com/n5ad/announcement-manager.git"
TEMP_CLONE="/tmp/supermon-announcements"
TARGET_DIR="/var/www/html/supermon/custom"
LINK_PHP="/var/www/html/supermon/link.php"
MP3_DIR="/mp3"
LOCAL_DIR="/etc/asterisk/local"

# ────────────────────────────────────────────────
# Helpers
# ────────────────────────────────────────────────
echo_step() { echo -e "\n\033[1;34m==>\033[0m $1"; }
warn() { echo -e "\033[1;33mWARNING:\033[0m $1" >&2; }
error() { echo -e "\033[1;31mERROR:\033[0m $1" >&2; exit 1; }
check_root() { [[ $EUID -eq 0 ]] || error "Run as root (sudo)."; }

# ────────────────────────────────────────────────
# STEP 1 — Install required packages
# ────────────────────────────────────────────────
check_root
echo_step "1. Installing required packages (git, sox, libsox-fmt-mp3, perl)"

apt update || error "apt update failed"
apt install -y git || error "Failed to install git"
apt install -y sox libsox-fmt-mp3 perl || error "Failed to install packages"

command -v git >/dev/null 2>&1 || error "git missing after install"

# ────────────────────────────────────────────────
# STEP 2 — User confirmation & node number
# ────────────────────────────────────────────────
echo ""
echo "Supermon Announcements Manager - Full Setup"
echo "────────────────────────────────────────────"
echo "Repo:        $REPO_URL"
echo "Target dir:  $TARGET_DIR"
echo "MP3 dir:     $MP3_DIR"
echo "Local dir:   $LOCAL_DIR"
echo "link.php:    $LINK_PHP"
echo ""
read -rp "Continue setup? (y/N) " answer
[[ "$answer" =~ ^[Yy]$ ]] || exit 0

read -rp "Enter AllStar node number: " NODE_NUMBER
[[ "$NODE_NUMBER" =~ ^[0-9]+$ ]] || error "Invalid node number"

# ────────────────────────────────────────────────
# STEP 3 — Clone repo
# ────────────────────────────────────────────────
echo_step "3. Cloning GitHub repository"
rm -rf "$TEMP_CLONE"
git clone --depth 1 "$REPO_URL" "$TEMP_CLONE"

# ────────────────────────────────────────────────
# STEP 4 — Copy PHP & INC files
# ────────────────────────────────────────────────
echo_step "4. Copying PHP and INC files"
mkdir -p "$TARGET_DIR"
cp -v "$TEMP_CLONE"/*.{php,inc} "$TARGET_DIR"/ 2>/dev/null || warn "No PHP/INC files found"
rm -rf "$TEMP_CLONE"

# ────────────────────────────────────────────────
# STEP 5 — Create /mp3 directory
# ────────────────────────────────────────────────
echo_step "5. Creating /mp3 directory"
mkdir -p "$MP3_DIR"

MP3_USER="${SUDO_USER:-$(whoami)}"
id -nG "$MP3_USER" | grep -qw www-data || usermod -aG www-data "$MP3_USER"

chown -R www-data:www-data "$MP3_DIR"
chmod -R 2775 "$MP3_DIR"

# ────────────────────────────────────────────────
# STEP 6 — Permissions for Supermon custom files
# ────────────────────────────────────────────────
echo_step "6. Setting permissions on Supermon custom files"
chown -R www-data:www-data "$TARGET_DIR"
find "$TARGET_DIR" -type f -name "*.php" -exec chmod 644 {} \;
find "$TARGET_DIR" -type f -name "*.inc" -exec chmod 644 {} \;

# ────────────────────────────────────────────────
# STEP 7 — Permissions for Asterisk sounds
# ────────────────────────────────────────────────
echo_step "7. Setting permissions on Asterisk sounds"
chown -R www-data:www-data /usr/local/share/asterisk/sounds
chmod -R 775 /usr/local/share/asterisk/sounds
chmod g+s /usr/local/share/asterisk/sounds
find /usr/local/share/asterisk/sounds -type d -exec chmod 2775 {} \;
find /usr/local/share/asterisk/sounds -type f -exec chmod 664 {} \;

# ────────────────────────────────────────────────
# STEP 8 — Install prerequisite scripts
# ────────────────────────────────────────────────
echo_step "8. Installing prerequisite Asterisk scripts"
mkdir -p "$LOCAL_DIR"
chmod 755 "$LOCAL_DIR"

PLAY_SCRIPT="$LOCAL_DIR/playaudio.sh"
if [[ ! -f "$PLAY_SCRIPT" ]]; then
cat > "$PLAY_SCRIPT" << EOF
#!/bin/bash
NODE="$NODE_NUMBER"
/usr/sbin/asterisk -rx "rpt localplay \${NODE} \$1"
EOF
chmod 755 "$PLAY_SCRIPT"
fi

CONVERT_SCRIPT="$LOCAL_DIR/audio_convert.sh"
if [[ ! -f "$CONVERT_SCRIPT" ]]; then
cat > "$CONVERT_SCRIPT" << 'EOF'
#!/bin/bash
sox "$1" -t raw -r 8000 -c 1 -e u-law "${2:-${1%.*}.ul}"
EOF
chmod 755 "$CONVERT_SCRIPT"
fi

# ────────────────────────────────────────────────
# STEP 9 — Install new link.php
# ────────────────────────────────────────────────
echo_step "9. Installing link.php"
[[ -f "$LINK_PHP" ]] && cp "$LINK_PHP" "$LINK_PHP.bak"
wget -O "$LINK_PHP" https://raw.githubusercontent.com/n5ad/announcement-manager/main/link.php
chown www-data:www-data "$LINK_PHP"
chmod 644 "$LINK_PHP"

# ────────────────────────────────────────────────
# STEP 10 — Create sudoers rule
# ────────────────────────────────────────────────
echo_step "10. Creating sudoers rules"
SUDOERS_FILE="/etc/sudoers.d/99-supermon-announcements"
[[ -f "$SUDOERS_FILE" ]] || cat > "$SUDOERS_FILE" << 'EOF'
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/playaudio.sh
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/audio_convert.sh
www-data ALL=(root) NOPASSWD: /usr/bin/crontab
www-data ALL=(root) NOPASSWD: /bin/cp, /bin/chown, /bin/chmod
www-data ALL=(root) NOPASSWD: /bin/rm /usr/local/share/asterisk/sounds/*.ul
www-data ALL=(root) NOPASSWD: /usr/local/bin/piper_prompt_tts.sh
EOF
chmod 0440 "$SUDOERS_FILE"

# ────────────────────────────────────────────────
# STEP 11 — Install Piper TTS
# ────────────────────────────────────────────────
echo_step "11. Installing Piper TTS"
if [[ ! -f /opt/piper/bin/piper ]]; then
wget https://github.com/rhasspy/piper/releases/download/v1.2.0/piper_arm64.tar.gz -O /tmp/piper.tgz
mkdir -p /opt/piper/bin /opt/piper/voices
tar -xzf /tmp/piper.tgz -C /opt/piper/bin
chmod +x /opt/piper/bin/piper
cd /opt/piper/voices
wget -4 https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
wget -4 https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json
chown www-data:www-data *.onnx*
chmod 644 *.onnx*
fi

# ────────────────────────────────────────────────
# STEP 12 — Download Piper helper files
# ────────────────────────────────────────────────
echo_step "12. Installing Piper helper scripts"
wget -O "$TARGET_DIR/piper_generate.php" https://raw.githubusercontent.com/n5ad/announcement-manager/main/piper_generate.php
chown www-data:www-data "$TARGET_DIR/piper_generate.php"
chmod 644 "$TARGET_DIR/piper_generate.php"

wget -O /usr/local/bin/piper_prompt_tts.sh https://raw.githubusercontent.com/n5ad/announcement-manager/main/piper_prompt_tts.sh
chmod +x /usr/local/bin/piper_prompt_tts.sh

# ────────────────────────────────────────────────
# STEP 13 — Test Piper
# ────────────────────────────────────────────────
echo_step "13. Testing Piper TTS"
echo "This is a Piper test on node $(hostname)" | \
/opt/piper/bin/piper --model /opt/piper/voices/en_US-lessac-medium.onnx --output_file /mp3/piper_test.wav

# ────────────────────────────────────────────────
# STEP 14 — Done
# ────────────────────────────────────────────────
echo_step "14. Setup complete"
echo "Announcements Manager is now available in Supermon"
echo "73 — N5AD"
