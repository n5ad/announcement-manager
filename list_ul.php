<?php

/*
 * Updated June 12, 2026
 * Modified by N5AD
 */

$SOUNDS_DIR = '/usr/local/share/asterisk/sounds/announcements';

$files = glob("$SOUNDS_DIR/*.ul");

$out = [];


foreach ($files as $f) {

    $out[] = basename($f); // only filename, not full path

}


header('Content-Type: application/json');

echo json_encode($out);

