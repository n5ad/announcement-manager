<?php

/*

 * list_cron.php

 * Lists AllStar announcement cron jobs 

 * Now shows ALL jobs using: playaudio.sh, globalplay.sh, polite_play.sh, polite_global.sh

 * CREATED BY N5AD

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


    // Match ANY of the 4 announcement scripts

    if (strpos($line, 'playaudio.sh') !== false || 

        strpos($line, 'playglobal.sh') !== false || 

        strpos($line, 'polite_play.sh') !== false || 

        strpos($line, 'polite_global.sh') !== false) {


        $parts = preg_split('/\s+/', $line, -1, PREG_SPLIT_NO_EMPTY);

        

        // First 5 fields = cron time

        $time = implode(" ", array_slice($parts, 0, 5));

        

        // Command = everything after

        $command = implode(" ", array_slice($parts, 5));


        // Extract script name and target file

        if (preg_match('/(playaudio\.sh|playglobal\.sh|polite_play\.sh|polite_global\.sh)\s+(.+)$/', $command, $matches)) {

            $script = $matches[1];

            $full_target = trim($matches[2]);


            // Get clean filename

            $file = basename($full_target);

            $file = preg_replace('/\.ul$/', '', $file);


            // Determine scope from script name

            if (strpos($script, 'global') !== false) {

                $scope = 'global';

            } else {

                $scope = 'local';

            }


            // Override from comment if present

            if (preg_match('/\[GLOBAL\]/i', $last_comment)) {

                $scope = 'global';

            } elseif (preg_match('/\[local\]/i', $last_comment)) {

                $scope = 'local';

            }


            $entries[] = [

                "time"    => $time,

                "file"    => $file,

                "desc"    => $last_comment ?: 'No description',

                "scope"   => $scope,

                "raw"     => $line,

                "script"  => $script

            ];


            // Reset comment

            $last_comment = "";

        }

    }

}


echo json_encode($entries);
