#!/usr/bin/env bash

#

# remove_announcement_manager.sh

# Completely removes the Supermon Announcements Manager + Piper TTS setup

# - Uninstalls git

# - Removes custom files, directories, and sudoers rules

# - Safe to run as root

# Author: N5AD - January 2026

set -euo pipefail


echo "WARNING: This will completely remove the Announcements Manager and Piper TTS setup!"

echo "This includes:"

echo "  - git package"

echo "  - /mp3 directory"

echo "  - /opt/piper directory"

echo "  - /etc/asterisk/local/playaudio.sh & audio_convert.sh"

echo "  - /usr/local/bin/piper_prompt_tts.sh"

echo "  - /var/www/html/supermon/custom/* files"

echo "  - /etc/sudoers.d/99-supermon-announcements"

echo ""

echo -n "Are you sure you want to continue? (y/N) "

read -r answer

[[ "$answer" =~ ^[Yy]$ ]] || { echo "Aborted."; exit 0; }


# 1. Uninstall git

echo "Removing git package..."

sudo apt remove -y git || true

sudo apt purge -y git || true

sudo apt autoremove -y || true


# 2. Remove custom scripts in /etc/asterisk/local

echo "Removing scripts in /etc/asterisk/local..."

sudo rm -f /etc/asterisk/local/announcemenet_manager.sh

sudo rm -f /etc/asterisk/local/playaudio.sh

sudo rm -f /etc/asterisk/local/audio_convert.sh


# 3. Remove sudoers rule

echo "Removing sudoers rule..."

sudo rm -f /etc/sudoers.d/99-supermon-announcements


# 4. Remove piper_prompt_tts.sh

echo "Removing piper_prompt_tts.sh..."

sudo rm -f /usr/local/bin/piper_prompt_tts.sh


# 5. Remove /mp3 directory

echo "Removing /mp3 directory..."


# 6. Remove /opt/piper directory

echo "Removing /opt/piper directory..."

sudo rm -rf /opt/piper


# 7. Remove all files in /var/www/html/supermon/custom/

echo "Removing files in /var/www/html/supermon/custom/..."

sudo rm -f /var/www/html/supermon/custom/*.php

sudo rm -f /var/www/html/supermon/custom/*.inc

 # 8. Restore index.html for allmon3
 echo "Restoring Allmon3 to original"
 sudo rm /usr/share/allmon3/index.html
 sudo mv /usr/share/allmon3/index.html.orig index.html
 
echo ""

echo "Cleanup complete!"

echo "All Announcements Manager and Piper TTS components have been removed."

echo "You can now run a fresh install if desired."

echo "73 â€” N5AD"
