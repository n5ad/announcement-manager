<?php
/*
 * list_cron.php
 * Lists AllStar announcement cron jobs with time, file, description, and scope (local/global)
 * CREATED BY N5AD — updated for local/global support
 */
header('Content-Type: application/json');

// Read root crontab
$cron = shell_exec('sudo crontab -l 2>/dev/null');
if ($cron === null || trim($cron) === '') {
    echo json_encode([]);
    exit;
}

$lines = explode("\n", $cron);
$entries = [];
$last_comment = "";

foreach ($lines as $line) {
    $line = trim($line);
    if ($line === "") continue;

    // Capture announcement description line
    if (strpos($line, '# Announcement:') === 0) {
        $last_comment = trim(str_replace('# Announcement:', '', $line));
        continue;
    }

    // Match lines that call either playaudio.sh or playglobal.sh
    if (strpos($line, 'polite_play.sh') !== false || strpos($line, 'polite_global.sh') !== false) {
        $parts = preg_split('/\s+/', $line, -1, PREG_SPLIT_NO_EMPTY);

        // First 5 fields = cron time
        $time = implode(" ", array_slice($parts, 0, 5));

        // Command = everything after the 5th field
        $command = implode(" ", array_slice($parts, 5));

        // Extract script name and target file
        if (preg_match('/(polite_play\.sh|polite_global\.sh)\s+(.+)$/', $command, $matches)) {
            $script = $matches[1];
            $full_target = trim($matches[2]);

            // Get just the base filename (without path or extension)
            $file = basename($full_target);
            $file = preg_replace('/\.ul$/', '', $file);  // in case extension snuck in

            // Determine scope from script name
            $scope = (strpos($script, 'playglobal.sh') !== false) ? 'global' : 'local';

            // Try to extract extra scope note from comment if present
            if (preg_match('/\[GLOBAL\]/i', $last_comment)) {
                $scope = 'global';
            } elseif (preg_match('/\[local\]/i', $last_comment)) {
                $scope = 'local';
            }

            $entries[] = [
                "time"     => $time,
                "file"     => $file,
                "desc"     => $last_comment,
                "scope"    => $scope,           // ← new field
                "raw"      => $line,
                "script"   => $script           // optional — useful for debugging
            ];

            // Reset comment after associating it
            $last_comment = "";
        }
    }
}

echo json_encode($entries);
