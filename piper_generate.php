<?php
// piper_generate.php - Generate .wav from text using Piper TTS
// Called from announcement.inc via AJAX

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

$text = trim($_POST['text'] ?? '');
$filename = basename(trim($_POST['filename'] ?? ''));

if (!$text || !$filename) {
    echo "Missing text or filename.";
    exit;
}

// Path to the wrapper script
$script = "/usr/local/bin/piper_prompt_tts.sh";

// Build safe command
$cmd = escapeshellcmd("sudo $script " . escapeshellarg($text) . " " . escapeshellarg($filename));

// Execute
exec($cmd . " 2>&1", $output, $retval);

if ($retval === 0) {
    echo "Success! Generated in /mp3/$filename.wav\n"
       . "You can now select it in the dropdown and convert to .ul.";
} else {
    echo "Failed: " . implode("\n", $output);
}
?>
