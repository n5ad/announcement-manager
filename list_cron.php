<?php

/**

 * list_announcements.php

 *

 * Lists AllStar announcement cron jobs created via Supermon

 * Identified by comments beginning with:

 *   # Announcement:

 *

 * CREATED BY N5AD

 */


header('Content-Type: application/json');


$cron = [];

$output = [];

$return_var = 0;


/*

 * Read root crontab safely

 */

exec('sudo crontab -l 2>/dev/null', $output, $return_var);


if ($return_var !== 0) {

    echo json_encode([

        'error' => 'Unable to read root crontab'

    ]);

    exit;

}


$current = null;


foreach ($output as $line) {


    $line = trim($line);


    // Detect announcement comment

    if (strpos($line, '# Announcement:') === 0) {


        $current = [

            'description' => trim(substr($line, strlen('# Announcement:'))),

            'schedule'    => '',

            'command'     => ''

        ];

        continue;

    }


    // If we just saw an announcement comment, next line is the cron entry

    if ($current && preg_match('/^\S+\s+\S+\s+\S+\s+\S+\s+\S+\s+/', $line)) {


        $parts = preg_split('/\s+/', $line, 6);


        $current['schedule'] = implode(' ', array_slice($parts, 0, 5));

        $current['command']  = $parts[5];


        $cron[] = $current;

        $current = null;

    }

}


echo json_encode($cron, JSON_PRETTY_PRINT);

