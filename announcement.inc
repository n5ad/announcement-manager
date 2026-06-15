<?php
// announcement.inc
// Loaded only after Supermon login
// Created by N5AD - Updated June 15, 2026
?>
<?php if (isset($_SESSION['sm61loggedin']) && $_SESSION['sm61loggedin'] === true) { ?>
<div id="announcement-manager" style="
    background: #f8f9fa;
    border: 2px solid #2c3e50;
    border-radius: 10px;
    padding: 25px;
    margin-top: 25px;
    box-shadow: 0 4px 12px rgba(0,0,0,0.15);
">
    <!-- Cron table layout -->
    <style>
        #cron_table td,
        #cron_table th {
            text-align: center !important;
            vertical-align: middle !important;
        }
    </style>

    <h1 style="text-align:center; color:#2c3e50; margin-top:0;">Announcement Management System</h1>
    <hr style="border: 1px solid #bdc3c7; margin: 20px 0;">
    
    <!-- ====================== PIPER TTS SECTION ====================== -->
    <div class="section" style="background: #ffffff; border: 2px solid #3498db; border-radius: 8px; padding: 20px; margin-bottom: 25px;">
        <h3 style="color:#2980b9; margin-top:0;">Generate Announcement with Piper TTS</h3>
       
        <textarea id="tts_text" rows="4" cols="70" placeholder="Enter the text you want to convert to speech. Commas and periods are acceptable, no other punctuation..."></textarea>
        <br><br>
       
        <label for="tts_filename">Filename (letters, numbers, hyphens, underscores only):</label><br>
        <input type="text" id="tts_filename" placeholder="e.g., myannounce" style="width: 280px;">
        <br><br>
       
        <label for="tts_voice">Voice Model:</label><br>
        <select id="tts_voice" class="submit" style="width: 320px;">
            <option value="/opt/piper/voices/en_US-lessac-medium.onnx">Lessac (American female)</option>
            <option value="/opt/piper/voices/en_US-joe-medium.onnx">Joe (American male)</option>
            <option value="/opt/piper/voices/en_US-amy-medium.onnx">Amy (American female)</option>
            <option value="/opt/piper/voices/en_US-kristin-medium.onnx">Kristin (American female)</option>
            <option value="/opt/piper/voices/en_US-libritts_r-medium.onnx">Libritts (British female)</option>
            <option value="/opt/piper/voices/en_US-ryan-low.onnx">Ryan (British female)</option>
        </select>
        <br><br>
       
        <button id="generate_tts" class="submit">Generate WAV File</button>
        <div id="tts_status" style="margin-top:12px; font-weight:bold;"></div>
    </div>

    <!-- ====================== MP3 UPLOAD SECTION ====================== -->
    <div class="section" style="background: #ffffff; border: 2px solid #4caf7a; border-radius: 8px; padding: 25px; margin-bottom: 25px; text-align: center;">
        <h4 style="color:#7dd9a1; margin-top:0;">Upload Your Own MP3 / WAV Files</h4>
        <label for="file"> This upload will automatically convert MP3 files to WAV for playback on ASL3</label><br>
        <div style="display: inline-flex; align-items: center; gap: 12px; flex-wrap: wrap; justify-content: center; margin: 15px 0;">
            <input type="file" id="mp3_upload" accept=".mp3,.wav" style="background:#1e252f; color:#e0e0e0;">
            <button id="upload_btn" class="submit">Upload to /mp3/</button>
        </div>
        <div id="upload_status" style="margin-top:12px; font-weight:bold;"></div>
    </div>

    <!-- ====================== SCHEDULER & PLAYBACK ====================== -->
    <div class="section" style="background: #ffffff; border: 2px solid #8e44ad; border-radius: 8px; padding: 20px; margin-bottom: 25px;">
        <h3 style="color:#8e44ad;">Announcement Scheduler</h3>
       
        <select id="mp3_select" class="submit" style="width: 320px; border:2px solid #444;">
            <option value="">-- Select Audio File --</option>
        </select>
       
        <div style="margin:15px 0;">
            <input type="button" class="submit" value="Install Announcement" id="install_announcement">
            <input type="button" class="submit" value="Delete" id="delete_mp3" style="background-color:#dc3545; color:white;">
            <input type="button" class="submit" value="Local Play" id="local_play_mp3" style="background:#28a745;color:white;">
            <input type="button" class="submit" value="Global Play" id="global_play_mp3" style="background:#fd7e14;color:white;">
        </div>
        
        <h4>One-Time / Manual Playback</h4>
        <select id="ul_select" class="submit" style="width: 320px; border:2px solid #444;">
            <option value="">-- Select Announcement --</option>
        </select>
       
        <div style="margin:12px 0;">
            <input type="button" class="submit" value="Play Now" id="run_ul">
            <input type="button" class="submit" value="Delete" id="delete_ul" style="background-color:#dc3545; color:white;">
        </div>
    </div>

    <!-- ====================== CRON MANAGER ====================== -->
    <div class="section" style="background: #ffffff; border: 2px solid #e67e22; border-radius: 8px; padding: 20px;">
        <h3 style="color:#e67e22;">Cron Manager</h3>
       
        <table id="cron_table" border="1" cellpadding="8" style="width:100%; border-collapse: collapse; background:#fff; border: 2px solid #e67e22;">
            <thead style="background:#f1f1f1;">
                <tr>
                    <th>Minute</th>
                    <th>Hour</th>
                    <th>Day</th>
                    <th>Month</th>
                    <th>DOW</th>
                    <th>File</th>
                    <th>Scope</th>
                    <th>Description</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody></tbody>
        </table>
    </div>
</div>

<!-- ====================== JAVASCRIPT ====================== -->
<script>
$(document).ready(function () {
    // ... [All your existing functions remain unchanged until loadCronList] ...

    // ==================== Load Cron List ====================
    function loadCronList() {
        $.getJSON("custom/list_cron.php", function(crons) {
            let table = $("#cron_table tbody");
            table.empty();
            crons.forEach(function(c) {
                let row = $("<tr>");
                let parts = c.time.split(" ");
                row.append($("<td>").text(parts[0] || "*"));
                row.append($("<td>").text(parts[1] || "*"));
                row.append($("<td>").text(parts[2] || "*"));
                row.append($("<td>").text(parts[3] || "*"));
                row.append($("<td>").text(parts[4] || "*"));
                row.append($("<td>").text(c.file));
                row.append($("<td>").text(c.scope ? c.scope.toUpperCase() : ""));
                row.append($("<td>").text(c.desc || ""));

                let isEnabled = !c.raw.startsWith('#');
                let toggleBtn = $("<button>")
                    .text(isEnabled ? "Enabled" : "Disabled")
                    .addClass("submit")
                    .css({ 
                        "background-color": isEnabled ? "#28a745" : "#dc3545", 
                        "color": "white", 
                        "border": "1px solid " + (isEnabled ? "#218838" : "#c82333"), 
                        "margin-right": "8px"
                    })
                    .click(function() {
                        let newState = !isEnabled;
                        if (!confirm(`Really ${newState ? "enable" : "disable"} "${c.file}"?`)) return;
                        $.post("custom/toggle_cron.php", { raw_line: c.raw, enable: newState ? 1 : 0 }, function(data) {
                            alert(data);
                            loadCronList();
                        });
                    });

                let scopeText = (c.scope && c.scope.toLowerCase() === 'global') ? " (Global)" : " (Local)";
                let playBtn = $("<button>")
                    .text("Play Now" + scopeText)
                    .css({ "background": "#28a745", "color": "white", "margin-right": "8px" })
                    .click(function() {
                        let scope = c.scope || "local";
                        if (!confirm("Play '" + c.file + "' now?\n\nScope: " + scope.toUpperCase())) return;
                        $.post("custom/run_announcement.php", { file: c.file, scope: scope }, function(data) { alert(data); })
                        .fail(function() { alert("Play Now failed."); });
                    });

                let editBtn = $("<button>").text("Edit").css({ "background": "#ffc107", "color": "black", "margin-right": "8px" }).click(function() {
                    // ... your existing edit code ...
                    let isNthJob = c.raw.includes("/bin/bash -c") && c.raw.includes("date +\\%d");
                    if (isNthJob) {
                        alert("Editing is not available for advanced/nth-week scheduled cron jobs.\n\nTo change this job, please delete it and create a new one.");
                        return;
                    }
                    let parts = c.time.split(" ");
                    let min = prompt("Minute:", parts[0]); if(min===null) return;
                    let hour = prompt("Hour:", parts[1]); if(hour===null) return;
                    let dom = prompt("DOM:", parts[2]); if(dom===null) return;
                    let month = prompt("Month:", parts[3]); if(month===null) return;
                    let dow = prompt("DOW:", parts[4]); if(dow===null) return;
                    $.post("custom/update_announcement.php", {
                        raw_line: c.raw,
                        min, hour, dom, month, dow
                    }, function(resp) {
                        alert(resp);
                        loadCronList();
                    }).fail(function() { alert("Edit failed."); });
                });

                let delBtn = $("<button>").text("Delete").css({ "background": "#dc3545", "color": "white" }).click(function() {
                    if (!confirm("Delete " + c.file + "?")) return;
                    $.post("custom/delete_announcement.php", { raw_line: c.raw }, function(data) {
                        alert(data);
                        loadCronList();
                    });
                });

                row.append($("<td>").append(toggleBtn, playBtn, editBtn, delBtn));
                table.append(row);
            });
        });
    }
    loadCronList();
});
</script>
<?php } ?>
