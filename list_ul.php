<?php

// list_ul.php - CREATED BY N5AD

require_once __DIR__ . '/auth_check.inc';

$SOUNDS_DIR = '/usr/local/share/asterisk/sounds/announcements';

$files = glob("$SOUNDS_DIR/*.ul");

$out = [];


foreach ($files as $f) {

    $out[] = basename($f); // only filename, not full path

}


header('Content-Type: application/json');

echo json_encode($out);

