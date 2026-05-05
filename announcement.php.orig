<?php
/**
 * announcement.php
 * Created by N5AD
 * Converts MP3 → u-law (.ul), copies to Asterisk sounds dir,
 * and installs a cron job (classic or nth-week style)
 * Updated to support local vs global playback scope
 */
$TMP_DIR = '/mp3';
$CONVERT_SCRIPT = '/etc/asterisk/local/audio_convert.sh';
$PLAY_SCRIPT_LOCAL  = '/etc/asterisk/local/playaudio.sh';   // local node only
$PLAY_SCRIPT_GLOBAL = '/etc/asterisk/local/playglobal.sh';  // local + all linked nodes
$SOUNDS_DIR = '/usr/local/share/asterisk/sounds/announcements';

// Get POST variables
$mp3     = isset($_POST['file']) ? basename($_POST['file']) : '';
$min     = $_POST['min'] ?? '';
$hour    = $_POST['hour'] ?? '';
$dom     = $_POST['dom'] ?? '';
$month   = $_POST['month'] ?? '';
$dow     = $_POST['dow'] ?? '';
$week    = $_POST['week'] ?? '*';
$use_nth = !empty($_POST['use_nth']) && $_POST['use_nth'] == 1;
$desc    = $_POST['desc'] ?? '';
$scope   = $_POST['scope'] ?? 'local';  // ← new: local or global

if (!$mp3) {
    die("No MP3 file specified.");
}

// Validate MP3 file exists
$src_mp3 = "$TMP_DIR/$mp3";
if (!file_exists($src_mp3)) {
    die("MP3 file not found: $src_mp3");
}

// Validate converter script exists and is executable
if (!is_executable($CONVERT_SCRIPT)) {
    die("Conversion script not found or not executable: $CONVERT_SCRIPT");
}

// Run conversion
$cmd_convert = escapeshellcmd("$CONVERT_SCRIPT $src_mp3");
exec($cmd_convert, $output, $ret);
if ($ret !== 0) {
    die("Conversion failed. Output: " . implode("\n", $output));
}

// Build .ul filename
$base_name = pathinfo($mp3, PATHINFO_FILENAME);
$ul_file = "$TMP_DIR/$base_name.ul";

// Check .ul was created
if (!file_exists($ul_file)) {
    die("Conversion failed: $ul_file not found.");
}

// Copy .ul file to Asterisk sounds directory
$cmd_copy = escapeshellcmd("sudo cp $ul_file $SOUNDS_DIR/$base_name.ul");
exec($cmd_copy, $copy_out, $copy_ret);
if ($copy_ret !== 0) {
    die("Failed to copy $ul_file to $SOUNDS_DIR. Check sudo permissions.");
}

// Set proper permissions and ownership
exec(escapeshellcmd("sudo chmod 644 $SOUNDS_DIR/$base_name.ul"));
exec(escapeshellcmd("sudo chown root:root $SOUNDS_DIR/$base_name.ul"));

// Choose playback script based on scope
$play_script = ($scope === 'global') ? $PLAY_SCRIPT_GLOBAL : $PLAY_SCRIPT_LOCAL;

// Optional: add scope to description comment for visibility in crontab/list_cron
$scope_note = ($scope === 'global') ? " [GLOBAL]" : " [local]";
$desc_clean = $desc ? "# Announcement: $desc$scope_note" : "# Announcement$scope_note";

// Install cron job if scheduling info provided
if ($min !== '' && $hour !== '' && $dom !== '' && $month !== '' && $dow !== '') {
    $play_target = "$SOUNDS_DIR/$base_name"; // no extension for play*.sh

    if ($use_nth && in_array($week, ['1','2','3','4','5'])) {
        // ── Nth week of the month ──────────────────────────────────────
        $low = ((int)$week - 1) * 7 + 1;
        $high = ((int)$week === 5) ? 31 : $low + 6;
        $cond = "[ \$(date +\\%d) -ge $low ] && [ \$(date +\\%d) -le $high ]";
        $cron_line = "$min $hour * * $dow /bin/bash -c '$cond && $play_script $play_target'";
        // Improve description for visibility
        $nth_suffix = ['','st','nd','rd','th','th'][(int)$week];
        if ($desc_clean) {
            $desc_clean .= " ({$week}{$nth_suffix} week of month - day $dow)";
        }
    } else {
        // ── Classic / standard cron style ──────────────────────────────
        $cron_line = "$min $hour $dom $month $dow $play_script $play_target";
    }

    // Append to root's crontab
    $tmp_cron = tempnam(sys_get_temp_dir(), 'cron_ann');

    // Get current crontab (suppress error if none exists yet)
    exec("sudo crontab -l > " . escapeshellarg($tmp_cron) . " 2>/dev/null");

    // Add description comment if present
    if ($desc_clean) {
        file_put_contents($tmp_cron, $desc_clean . "\n", FILE_APPEND);
    }

    // Add the cron line
    file_put_contents($tmp_cron, $cron_line . "\n", FILE_APPEND);

    // Install new crontab
    exec("sudo crontab " . escapeshellarg($tmp_cron), $cron_out, $cron_ret);
    unlink($tmp_cron);

    if ($cron_ret !== 0) {
        die("Failed to install cron job. Check sudo permissions.");
    }

    echo "Conversion and cron job installation successful!\n";
    echo "Cron line: $cron_line\n";
    echo "Playback scope: " . ucfirst($scope) . "\n";
    if ($use_nth) {
        echo "Mode: Nth-week scheduling (week $week)\n";
    } else {
        echo "Mode: Standard scheduling\n";
    }
} else {
    echo "Conversion successful! No cron job installed (missing scheduling parameters).\n";
}

echo "UL file installed at: $SOUNDS_DIR/$base_name.ul\n";
?>
