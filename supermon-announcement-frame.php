<?php
// supermon-announcement-frame.php
// Security: Only allow access from within Supermon
// created by N5AD June, 2026
// for future implementaton of Iframe in supermon
// rather than modifying link.php

$referer = $_SERVER['HTTP_REFERER'] ?? '';

if (empty($referer) || 
    (stripos($referer, '/supermon') === false)) {
    
    // Not coming from Supermon → deny direct access
    http_response_code(403);
    echo "<h2 style='text-align:center; color:red; margin-top:80px;'>Access Denied</h2>";
    echo "<p style='text-align:center;'>This page can only be accessed from within Supermon.</p>";
    echo "<p style='text-align:center;'><a href='/supermon/'>← Go to Supermon Login</a></p>";
    exit;
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Announcement Manager</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background: #f4f4f4;
            display: flex;
            justify-content: center;
            min-height: 100vh;
        }
        .container {
            width: 100%;
            max-width: 1700px;
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 12px rgba(0,0,0,0.1);
            text-align: center;
        }
        h1 { text-align: center; margin-bottom: 25px; }
        table, form, div { margin-left: auto; margin-right: auto; }
        @media (max-width: 1800px) { .container { max-width: 95%; } }
        
        .submit {
            padding: 8px 16px;
            margin: 5px;
            font-size: 14px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Announcement Manager</h1>
        <?php include 'announcement-frame.inc'; ?>
    </div>
</body>
</html>
