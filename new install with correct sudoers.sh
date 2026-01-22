#!/usr/bin/env bash
#
# setup-supermon-announcements.sh
# Fully automates Supermon Announcements Manager setup:
# - Copies files from GitHub to /var/www/html/supermon/custom/
# - Creates /mp3 directory with correct permissions (2775, setgid)
# - Automatically grants access to the invoking user
# - Sets ownership & permissions on files
# - Adds correct sudoers rules (drop-in file)
# - Safe & idempotent (can run multiple times)
#
# Run as root: sudo bash setup-supermon-announcements.sh
# Author: N5AD - January 2026 (updated)

set -euo pipefail

# ────────────────────────────────────────────────
# CONFIG
# ────────────────────────────────────────────────

REPO_URL="https://github.com/n5ad/Supermon-7.4-announcement-creation-and-management-of-cron.git"
TEMP_CLONE="/tmp/supermon-announcements"
TARGET_DIR="/var/www/html/supermon/custom"
MP3_DIR="/mp3"
SUDOERS_DROPIN="/etc/sudoers.d/99-supermon-announcements"

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
echo "Supermon Announcements Manager - Full Setup"
echo "──────────────────────────────────────────────"
echo "GitHub Repo: $REPO_URL"
echo "Target dir:  $TARGET_DIR"
echo "MP3 dir:     $MP3_DIR"
echo ""

echo -n "Continue setup? (y/N) "
read -r answer
[[ "$answer" =~ ^[Yy]$ ]] || { echo "Aborted."; exit 0; }

# 1. Clone repo
echo_step "1. Cloning GitHub repo"
rm -rf "$TEMP_CLONE"
git clone --depth 1 "$REPO_URL" "$TEMP_CLONE" || error "Git clone failed"

# 2. Copy PHP & inc files
echo_step "2. Copying files to $TARGET_DIR"
mkdir -p "$TARGET_DIR"
cp -v "$TEMP_CLONE"/*.{php,inc} "$TARGET_DIR"/ 2>/dev/null || warn "No .php/.inc files found"
rm -rf "$TEMP_CLONE"

# 3. Create /mp3 dir + permissions
echo_step "3. Creating /mp3 directory"
mkdir -p "$MP3_DIR"

# Automatically detect the user who invoked sudo
MP3_USER="${SUDO_USER:-$(whoami)}"
echo "Granting /mp3 access to user: $MP3_USER"

# Add user to www-data group if not already a member
if id -nG "$MP3_USER" | grep -qw "www-data"; then
    echo "$MP3_USER is already in www-data group"
else
    echo "Adding $MP3_USER to www-data group"
    usermod -aG www-data "$MP3_USER"
fi

# Set ownership & permissions on /mp3 (setgid so new files inherit group)
chown -R www-data:www-data "$MP3_DIR"
chmod -R 2775 "$MP3_DIR"

echo "MP3 directory permissions set with setgid. $MP3_USER can now access /mp3."

# 4. Set ownership & permissions on custom files
echo_step "4. Setting ownership & permissions"
chown -R www-data:www-data "$TARGET_DIR"
find "$TARGET_DIR" -type f -name "*.php" -exec chmod 644 {} \;
find "$TARGET_DIR" -type f -name "*.inc" -exec chmod 644 {} \;

# 5. Add sudoers rules (drop-in file) safely
echo_step "5. Adding sudoers rules"

read -r -d '' SUDO_CONTENT << 'EOF'
# Supermon Announcements Manager - N5AD
# Passwordless sudo for www-data to run required commands
www-data ALL=(ALL) NOPASSWD: /usr/bin/crontab
www-data ALL=(ALL) NOPASSWD: /bin/cp, /bin/chown, /bin/chmod
www-data ALL=(ALL) NOPASSWD: /etc/asterisk/local/playaudio.sh
EOF

TMP_SUDOERS="$(mktemp)"
echo "$SUDO_CONTENT" > "$TMP_SUDOERS"
chmod 440 "$TMP_SUDOERS"

# Validate syntax and install
if visudo -c -f "$TMP_SUDOERS"; then
    echo "Sudoers syntax OK — installing to $SUDOERS_DROPIN"
    mv "$TMP_SUDOERS" "$SUDOERS_DROPIN"
else
    rm -f "$TMP_SUDOERS"
    error "Sudoers syntax check FAILED! Check content manually."
fi

# 6. Final verification
echo_step "6. Setup complete – verification"
echo "Run these to test:"
echo "  sudo -u www-data sudo /etc/asterisk/local/playaudio.sh netreminder"
echo "  sudo -u www-data sudo crontab -l"
echo "  ls -ld $TARGET_DIR $MP3_DIR"
echo ""
echo "Log into Supermon → Announcements Manager should appear."
echo "73 — N5AD"
