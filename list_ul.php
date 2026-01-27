<?php

// list_ul.php

// CREATED BY N5AD

$SOUNDS_DIR = '/usr/local/share/asterisk/sounds/announcements';

$files = glob("$SOUNDS_DIR/*.ul");

$out = [];


foreach ($files as $f) {

    $out[] = basename($f); // only filename, not full path

}


header('Content-Type: application/json');

echo json_encode($out);

