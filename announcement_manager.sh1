#!/usr/bin/env bash
#
# setup-supermon-announcements.sh
# Fully automates Supermon Announcements Manager setup:
# - Installs required packages: sox + libsox-fmt-mp3 (for MP3 support) + git + perl
# - Copies files from GitHub to /var/www/html/supermon/custom/
# - Installs prerequisite scripts: playaudio.sh & audio_convert.sh in /etc/asterisk/local/ (exact from KD5FMU repos)
# - Creates /mp3 directory with correct permissions (2775, setgid)
# - Automatically grants access to the invoking user
# - Sets ownership & permissions on files
# - Backs up old link.php to link.php.bak and installs new link.php from repo
# - Creates /etc/sudoers.d/99-supermon-announcements for www-data (passwordless access to required commands)
# - Installs Piper TTS 1.2.0 ARM64 (binary + libs in /opt/piper/bin/, voices in /opt/piper/voices/)
# - Downloads piper_generate.php and piper_prompt_tts.sh and makes them executable
# - Safe & idempotent (can run multiple times)
#
# Run as root: sudo bash setup-supermon-announcements.sh
# Author: N5AD - January 2026 (updated)
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
ANNOUNCE_DIR="/usr/local/share/asterisk/sounds/announcements"

# ────────────────────────────────────────────────
# Helpers
# ────────────────────────────────────────────────
echo_step() { echo -e "\n\033[1;34m==>\033[0m $1"; }
warn() { echo -e "\033[1;33mWARNING:\033[0m $1" >&2; }
error() { echo -e "\033[1;31mERROR:\033[0m $1" >&2; exit 1; }
check_root() { [[ $EUID -eq 0 ]] || error "Run as root (sudo)."; }

# ────────────────────────────────────────────────
# STEP 1. Install required packages FIRST – force git install
# ────────────────────────────────────────────────
check_root
echo_step "1. Installing required packages (sox, libsox-fmt-mp3, git, perl)"
apt update || error "apt update failed. Check internet or sources.list."
apt install -y git || error "Failed to install git. Check internet/apt sources."
apt install -y sox libsox-fmt-mp3 perl || error "Failed to install other packages."
echo "All required packages (sox, libsox-fmt-mp3, git, perl) installed or already present."

if ! command -v git >/dev/null 2>&1; then
    error "git is still not installed after apt. Check your internet or apt sources."
fi

echo ""
echo "Supermon Announcements Manager - Full Setup"
echo "──────────────────────────────────────────────"
echo "GitHub Repo: $REPO_URL"
echo "Target dir: $TARGET_DIR"
echo "MP3 dir: $MP3_DIR"
echo "Local scripts dir: $LOCAL_DIR"
echo "link.php location: $LINK_PHP"
echo ""
echo -n "Continue setup? (y/N) "
read -r answer
[[ "$answer" =~ ^[Yy]$ ]] || { echo "Aborted."; exit 0; }

# STEP 2. Prompt for AllStar node number
echo ""
echo_step "2. Enter your AllStar node number"
echo -n "Node number (e.g., 12345): "
read -r NODE_NUMBER
if [[ ! "$NODE_NUMBER" =~ ^[0-9]+$ ]]; then
    error "Invalid node number! Please enter digits only."
fi
echo "Using node number: $NODE_NUMBER"

# STEP 3. Clone repo
echo_step "3. Cloning GitHub repo"
rm -rf "$TEMP_CLONE"
git clone --depth 1 "$REPO_URL" "$TEMP_CLONE" || error "Git clone failed"

# STEP 4. Copy PHP & inc files
echo_step "4. Copying files to $TARGET_DIR"
mkdir -p "$TARGET_DIR"
cp -v "$TEMP_CLONE"/*.{php,inc} "$TARGET_DIR"/ 2>/dev/null || warn "No .php/.inc files found"
rm -rf "$TEMP_CLONE"

# STEP 5. Create /mp3 dir + permissions
echo_step "5. Creating /mp3 directory"
mkdir -p "$MP3_DIR"
MP3_USER="${SUDO_USER:-$(whoami)}"
echo "Granting /mp3 access to user: $MP3_USER"
if id -nG "$MP3_USER" | grep -qw "www-data"; then
    echo "$MP3_USER is already in www-data group"
else
    echo "Adding $MP3_USER to www-data group"
    usermod -aG www-data "$MP3_USER"
fi
chown -R www-data:www-data "$MP3_DIR"
chmod -R 2775 "$MP3_DIR"
echo "MP3 directory permissions set with setgid. $MP3_USER can now access /mp3."

# STEP 6. Set ownership & permissions on custom files
echo_step "6. Setting ownership & permissions"
chown -R www-data:www-data "$TARGET_DIR"
find "$TARGET_DIR" -type f -name "*.php" -exec chmod 644 {} \;
find "$TARGET_DIR" -type f -name "*.inc" -exec chmod 644 {} \;

# STEP 7. Create Announcements dir + permissions
echo_step "7. Creating Announcements dir + permissions"
mkdir -p "$ANNOUNCE_DIR"
chown -R www-data:www-data "$ANNOUNCE_DIR"
chmod -R 2775 "$ANNOUNCE_DIR"

# STEP 8. Install prerequisite scripts in /etc/asterisk/local/ (if missing)
echo_step "8. Installing prerequisite scripts in $LOCAL_DIR"
mkdir -p "$LOCAL_DIR"
chown asterisk:asterisk "$LOCAL_DIR" 2>/dev/null || chown root:root "$LOCAL_DIR"
chmod 755 "$LOCAL_DIR"

# ----- playaudio.sh -----
PLAY_SCRIPT="$LOCAL_DIR/playaudio.sh"
if [[ ! -f "$PLAY_SCRIPT" ]]; then
    echo "Creating $PLAY_SCRIPT (missing)"
    cat > "$PLAY_SCRIPT" << EOF
#!/bin/bash
#
# playaudio.sh – Play an audio file over an AllStarLink v3 node (Debian 12)
NODE="$NODE_NUMBER"
if [ "\$EUID" -ne 0 ]; then
    echo "This script must be run with sudo or as root."
    exit 1
fi
if [ -z "\$1" ]; then
    echo "Usage: \$0 <audio_file_without_extension>"
    exit 1
fi
/usr/sbin/asterisk -rx "rpt localplay \${NODE} \$1"
EOF
    chmod +x "$PLAY_SCRIPT"
    chown asterisk:asterisk "$PLAY_SCRIPT" 2>/dev/null || chown root:root "$PLAY_SCRIPT"
    chmod 755 "$PLAY_SCRIPT"
    echo "Created $PLAY_SCRIPT with node number: $NODE_NUMBER"
else
    echo "$PLAY_SCRIPT already exists – skipping"
fi

# ----- audio_convert.sh -----
CONVERT_SCRIPT="$LOCAL_DIR/audio_convert.sh"
if [[ ! -f "$CONVERT_SCRIPT" ]]; then
    echo "Creating $CONVERT_SCRIPT (missing)"
    cat > "$CONVERT_SCRIPT" << 'EOF'
#!/bin/bash
#
# audio_convert.sh - Convert audio file to ulaw .ul
#
# Usage: audio_convert.sh input_file [output_file.ul]
#
# If output_file is not specified, it will be named the same as input_file but with .ul extension
# Requires sox (install with apt install sox libsox-fmt-mp3)
if [ $# -lt 1 ]; then
    echo "Usage: \$0 [input_file] [output_file.ul]"
    exit 1
fi
INPUT_FILE="$1"
OUTPUT_FILE="${2:-${INPUT_FILE%.*}.ul}"
sox "$INPUT_FILE" -t raw -r 8000 -c 1 -e u-law "$OUTPUT_FILE"
if [ $? -eq 0 ]; then
    echo "Conversion successful!"
    echo "Output file: $OUTPUT_FILE"
else
    echo "Error: Conversion failed."
fi
EOF
    chmod +x "$CONVERT_SCRIPT"
    chown asterisk:asterisk "$CONVERT_SCRIPT" 2>/dev/null || chown root:root "$CONVERT_SCRIPT"
    chmod 755 "$CONVERT_SCRIPT"
    echo "Created $CONVERT_SCRIPT"
else
    echo "$CONVERT_SCRIPT already exists – skipping"
fi

chmod +x "$PLAY_SCRIPT" "$CONVERT_SCRIPT" 2>/dev/null || true
echo "Verified: Both scripts are executable."

# STEP 9. Backup old link.php and install new link.php from repo
echo_step "9. Installing new link.php from repository (backup created)"
if [[ -f "$LINK_PHP" ]]; then
    cp "$LINK_PHP" "${LINK_PHP}.bak"
    echo "Backup created: $LINK_PHP.bak"
fi
sudo wget -O "$LINK_PHP" \
  https://raw.githubusercontent.com/n5ad/announcement-manager/main/link.php || error "Failed to download new link.php"
chown www-data:www-data "$LINK_PHP"
chmod 644 "$LINK_PHP"
echo "New link.php installed successfully."

# STEP 10. Create sudoers rule for www-data
echo_step "10. Creating sudoers rule for www-data (/etc/sudoers.d/99-supermon-announcements)"
SUDOERS_FILE="/etc/sudoers.d/99-supermon-announcements"
if [[ -f "$SUDOERS_FILE" ]]; then
    echo "$SUDOERS_FILE already exists – skipping"
else
    cat > "$SUDOERS_FILE" << 'EOF'
# /etc/sudoers.d/99-supermon-announcements
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/playaudio.sh
www-data ALL=(root) NOPASSWD: /usr/bin/crontab
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/audio_convert.sh
www-data ALL=(ALL) NOPASSWD: /bin/cp, /bin/chown, /bin/chmod
www-data ALL=(root) NOPASSWD: /usr/local/bin/piper_prompt_tts.sh
www-data ALL=(root) NOPASSWD: /bin/rm /usr/local/share/asterisk/sounds/announcements/*.ul
EOF
    chmod 0440 "$SUDOERS_FILE"
    chown root:root "$SUDOERS_FILE"
    echo "Sudoers file created successfully."
fi

# STEP 11. Install Piper TTS 1.2.0 ARM64
echo_step "11. Installing Piper TTS 1.2.0 ARM64"
if [[ -f "/opt/piper/bin/piper" ]]; then
    echo "Piper already installed – skipping"
else
    sudo wget https://github.com/rhasspy/piper/releases/download/v1.2.0/piper_arm64.tar.gz -O /tmp/piper.tar.gz
    sudo mkdir -p /opt/piper/bin
    sudo tar -xzf /tmp/piper.tar.gz -C /opt/piper/bin
    sudo chmod +x /opt/piper/bin/piper
    sudo mkdir -p /opt/piper/voices
    cd /opt/piper/voices
    sudo wget -4 https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
    sudo wget -4 https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json
    sudo chown www-data:www-data *.onnx *.onnx.json
    sudo chmod 644 *.onnx *.onnx.json
    rm /tmp/piper.tar.gz
    echo "Piper installed successfully."
fi

# STEP 12. Download piper_generate.php and piper_prompt_tts.sh
echo_step "12. Downloading piper_prompt_tts.sh"


if [[ -f "/usr/local/bin/piper_prompt_tts.sh" ]]; then
    echo "/usr/local/bin/piper_prompt_tts.sh already exists – skipping"
else
    sudo wget -O /usr/local/bin/piper_prompt_tts.sh \
      https://raw.githubusercontent.com/n5ad/announcement-manager/main/piper_prompt_tts.sh
    sudo chown root:root /usr/local/bin/piper_prompt_tts.sh
    sudo chmod +x /usr/local/bin/piper_prompt_tts.sh
    echo "piper_prompt_tts.sh downloaded and made executable."
fi

# STEP 13. Test Piper installation
echo_step "13. Testing Piper installation"
/opt/piper/bin/piper/piper --version
echo "This is a test of Piper TTS on node $(hostname)" | \
/opt/piper/bin/piper/piper --model /opt/piper/voices/en_US-lessac-medium.onnx --output_file /mp3/piper_test.wav
ls -l /mp3/piper_test.wav

# STEP 14. Final verification
echo_step "14. Setup complete – verification"
echo "I hope you get a lot of use from this"
echo "Log into Supermon → Announcements Manager should now appear at the bottom."
echo "73 — N5AD"
