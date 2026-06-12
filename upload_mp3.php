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

$filename = basename($file['name']);   // Clean filename


$target_dir = "/mp3/";

$target_file = $target_dir . $filename;


$allowed = ['mp3','wav','MP3','WAV'];

$ext = strtolower(pathinfo($filename, PATHINFO_EXTENSION));


if (!in_array($ext, $allowed)) {

    echo "Only MP3 and WAV files allowed.";

    exit;

}


if (move_uploaded_file($file['tmp_name'], $target_file)) {

    chmod($target_file, 0664);

    chown($target_file, 'http');

    echo "✅ Uploaded successfully: " . htmlspecialchars($filename);

} else {

    echo "❌ Failed to upload file.";

}

?>


