<?php
// allmon-globalplay.php - Global playback using playglobal.sh (rpt playback)

require_once __DIR__ . '/auth_check.inc';

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

if (empty($_POST['file'])) {
    echo "No file specified.";
    exit;
}

// Sanitize: base filename only
$base = basename($_POST['file']);
$base_name = pathinfo($base, PATHINFO_FILENAME);  // strip extension if present

// For MP3/WAV from /mp3/ dir - use full absolute path (Asterisk supports this)
$play_path = "/mp3/" . $base_name;  // no extension needed - Asterisk auto-detects MP3/WAV

// If you ever want to support both dirs, you could add logic based on source, but for now this is MP3-focused

$play_script = "/etc/asterisk/local/playglobal.sh";

if (!is_executable($play_script)) {
    echo "playglobal.sh not found or not executable at $play_script.";
    exit;
}

$cmd = 'sudo ' . escapeshellarg($play_script) . ' ' . escapeshellarg($play_path);

exec($cmd . " 2>&1", $output, $retval);

if ($retval === 0) {
    echo "Global playback started for '$base_name' (MP3/WAV from /mp3/).";
} else {
    $error_msg = implode("\n", $output);
    echo "Failed to play '$base_name' globally.\nCode: $retval\nOutput: $error_msg";
}
?>
