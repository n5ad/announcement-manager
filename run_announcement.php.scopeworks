<?php
/**
 * run_announcement.php
 * Plays a file immediately on the local AllStar node
 * Supports two directories:
 *   - announcements/ (for .ul files)
 *   - /mp3/ (for raw MP3/WAV files)
 * Original by N5AD - updated to support MP3/WAV directly
 */

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

if (empty($_POST['file'])) {
    echo "No file specified.";
    exit;
}

// Sanitize
$base = basename($_POST['file']);
$base_name = pathinfo($base, PATHINFO_FILENAME);

// Get scope (default to local for safety)
$scope = strtolower($_POST['scope'] ?? 'local');
$source = $_POST['source'] ?? 'ul';

// Determine playback path
if ($source === 'mp3') {
    $play_path = "/mp3/" . $base_name;
    $type_desc = "MP3/WAV";
} else {
    $play_path = "announcements/" . $base_name;
    $type_desc = "Announcement";
}

if ($scope === 'global') {
    $play_script = "/etc/asterisk/local/playglobal.sh";  // or whatever your global script is
    $echo_msg = "Playing '$base_name' **GLOBALLY** now.";
} else {
    $play_script = "/etc/asterisk/local/playaudio.sh";
    $echo_msg = "Playing '$base_name' locally now.";
}

// Security check
if (!is_executable($play_script)) {
    echo "Playback script not found or not executable: $play_script";
    exit;
}

// Execute
$cmd = escapeshellcmd("sudo $play_script " . escapeshellarg($play_path));
exec($cmd . " 2>&1", $output, $retval);

if ($retval === 0) {
    echo $echo_msg;
} else {
    $error = implode("\n", $output);
    echo "Failed to play '$base_name'.\nCode: $retval\nOutput: $error";
}
?>
