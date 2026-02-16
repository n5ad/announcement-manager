<?php

/**
 * list_mp3.php
 * Lists both .mp3 and .wav files in /mp3/ for the Announcements Manager
 * CREATED BY N5AD
 */

require_once __DIR__ . '/auth_check.inc';

$files = [];


// Find .mp3 files

$mp3_files = glob("/mp3/*.mp3");

foreach ($mp3_files as $f) {

    $files[] = basename($f);

}


// Find .wav files

$wav_files = glob("/mp3/*.wav");

foreach ($wav_files as $f) {

    $files[] = basename($f);

}
// Sort alphabetically for nicer display

sort($files);


// Remove duplicates (in case someone has both .mp3 and .wav with same base name)
$files = array_values(array_unique($files));

header('Content-Type: application/json');

echo json_encode($files);

?>
