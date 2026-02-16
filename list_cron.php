<?php
/*
 * list_cron.php
 * Lists AllStar announcement cron jobs with time, file, description
 * CREATED BY N5AD
 */

require_once __DIR__ . '/auth_check.inc';

header('Content-Type: application/json');

// Read root crontab
$cron = shell_exec('sudo crontab -l 2>/dev/null');
if ($cron === null) {
    echo json_encode([]);
    exit;
}

$lines = explode("\n", $cron);
$entries = [];
$last_comment = "";

foreach ($lines as $line) {
    $line = trim($line);
    if ($line === "") continue;

    // Capture announcement description
    if (strpos($line, '# Announcement:') === 0) {
        $last_comment = trim(str_replace('# Announcement:', '', $line));
        continue;
    }

    // Match AllStar playaudio cron lines
    if (strpos($line, 'playaudio.sh') !== false) {

        $parts = preg_split('/\s+/', $line);

        // First 5 fields are cron time
        $time = implode(" ", array_slice($parts, 0, 5));

        // Last part is sound file path (no extension)
        $file = basename(end($parts));

        $entries[] = [
            "time" => $time,
            "file" => $file,
            "desc" => $last_comment,
            "raw"  => $line
        ];

        // Reset comment so it only applies to one entry
        $last_comment = "";
    }
}

echo json_encode($entries);
