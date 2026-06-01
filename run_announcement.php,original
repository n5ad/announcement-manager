<?php
/**
 * run_announcement.php
 * Plays a file immediately on the local AllStar node
 * Supports two directories:
 *   - announcements/ (for .ul files)
 *   - /mp3/ (for raw MP3/WAV files)
 * Original by N5AD - updated to support MP3/WAV directly
 */

// Only accept POST requests
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

if (empty($_POST['file'])) {
    echo "No file specified.";
    exit;
}

// Sanitize input - just the base filename
$base = basename($_POST['file']);
$base_name = pathinfo($base, PATHINFO_FILENAME); // strip extension

// Determine source directory based on POST param
$source = $_POST['source'] ?? 'ul'; // default to ul

if ($source === 'mp3') {
    // Play from /mp3/ (raw MP3/WAV)
    $play_path = "/mp3/" . $base_name;
    $echo_msg  = "Playing '$base_name' (MP3/WAV from /mp3/) locally now.";
} else {
    // Default: play from announcements/ (for .ul files)
    $play_path = "announcements/" . $base_name;
    $echo_msg  = "Playing '$base_name' (from announcements/) locally now.";
}

// Path to play script (local playback)
$play_script = "/etc/asterisk/local/playaudio.sh";

// Verify script exists and is executable
if (!is_executable($play_script)) {
    echo "playaudio.sh not found or not executable at $play_script.";
    exit;
}

// Build and execute command
$cmd = escapeshellcmd("sudo $play_script " . escapeshellarg($play_path));
exec($cmd . " 2>&1", $output, $retval);

if ($retval === 0) {
    echo $echo_msg;
} else {
    $error_msg = implode("\n", $output);
    echo "Failed to play '$base_name'.\nCode: $retval\nOutput: $error_msg";
}
?>
