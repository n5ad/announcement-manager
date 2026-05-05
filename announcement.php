<?php

/**

 * announcement.php

 * Converts MP3 â†’ Asterisk .ul format and installs cron job

 * Updated to include Mode and Scope in description comment

 * Created by N5AD

 */


$TMP_DIR        = '/mp3';

$CONVERT_SCRIPT = '/etc/asterisk/local/audio_convert.sh';

$SOUNDS_DIR     = '/usr/local/share/asterisk/sounds/announcements';


// Playback scripts

$PLAY_LOCAL_PRIORITY  = '/etc/asterisk/local/playaudio.sh';

$PLAY_GLOBAL_PRIORITY = '/etc/asterisk/local/globalplay.sh';

$PLAY_LOCAL_POLITE    = '/etc/asterisk/local/polite_play.sh';

$PLAY_GLOBAL_POLITE   = '/etc/asterisk/local/polite_global.sh';


// Get form data

$mp3     = isset($_POST['file'])     ? basename($_POST['file']) : '';

$min     = $_POST['min']     ?? '';

$hour    = $_POST['hour']    ?? '';

$dom     = $_POST['dom']     ?? '*';

$month   = $_POST['month']   ?? '*';

$dow     = $_POST['dow']     ?? '*';

$desc    = $_POST['desc']    ?? '';

$scope   = $_POST['scope']   ?? 'local';

$mode    = $_POST['mode']    ?? 'polite';

$use_nth = !empty($_POST['use_nth']) && $_POST['use_nth'] == 1;

$week    = $_POST['week']    ?? '*';


if (!$mp3) {

    die("Error: No MP3 file specified.");

}


// ... (conversion and file copy code remains the same) ...


// Select correct playback script

if ($scope === 'global') {

    $play_script = ($mode === 'priority') ? $PLAY_GLOBAL_PRIORITY : $PLAY_GLOBAL_POLITE;

} else {

    $play_script = ($mode === 'priority') ? $PLAY_LOCAL_PRIORITY : $PLAY_LOCAL_POLITE;

}


// === UPDATED: Better description with Mode + Scope ===

$mode_note  = ($mode === 'priority') ? "PRIORITY" : "POLITE";

$scope_note = ($scope === 'global')  ? "GLOBAL" : "LOCAL";


$desc_clean = $desc 

    ? "# Announcement: $desc [$mode_note] [$scope_note]"

    : "# Announcement [$mode_note] [$scope_note]";


// Build cron job (rest of the file stays the same)

if ($min !== '' && $hour !== '') {

    $play_target = "$SOUNDS_DIR/" . pathinfo($mp3, PATHINFO_FILENAME);


    if ($use_nth && in_array($week, ['1','2','3','4','5'])) {

        $low  = ((int)$week - 1) * 7 + 1;

        $high = ((int)$week == 5) ? 31 : $low + 6;

        $cond = "[ \$(date +\\%d) -ge $low ] && [ \$(date +\\%d) -le $high ]";

        $cron_line = "$min $hour * * $dow /bin/bash -c '$cond && $play_script $play_target'";

        $desc_clean .= " (Week $week)";

    } else {

        $cron_line = "$min $hour $dom $month $dow $play_script $play_target";

    }


    $tmp_cron = tempnam(sys_get_temp_dir(), 'ann_cron_');

    exec("sudo crontab -l > " . escapeshellarg($tmp_cron) . " 2>/dev/null");


    file_put_contents($tmp_cron, $desc_clean . "\n", FILE_APPEND);

    file_put_contents($tmp_cron, $cron_line . "\n", FILE_APPEND);


    exec("sudo crontab " . escapeshellarg($tmp_cron), $out, $ret);

    unlink($tmp_cron);


    if ($ret !== 0) {

        die("Error: Failed to install cron job.");

    }


    echo "âœ… Announcement installed successfully!\n";

    echo "File     : " . pathinfo($mp3, PATHINFO_FILENAME) . ".ul\n";

    echo "Mode     : " . strtoupper($mode) . "\n";

    echo "Scope    : " . strtoupper($scope) . "\n";

    echo "Schedule : $min $hour $dom $month $dow\n";


} else {

    echo "âœ… File converted successfully (no schedule provided).\n";

}


echo "\nUL file saved to: $SOUNDS_DIR/" . pathinfo($mp3, PATHINFO_FILENAME) . ".ul";

?>
