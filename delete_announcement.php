<?php

if (!isset($_POST['raw_line'])) {
    echo "Error: Missing cron line";
    exit;
}

$raw = trim($_POST['raw_line']);

// Get root crontab
exec('sudo crontab -l', $crons);

$tempfile = tempnam(sys_get_temp_dir(), 'cron');
file_put_contents($tempfile, '');

$prev_line = null;

foreach ($crons as $line) {

    $trimmed = trim($line);

    // If this is the cron line being deleted
    if ($trimmed === $raw) {

        // If previous line was a comment, remove it
        if ($prev_line !== null && strpos(trim($prev_line), '#') === 0) {
            // Do nothing — comment already skipped
        }

        // Skip this cron line
        $prev_line = null;
        continue;
    }

    // Write previous line if it wasn't skipped
    if ($prev_line !== null) {
        file_put_contents($tempfile, $prev_line . PHP_EOL, FILE_APPEND);
    }

    $prev_line = $line;
}

// Write last line if needed
if ($prev_line !== null) {
    file_put_contents($tempfile, $prev_line . PHP_EOL, FILE_APPEND);
}

// Install updated crontab
exec("sudo crontab $tempfile");
unlink($tempfile);

echo "Deleted cron and description successfully";

?>
