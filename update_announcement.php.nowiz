<?php

if(!isset($_POST['raw_line'])) { echo "Missing old cron line"; exit; }


$old = trim($_POST['raw_line']);

$min   = $_POST['min'];

$hour  = $_POST['hour'];

$dom   = $_POST['dom'];

$month = $_POST['month'];

$dow   = $_POST['dow'];


// Build new cron line, keep the UL file path from old line

if(preg_match('/playaudio\.sh\s+.*\/usr\/local\/share\/asterisk\/sounds\/announcements\/(\S+)/', $old, $matches)){

    $file = $matches[1];

    $new = "$min $hour $dom $month $dow /etc/asterisk/local/playaudio.sh /usr/local/share/asterisk/sounds/announcementes/$file";

} else {

    echo "Failed to parse old cron line"; exit;

}


// Replace old with new in crontab

$tempfile = tempnam(sys_get_temp_dir(), 'cron');

exec('sudo crontab -l', $crons);

file_put_contents($tempfile, '');

foreach($crons as $line){

    if(trim($line) === $old){

        file_put_contents($tempfile, $new.PHP_EOL, FILE_APPEND);

    } else {

        file_put_contents($tempfile, $line.PHP_EOL, FILE_APPEND);

    }

}

exec("sudo crontab $tempfile");

unlink($tempfile);


echo "Updated cron for $file to $new";

?>

