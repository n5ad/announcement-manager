<?php
// allmon-announcement-frame.php
// Security: Block direct access unless coming from Allmon3

$referer = $_SERVER['HTTP_REFERER'] ?? '';

if (empty($referer) || 
    (stripos($referer, '/allmon3') === false && stripos($referer, '/supermon') === false)) {
    
    // Not coming from Allmon3 → deny direct access
    http_response_code(403);
    echo "<h2 style='text-align:center; color:red; margin-top:80px;'>Access Denied</h2>";
    echo "<p style='text-align:center;'>This page can only be accessed from within Allmon3.</p>";
    echo "<p style='text-align:center;'><a href='/allmon3/'>← Go to Allmon3 Login</a></p>";
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
    </style>
</head>
<body>
    <div class="container">
        <h1>Announcement Manager</h1>
        <?php include 'allmon-announcement.inc'; ?>
    </div>
</body>
</html>
