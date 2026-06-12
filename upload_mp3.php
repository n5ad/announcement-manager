<?php
/*
 * Updated June 12, 2026
 * Modified by N5AD
 */

if (!isset($_FILES['file'])) {
    echo "No file uploaded.";
    exit;
}

$file = $_FILES['file'];
$filename = basename($file['name']);
$ext = strtolower(pathinfo($filename, PATHINFO_EXTENSION));

$allowed = ['mp3', 'wav'];

if (!in_array($ext, $allowed)) {
    echo "Only MP3 and WAV files allowed.";
    exit;
}

/* Upload to /tmp first */
$tmp_input = "/tmp/" . $filename;

if (!move_uploaded_file($file['tmp_name'], $tmp_input)) {
    echo "❌ Failed to upload temporary file.";
    exit;
}

/* Build final WAV filename */
$base = pathinfo($filename, PATHINFO_FILENAME);
$output_file = "/mp3/" . $base . ".wav";

/* Remove existing output file if present */
if (file_exists($output_file)) {
    unlink($output_file);
}

/* Convert with SoX */
$cmd = sprintf(
    'sox %s -r 8000 -c 1 -b 16 -e signed-integer %s gain -n 2>&1',
    escapeshellarg($tmp_input),
    escapeshellarg($output_file)
);

exec($cmd, $output, $retval);

/* Remove temporary upload */
unlink($tmp_input);

if ($retval !== 0 || !file_exists($output_file)) {
    echo "❌ Conversion failed.<br>";
    echo "<pre>" . htmlspecialchars(implode("\n", $output)) . "</pre>";
    exit;
}

/* Set permissions */
chmod($output_file, 0664);

/* Change owner if desired */
@chown($output_file, 'http');

echo "✅ Uploaded and converted successfully: " .
     htmlspecialchars(basename($output_file));

?>
