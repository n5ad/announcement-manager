<?php

// allmon-announcement.inc

// Include this file in Allmon3 after successful login

if (!isset($_SESSION['loggedin']) || $_SESSION['loggedin'] !== true) {

    return;

}

?>

<hr style="margin-top:30px;">

<div id="bottom_cron_section" style="margin-top:20px;">


<h3>Generate Announcement with Piper TTS</h3>

<textarea id="tts_text" rows="4" cols="60" placeholder="Enter the text you want to convert to speech. Commas and periods are accepted, no other punctuation is allowed..."></textarea><br><br>

<label for="tts_filename">Filename (one word, letters/numbers/hyphen/underscore only, no extension):</label><br>

<input type="text" id="tts_filename" placeholder="e.g. morningnet" style="width:220px;">


<!-- Added voice selection dropdown -->

<br><br>

<label for="tts_voice">Please select your voice model:</label>

<select id="tts_voice" class="submit" style="width: 250px;">

    <option value="/opt/piper/voices/en_US-lessac-medium.onnx">Lessac (Medium, American female)</option>

    <option value="/opt/piper/voices/en_US-joe-medium.onnx">Joe (Medium, American male)</option>

    <option value="/opt/piper/voices/en_US-amy-medium.onnx">Amy (Medium, American female)</option>

    <option value="/opt/piper/voices/en_US-kristin-medium.onnx">Kristin (Medium, American female)</option>

    <option value="/opt/piper/voices/en_US-libritts_r-medium.onnx">Libritts_r (Medium, British female)</option>
    
    <option value="/opt/piper/voices/en_US-ryan-high.onnx">Ryan (High, British female)</option>

    <!-- Add more here as you download them -->

</select>


<br><br>


<button id="generate_tts" class="submit">Generate WAV File</button>

<div id="tts_status" style="margin-top:10px; font-weight:bold; color:#28a745;"></div>


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

$(document).ready(function() {


    function loadMP3Dropdown() {

        $.getJSON("/supermon/custom/list_mp3.php", function(files) {

            $("#mp3_select").empty().append($("<option>").val("").text("-- Select Audio File (MP3/WAV) --"));

            $.each(files, function(i, f) { $("#mp3_select").append($("<option>").val(f).text(f)); });

        }).fail(function() { console.error("Failed to load MP3 list"); });

    }

    loadMP3Dropdown();


    function loadULDropdown() {

        $.getJSON("/supermon/custom/list_ul.php", function(files) {

            $("#ul_select").empty().append($("<option>").val("").text("-- Select .ul Announcement --"));

            $.each(files, function(i, f) { $("#ul_select").append($("<option>").val(f).text(f)); });

        }).fail(function() { console.error("Failed to load UL list"); });

    }

    loadULDropdown();


    // ====== Cron List with Separated Time Columns & Toggle ======

    function loadCronList() {

        $.getJSON("/supermon/custom/list_cron.php", function(crons) {

            let table = $("#cron_table tbody");

            table.empty();

            crons.forEach(function(c) {

                let row = $("<tr>");

                // Split time into separate columns

                let timeParts = c.time.split(" ");

                row.append($("<td>").text(timeParts[0] || "*")); // Minute

                row.append($("<td>").text(timeParts[1] || "*")); // Hour

                row.append($("<td>").text(timeParts[2] || "*")); // Day of Month

                row.append($("<td>").text(timeParts[3] || "*")); // Month

                row.append($("<td>").text(timeParts[4] || "*")); // Day of Week

                row.append($("<td>").text(c.file));

                row.append($("<td>").text(c.desc || ""));

                // Toggle Enabled/Disabled

                let isEnabled = !c.raw.startsWith('#');

                let toggleBtn = $("<button>")

                    .text(isEnabled ? "Enabled" : "Disabled")

                    .css({

                        "background-color": isEnabled ? "#28a745" : "#dc3545",

                        "color": "white",

                        "border": "1px solid " + (isEnabled ? "#218838" : "#c82333"),

                        "padding": "6px 12px",

                        "cursor": "pointer",

                        "border-radius": "4px",

                        "font-weight": "bold"

                    })

                    .hover(

                        function() { $(this).css("background-color", isEnabled ? "#218838" : "#c82333"); },

                        function() { $(this).css("background-color", isEnabled ? "#28a745" : "#dc3545"); }

                    )

                    .click(function() {

                        let $this = $(this);

                        let newState = !isEnabled;

                        let action = newState ? "enable" : "disable";

                        if (!confirm(`Really ${action} "${c.file}"?`)) return;

                        $this.prop("disabled", true).text("Updatingâ€¦");

                        $.post("/supermon/custom/toggle_cron.php", {

                            raw_line: c.raw,

                            enable: newState ? 1 : 0

                        }, function(data) {

                            alert(data);

                            loadCronList();

                        }).fail(function() {

                            alert("Toggle failed.");

                        }).always(function() {

                            $this.prop("disabled", false);

                        });

                    });

                row.append($("<td>").append(toggleBtn));

                let acts = $("<td>");

                $("<button>").text("Play").addClass("small-btn play").click(function(){

                    $.post("/supermon/custom/run_announcement.php", {file: c.file}, function(r){ alert(r); });

                }).appendTo(acts);

                $("<button>").text("Edit").addClass("small-btn edit").click(function(){

                    let min = prompt("Minute", timeParts[0]); if(min===null)return;

                    let hour = prompt("Hour", timeParts[1]); if(hour===null)return;

                    let dom = prompt("Day", timeParts[2]); if(dom===null)return;

                    let month = prompt("Month", timeParts[3]); if(month===null)return;

                    let dow = prompt("DOW", timeParts[4]); if(dow===null)return;

                    $.post("/supermon/custom/update_announcement.php", {

                        raw_line: c.raw, min, hour, dom, month, dow

                    }, function(resp){ alert(resp); loadCronList(); });

                }).appendTo(acts);

                $("<button>").text("Delete").addClass("small-btn del").click(function(){

                    if(!confirm("Delete cron for "+c.file+"?")) return;

                    $.post("/supermon/custom/delete_announcement.php", {raw_line: c.raw}, function(r){

                        alert(r); loadCronList();

                    });

                }).appendTo(acts);

                row.append(acts);

                table.append(row);

            });

        }).fail(function() { console.error("Failed to load cron list"); });

    }

    loadCronList();


    // Install Announcement

    $("#install_announcement").click(function() {

        let mp3 = $("#mp3_select").val();

        if (!mp3) { alert("Please select an MP3 file."); return; }

        let min = prompt("Minute: Which minute of the hour? ", "45"); if(min===null) return;

        let hour = prompt("Hour: Which hours do you want? * = every hour or a specific like 8,13,20 or a range like 8-20", "8-21"); if(hour===null) return;

        let dom = prompt("DOM: Which days of the month? * is everyday or a specific like 1,7,18 or 1-17 as a range", "1-24"); if(dom===null) return;

        let month = prompt("Month: Which month(s) * = every or specific like 3 or a range like 7-10", "*"); if(month===null) return;

        let dow = prompt("DOW: Which day(s) of the week? Sun=1 Sat =7 you can choose all * or a specific or a range", "*"); if(dow===null) return;

        let desc = prompt("Description: Name this announcement", "Scheduled announcement");

        if (!desc || desc.trim()==="") {

            alert("Description required.");

            return;

        }

        $.post("/supermon/custom/announcement.php", {

            file: mp3,

            min: min,

            hour: hour,

            dom: dom,

            month: month,

            dow: dow,

            desc: desc

        }, function(data) {

            alert(data);

            loadULDropdown();

            loadCronList();

            loadMP3Dropdown();

        });

    });


    // Delete MP3/WAV

    $("#delete_mp3").click(function() {

        let f = $("#mp3_select").val();

        if (!f || !confirm("Delete " + f + "?")) return;

        $.post("/supermon/custom/delete_file.php", {type:"mp3", file:f}, function(r){

            alert(r);

            loadMP3Dropdown();

        });

    });


    // Local Play for MP3/WAV â€” uses source: "mp3"

    $("#local_play_mp3").click(function() {

        let f = $("#mp3_select").val();

        if (!f) {

            alert("Select an MP3/WAV file first");

            return;

        }

        $.post("/supermon/custom/run_announcement.php", { 

            file: f,

            source: "mp3"   // Tells PHP to use /mp3/ directory

        }, function(r){

            alert(r);

        }).fail(function() {

            alert("Local play request failed - check server logs or permissions.");

        });

    });


    // Global Play for MP3/WAV â€” uses source: "mp3"

    $("#global_play_mp3").click(function() {

        let f = $("#mp3_select").val();

        if (!f) {

            alert("Select an MP3/WAV file first");

            return;

        }

        if (!confirm("WARNING: This announcement will play on this node AND all currently linked nodes.\n\nContinue?")) {

            return;

        }

        $.post("/supermon/custom/globalplay.php", { 

            file: f,

            source: "mp3"   // Tells globalplay.php to use /mp3/

        }, function(r){

            alert(r);

        }).fail(function() {

            alert("Global play request failed - check server logs or permissions.");

        });

    });


    // Play Now for UL files â€” uses source: "ul"

    $("#run_ul").click(function() {

        let f = $("#ul_select").val();

        if (!f) { alert("Select announcement first"); return; }

        $.post("/supermon/custom/run_announcement.php", { 

            file: f,

            source: "ul"    // Tells PHP to use announcements/ subdirectory

        }, function(r){ 

            alert(r); 

        }).fail(function() {

            alert("Play Now failed - check server logs or permissions.");

        });

    });


    // Delete .ul

    $("#delete_ul").click(function() {

        let f = $("#ul_select").val();

        if (!f || !confirm("Delete " + f + "?")) return;

        $.post("/supermon/custom/delete_file.php", {type:"ul", file:f}, function(r){

            alert(r);

            loadULDropdown();

        });

    });


    // Generate TTS

    $("#generate_tts").click(function() {

        let text = $("#tts_text").val().trim();

        let fn = $("#tts_filename").val().trim();

        if (!text || !fn) { alert("Text and filename required"); return; }

        if (!/^[a-zA-Z0-9_-]+$/.test(fn)) {

            alert("Filename: letters, numbers, hyphen or underscore only");

            return;

        }

        $.post("/supermon/custom/piper_generate.php", {text: text, filename: fn}, function(resp) {

            $("#tts_status").text(resp);

            if (resp.includes("Success")) {

                setTimeout(loadMP3Dropdown, 1500);

            }

        });

    });

});

</script>


<style>

    .small-btn { font-size:0.9em; padding:4px 8px; margin:0 3px; cursor:pointer; }

    .play { background:#17a2b8; color:white; border:none; }

    .edit { background:#ffc107; color:black; border:none; }

    .del { background:#dc3545; color:white; border:none; }

    .submit { padding:6px 12px; margin:4px; }

    #local_play_mp3:hover, #global_play_mp3:hover { opacity:0.85; }

    #cron_table th, #cron_table td {

        border: 1px solid #666;

        padding: 8px 10px;

        text-align: center;

    }

    #cron_table tbody tr:nth-child(even) {

        background-color: #f9f9f9;

    }

    #cron_table tbody tr:hover {

        background-color: #f0f8ff;

    }

    #cron_table td {

        white-space: nowrap;

        overflow: hidden;

        text-overflow: ellipsis;

        max-width: 140px;

    }

    #cron_table td:last-child {

        white-space: normal;

        max-width: none;

    }

</style>

<?php } ?>
