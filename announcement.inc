<?php
// announcement.inc
// Loaded only after Supermon login
// Created by N5AD - Updated June 12,2026
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
    <h1 style="text-align:center; color:#2c3e50; margin-top:0;">Announcement Management System</h1>
    <hr style="border: 1px solid #bdc3c7; margin: 20px 0;">
    <!-- ====================== PIPER TTS SECTION ====================== -->
    <div class="section" style="
        background: #ffffff;
        border: 2px solid #3498db;
        border-radius: 8px;
        padding: 20px;
        margin-bottom: 25px;
    ">
        <h3 style="color:#2980b9; margin-top:0;">Generate Announcement with Piper TTS</h3>
        
        <textarea id="tts_text" rows="4" cols="70" 
            placeholder="Enter the text you want to convert to speech. Commas and periods are acceptable, no other punctuation..."></textarea>
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
   <!-- ====================== MP3 UPLOAD SECTION (FULLY CENTERED) ====================== -->
<div class="section" style="
    background: #ffffff;
    border: 2px solid #4caf7a;
    border-radius: 8px;
    padding: 25px;
    margin-bottom: 25px;
    text-align: center;
">
    <h4 style="color:#7dd9a1; margin-top:0;">Upload Your Own MP3 / WAV Files</h4>
    <h6> "this upload will automatically convert mp3 files to wav for playback in ASL3"</h6>
    <div style="
        display: inline-flex; 
        align-items: center; 
        gap: 12px; 
        flex-wrap: wrap; 
        justify-content: center;
        margin: 15px 0;
    ">
        <input type="file" id="mp3_upload" accept=".mp3,.wav" 
               style="background:#1e252f; color:#e0e0e0;">
        <button id="upload_btn" class="submit">Upload to /mp3/</button>
    </div>
    
    <div id="upload_status" style="margin-top:12px; font-weight:bold;"></div>
</div>
    <!-- ====================== SCHEDULER & PLAYBACK ====================== -->
    <div class="section" style="
        background: #ffffff;
        border: 2px solid #8e44ad;
        border-radius: 8px;
        padding: 20px;
        margin-bottom: 25px;
    ">
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
    <div class="section" style="
        background: #ffffff;
        border: 2px solid #e67e22;
        border-radius: 8px;
        padding: 20px;
    ">
        <h3 style="color:#e67e22;">Cron Manager</h3>
        
        <table id="cron_table" border="1" cellpadding="8" style="
            width:100%; 
            border-collapse: collapse; 
            background:#fff;
            border: 2px solid #e67e22;
        ">
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
                    <th>Enabled</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody></tbody>
        </table>
    </div>
</div> <!-- End #announcement-manager -->
<!-- ====================== JAVASCRIPT (unchanged except minor cleanup) ====================== -->
<script>
$(document).ready(function () {
    function loadMP3Dropdown() {
        $.getJSON("custom/list_mp3.php", function(files) {
            $("#mp3_select").empty().append(
                $("<option>").val("").text("-- Select Audio File --")
            );
            files.forEach(function(f) {
                $("#mp3_select").append($("<option>").val(f).text(f));
            });
        });
    }
    loadMP3Dropdown();
    function loadULDropdown() {
        $.getJSON("custom/list_ul.php", function(files) {
            $("#ul_select").empty().append(
                $("<option>").val("").text("-- Select Announcement --")
            );
            files.forEach(function(f) {
                $("#ul_select").append($("<option>").val(f).text(f));
            });
        });
    }
    loadULDropdown();
// ==================== MP3 Upload ====================
$("#upload_btn").click(function() {
    let fileInput = $("#mp3_upload")[0];
    if (!fileInput.files.length) {
        alert("Please select a file first.");
        return;
    }
    let formData = new FormData();
    formData.append("file", fileInput.files[0]);
    $("#upload_status").text("Uploading...").css("color", "orange");
    $.ajax({
        url: "custom/upload_mp3.php",
        type: "POST",
        data: formData,
        processData: false,
        contentType: false,
        success: function(response) {
            $("#upload_status").html(response).css("color", "green");
            loadMP3Dropdown();   // Refresh the MP3 list
        },
        error: function() {
            $("#upload_status").text(" Upload failed.").css("color", "red");
        }
    });
});
    // ==================== Button Handlers ====================
    $("#delete_mp3").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) { alert("Select an MP3 file."); return; }
        if (!confirm("Delete '" + mp3 + "'?")) return;
        $.post("custom/delete_file.php", { type: "mp3", file: mp3 }, function(data) {
            alert(data);
            loadMP3Dropdown();
        }).fail(function() { alert("Delete failed."); });
    });
    $("#local_play_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f) { alert("Select an MP3/WAV file first"); return; }
        $.post("/supermon/custom/run_announcement.php", { file: f, source: "mp3" }, function(r){ alert(r); })
        .fail(function() { alert("Local play failed."); });
    });
    $("#global_play_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f) { alert("Select an MP3/WAV file first"); return; }
        if (!confirm("WARNING: This will play on this node AND all linked nodes.\n\nContinue?")) return;
        $.post("/supermon/custom/globalplay.php", { file: f, source: "mp3" }, function(r){ alert(r); })
        .fail(function() { alert("Global play failed."); });
    });
    $("#run_ul").click(function() {
        let ulFile = $("#ul_select").val();
        if (!ulFile) { alert("Select a UL file."); return; }
        $.post("/supermon/custom/run_announcement.php", { file: ulFile, source: "ul" }, function(data) { alert(data); })
        .fail(function() { alert("Play Now failed."); });
    });
    $("#delete_ul").click(function() {
        let ul = $("#ul_select").val();
        if (!ul) { alert("Select a UL file."); return; }
        if (!confirm("Delete '" + ul + "' (.ul file)?")) return;
        $.post("custom/delete_file.php", { type: "ul", file: ul }, function(data) {
            alert(data);
            loadULDropdown();
        }).fail(function() { alert("Delete failed."); });
    });
    $("#generate_tts").click(function() {
        let text = $("#tts_text").val().trim();
        let filename = $("#tts_filename").val().trim();
        let voice = $("#tts_voice").val();
        if (!text) { alert("Enter some text."); return; }
        if (!filename) { alert("Enter a filename."); return; }
        if (!confirm("Generate '" + filename + ".wav'?")) return;
        $("#tts_status").text("Generating...").css("color", "#28a745");
        $.post("custom/piper_generate.php", { text: text, filename: filename, voice: voice })
        .done(function(resp) {
            $("#tts_status").text(resp);
            if (resp.toLowerCase().includes("success")) {
                $("#tts_text").val("");
                $("#tts_filename").val("");
                setTimeout(loadMP3Dropdown, 1200);
            }
        })
        .fail(function() {
            $("#tts_status").text("Failed - check logs").css("color", "#dc3545");
        });
    });
    // ==================== Install Announcement ====================
    $("#install_announcement").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) { alert("Select an MP3 file."); return; }
        let min = prompt("Minute: Which minute of the hour do you want this to play?", "45"); if(min===null) return;
        let hour = prompt("Hour: Which hour or range of hours do you want this to play?\n* = every hour\nsingle= 12\nselected= 6,10,16\nrange= 7-20\nevery 2 hours between selected = 7-19/2", "7-20"); if(hour===null) return;
        let dom = prompt("DOM: Which day or range of days do you want this to play?\n* = everyday\n1-31\nselected= 1,7,23\nrange= 12-18", "*"); if(dom===null) return;
        let month = prompt("Month: Which Months do you want this to play?\n* = every month\nJan = 1 Dec = 12", "*"); if(month===null) return;
        let dow = prompt("DOW: Which Day or days of the week do you want this to play?\n* = every day\nSun=0 or 7 Sat=6\nor range/list\n\nSingle number 1-7 = nth-week scheduling", "*"); if(dow===null) return;
        let week = "*";
        let useNth = false;
        if (/^[1-7]$/.test(dow.trim())) {
            week = prompt("Week of month?\n* = every week\n1ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¢ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡Ãƒâ€šÃ‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬Ãƒâ€¦Ã¢â‚¬Å“5 = specific week", "*");
            if (week === null) week = "*";
            week = week.trim();
            if (week !== "*" && week !== "" && /^[1-5]$/.test(week)) {
                useNth = true;
            } else {
                week = "*";
            }
        }
        let desc = prompt("Description:", "Scheduled announcement");
        if (!desc || desc.trim()==="") {
            alert("Description required.");
            return;
        }
        // Scope selection
        let scopeChoice = prompt(
            "Playback scope?\n\n" +
            "local = play only on this node\n" +
            "global = play on this node AND all linked nodes\n\n" +
            "Type 'local' or 'global' (default = local)",
            "local"
        );
        if (scopeChoice === null) return;
        scopeChoice = scopeChoice.trim().toLowerCase();
        let scope = (scopeChoice === "global") ? "global" : "local";
        if (scope === "global") {
            if (!confirm("GLOBAL PLAYBACK SELECTED\n\nThis announcement will play across ALL connected nodes.\n\nContinue?")) {
                return;
            }
        }
        // Polite / Priority selection
        let modeChoice = prompt(
            "Play Mode?\n\n" +
            "polite = Wait for clear channel before playing (recommended)\n" +
            "priority = Play immediately (may interrupt ongoing transmission)\n\n" +
            "Type 'polite' or 'priority' (default = polite)",
            "polite"
        );
        if (modeChoice === null) return;
        let mode = (modeChoice.trim().toLowerCase() === "priority") ? "priority" : "polite";
        // Send to announcement.php
        $.post("custom/announcement.php", {
            file: mp3,
            min, hour, dom, month, dow, week,
            use_nth: useNth ? 1 : 0,
            desc,
            scope: scope,
            mode: mode
        }, function(data) {
            alert(data);
            loadULDropdown();
            loadCronList();
            loadMP3Dropdown();
        }).fail(function() {
            alert("Install failed.");
        });
    });
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
                row.append($("<td>").text(c.scope ? c.scope.toUpperCase() : "ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¢ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡Ãƒâ€šÃ‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬Ãƒâ€šÃ‚Â"));
                row.append($("<td>").text(c.desc || ""));
                let isEnabled = !c.raw.startsWith('#');
                let toggleBtn = $("<button>")
                    .text(isEnabled ? "Enabled" : "Disabled")
                    .css({ "background-color": isEnabled ? "#28a745" : "#dc3545", "color": "white", "border": "1px solid " + (isEnabled ? "#218838" : "#c82333"), "padding": "6px 12px", "cursor": "pointer" })
                    .click(function() {
                        let newState = !isEnabled;
                        if (!confirm(`Really ${newState ? "enable" : "disable"} "${c.file}"?`)) return;
                        $.post("custom/toggle_cron.php", { raw_line: c.raw, enable: newState ? 1 : 0 }, function(data) {
                            alert(data);
                            loadCronList();
                        });
                    });
                // === UPDATED PLAY NOW BUTTON ===
                let scopeText = (c.scope && c.scope.toLowerCase() === 'global') ? " (Global)" : " (Local)";
                let playBtn = $("<button>")
                    .text("Play Now" + scopeText)
                    .css({ "background": "#28a745", "color": "white", "margin-right": "8px" })
                    .click(function() {
                        let scope = c.scope || "local";
                        if (!confirm("Play '" + c.file + "' now?\n\nScope: " + scope.toUpperCase())) return;
                        $.post("custom/run_announcement.php", { 
                            file: c.file,
                            scope: scope
                        }, function(data) { 
                            alert(data); 
                        })
                        .fail(function() { alert("Play Now failed."); });
                    });
                let editBtn = $("<button>").text("Edit").css({ "background": "#ffc107", "color": "black", "margin-right": "8px" }).click(function() {
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
                    }).fail(function() {
                        alert("Edit failed.");
                    });
                });
                let delBtn = $("<button>").text("Delete").css({ "background": "#dc3545", "color": "white" }).click(function() {
                    if (!confirm("Delete " + c.file + "?")) return;
                    $.post("custom/delete_announcement.php", { raw_line: c.raw }, function(data) {
                        alert(data);
                        loadCronList();
                    });
                });
                row.append($("<td>").append(toggleBtn));
                row.append($("<td>").append(playBtn).append(editBtn).append(delBtn));
                table.append(row);
            });
        });
    }
    loadCronList();
});
</script>
<?php } ?>
