<?php

/**

 * announcement.php

 * Created by N5AD

 * Converts MP3 → u-law (.ul), copies to Asterisk sounds dir,

 * and optionally installs a cron job.

 */


$TMP_DIR = '/mp3';

$CONVERT_SCRIPT = '/etc/asterisk/local/audio_convert.sh';

$PLAY_SCRIPT = '/etc/asterisk/local/playaudio.sh';

$SOUNDS_DIR = '/usr/local/share/asterisk/sounds/announcements';


// Get POST variables

$mp3 = isset($_POST['file']) ? basename($_POST['file']) : '';

$min = $_POST['min'] ?? '';

$hour = $_POST['hour'] ?? '';

$dom = $_POST['dom'] ?? '';

$month = $_POST['month'] ?? '';

$dow = $_POST['dow'] ?? '';

$desc = $_POST['desc'] ?? '';


if (!$mp3) {

    die("No MP3 file specified.");

}


// Validate MP3 file exists

$src_mp3 = "$TMP_DIR/$mp3";

if (!file_exists($src_mp3)) {

    die("MP3 file not found: $src_mp3");

}


// Validate converter script exists

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


// Copy .ul file to Asterisk sounds directory using sudo

$cmd_copy = escapeshellcmd("sudo cp $ul_file $SOUNDS_DIR/$base_name.ul");

exec($cmd_copy, $copy_out, $copy_ret);

if ($copy_ret !== 0) {

    die("Failed to copy $ul_file to $SOUNDS_DIR. Check sudo permissions.");

}


// Set permissions and ownership

exec(escapeshellcmd("sudo chmod 644 $SOUNDS_DIR/$base_name.ul"));

exec(escapeshellcmd("sudo chown root:root $SOUNDS_DIR/$base_name.ul"));


// Install cron job if scheduling info provided

if ($min && $hour && $dom && $month && $dow) {

    $play_target = "$SOUNDS_DIR/$base_name"; // NO extension for playaudio.sh


    // Optional comment

    $comment_line = $desc ? "# Announcement: $desc" : '';


    // Build cron line

    $cron_line = "$min $hour $dom $month $dow $PLAY_SCRIPT $play_target";


    // Append to root's crontab

    $tmp_cron = tempnam(sys_get_temp_dir(), 'cron');

    exec("sudo crontab -l > $tmp_cron 2>/dev/null"); // get current root crontab

    if ($comment_line) {

        file_put_contents($tmp_cron, $comment_line . PHP_EOL, FILE_APPEND);

    }

    file_put_contents($tmp_cron, $cron_line . PHP_EOL, FILE_APPEND);

    exec("sudo crontab $tmp_cron", $cron_out, $cron_ret);

    unlink($tmp_cron);


    if ($cron_ret !== 0) {

        die("Failed to install cron job.");

    }

    echo "Conversion and cron job installation successful!\n";

    echo "Cron line: $cron_line\n";

} else {

    echo "Conversion successful! No cron job installed.\n";

}


echo "UL file installed at: $SOUNDS_DIR/$base_name.ul\n";

?>

