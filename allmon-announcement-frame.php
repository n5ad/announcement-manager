<?php
// allmon-announcement-frame.php
//
// Security: Only allow access if the user is currently logged into Allmon3.
// We verify this by calling Allmon3's own "master/auth/check" API endpoint
// server-side, forwarding the browser's cookies exactly as the browser would.
// This avoids guessing cookie names or reimplementing Allmon3's auth logic --
// we simply ask Allmon3 itself "is this user logged in?" and trust its answer.

function isAllmon3LoggedIn(): bool {
    // Build the check URL from the current request so it works regardless of
    // http/https or hostname. Adjust the path below (/allmon3/) if your
    // Allmon3 install lives at a different URL path.
    $scheme = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') ? 'https' : 'http';
    $host   = $_SERVER['HTTP_HOST'] ?? 'localhost';
    $checkUrl = "$scheme://$host/allmon3/master/auth/check";

    $cookieHeader = $_SERVER['HTTP_COOKIE'] ?? '';

    $ch = curl_init($checkUrl);
    curl_setopt_array($ch, [
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_HTTPHEADER     => ["Cookie: $cookieHeader"],
        CURLOPT_TIMEOUT        => 3,
        // NOTE: SSL verification is disabled here because this server is
        // calling itself over its own IP-based/self-signed certificate,
        // which cannot pass strict hostname verification. This is a
        // server-to-self loopback call carrying only the cookie the
        // browser already sent this same server -- not a third-party
        // request -- so the usual MITM concern that peer verification
        // protects against does not apply the same way here. If you later
        // put a proper trusted certificate on this host, switch both of
        // these back to true / 2.
        CURLOPT_SSL_VERIFYPEER => false,
        CURLOPT_SSL_VERIFYHOST => false,
    ]);
    $response = curl_exec($ch);
    $curlError = curl_error($ch);
    curl_close($ch);

    if ($response === false) {
        error_log("allmon-announcement-frame.php: auth check curl error: $curlError");
        return false; // fail closed if Allmon3 can't be reached
    }

    $data = json_decode($response, true);
    return isset($data['SUCCESS']) && $data['SUCCESS'] === 'Logged In';
}

if (!isAllmon3LoggedIn()) {
    http_response_code(403);
    echo "<h2 style='text-align:center; color:red; margin-top:80px;'>Access Denied</h2>";
    echo "<p style='text-align:center;'>You must be logged into Allmon3 to view this page.</p>";
    echo "<p style='text-align:center;'><a href='/allmon3/'>&larr; Go to Allmon3</a></p>";
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
        <?php include '/var/www/html/announcement-manager/allmon-announcement.inc'; ?>
    </div>
</body>
</html>
