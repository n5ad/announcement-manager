#!/usr/bin/env bash
#
# install-supermon-announcements-full.sh
# COMPLETE installer for N5AD's Supermon 7.4+ Announcements Manager
# Includes sudoers file from GitHub, delete_file.php, link.php fix, etc.
#
# Run as root: sudo bash install-supermon-announcements-full.sh
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
echo_step "1. Cloning GitHub repo"
rm -rf "$TEMP_CLONE"
git clone --depth 1 "$REPO_URL" "$TEMP_CLONE" || error "Git clone failed"

# 2. Create directories
echo_step "2. Creating required directories"
mkdir -p "$CUSTOM_DIR" "$MP3_DIR" "$SOUNDS_DIR"

# 3. Copy PHP files
echo_step "3. Installing PHP files"
cp -v "$TEMP_CLONE"/*.php "$TEMP_CLONE"/*.inc "$CUSTOM_DIR"/ 2>/dev/null || warn "No PHP/inc files copied"

# 4. Install sudoers file from repo
echo_step "4. Installing sudoers file from GitHub"
if [[ -f "$TEMP_CLONE/99-supermon-announcements" ]]; then
    sudo cp "$TEMP_CLONE/99-supermon-announcements" "$SUDOERS_DROPIN"
    sudo chmod 0440 "$SUDOERS_DROPIN"
    echo "sudoers file installed and permissions set (0440)"
else
    warn "99-supermon-announcements not found in repo - skipping sudoers install"
fi

# 5. Verify sudoers syntax
echo_step "5. Verifying sudoers syntax"
if visudo -c >/dev/null 2>&1; then
    echo "sudoers syntax check passed"
else
    error "sudoers syntax FAILED! Check $SUDOERS_DROPIN manually."
fi

# 6. Create delete_file.php (if not in repo yet)
echo_step "6. Creating delete_file.php endpoint"
cat > "${CUSTOM_DIR}/delete_file.php" << 'EOF'
<?php
// delete_file.php - Delete MP3 or UL file

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

if (empty($_POST['type']) || empty($_POST['file'])) {
    echo "Missing parameters.";
    exit;
}

$type = $_POST['type']; // 'mp3' or 'ul'
$filename = basename($_POST['file']);

if ($type === 'mp3') {
    $file_path = "/mp3/" . $filename;
} elseif ($type === 'ul') {
    // Use base name only (strip .ul if present)
    $base = pathinfo($filename, PATHINFO_FILENAME);
    $file_path = "/usr/local/share/asterisk/sounds/" . $base . ".ul";
} else {
    echo "Invalid type.";
    exit;
}

if (!file_exists($file_path)) {
    echo "File not found: $filename";
    exit;
}

if (unlink($file_path)) {
    echo "Deleted $filename successfully.";
} else {
    echo "Failed to delete $filename. Check permissions.";
}
?>
EOF

# 7. Permissions & ownership
echo_step "7. Setting ownership & permissions"
chown -R ${APACHE_USER}:${APACHE_USER} "$CUSTOM_DIR" "$MP3_DIR"
chmod -R 755 "$CUSTOM_DIR" "$MP3_DIR"
chown -R ${APACHE_USER}:${APACHE_USER} "$MP3_DIR"
chmod -R 775 "$MP3_DIR"   # allow delete

chown -R ${ASTERISK_USER}:${ASTERISK_GROUP} "$SOUNDS_DIR"
chmod -R 775 "$SOUNDS_DIR"   # allow delete

# Existing scripts
chmod 755 "$PLAY_SCRIPT" 2>/dev/null && chown root:root "$PLAY_SCRIPT" 2>/dev/null || warn "playaudio.sh not found"
chmod 755 "$CONVERT_SCRIPT" 2>/dev/null && chown root:root "$CONVERT_SCRIPT" 2>/dev/null || warn "audio_convert.sh not found"

# 8. Fix link.php ending
echo_step "8. Fixing link.php ending"
if [[ -f "$LINKPHP_PATH" ]]; then
    cp "$LINKPHP_PATH" "$LINKPHP_BACKUP"
    echo "Backup created: $LINKPHP_BACKUP"

    sed -i "/include_once \"footer.inc\";/,\$d" "$LINKPHP_PATH"
    echo "$DESIRED_ENDING" >> "$LINKPHP_PATH"
    echo "link.php updated"
else
    warn "link.php not found – skipping fix"
fi

# 9. Reload services
echo_step "9. Reloading services"
systemctl reload apache2 2>/dev/null || warn "Apache reload failed"
asterisk -rx "core reload" 2>/dev/null || warn "Asterisk reload failed"

# 10. Done
echo_step "10. Installation complete!"
echo "Verification commands:"
echo "  sudo -u www-data sudo ${PLAY_SCRIPT} netreminder"
echo "  sudo -u www-data sudo crontab -l"
echo "  ls -ld ${CUSTOM_DIR} ${MP3_DIR} ${SOUNDS_DIR}"
echo "  tail -n 5 ${LINKPHP_PATH}"
echo ""
echo "Log into Supermon → Announcements Manager should appear."
echo "Test Delete MP3 / Delete UL buttons."
echo "73 — N5AD"
