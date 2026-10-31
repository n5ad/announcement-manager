<?php
// allmon-announcement-frame.php
// Updated by N5AD, July 2026


function isAllmon3LoggedIn(): bool {
  
    $checkUrl = "https://127.0.0.1/allmon3/master/auth/check";

    $cookieHeader = $_SERVER['HTTP_COOKIE'] ?? '';

    $ch = curl_init($checkUrl);
    curl_setopt_array($ch, [
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_HTTPHEADER     => ["Cookie: $cookieHeader"],
        CURLOPT_TIMEOUT        => 5,
        CURLOPT_FOLLOWLOCATION => false,
        CURLOPT_SSL_VERIFYPEER => false,
        CURLOPT_SSL_VERIFYHOST => false,
    ]);

    $response  = curl_exec($ch);
    $curlError = curl_error($ch);
    curl_close($ch);

    if ($response === false) {
        error_log("allmon-announcement-frame.php: auth check curl error: $curlError");
        return false; // fail closed
    }

    $data = json_decode($response, true);
    return isset($data['SUCCESS']) && $data['SUCCESS'] === 'Logged In';
}

// ====================== AUTH CHECK ======================
if (!isAllmon3LoggedIn()) {
    http_response_code(403);
    echo "<h2 style='text-align:center; color:red; margin-top:80px;'>Access Denied</h2>";
    echo "<p style='text-align:center;'>Please log into Allmon3 then refresh the page.</p>";
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
            min-height: 1600px;
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
  <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
</head>
<body>
    <div class="container">
     
        <?php include '/usr/local/share/announcement-manager/announcement.inc'; ?>
    </div>
</body>
</html>
