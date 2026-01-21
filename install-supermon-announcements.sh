#!/usr/bin/env bash
#
# install-supermon-announcements.sh
# COMPLETE installer for N5AD's Supermon 7.4+ Announcements Manager
# Clones PHP files, sets permissions, configures sudoers, AND fixes link.php ending
#
# Run as root: sudo bash install-supermon-announcements.sh
#
# GitHub: https://github.com/n5ad/Supermon-7.4-announcement-creation-and-management-of-cron
# Author: N5AD - January 2026

set -euo pipefail

# ────────────────────────────────────────────────
# CONFIGURATION
# ────────────────────────────────────────────────

REPO_URL="https://github.com/n5ad/Supermon-7.4-announcement-creation-and-management-of-cron.git"
TEMP_CLONE="/tmp/supermon-announcements"

APACHE_USER="www-data"
ASTERISK_USER="asterisk"
ASTERISK_GROUP="asterisk"

SUPERMON_ROOT="/var/www/html/supermon"
CUSTOM_DIR="${SUPERMON_ROOT}/custom"
MP3_DIR="/mp3"
SOUNDS_DIR="/usr/local/share/asterisk/sounds"
LOCAL_DIR="/etc/asterisk/local"

LINKPHP_PATH="${SUPERMON_ROOT}/link.php"
LINKPHP_BACKUP="${LINKPHP_PATH}.bak.$(date +%Y%m%d-%H%M%S)"

SUDOERS_DROPIN="/etc/sudoers.d/99-supermon-announcements"

PLAY_SCRIPT="${LOCAL_DIR}/playaudio.sh"
CONVERT_SCRIPT="${LOCAL_DIR}/audio_convert.sh"

# Desired ending for link.php
DESIRED_ENDING='<?php
include_once "custom/announcement.inc";
echo "<br><br>"; // Two line breaks
include_once "footer.inc";
?>'

# ────────────────────────────────────────────────
# Helpers
# ────────────────────────────────────────────────

echo_step() { echo -e "\n\033[1;34m==>\033[0m $1"; }
warn() { echo -e "\033[1;33mWARNING:\033[0m $1" >&2; }
error() { echo -e "\033[1;31mERROR:\033[0m $1" >&2; exit 1; }

check_root() { [[ $EUID -eq 0 ]] || error "Run as root (sudo)."; }

# ────────────────────────────────────────────────
# Main
# ────────────────────────────────────────────────

check_root

echo ""
echo "COMPLETE Installer: Supermon 7.4+ Announcements Manager"
echo "────────────────────────────────────────────────────────"
echo "GitHub Repo: $REPO_URL"
echo "Assumes playaudio.sh & audio_convert.sh already installed"
echo ""

echo -n "Continue installation? (y/N) "
read -r answer
[[ "$answer" =~ ^[Yy]$ ]] || { echo "Aborted."; exit 0; }

# 1. Clone repo
echo_step "1. Cloning GitHub repo (PHP files)"
rm -rf "$TEMP_CLONE"
git clone --depth 1 "$REPO_URL" "$TEMP_CLONE" || error "Git clone failed"

# 2. Directories
echo_step "2. Creating directories"
mkdir -p "$CUSTOM_DIR" "$MP3_DIR" "$SOUNDS_DIR"

# 3. Copy PHP files
echo_step "3. Installing PHP files"
cp -v "$TEMP_CLONE"/*.php "$TEMP_CLONE"/*.inc "$CUSTOM_DIR"/ 2>/dev/null || warn "No PHP/inc files"

rm -rf "$TEMP_CLONE"

# 4. Permissions & ownership
echo_step "4. Setting ownership & permissions"
chown -R ${APACHE_USER}:${APACHE_USER} "$CUSTOM_DIR" "$MP3_DIR"
chmod -R 755 "$CUSTOM_DIR" "$MP3_DIR"

chown -R ${ASTERISK_USER}:${ASTERISK_GROUP} "$SOUNDS_DIR"
chmod -R 755 "$SOUNDS_DIR"

# Existing scripts
chmod 755 "$PLAY_SCRIPT" 2>/dev/null && chown root:root "$PLAY_SCRIPT" 2>/dev/null || warn "playaudio.sh not found"
chmod 755 "$CONVERT_SCRIPT" 2>/dev/null && chown root:root "$CONVERT_SCRIPT" 2>/dev/null || warn "audio_convert.sh not found"

# 5. Fix link.php ending
echo_step "5. Fixing link.php ending"
if [[ -f "$LINKPHP_PATH" ]]; then
    cp "$LINKPHP_PATH" "$LINKPHP_BACKUP"
    echo "Backup created: $LINKPHP_BACKUP"

    # Keep everything up to (but not including) the footer include
    sed -i "/include_once \"footer.inc\";/,\$d" "$LINKPHP_PATH"

    # Append desired ending
    echo "$DESIRED_ENDING" >> "$LINKPHP_PATH"
    echo "link.php updated – now ends with announcement include + breaks + footer"
else
    warn "link.php not found at $LINKPHP_PATH – skipping fix"
fi

# 6. Sudoers
echo_step "6. Configuring sudoers"
cat > "$SUDOERS_DROPIN" << EOF
# Supermon Announcements Manager - N5AD
${APACHE_USER} ALL=(root) NOPASSWD: ${PLAY_SCRIPT}
${APACHE_USER} ALL=(root) NOPASSWD: /usr/bin/crontab
${APACHE_USER} ALL=(root) NOPASSWD: ${CONVERT_SCRIPT}
EOF

chmod 0440 "$SUDOERS_DROPIN"
visudo -c >/dev/null 2>&1 && echo "sudoers OK" || error "sudoers syntax FAILED"

# 7. Reload
echo_step "7. Reloading services"
systemctl reload apache2 2>/dev/null || warn "Apache reload failed"
asterisk -rx "core reload" 2>/dev/null || warn "Asterisk reload failed"

# 8. Done
echo_step "8. Installation complete!"
echo "Verification commands:"
echo "  sudo -u www-data sudo ${PLAY_SCRIPT} netreminder"
echo "  sudo -u www-data sudo crontab -l"
echo "  ls -ld ${CUSTOM_DIR} ${MP3_DIR} ${SOUNDS_DIR}"
echo "  tail -n 5 ${LINKPHP_PATH}"
echo ""
echo "Log into Supermon → Announcements Manager should now appear."
echo "73 — N5AD"