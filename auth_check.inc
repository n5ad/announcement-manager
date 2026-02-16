<?php
/**
 * auth_check.inc - Require authentication for Announcement Manager API endpoints
 * Supports both Supermon (sm61loggedin) and Allmon3 (loggedin) session variables.
 * Include this at the top of any custom PHP file that modifies data.
 */
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
$authenticated = (isset($_SESSION['sm61loggedin']) && $_SESSION['sm61loggedin'] === true)
    || (isset($_SESSION['loggedin']) && $_SESSION['loggedin'] === true);
if (!$authenticated) {
    http_response_code(403);
    header('Content-Type: text/plain');
    echo 'Authentication required.';
    exit;
}
