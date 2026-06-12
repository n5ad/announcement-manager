<?php

/**

 * announcement.php - ASL3 Final Version

 * Created by N5AD

 * Converts MP3 → u-law (.ul), installs to Asterisk sounds,

 * and sets up cron job with support for:

 *   - Local vs Global playback

 *   - Polite vs Priority mode

 *   - Standard + Nth week of month scheduling

 * updated June 12, 2026
 
 */


$TMP_DIR     = '/mp3';

$CONVERT_SCRIPT = '/etc/asterisk/local/audio_convert.sh';


$PLAY_SCRIPTS = [

    'local' => [

        'polite'    => '/etc/asterisk/local/polite_play.sh',

        'priority'  => '/etc/asterisk/local/playaudio.sh'

    ],

    'global' => [

        'polite'    => '/etc/asterisk/local/polite_global.sh',

        'priority'  => '/etc/asterisk/local/playglobal.sh'

    ]

];


$SOUNDS_DIR = '/usr/local/share/asterisk/sounds/announcements';


// Get POST data

$mp3      = isset($_POST['file']) ? basename($_POST['file']) : '';

$min      = $_POST['min'] ?? '';

$hour     = $_POST['hour'] ?? '';

$dom      = $_POST['dom'] ?? '*';

$month    = $_POST['month'] ?? '*';

$dow      = $_POST['dow'] ?? '*';

$week     = $_POST['week'] ?? '*';

$use_nth  = !empty($_POST['use_nth']) && $_POST['use_nth'] == 1;

$desc     = $_POST['desc'] ?? 'Announcement';

$scope    = $_POST['scope'] ?? 'local';

$mode     = $_POST['mode'] ?? 'polite';   // polite or priority


if (!$mp3) {

    die("Error: No MP3 file specified.");

}


// Validate source file

$src_mp3 = "$TMP_DIR/$mp3";

if (!file_exists($src_mp3)) {

    die("Error: MP3 file not found: $src_mp3");

}


// Validate converter

if (!is_executable($CONVERT_SCRIPT)) {

    die("Error: Conversion script not found or not executable.");

}


// Convert MP3 to .ul

$base_name = pathinfo($mp3, PATHINFO_FILENAME);

$ul_file   = "$TMP_DIR/$base_name.ul";


$cmd_convert = escapeshellcmd("$CONVERT_SCRIPT " . escapeshellarg($src_mp3) . " " . escapeshellarg($ul_file));

exec($cmd_convert, $output, $ret);


if ($ret !== 0 || !file_exists($ul_file)) {

    die("Error: Audio conversion failed.");

}


// Copy to Asterisk sounds directory

$dest_ul = "$SOUNDS_DIR/$base_name.ul";

exec(escapeshellcmd("sudo cp " . escapeshellarg($ul_file) . " " . escapeshellarg($dest_ul)), $out, $ret);

if ($ret !== 0) {

    die("Error: Failed to copy .ul file to sounds directory.");

}


// Set permissions

exec(escapeshellcmd("sudo chmod 644 " . escapeshellarg($dest_ul)));

exec(escapeshellcmd("sudo chown root:root " . escapeshellarg($dest_ul)));


// Select correct playback script

$play_script = $PLAY_SCRIPTS[$scope][$mode] ?? $PLAY_SCRIPTS['local']['polite'];


// Description for crontab

$scope_note = strtoupper($scope);

$mode_note  = strtoupper($mode);

$desc_clean = "# Announcement: $desc [$mode_note] [$scope_note]";


// Install cron if scheduling parameters provided

if ($min !== '' && $hour !== '') {

    $play_target = "$SOUNDS_DIR/$base_name";


    if ($use_nth && in_array($week, ['1','2','3','4','5'])) {

        // Nth week of the month

        $low  = ((int)$week - 1) * 7 + 1;

        $high = ($week == 5) ? 31 : $low + 6;

        $cond = "[ \$(date +\\%d) -ge $low ] && [ \$(date +\\%d) -le $high ]";

        

        // FIXED: No trailing apostrophe

        $cron_line = "$min $hour * * $dow /bin/bash -c '$cond && $play_script $play_target'";


        $nth_suffix = ['','st','nd','rd','th','th'][(int)$week];

        $desc_clean .= " ({$week}{$nth_suffix} week - $dow)";

    } else {

        // Standard cron

        $cron_line = "$min $hour $dom $month $dow $play_script $play_target";

    }


    // === Safe Cron Installation ===

    $tmp_cron = tempnam(sys_get_temp_dir(), 'ann_cron_');


    exec("sudo crontab -l > " . escapeshellarg($tmp_cron) . " 2>/dev/null || true");


    file_put_contents($tmp_cron, $desc_clean . "\n", FILE_APPEND);

    file_put_contents($tmp_cron, $cron_line . "\n", FILE_APPEND);


    exec("sudo crontab " . escapeshellarg($tmp_cron), $cron_out, $cron_ret);

    unlink($tmp_cron);


    if ($cron_ret !== 0) {

        die("Error: Failed to install cron job.");

    }


    echo "✅ Announcement installed successfully!\n";

    echo "File   : $base_name.ul\n";

    echo "Mode   : $mode_note\n";

    echo "Scope  : $scope_note\n";

    echo "Schedule: $cron_line\n";

} else {

    echo "✅ File converted successfully (no schedule set).\n";

}


echo "UL saved to: $dest_ul\n";

?>
