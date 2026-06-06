<?php
// announcement-frame.inc - FULL STANDALONE VERSION
// created by N5AD, June 2026
// for future implementation of Ifframe in supermon
// rather than modifying link.php
ini_set('display_errors', 1);
error_reporting(E_ALL & ~E_NOTICE);
?>

<hr style="margin-top:30px;">
<div id="bottom_cron_section" style="margin-top:20px;">
<h1>Announcement Management System</h1>

<!-- Piper TTS Section -->
<h3>Generate Announcement with Piper TTS</h3>
<textarea id="tts_text" rows="4" cols="60" placeholder="Enter the text you want to convert to speech. Commas and periods are acceptable, no other punctuation is allowed..."></textarea>
<br><br>
<label for="tts_filename">Letters, numbers, hyphens and underscores accepted. Don't use extensions:</label>
<input type="text" id="tts_filename" placeholder="e.g., myannounce" style="width: 200px;">
<br><br>
<label for="tts_voice">Please select your voice model:</label>
<select id="tts_voice" class="submit" style="width: 250px;">
    <option value="/opt/piper/voices/en_US-lessac-medium.onnx">Lessac (Medium, American female)</option>
    <option value="/opt/piper/voices/en_US-joe-medium.onnx">Joe (Medium, American male)</option>
    <option value="/opt/piper/voices/en_US-amy-medium.onnx">Amy (Medium, American female)</option>
    <option value="/opt/piper/voices/en_US-kristin-medium.onnx">Kristin (Medium, American female)</option>
    <option value="/opt/piper/voices/en_US-libritts_r-medium.onnx">Libritts_r (Medium, British female)</option>
    <option value="/opt/piper/voices/en_US-ryan-high.onnx">Ryan (High, British female)</option>
</select>
<br><br>
<button id="generate_tts" class="submit">Generate WAV File</button>
<div id="tts_status" style="margin-top:10px; font-weight:bold; color:#28a745;"></div>

<!-- MP3 Upload -->
<h4>Alternatively, you may upload your own MP3/WAV files</h4>
<div style="position:relative; display:inline-block;">
    <input type="file" id="mp3_upload" accept=".mp3,.wav" style="margin-bottom:8px;">
    <button id="upload_btn" class="submit">Upload to /mp3/</button>
</div>
<div id="upload_status" style="margin-top:10px; font-weight:bold;"></div>
<br><br>

<h3>Announcement Scheduler</h3>
<select id="mp3_select" class="submit" style="border:2px solid #444; width: 250px;">
    <option value="">-- Select Audio File (MP3-WAV) --</option>
</select>
<input type="button" class="submit" value="Install Announcement" id="install_announcement">
<input type="button" class="submit" value="Delete MP3" id="delete_mp3" style="background-color:#dc3545; color:white;">
<input type="button" class="submit" value="Local Play" id="local_play_mp3" style="background:#28a745;color:white;">
<input type="button" class="submit" value="Global Play" id="global_play_mp3" style="background:#fd7e14;color:white;">
<br><br>

<select id="ul_select" class="submit" style="border:2px solid #444; width: 250px;">
    <option value="">-- Select Announcement --</option>
</select>
<input type="button" class="submit" value="Play Now" id="run_ul">
<input type="button" class="submit" value="Delete UL" id="delete_ul" style="background-color:#dc3545; color:white;">
<br><br>

<h3>Cron Manager</h3>
<table id="cron_table" border="1" cellpadding="5" style="width:100%; border-collapse: collapse;">
    <thead>
        <tr>
            <th>Minute</th><th>Hour</th><th>Day of Month</th><th>Month</th><th>Day of Week</th>
            <th>Name of File</th><th>Scope</th><th>Description</th><th>Enabled</th><th>Actions</th>
        </tr>
    </thead>
    <tbody></tbody>
</table>
</div>

<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
<script>
// Absolute path for all AJAX calls
const customPath = "/supermon/custom/";

$(document).ready(function () {

    function loadMP3Dropdown() {
        $.getJSON(customPath + "list_mp3.php", function(files) {
            $("#mp3_select").empty().append($("<option>").val("").text("-- Select Audio File (MP3-WAV) --"));
            files.forEach(f => $("#mp3_select").append($("<option>").val(f).text(f)));
        });
    }
    loadMP3Dropdown();

    function loadULDropdown() {
        $.getJSON(customPath + "list_ul.php", function(files) {
            $("#ul_select").empty().append($("<option>").val("").text("-- Select Announcement --"));
            files.forEach(f => $("#ul_select").append($("<option>").val(f).text(f)));
        });
    }
    loadULDropdown();

    function loadCronList() {
        $.getJSON(customPath + "list_cron.php", function(crons) {
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
                row.append($("<td>").text(c.scope ? c.scope.toUpperCase() : "—"));
                row.append($("<td>").text(c.desc || ""));

                let isEnabled = !c.raw.startsWith('#');
                let toggleBtn = $("<button>").text(isEnabled ? "Enabled" : "Disabled")
                    .css({backgroundColor: isEnabled ? "#28a745" : "#dc3545", color: "white"})
                    .click(function() {
                        let newState = !isEnabled;
                        if (!confirm(`Really ${newState ? "enable" : "disable"} "${c.file}"?`)) return;
                        $.post(customPath + "toggle_cron.php", { raw_line: c.raw, enable: newState ? 1 : 0 }, function(data) {
                            alert(data);
                            loadCronList();
                        });
                    });

                let scopeText = (c.scope && c.scope.toLowerCase() === 'global') ? " (Global)" : " (Local)";
                let playBtn = $("<button>").text("Play Now" + scopeText)
                    .css({background: "#28a745", color: "white", marginRight: "8px"})
                    .click(function() {
                        let scope = c.scope || "local";
                        if (!confirm("Play '" + c.file + "' now?\nScope: " + scope.toUpperCase())) return;
                        $.post(customPath + "run_announcement.php", { file: c.file, scope: scope }, function(data) { alert(data); });
                    });

                let editBtn = $("<button>").text("Edit").css({background: "#ffc107", color: "black", marginRight: "8px"})
                    .click(function() {
                        let isNthJob = c.raw.includes("/bin/bash -c") && c.raw.includes("date +\\%d");
                        if (isNthJob) { alert("Editing not available for nth-week jobs."); return; }
                        let parts = c.time.split(" ");
                        let min = prompt("Minute:", parts[0]); if(min===null) return;
                        let hour = prompt("Hour:", parts[1]); if(hour===null) return;
                        let dom = prompt("DOM:", parts[2]); if(dom===null) return;
                        let month = prompt("Month:", parts[3]); if(month===null) return;
                        let dow = prompt("DOW:", parts[4]); if(dow===null) return;
                        $.post(customPath + "update_announcement.php", { raw_line: c.raw, min, hour, dom, month, dow }, function(resp) {
                            alert(resp); loadCronList();
                        });
                    });

                let delBtn = $("<button>").text("Delete").css({background: "#dc3545", color: "white"})
                    .click(function() {
                        if (!confirm("Delete " + c.file + "?")) return;
                        $.post(customPath + "delete_announcement.php", { raw_line: c.raw }, function(data) {
                            alert(data); loadCronList();
                        });
                    });

                row.append($("<td>").append(toggleBtn));
                row.append($("<td>").append(playBtn, editBtn, delBtn));
                table.append(row);
            });
        });
    }
    loadCronList();

    // ==================== All Button Handlers ====================
    $("#upload_btn").click(function() {
        let fileInput = $("#mp3_upload")[0];
        if (!fileInput.files.length) { alert("Please select a file first."); return; }
        let formData = new FormData();
        formData.append("file", fileInput.files[0]);
        $("#upload_status").text("Uploading...").css("color", "orange");
        $.ajax({
            url: customPath + "upload_mp3.php",
            type: "POST",
            data: formData,
            processData: false,
            contentType: false,
            success: function(response) {
                $("#upload_status").html(response).css("color", "green");
                loadMP3Dropdown();
            },
            error: function() { $("#upload_status").text("❌ Upload failed.").css("color", "red"); }
        });
    });

    $("#delete_mp3").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) { alert("Select an MP3 file."); return; }
        if (!confirm("Delete '" + mp3 + "'?")) return;
        $.post(customPath + "delete_file.php", { type: "mp3", file: mp3 }, function(data) {
            alert(data); loadMP3Dropdown();
        });
    });

    $("#local_play_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f) { alert("Select a file first"); return; }
        $.post(customPath + "run_announcement.php", { file: f, source: "mp3" }, function(r){ alert(r); });
    });

    $("#global_play_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f) { alert("Select a file first"); return; }
        if (!confirm("WARNING: This will play on this node AND all linked nodes.")) return;
        $.post(customPath + "globalplay.php", { file: f, source: "mp3" }, function(r){ alert(r); });
    });

    $("#run_ul").click(function() {
        let ulFile = $("#ul_select").val();
        if (!ulFile) { alert("Select a UL file."); return; }
        $.post(customPath + "run_announcement.php", { file: ulFile, source: "ul" }, function(data) { alert(data); });
    });

    $("#delete_ul").click(function() {
        let ul = $("#ul_select").val();
        if (!ul) { alert("Select a UL file."); return; }
        if (!confirm("Delete '" + ul + "'?")) return;
        $.post(customPath + "delete_file.php", { type: "ul", file: ul }, function(data) {
            alert(data); loadULDropdown();
        });
    });

    $("#generate_tts").click(function() {
        let text = $("#tts_text").val().trim();
        let filename = $("#tts_filename").val().trim();
        let voice = $("#tts_voice").val();
        if (!text || !filename) { alert("Text and filename are required."); return; }
        if (!confirm("Generate '" + filename + ".wav'?")) return;
        $("#tts_status").text("Generating...").css("color", "#28a745");
        $.post(customPath + "piper_generate.php", { text: text, filename: filename, voice: voice })
        .done(function(resp) {
            $("#tts_status").text(resp);
            if (resp.toLowerCase().includes("success")) {
                $("#tts_text").val(""); $("#tts_filename").val("");
                setTimeout(loadMP3Dropdown, 1200);
            }
        })
        .fail(function() { $("#tts_status").text("Failed - check logs").css("color", "red"); });
    });

    $("#install_announcement").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) { alert("Select an MP3 file."); return; }
        // (the full prompt logic from your original)
        let min = prompt("Minute:", "45"); if(min===null) return;
        let hour = prompt("Hour:", "7-20"); if(hour===null) return;
        let dom = prompt("DOM:", "*"); if(dom===null) return;
        let month = prompt("Month:", "*"); if(month===null) return;
        let dow = prompt("DOW:", "*"); if(dow===null) return;
        let desc = prompt("Description:", "Scheduled announcement"); if(!desc) return;

        let scope = prompt("Scope (local/global):", "local").toLowerCase() === "global" ? "global" : "local";
        let mode = prompt("Mode (polite/priority):", "polite").toLowerCase() === "priority" ? "priority" : "polite";

        $.post(customPath + "announcement.php", {
            file: mp3, min, hour, dom, month, dow, desc, scope, mode
        }, function(data) {
            alert(data);
            loadULDropdown(); loadCronList(); loadMP3Dropdown();
        });
    });

});
</script>
