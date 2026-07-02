<div>

<?php

// allmon-announcement.inc

// Allmon3 Version - June 2026 - Full features + Smart Cron Play Now

?>

<hr style="margin-top:30px;">

<div id="bottom_cron_section" style="margin-top:20px;">


    <h3>Generate Announcement with Piper TTS</h3>

    <textarea id="tts_text" rows="4" cols="60" placeholder="Enter the text you want to convert to speech. Commas and periods are acceptable, no other punctuation is allowed..."></textarea>

    <br><br>

    <label for="tts_filename">Filename (letters/numbers/hyphen/underscore only, no extension):</label><br>

    <input type="text" id="tts_filename" placeholder="e.g. morningnet" style="width:220px;">

    <br><br>

    <label for="tts_voice">Select voice model:</label>

    <select id="tts_voice" class="submit" style="width: 250px;">

        <option value="/opt/piper/voices/en_US-lessac-medium.onnx">Lessac (Medium, American female)</option>

        <option value="/opt/piper/voices/en_US-joe-medium.onnx">Joe (Medium, American male)</option>

        <option value="/opt/piper/voices/en_US-amy-medium.onnx">Amy (Medium, American female)</option>

        <option value="/opt/piper/voices/en_US-kristin-medium.onnx">Kristin (Medium, American female)</option>

        <option value="/opt/piper/voices/en_US-libritts_r-medium.onnx">Libritts_r (Medium, British female)</option>

        <option value="/opt/piper/voices/en_US-ryan-low.onnx">Ryan (low, British female)</option>

    </select>

    <button id="generate_tts" class="submit">Generate WAV File</button>

    <div id="tts_status" style="margin-top:10px; font-weight:bold; color:#28a745;"></div>


    <!-- MP3 Upload Section -->

    <h4>Alternatively, you may upload your own MP3/WAV files created elsewhere</h4>

    <h4>you may use these for scheduling or immediate playback</h4>

    <div style="position:relative; display:inline-block;">

        <input type="file" id="mp3_upload" accept=".mp3,.wav" style="margin-bottom:8px;">

        <button id="upload_btn" class="submit">Upload to /mp3/</button>

    </div>

    <div id="upload_status" style="margin-top:10px; font-weight:bold;"></div>

    <br><br>


    <h3>Announcement Scheduler</h3>

    <select id="mp3_select" class="submit" style="border:2px solid #444; width: 280px;">

        <option value="">-- Select Audio File (MP3 or WAV) --</option>

    </select>

    <input type="button" class="submit" value="Install Announcement" id="install_announcement">

    <input type="button" class="submit" value="Delete MP3/WAV" id="delete_mp3" style="background:#dc3545;color:white;border:1px solid #c82333;">

    <input type="button" class="submit" value="Local Play" id="local_play_mp3" style="background:#28a745;color:white;border:1px solid #1e7e34;">

    <input type="button" class="submit" value="Global Play" id="global_play_mp3" style="background:#fd7e14;color:white;border:1px solid #e06b00;">

    <br><br>


    <select id="ul_select" class="submit" style="border:2px solid #444; width: 280px;">

        <option value="">-- Select .ul Announcement --</option>

    </select>

    <input type="button" class="submit" value="Play Now" id="run_ul">

    <input type="button" class="submit" value="Delete .ul" id="delete_ul" style="background:#dc3545;color:white;border:1px solid #c82333;">

    <br><br>


    <h3>Cron Manager</h3>

    <div style="overflow-x:auto; margin-top:10px;">

        <table id="cron_table" style="width:100%; border-collapse:collapse; border:1px solid #666; font-size:0.95em;">

            <thead style="background:#444;color:white;">

                <tr>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">Min</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">Hour</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">DOM</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">Mon</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">DOW</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">File</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">Scope</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">Description</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">Enabled</th>

                    <th style="padding:8px; border:1px solid #666; text-align:center;">Actions</th>

                </tr>

            </thead>

            <tbody></tbody>

        </table>

    </div>

</div>


<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>

<script>

$(document).ready(function () {


    // ==================== MP3 Upload ====================

    $("#upload_btn").on('click', function() {

        let fileInput = $("#mp3_upload")[0];

        if (!fileInput.files.length) { alert("Please select a file first."); return; }

        let formData = new FormData();

        formData.append("file", fileInput.files[0]);

        $("#upload_status").text("Uploading...").css("color", "orange");


        $.ajax({

            url: "/announcement-manager/upload_mp3.php",

            type: "POST",

            data: formData,

            processData: false,

            contentType: false,

            success: function(response) {

                $("#upload_status").html(response).css("color", "green");

                loadMP3Dropdown();

            },

            error: function() { $("#upload_status").text("❌  Upload failed.").css("color", "red"); }

        });

    });


    function loadMP3Dropdown() {

        $.getJSON("/announcement-manager/list_mp3.php", function(files) {

            $("#mp3_select").empty().append($("<option>").val("").text("-- Select Audio File (MP3/WAV) --"));

            $.each(files, function(i, f) { $("#mp3_select").append($("<option>").val(f).text(f)); });

        });

    }

    loadMP3Dropdown();


    function loadULDropdown() {

        $.getJSON("/announcement-manager/list_ul.php", function(files) {

            $("#ul_select").empty().append($("<option>").val("").text("-- Select .ul Announcement --"));

            $.each(files, function(i, f) { $("#ul_select").append($("<option>").val(f).text(f)); });

        });

    }

    loadULDropdown();


    // ==================== Cron List with Smart Play Now ====================

    function loadCronList() {

        $.getJSON("/announcement-manager/list_cron.php", function(crons) {

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


                let scope = (c.scope || "local").toLowerCase();

                let scopeText = scope.toUpperCase();

                let scopeCell = $("<td>").text(scopeText).css({

                    fontWeight: "bold",

                    color: scope === "global" ? "#fd7e14" : "#28a745"

                });

                row.append(scopeCell);

                row.append($("<td>").text(c.desc || ""));


                let isEnabled = !c.raw.startsWith('#');

                let toggleBtn = $("<button>")

                    .text(isEnabled ? "Enabled" : "Disabled")

                    .css({backgroundColor: isEnabled ? "#28a745" : "#dc3545", color:"white", padding:"6px 12px", cursor:"pointer", borderRadius:"4px"})

                    .click(function() {

                        let newState = !isEnabled;

                        if (!confirm(`Really ${newState ? "enable" : "disable"} "${c.file}"?`)) return;

                        $.post("/announcement-manager/toggle_cron.php", {raw_line: c.raw, enable: newState?1:0}, function(data){ alert(data); loadCronList(); });

                    });

                row.append($("<td>").append(toggleBtn));


                let acts = $("<td>").css("white-space", "nowrap");

                let playBtn = $("<button>").text("Play Now (" + scopeText + ")")

                    .addClass("small-btn play")

                    .click(function() {

                        if (!confirm("Play '" + c.file + "' now?\n\nScope: " + scopeText)) return;

                        let url = (scope === "global") ? "/announcement-manager/globalplay.php" : "/announcement-manager/run_announcement.php";

                        $.post(url, { file: c.file, source: "mp3" }, function(r){ alert(r); })

                        .fail(function(){ alert("Play failed."); });

                    });

                acts.append(playBtn);


                $("<button>").text("Edit").addClass("small-btn edit").click(function() {

                    if (c.raw.includes("/bin/bash -c") && c.raw.includes("date +\\%d")) {

                        alert("Editing not available for nth-week jobs. Delete and recreate.");

                        return;

                    }

                    let p = c.time.split(" ");

                    let min = prompt("Minute:", p[0]); if(min===null) return;

                    let hour = prompt("Hour:", p[1]); if(hour===null) return;

                    let dom = prompt("DOM:", p[2]); if(dom===null) return;

                    let month = prompt("Month:", p[3]); if(month===null) return;

                    let dow = prompt("DOW:", p[4]); if(dow===null) return;

                    $.post("/announcement-manager/update_announcement.php", {raw_line:c.raw, min, hour, dom, month, dow}, resp => {alert(resp); loadCronList();});

                }).appendTo(acts);


                $("<button>").text("Delete").addClass("small-btn del").click(() => {

                    if (!confirm("Delete cron for " + c.file + "?")) return;

                    $.post("/announcement-manager/delete_announcement.php", {raw_line: c.raw}, r => {alert(r); loadCronList();});

                }).appendTo(acts);


                row.append(acts);

                table.append(row);

            });

        });

    }

    loadCronList();


    // ==================== Install Announcement ====================

    $("#install_announcement").on('click', function() {

        let mp3 = $("#mp3_select").val();

        if (!mp3) { alert("Please select an MP3 file."); return; }


        let min = prompt("Minute:", "45"); if(min===null) return;

        let hour = prompt("Hour:", "7-20"); if(hour===null) return;

        let dom = prompt("DOM:", "*"); if(dom===null) return;

        let month = prompt("Month:", "*"); if(month===null) return;

        let dow = prompt("DOW:", "*"); if(dow===null) return;


        let week = "*"; let useNth = false;

        if (/^[1-7]$/.test(dow.trim())) {

            week = prompt("Week of month? (* = every, 1-5 = specific)", "*");

            if (week !== null && /^[1-5]$/.test(week.trim())) useNth = true;

        }


        let desc = prompt("Description:", "Scheduled announcement");

        if (!desc || !desc.trim()) { alert("Description required."); return; }


        let scopeInput = prompt("Playback scope? (local or global)", "local");

        let scope = (scopeInput && scopeInput.trim().toLowerCase() === "global") ? "global" : "local";

        if (scope === "global" && !confirm("GLOBAL → will play on ALL connected nodes. Continue?")) return;


        let modeChoice = prompt("Play Mode?\n\npolite = Wait for clear channel (recommended)\npriority = Play immediately", "polite");

        let mode = (modeChoice && modeChoice.trim().toLowerCase() === "priority") ? "priority" : "polite";


        $.post("/announcement-manager/announcement.php", {

            file: mp3, min, hour, dom, month, dow, week,

            use_nth: useNth ? 1 : 0, desc, scope, mode

        }, function(data) {

            alert(data);

            loadULDropdown();

            loadCronList();

            loadMP3Dropdown();

        });

    });


    // ==================== TTS Generate ====================

    $("#generate_tts").on('click', function() {

        let text = $("#tts_text").val().trim();

        let fn = $("#tts_filename").val().trim();

        let voice = $("#tts_voice").val();

        if (!text || !fn) { alert("Text and filename are required"); return; }

        if (!/^[a-zA-Z0-9_-]+$/.test(fn)) {

            alert("Filename: letters, numbers, hyphen or underscore only");

            return;

        }

        $("#tts_status").text("Generating WAV... Please wait").css("color", "#28a745");

        $.post("/announcement-manager/piper_generate.php", { text: text, filename: fn, voice: voice })

            .done(function(resp) {

                $("#tts_status").text(resp);

                if (resp.toLowerCase().includes("success") || resp.toLowerCase().includes("generated")) {

                    $("#tts_text").val("");

                    $("#tts_filename").val("");

                    setTimeout(loadMP3Dropdown, 1200);

                }

            })

            .fail(function() {

                $("#tts_status").text("Failed to generate WAV - check logs").css("color", "#dc3545");

            });

    });


    // ==================== Other Buttons ====================

    $("#delete_mp3").on('click', function() {

        let f = $("#mp3_select").val();

        if (!f || !confirm("Delete " + f + "?")) return;

        $.post("/announcement-manager/delete_file.php", {type:"mp3", file:f}, function(r){ alert(r); loadMP3Dropdown(); });

    });


    $("#local_play_mp3").on('click', function() {

        let f = $("#mp3_select").val();

        if (!f) { alert("Select an MP3/WAV file first"); return; }

        $.post("/announcement-manager/run_announcement.php", { file: f, source: "mp3" }, function(r){ alert(r); });

    });


    $("#global_play_mp3").on('click', function() {

        let f = $("#mp3_select").val();

        if (!f) { alert("Select an MP3/WAV file first"); return; }

        if (!confirm("WARNING: This will play on this node AND all linked nodes.\n\nContinue?")) return;

        $.post("/announcement-manager/globalplay.php", { file: f, source: "mp3" }, function(r){ alert(r); });

    });


    $("#run_ul").on('click', function() {

        let f = $("#ul_select").val();

        if (!f) { alert("Select announcement first"); return; }

        $.post("/announcement-manager/run_announcement.php", { file: f, source: "ul" }, function(r){ alert(r); });

    });


    $("#delete_ul").on('click', function() {

        let f = $("#ul_select").val();

        if (!f || !confirm("Delete " + f + "?")) return;

        $.post("/announcement-manager/delete_file.php", {type:"ul", file:f}, function(r){ alert(r); loadULDropdown(); });

    });


});

</script>


<style>

    .small-btn { font-size:0.9em; padding:4px 8px; margin:0 3px; cursor:pointer; border:none; border-radius:4px; }

    .play { background:#17a2b8; color:white; }

    .edit { background:#ffc107; color:black; }

    .del { background:#dc3545; color:white; }

    .submit { padding:6px 12px; margin:4px; }

    #cron_table th, #cron_table td { border: 1px solid #666; padding: 8px 10px; text-align: center; }

    #cron_table tbody tr:nth-child(even) { background-color: #f9f9f9; }

    #cron_table tbody tr:hover { background-color: #f0f8ff; }

</style>

</div>
