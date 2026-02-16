<?php
/**
 * cron_helpers.inc - Validation helpers for cron-related operations
 */

/**
 * Validate and return sanitized raw_line from POST.
 * Rejects newlines (injection), excessive length, empty.
 * Returns sanitized string or null if invalid.
 */
function validate_raw_line(?string $raw): ?string {
    if ($raw === null) {
        return null;
    }
    $trimmed = trim($raw);
    if ($trimmed === '' || strlen($trimmed) > 1024) {
        return null;
    }
    if (strpos($raw, "\n") !== false || strpos($raw, "\r") !== false) {
        return null;
    }
    return $trimmed;
}
