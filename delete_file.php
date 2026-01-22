<?php
// delete_file.php - Delete MP3 or UL file

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

if (empty($_POST['type']) || empty($_POST['file'])) {
    echo "Missing parameters.";
    exit;
}

$type = $_POST['type']; // 'mp3' or 'ul'
$filename = basename($_POST['file']);

if ($type === 'mp3') {
    $file_path = "/mp3/" . $filename;
} elseif ($type === 'ul') {
    $file_path = "/usr/local/share/asterisk/sounds/" . $filename;
} else {
    echo "Invalid type.";
    exit;
}

if (!file_exists($file_path)) {
    echo "File not found: $filename";
    exit;
}

if (unlink($file_path)) {
    echo "Deleted $filename successfully.";
} else {
    echo "Failed to delete $filename. Check permissions.";
}
?>