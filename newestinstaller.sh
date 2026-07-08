#!/usr/bin/env bash
#
# 
# - Fully automates Announcements Manager setup for Allmon3 and Supermon+
# - Installs required packages: sox + libsox-fmt-mp3 (for MP3 support) + git + perl
# - Copies files from GitHub to /var/www/html/announcement-manager/
# - Installs prerequisite scripts: playaudio.sh, playglobal.sh, polite_global, polite_play  & audio_convert.sh in /etc/asterisk/local/ (exact from KD5FMU repos and two custom by n5ad)
# - Creates /mp3 directory with correct permissions (2775, setgid)
# - Automatically grants access to the invoking user
# - Sets ownership & permissions on files
# - Removes old Announcement Manager installation if it exists
# - Creates /etc/sudoers.d/99-supermon-announcements for www-data (passwordless access to required commands)
# - Installs Piper TTS 1.2.0 ARM64 (binary + libs in /opt/piper/bin/, voices in /opt/piper/voices/)
# - Downloads piper_generate.php and piper_prompt_tts.sh and makes them executable
# - Safe & idempotent (can run multiple times)
#
# Run as root: sudo bash announcement_manager.sh
# Author: N5AD - Januaary 2026 , July 2026 (updated)
set -euo pipefail

# CONFIG

REPO_URL="https://github.com/n5ad/announcement-manager.git"
TEMP_CLONE="/tmp/announcement-manager"
TARGET_DIR="/var/www/html/announcement-manager"
MP3_DIR="/mp3"
LOCAL_DIR="/etc/asterisk/local"
ANNOUNCE_DIR="/usr/local/share/asterisk/sounds/announcements"
OLD_DIR="/var/www/html/supermon/custom"
#
# Helpers
# 
echo_step() { echo -e "\n\033[1;34m==>\033[0m $1"; }
warn() { echo -e "\033[1;33mWARNING:\033[0m $1" >&2; }
error() { echo -e "\033[1;31mERROR:\033[0m $1" >&2; exit 1; }
check_root() { [[ $EUID -eq 0 ]] || error "Run as root (sudo)."; }
# 
# STEP 1. Install required packages FIRST   force git install
# 
check_root
echo_step "1. Installing required packages (sox, libsox-fmt-mp3, git, perl)"
# sudo apt update && sudo apt upgrade -y || error "apt update failed. Check internet or sources.list."
apt install -y git || error "Failed to install git. Check internet/apt sources."
apt install -y sox libsox-fmt-mp3 perl || error "Failed to install other packages."
echo "All required packages (sox, libsox-fmt-mp3, git, perl) installed or already present."
if ! command -v git >/dev/null 2>&1; then
    error "git is still not installed after apt. Check your internet or apt sources."
fi
 
echo ""
echo " Announcements Manager - Full Setup"
echo "GitHub Repo: $REPO_URL"
echo "Target dir: $TARGET_DIR"
echo "MP3 dir: $MP3_DIR"
echo "Local scripts dir: $LOCAL_DIR"
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

#!/bin/bash
#

# Standalone tester for the Allmon3 iframe insertion logic.
# Prompts for a node number, backs up allmon3.ini, then applies the same
# logic used in announcement_manager.sh.
#


INI_FILE="/etc/allmon3/allmon3.ini"





echo "Using node number: $NODE_NUMBER"

# --- Backup before touching the real file ---
BACKUP_FILE="${INI_FILE}.bak.$(date +%Y%m%d-%H%M%S)"
cp -v "$INI_FILE" "$BACKUP_FILE"
echo "Backup created: $BACKUP_FILE"
echo "(Restore anytime with: cp \"$BACKUP_FILE\" \"$INI_FILE\")"
echo ""

# --- Check if already configured (scoped to this node's section only) ---
ALREADY_CONFIGURED=$(awk -v node="$NODE_NUMBER" '
    BEGIN { in_section=0; found=0 }
    {
        if ($0 ~ "^\\[") {
            if ($0 ~ "^\\[" node "\\]$") {
                in_section = 1
            } else {
                in_section = 0
            }
        } else if (in_section && $0 ~ "^iframepost") {
            found = 1
        }
    }
    END { print found }
' "$INI_FILE")

if [ "$ALREADY_CONFIGURED" = "1" ]; then
    echo "Iframe entries already exist for node $NODE_NUMBER → skipping"
else
    echo "Adding iframe lines under [${NODE_NUMBER}] ..."

    awk -v node="$NODE_NUMBER" '
    {
        if (in_section && $0 ~ "^\\[") {
            print "iframepost=/announcement-manager/allmon-announcement-frame.php"
            print "iframepre="
            inserted = 1
            in_section = 0
        }

        print $0

        if ($0 ~ "^\\[" node "\\]$") {
            in_section = 1
        }
    }
    END {
        if (in_section && !inserted) {
            print "iframepost=/announcement-manager/allmon-announcement-frame.php"
            print "iframepre="
            inserted = 1
        }
        if (!inserted) {
            print ""
            print "[" node "]"
            print "iframepost=/announcement-manager/allmon-announcement-frame.php"
            print "iframepre="
        }
    }
    ' "$INI_FILE" > "$INI_FILE.tmp" && mv "$INI_FILE.tmp" "$INI_FILE"

    echo "Successfully added iframe entries for node $NODE_NUMBER"
fi

echo ""
echo "=== Resulting $INI_FILE ==="
cat "$INI_FILE"
echo ""
echo "=== Diff vs backup ==="
diff -u "$BACKUP_FILE" "$INI_FILE" || true
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
sudo rm -rf "$OLD_DIR"

# STEP 8. Install prerequisite scripts in /etc/asterisk/local/ (if missing)
echo_step "8. Installing prerequisite scripts in $LOCAL_DIR"
mkdir -p "$LOCAL_DIR"
chown asterisk:asterisk "$LOCAL_DIR" 2>/dev/null || chown root:root "$LOCAL_DIR"
chmod 755 "$LOCAL_DIR"
# ----- playglobal.sh -----
GLOBAL_SCRIPT="$LOCAL_DIR/playglobal.sh"
if [[ ! -f "$GLOBAL_SCRIPT" ]]; then
    echo "Creating $GLOBAL_SCRIPT (missing)"
    cat > "$GLOBAL_SCRIPT" << EOF
#!/bin/bash
#
# playglobal.sh   Play an audio file over an AllStarLink v3 node (Debian 12)
NODE="$NODE_NUMBER"
if [ "\$EUID" -ne 0 ]; then
    echo "This script must be run with sudo or as root."
    exit 1
fi
if [ -z "\$1" ]; then
    echo "Usage: \$0 <audio_file_without_extension>"
    exit 1
fi
/usr/sbin/asterisk -rx "rpt playback \${NODE} \$1"
EOF
    chmod +x "$GLOBAL_SCRIPT"
    chown asterisk:asterisk "$GLOBAL_SCRIPT" 2>/dev/null || chown root:root "$GLOBAL_SCRIPT"
    chmod 755 "$GLOBAL_SCRIPT"
    echo "Created $GLOBAL_SCRIPT with node number: $NODE_NUMBER"
else
    echo "$GLOBAL_SCRIPT already exists →skipping"
fi
# ----- polite_global.sh -----
POLITE_GLOBAL_SCRIPT="$LOCAL_DIR/polite_global.sh"
if [[ ! -f "$POLITE_GLOBAL_SCRIPT" ]]; then
    echo "Creating $POLITE_GLOBAL_SCRIPT (missing)"
    cat > "$POLITE_GLOBAL_SCRIPT" << EOF
#!/bin/bash
FILE=\$1
NODE="$NODE_NUMBER"
MAX_WAIT=300
CHECK_INTERVAL=1
TAIL_DELAY=2
is_busy() {
    RESULT=\$(asterisk -rx "rpt show variables \$NODE" 2>/dev/null | grep "RPT_RXKEYED" | awk -F= '{print \$2}' | tr -d ' ')
    [ "\$RESULT" = "1" ]
}
WAITED=0
while true; do
    if is_busy; then
        sleep \$CHECK_INTERVAL
        WAITED=\$((WAITED + CHECK_INTERVAL))
        if [ "\$WAITED" -ge "\$MAX_WAIT" ]; then
            break
        fi
    else
        sleep \$TAIL_DELAY
        if is_busy; then
            continue
        fi
        break
    fi
done
/usr/sbin/asterisk -rx "rpt playback \${NODE} \$FILE"
EOF
    chmod +x "$POLITE_GLOBAL_SCRIPT"
    chown asterisk:asterisk "$POLITE_GLOBAL_SCRIPT" 2>/dev/null || chown root:root "$POLITE_GLOBAL_SCRIPT"
    chmod 755 "$POLITE_GLOBAL_SCRIPT"
    echo "Created $POLITE_GLOBAL_SCRIPT"
else
    echo "$POLITE_GLOBAL_SCRIPT already exists  →skipping"
fi
# ----- playaudio.sh -----
PLAY_SCRIPT="$LOCAL_DIR/playaudio.sh"
if [[ ! -f "$PLAY_SCRIPT" ]]; then
    echo "Creating $PLAY_SCRIPT (missing)"
    cat > "$PLAY_SCRIPT" << EOF
#!/bin/bash
#
# playaudio.sh   Play an audio file over an AllStarLink v3 node (Debian 12)
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
    echo "$PLAY_SCRIPT already exists →skipping"
fi
# ----- polite_play.sh -----
POLITE_PLAY_SCRIPT="$LOCAL_DIR/polite_play.sh"
if [[ ! -f "$POLITE_PLAY_SCRIPT" ]]; then
    echo "Creating $POLITE_PLAY_SCRIPT (missing)"
    cat > "$POLITE_PLAY_SCRIPT" << EOF
#!/bin/bash
FILE=\$1
NODE="$NODE_NUMBER"
MAX_WAIT=300
CHECK_INTERVAL=1
TAIL_DELAY=2
is_busy() {
    RESULT=\$(asterisk -rx "rpt show variables \$NODE" 2>/dev/null | grep "RPT_RXKEYED" | awk -F= '{print \$2}' | tr -d ' ')
    [ "\$RESULT" = "1" ]
}
WAITED=0
while true; do
    if is_busy; then
        sleep \$CHECK_INTERVAL
        WAITED=\$((WAITED + CHECK_INTERVAL))
        if [ "\$WAITED" -ge "\$MAX_WAIT" ]; then
            break
        fi
    else
        sleep \$TAIL_DELAY
        if is_busy; then
            continue
        fi
        break
    fi
done
/usr/sbin/asterisk -rx "rpt localplay \${NODE} \$FILE"
EOF
    chmod +x "$POLITE_PLAY_SCRIPT"
    chown asterisk:asterisk "$POLITE_PLAY_SCRIPT" 2>/dev/null || chown root:root "$POLITE_PLAY_SCRIPT"
    chmod 755 "$POLITE_PLAY_SCRIPT"
    echo "Created $POLITE_PLAY_SCRIPT"
else
    echo "$POLITE_PLAY_SCRIPT already exists  →skipping"
fi
# ----- audio_convert.sh -----
CONVERT_SCRIPT="$LOCAL_DIR/audio_convert.sh"
if [[ ! -f "$CONVERT_SCRIPT" ]]; then
    echo "Creating $CONVERT_SCRIPT (missing)"
else
    echo "Updating $CONVERT_SCRIPT with latest version"
fi

cat > "$CONVERT_SCRIPT" << 'EOF'
#!/bin/bash
#
# audio_convert.sh - Convert audio file to ulaw .ul with optional leading pause
#
# Usage: audio_convert.sh input_file [output_file.ul] [pause_seconds]
#
# - If output_file is not specified, it will be named like input_file but with .ul extension
# - pause_seconds: optional number of seconds of silence to add at the start (default 0)
#
# Requires sox (apt install sox libsox-fmt-mp3)

if [ $# -lt 1 ]; then
    echo "Usage: $0 input_file [output_file.ul] [pause_seconds]"
    echo "Example: $0 announcement.mp3 announcement.ul 1.5"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="${2:-${INPUT_FILE%.*}.ul}"
PAUSE_SECONDS="${3:-0}"

# Validate pause is a number (including decimals)
if ! [[ "$PAUSE_SECONDS" =~ ^[0-9]*\.?[0-9]+$ ]]; then
    echo "Error: pause_seconds must be a number (e.g. 1, 0.5, 2.3)"
    exit 1
fi

# If pause > 0, create a temporary silence file and concatenate
if (( $(awk 'BEGIN {print ('"$PAUSE_SECONDS"' > 0)}') )); then
    TEMP_SILENCE=$(mktemp --suffix=.wav)
   
    # Create silence at correct format
    sox -n -r 8000 -c 1 -e u-law "$TEMP_SILENCE" trim 0 "$PAUSE_SECONDS"
   
    # Concatenate silence + original audio, then convert to ulaw
    sox "$TEMP_SILENCE" "$INPUT_FILE" -t raw -r 8000 -c 1 -e u-law "$OUTPUT_FILE"
   
    rm -f "$TEMP_SILENCE"
else
    # No pause — original behavior
    sox "$INPUT_FILE" -t raw -r 8000 -c 1 -e u-law "$OUTPUT_FILE"
fi

if [ $? -eq 0 ]; then
    echo "Conversion successful!"
    echo "Output file: $OUTPUT_FILE"
    if (( $(awk 'BEGIN {print ('"$PAUSE_SECONDS"' > 0)}') )); then
        echo "Added ${PAUSE_SECONDS} second pause at the beginning."
    fi
else
    echo "Error: Conversion failed."
fi
EOF

chmod +x "$CONVERT_SCRIPT"
chown asterisk:asterisk "$CONVERT_SCRIPT" 2>/dev/null || chown root:root "$CONVERT_SCRIPT"
chmod 755 "$CONVERT_SCRIPT"
echo "audio_convert.sh installed/updated successfully."
# STEP 9. Safely patch link.php to include announcement manager (instead of full overwrite)
echo_step "9. Updating footer.inc and restoring link.php (Announcement Manager integration)"

# Define paths
LINK_PHP="/var/www/html/supermon/link.php"
FOOTER_INC="/var/www/html/supermon/footer.inc"
CSS_FILE="/var/www/html/supermon/supermon.css"
BACKUP_SUFFIX=".bak.$(date +%Y%m%d-%H%M%S)"

# === 1. Restore / Fix link.php to desired ending ===
echo_step "10a. Restoring link.php - removing announcement if previously installed"


if [ -f "$LINK_PHP" ]; then
    BACKUP_LINK="${LINK_PHP}${BACKUP_SUFFIX}"
    cp -v "$LINK_PHP" "$BACKUP_LINK"
    echo "Backup of link.php created: $BACKUP_LINK"

    # ONLY remove the announcement include line - nothing else
    if grep -q 'include_once.*custom/announcement\.inc' "$LINK_PHP"; then
        sed -i '/include_once.*custom\/announcement\.inc/d' "$LINK_PHP"
        echo "Removed include_once \"custom/announcement.inc\"; from link.php"
    else
        echo "Announcement include not found in link.php - no change needed"
    fi

    # Fix permissions
    chown www-data:www-data "$LINK_PHP" 2>/dev/null || true
    chmod 644 "$LINK_PHP" 2>/dev/null || true
else
    echo " → link.php not found →skipping"
fi

echo_step "10b. Adding announcement  to footer.inc"

FOOTER_INC="/var/www/html/supermon/footer.inc"
BACKUP_SUFFIX=".bak.$(date +%Y%m%d-%H%M%S)"

if [ ! -f "$FOOTER_INC" ]; then
    echo " → footer.inc not found →skipping"
else
    BACKUP_FOOTER="${FOOTER_INC}${BACKUP_SUFFIX}"
    cp -v "$FOOTER_INC" "$BACKUP_FOOTER"
    echo "Backup of footer.inc created: $BACKUP_FOOTER"

    if grep -q 'include_once.*/announcement-manager/announcement\.inc' "$FOOTER_INC"; then
        echo "Announcement include already present — →skipping"
    else
        echo "Patching footer.inc..."

        awk '
        # Look for the start of the if block
        /if \(\$_SESSION\['"'"'sm61loggedin'"'"'\] === true\) \{/ {
            print
            inblock = 1
            next
        }
        # When we find the closing ?> while inside the block, insert after it
        inblock && /^\s*\?>\s*$/ {
            print
            print "<?php include_once \"custom/announcement.inc\"; ?> <br><br>"
            inblock = 0
            next
        }
        { print }
        ' "$FOOTER_INC" > "$FOOTER_INC.tmp" && mv "$FOOTER_INC.tmp" "$FOOTER_INC"

        echo "footer.inc patched correctly (include added after ?>)."
    fi

    chown www-data:www-data "$FOOTER_INC" 2>/dev/null || true
    chmod 644 "$FOOTER_INC" 2>/dev/null || true
fi

echo_step "10c. Appending footer CSS to supermon.css"

CSS_FILE="/var/www/html/supermon/supermon.css"
BACKUP_SUFFIX=".bak.$(date +%Y%m%d-%H%M%S)"

if [ -f "$CSS_FILE" ]; then
    BACKUP_CSS="${CSS_FILE}${BACKUP_SUFFIX}"
    cp -v "$CSS_FILE" "$BACKUP_CSS"
    echo "Backup of supermon.css created: $BACKUP_CSS"

    # Check to see if previously updated
    if grep -q "ANNOUNCEMENT_MANAGER" "$CSS_FILE"; then
        echo "CSS already contains Announcement Manager footer styles — →skipping"
    else
        echo "Appending Announcement Manager footer styles..."

        cat << 'EOF' >> "$CSS_FILE"

#footer {
    /* ANNOUNCEMENT_MANAGER */

    max-width: 75%;
    margin: 0 auto;
    padding: 0 15px;
}
EOF

        echo "supermon.css updated with footer styles."
    fi

    chown www-data:www-data "$CSS_FILE" 2>/dev/null || true
    chmod 644 "$CSS_FILE" 2>/dev/null || true
else
    echo " → $CSS_FILE not found →skipping CSS update"
fi

echo "Section 10 completed successfully."
echo_step "10.1. Applying your preferred IPv4 LAN detection"
if [ ! -f "$LINK_PHP" ]; then
    echo " â†’ link.php missing â†’ →skipping 10.1"
else
   
    sed -i '/if (empty(\$WANONLY)) {/,/}/d' "$LINK_PHP"
    
    
    cat > /tmp/ip-block.txt << 'EOF'
if (empty($WANONLY)) {
   $myip = exec("$WGET -t 1 -T 3 -q -O- http://checkip.dyndns.org:8245 |$CUT -d':' -f2 |$CUT -d' ' -f2 |$CUT -d'<' -f1");
   $WL=""; $mylanip = exec("ip -4 addr show scope global | awk '/inet/ {print $2}' | cut -d/ -f1 | head -1");
EOF
    
    # Append it right after the $mgrport line (anchor point)
    sed -i "/\$mgrport = exec.*port = /r /tmp/ip-block.txt" "$LINK_PHP"
    
    # Clean up temp file
    rm -f /tmp/ip-block.txt
    
    # Optional: nicer label in printed line
    sed -i 's|\[ \$hostname \] \[ WAN:.*\] \[ .*LAN: \${mylanip} \]|[ $hostname ] [ WAN: ${myip} ] [ LAN: ${mylanip} ]|g' "$LINK_PHP"
    
    chown www-data:www-data "$LINK_PHP" 2>/dev/null || true
    chmod 644 "$LINK_PHP" 2>/dev/null || true
    
    echo "Preferred IPv4 LAN block inserted successfully."
fi

# STEP 12. Create sudoers rule for www-data
echo_step "12. Creating sudoers rule for www-data (/etc/sudoers.d/99-supermon-announcements)"
SUDOERS_FILE="/etc/sudoers.d/99-supermon-announcements"
# if [[ -f "$SUDOERS_FILE" ]]; then
#    echo "$SUDOERS_FILE already exists   →skipping"
# else
    cat > "$SUDOERS_FILE" << 'EOF'
# /etc/sudoers.d/99-supermon-announcements
# this file is managed by announcement_manager.sh do not edit manually
# if you there are updates released for this feature, just re-run the install file
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/playaudio.sh
www-data ALL=(root) NOPASSWD: /usr/bin/crontab
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/audio_convert.sh
www-data ALL=(ALL) NOPASSWD: /bin/cp, /bin/chown, /bin/chmod
www-data ALL=(root) NOPASSWD: /usr/local/bin/piper_prompt_tts.sh
www-data ALL=(root) NOPASSWD: /bin/rm /usr/local/share/asterisk/sounds/announcements/*.ul
www-data ALL=(ALL) NOPASSWD: /etc/asterisk/local/playglobal.sh
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/polite_play.sh
www-data ALL=(root) NOPASSWD: /etc/asterisk/local/polite_global.sh
EOF
    chmod 0440 "$SUDOERS_FILE"
    chown root:root "$SUDOERS_FILE"
    echo "Sudoers file created successfully."
# fi
# STEP 13. Install Piper TTS 1.2.0 (amd64 or arm64)
echo_step "13. Installing Piper TTS 1.2.0"
# Detect architecture
ARCH=$(uname -m)
if [[ "$ARCH" == "x86_64" || "$ARCH" == "amd64" ]]; then
    PIPER_FILE="piper_amd64.tar.gz"
    echo "Detected x86_64 architecture   using amd64 binary"
elif [[ "$ARCH" == "aarch64" || "$ARCH" == "arm64" ]]; then
    PIPER_FILE="piper_arm64.tar.gz"
    echo "Detected aarch64/arm64 architecture   using arm64 binary"
else
    echo "Error: Unsupported architecture: $ARCH"
    exit 1
fi
# Piper binary path (note the nested piper/piper)
PIPER_BIN="/opt/piper/bin/piper/piper"
# Install Piper binary only if missing
if [[ -f "$PIPER_BIN" && -x "$PIPER_BIN" ]]; then
    echo "Piper binary already installed at $PIPER_BIN   →skipping download"
else
    echo "Downloading and installing Piper binary..."
    sudo wget https://github.com/rhasspy/piper/releases/download/v1.2.0/$PIPER_FILE -O /tmp/piper.tar.gz
    sudo mkdir -p /opt/piper/bin
    sudo tar -xzf /tmp/piper.tar.gz -C /opt/piper/bin
    sudo chmod +x "$PIPER_BIN"
    rm -f /tmp/piper.tar.gz
    echo "Piper binary installed at $PIPER_BIN"
fi
# Create voices directory
sudo mkdir -p /opt/piper/voices
cd /opt/piper/voices
# Function to download a voice if missing (same as before)
download_voice() {
    local onnx_file="$1"
    local json_file="${onnx_file}.json"
    local base_url="https://huggingface.co/rhasspy/piper-voices/resolve/main"
    if [[ -f "$onnx_file" && -f "$json_file" ]]; then
        echo "Voice $onnx_file already exists   →skipping"
    else
        echo "Downloading voice: $onnx_file"
        sudo wget -4 "$base_url/$2" -O "$onnx_file"
        sudo wget -4 "$base_url/$3" -O "$json_file"
    fi
}
# Download voices only if missing
download_voice "en_US-lessac-medium.onnx"     "en/en_US/lessac/medium/en_US-lessac-medium.onnx"     "en/en_US/lessac/medium/en_US-lessac-medium.onnx.json"
download_voice "en_US-joe-medium.onnx"        "en/en_US/joe/medium/en_US-joe-medium.onnx"          "en/en_US/joe/medium/en_US-joe-medium.onnx.json"
download_voice "en_US-amy-medium.onnx"        "en/en_US/amy/medium/en_US-amy-medium.onnx"          "en/en_US/amy/medium/en_US-amy-medium.onnx.json"
download_voice "en_US-kristin-medium.onnx"    "en/en_US/kristin/medium/en_US-kristin-medium.onnx"  "en/en_US/kristin/medium/en_US-kristin-medium.onnx.json"
download_voice "en_US-libritts_r-medium.onnx" "en/en_US/libritts_r/medium/en_US-libritts_r-medium.onnx" "en/en_US/libritts_r/medium/en_US-libritts_r-medium.onnx.json"
download_voice "en_US-ryan-low.onnx"          "en/en_US/ryan/low/en_US-ryan-low.onnx"              "en/en_US/ryan/low/en_US-ryan-low.onnx.json"
# Set permissions
sudo chown www-data:www-data *.onnx *.onnx.json 2>/dev/null || true
sudo chmod 644 *.onnx *.onnx.json 2>/dev/null || true
# === Set slower speaking rate for lessac (only if the file exists) ===
if [[ -f "/opt/piper/voices/en_US-lessac-medium.onnx.json" ]]; then
    echo "Setting Piper voice speed (length_scale = 1.2) for lessac-medium"
    JSON_FILE="/opt/piper/voices/en_US-lessac-medium.onnx.json"
    
    sudo cp -f "$JSON_FILE" "${JSON_FILE}.orig" 2>/dev/null || true
    sudo sed -i 's/"length_scale"[[:space:]]*:[[:space:]]*[0-9.]\+/"length_scale": 1.2/' "$JSON_FILE"
    
    if grep -q '"length_scale": 1.2' "$JSON_FILE"; then
        echo "Successfully set length_scale to 1.2"
    else
        echo "Warning: length_scale was not changed"
    fi
fi
echo "Piper TTS setup completed."
# STEP 14. Download piper_generate.php and piper_prompt_tts.sh
echo_step "14. Downloading piper_prompt_tts.sh"
if [[ -f "/usr/local/bin/piper_prompt_tts.sh" ]]; then
    echo "/usr/local/bin/piper_prompt_tts.sh already exists   →skipping"
else
    sudo wget -O /usr/local/bin/piper_prompt_tts.sh https://raw.githubusercontent.com/n5ad/announcement-manager/main/piper_prompt_tts.sh
    sudo chown root:root /usr/local/bin/piper_prompt_tts.sh
    sudo chmod +x /usr/local/bin/piper_prompt_tts.sh
    echo "piper_prompt_tts.sh downloaded and made executable."
fi
# STEP 15. Test Piper installation
echo_step "15. Testing Piper installation"
/opt/piper/bin/piper/piper --version
echo "This is a test of Piper TTS on node $(hostname)" | \
/opt/piper/bin/piper/piper --model /opt/piper/voices/en_US-lessac-medium.onnx --output_file /mp3/piper_test.wav
ls -l /mp3/piper_test.wav

echo "I hope you get a lot of use from this"
echo "Log into Supermon or Allmon3 and Announcements Manager should now appear at the bottom."
echo "73  N5AD"
