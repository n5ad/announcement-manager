<?php

// announcement.inc

// Loaded only after Supermon login

?>

<?php if (isset($_SESSION['sm61loggedin']) && $_SESSION['sm61loggedin'] === true) { ?>

<hr style="margin-top:30px;">

<div id="bottom_cron_section" style="margin-top:20px;">

<h1>Announcement Management System</h1>

<!-- Piper TTS Section - MOVED TO TOP -->

<h3>Generate Announcement with Piper TTS</h3>

<textarea id="tts_text" rows="4" cols="60" placeholder="Enter the text you want to convert to speech..."></textarea>

<br><br>

<label for="tts_filename">Letters, numbers, hyphens and underscores accepted. Don't use extensions:</label>

<input type="text" id="tts_filename" placeholder="e.g., myannounce" style="width: 200px;">

<br><br>


<!-- NEW: Voice dropdown (added here, no layout change) -->

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

<!-- MP3 Section -->

<select id="mp3_select" class="submit" style="border:2px solid #444; width: 250px;">

    <option value="">-- Select Audio File (MP3-WAV) --</option>

</select>

<input type="button" class="submit" value="Install Announcement" id="install_announcement">

<input type="button" class="submit" value="Delete MP3" id="delete_mp3" style="background-color:#dc3545; color:white; border:1px solid #c82333;">

<input type="button" class="submit" value="Local Play" id="local_play_mp3" style="background:#28a745;color:white;border:1px solid #1e7e34;">

<input type="button" class="submit" value="Global Play" id="global_play_mp3" style="background:#fd7e14;color:white;border:1px solid #e06b00;">

<br><br>

<!-- UL Section -->

<select id="ul_select" class="submit" style="border:2px solid #444; width: 250px;">

    <option value="">-- Select Announcement --</option>

</select>

<input type="button" class="submit" value="Play Now" id="run_ul">

<input type="button" class="submit" value="Delete UL" id="delete_ul" style="background-color:#dc3545; color:white; border:1px solid #c82333;">

<br><br>

<h3>Cron Manager</h3>

<table id="cron_table" border="1" cellpadding="5" style="width:100%; border-collapse: collapse;">

    <thead>

        <tr>

            <th>Minute</th>

            <th>Hour</th>

            <th>Day of Month</th>

            <th>Month</th>

            <th>Day of Week</th>

            <th>Name of File</th>

            <th>Description</th>

            <th>Enabled</th>

            <th>Actions</th>

        </tr>

    </thead>

    <tbody>

        <!-- Cron entries will be populated here -->

    </tbody>

</table>

</div>

<!-- JavaScript -->

<script>

$(document).ready(function () {

    // ====== MP3 Dropdown ======

    function loadMP3Dropdown() {

        $.getJSON("custom/list_mp3.php", function(files) {

            $("#mp3_select").empty().append(

                $("<option>").val("").text("-- Select Audio File (MP3-WAV) --")

            );

            files.forEach(function(f) {

                $("#mp3_select").append($("<option>").val(f).text(f));

            });

        });

    }

    loadMP3Dropdown();

    // ====== UL Dropdown ======

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

    // ====== Delete MP3 ======

    $("#delete_mp3").click(function() {

        let mp3 = $("#mp3_select").val();

        if (!mp3) { alert("Select an MP3 file."); return; }

        if (!confirm("Delete '" + mp3 + "'?")) return;

        $.post("custom/delete_file.php", { type: "mp3", file: mp3 }, function(data) {

            alert(data);

            loadMP3Dropdown();

        }).fail(function() { alert("Delete failed."); });

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

            source: "mp3" // Tells PHP to use /mp3/ directory

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

            source: "mp3" // Assuming globalplay.php also supports source

        }, function(r){

            alert(r);

        }).fail(function() {

            alert("Global play request failed - check server logs or permissions.");

        });

    });

    // Play Now for UL files â€” uses source: "ul"

    $("#run_ul").click(function() {

        let ulFile = $("#ul_select").val();

        if (!ulFile) { alert("Select a UL file."); return; }

        $.post("/supermon/custom/run_announcement.php", {

            file: ulFile,

            source: "ul" // Tells PHP to use announcements/ subdirectory

        }, function(data) {

            alert(data);

        }).fail(function() {

            alert("Play Now failed - check server logs or permissions.");

        });

    });

    // Delete UL

    $("#delete_ul").click(function() {

        let ul = $("#ul_select").val();

        if (!ul) { alert("Select a UL file."); return; }

        if (!confirm("Delete '" + ul + "' (.ul file)?")) return;

        $.post("custom/delete_file.php", { type: "ul", file: ul }, function(data) {

            alert(data);

            loadULDropdown();

        }).fail(function() { alert("Delete failed."); });

    });

    // ====== Piper TTS Generation (updated to support voice dropdown) ======

    $("#generate_tts").click(function() {

        let text = $("#tts_text").val().trim();

        let filename = $("#tts_filename").val().trim();

        let voice = $("#tts_voice").val();  // NEW: selected voice path


        if (!text) { alert("Enter some text to convert."); return; }

        if (!filename) { alert("Enter a filename."); return; }


        if (!confirm("Generate WAV file '" + filename + ".wav' using selected voice?")) return;


        $("#tts_status").text("Generating WAV... Please wait.").css("color", "#28a745");


        $.post("custom/piper_generate.php", {

            text: text,

            filename: filename,

            voice: voice   // NEW: send selected voice

        }, function(data) {

            $("#tts_status").text(data);

            loadMP3Dropdown(); // Refresh MP3 dropdown to show new .wav file

        }).fail(function() {

            $("#tts_status").text("Failed to generate WAV file.").css("color", "#dc3545");

        });

    });

    // ====== Cron List with Separated Time Columns & Toggle ======

    function loadCronList() {

        $.getJSON("custom/list_cron.php", function(crons) {

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

                        $.post("custom/toggle_cron.php", {

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

                // Play Now - Green

                let playBtn = $("<button>")

                    .text("Play Now")

                    .css({

                        "background-color": "#28a745",

                        "color": "white",

                        "border": "1px solid #218838",

                        "padding": "6px 12px",

                        "margin-right": "8px",

                        "cursor": "pointer",

                        "border-radius": "4px",

                        "font-weight": "bold"

                    })

                    .hover(

                        function() { $(this).css("background-color", "#218838"); },

                        function() { $(this).css("background-color", "#28a745"); }

                    )

                    .click(function() {

                        let $this = $(this);

                        if ($this.prop("disabled")) return;

                        if (!confirm("Play announcement '" + c.file + "' right now?")) return;

                        $this.prop("disabled", true).text("Playingâ€¦");

                        $.post("custom/run_announcement.php", { file: c.file }, function(data) {

                            alert(data);

                        }).always(function() {

                            $this.prop("disabled", false).text("Play Now");

                        }).fail(function() {

                            alert("Failed to send play command. Check server logs.");

                        });

                    });

                // Edit - Yellow

                let editBtn = $("<button>")

                    .text("Edit")

                    .css({

                        "background-color": "#ffc107",

                        "color": "black",

                        "border": "1px solid #e0a800",

                        "padding": "6px 12px",

                        "margin-right": "8px",

                        "cursor": "pointer",

                        "border-radius": "4px",

                        "font-weight": "bold"

                    })

                    .hover(

                        function() { $(this).css("background-color", "#e0a800"); },

                        function() { $(this).css("background-color", "#ffc107"); }

                    )

                    .click(function() {

                        let parts = c.time.split(" ");

                        let min = prompt("Minute: Which minute of the hour?", parts[0]); if(min===null) return;

                        let hour = prompt("Hour: Which Hours do you want? 1,3,7= specific hours 2-19 is a range of hours", parts[1]); if(hour===null) return;

                        let dom = prompt("Day of Month: Which days of the Month? choose * for every day, or choose which days ie. 1-25", parts[2]); if(dom===null) return;

                        let month = prompt("Month: Which Months do you want? * for everymoth, or you can choose specific or a range", parts[3]); if(month===null) return;

                        let dow = prompt("Day of Week: Which days of the week? * is everyday or you can choose 1-7 where Sun=1", parts[4]); if(dow===null) return;

                        $.post("custom/update_announcement.php", {

                            raw_line: c.raw,

                            min: min,

                            hour: hour,

                            dom: dom,

                            month: month,

                            dow: dow

                        }, function(data) {

                            alert(data);

                            loadCronList();

                        });

                    });

                // Delete - Red

                let delBtn = $("<button>")

                    .text("Delete")

                    .css({

                        "background-color": "#dc3545",

                        "color": "white",

                        "border": "1px solid #c82333",

                        "padding": "6px 12px",

                        "cursor": "pointer",

                        "border-radius": "4px",

                        "font-weight": "bold"

                    })

                    .hover(

                        function() { $(this).css("background-color", "#c82333"); },

                        function() { $(this).css("background-color", "#dc3545"); }

                    )

                    .click(function() {

                        if (!confirm("Delete cron for " + c.file + "? This cannot be undone.")) return;

                        $.post("custom/delete_announcement.php", {

                            raw_line: c.raw

                        }, function(data) {

                            alert(data);

                            loadCronList();

                        });

                    });

                // Add columns

                row.append($("<td>").append(toggleBtn));

                row.append($("<td>").append(playBtn).append(editBtn).append(delBtn));

                table.append(row);

            });

        });

    }

    loadCronList();

    // ====== Install Announcement ======

    $("#install_announcement").click(function() {

        let mp3 = $("#mp3_select").val();

        if (!mp3) { alert("Please select an MP3 file."); return; }

        let min = prompt("Minute: Which minuteof the hour? ", "45"); if(min===null) return;

        let hour = prompt("Hour: Which hours do you want? * = every hour or a specific like 8,13,20 or a range like 8-20", "8-21"); if(hour===null) return;

        let dom = prompt("DOM: Which days of the month? * is everyday or a specific like 1,7,18 or 1-17 as a range", "1-24"); if(dom===null) return;

        let month = prompt("Month: Which month(s) * = every or specific like 3 or a range like 7-10", "*"); if(month===null) return;

        let dow = prompt("DOW: Which day(s) of the week? Sun=1 Sat =7 you can choose all * or a specific or a range", "*"); if(dow===null) return;

        let desc = prompt("Description: Name this announcement", "Scheduled announcement");

        if (!desc || desc.trim()==="") {

            alert("Description required.");

            return;

        }

        $.post("custom/announcement.php", {

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

        });

    });

});

</script>

<?php } ?>
