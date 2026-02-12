<?php
/**
 * custom/update_announcement.php
 * this will convert an advanced cron job to a simple cronjob
 * if the single DOW is changed to multiple DOW
 * created by N5AD February 2026
 */

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

$raw_line = trim($_POST['raw_line'] ?? '');
$min      = trim($_POST['min']      ?? '');
$hour     = trim($_POST['hour']     ?? '');
$dom      = trim($_POST['dom']      ?? '');
$month    = trim($_POST['month']    ?? '');
$dow      = trim($_POST['dow']      ?? '');
$week     = trim($_POST['week']     ?? '*');
$use_nth  = !empty($_POST['use_nth']) && $_POST['use_nth'] == 1;

if (!$raw_line || $min === '' || $hour === '' || $dom === '' || $month === '' || $dow === '') {
    echo "Missing required fields.";
    exit;
}

// Read current crontab
$output = [];
$retval = 0;
exec('sudo crontab -l 2>/dev/null', $output, $retval);

if ($retval !== 0 && !empty($output)) {
    echo "Failed to read current crontab.";
    exit;
}

$new_crontab = [];
$found = false;
$comment_line = null;

foreach ($output as $line) {
    $trimmed = trim($line);

    // Preserve comment line
    if (strpos($trimmed, '# Announcement:') === 0) {
        $comment_line = $trimmed;
        continue;
    }

    // Match the job line
    if ($trimmed === $raw_line || strpos($trimmed, $raw_line) !== false) {
        $found = true;

        // Extract the full command part after time fields
        if (preg_match('/^\S+\s+\S+\s+\S+\s+\S+\s+\S+\s+(.+)$/', $line, $matches)) {
            $full_command = trim($matches[1]);
        } else {
            $full_command = '/etc/asterisk/local/playaudio.sh /unknown/path';
        }

        // If it's an nth-week job, extract just the playaudio part cleanly
        $play_cmd = $full_command;
        if (strpos($full_command, '/bin/bash -c') !== false) {
            // Grab playaudio.sh + path from inside the bash command
            if (preg_match('/playaudio\.sh\s+([^\'"]+?)(?=\s*[\'"]?\))/i', $full_command, $play_matches)) {
                $play_cmd = '/etc/asterisk/local/playaudio.sh ' . trim($play_matches[1]);
            } elseif (preg_match('/playaudio\.sh\s+(.+)$/i', $full_command, $play_matches)) {
                $play_cmd = '/etc/asterisk/local/playaudio.sh ' . trim($play_matches[1]);
            }
        }

        // Remove any trailing quote or parenthesis that might be left
        $play_cmd = trim($play_cmd, " ')\t");

        // Build new line
        if ($use_nth && in_array($week, ['1','2','3','4','5']) && preg_match('/^[1-7]$/', trim($dow))) {
            // Rebuild nth-week
            $low  = ((int)$week - 1) * 7 + 1;
            $high = ((int)$week === 5) ? 31 : $low + 6;

            $cond = "[ \$(date +\\%d) -ge $low ] && [ \$(date +\\%d) -le $high ]";

            $new_line = "$min $hour $dom $month $dow /bin/bash -c '$cond && $play_cmd'";
        } else {
            // Classic style - no trailing quote
            $new_line = "$min $hour $dom $month $dow $play_cmd";
        }

        // Add comment if present
        if ($comment_line) {
            $new_crontab[] = $comment_line;
								 
        }
        $new_crontab[] = $new_line;
    } else {
        $new_crontab[] = $line;
    }
}

if (!$found) {
    echo "Original cron line not found in crontab.";
    exit;
}

// Write updated crontab
$tempfile = tempnam(sys_get_temp_dir(), 'cron_update_');
file_put_contents($tempfile, implode("\n", $new_crontab) . "\n");

exec("sudo crontab $tempfile", $out, $ret);
unlink($tempfile);

if ($ret === 0) {
    echo "Cron job updated successfully.";
} else {
    echo "Failed to update crontab - check sudo permissions or logs.";
}
?>
