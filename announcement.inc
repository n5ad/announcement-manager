<?php
// announcement.inc
// Loaded only after Supermon login
// added advanced cronjob creation using logic like 2nd monday of the month
// created by N5AD February 2026
?>
<?php if (isset($_SESSION['sm61loggedin']) && $_SESSION['sm61loggedin'] === true) { ?>
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
<h3>Announcement Scheduler</h3>
<select id="mp3_select" class="submit" style="border:2px solid #444; width: 250px;">
    <option value="">-- Select Audio File (MP3-WAV) --</option>
</select>
<input type="button" class="submit" value="Install Announcement" id="install_announcement">
<input type="button" class="submit" value="Delete MP3" id="delete_mp3" style="background-color:#dc3545; color:white; border:1px solid #c82333;">
<input type="button" class="submit" value="Local Play" id="local_play_mp3" style="background:#28a745;color:white;border:1px solid #1e7e34;">
<input type="button" class="submit" value="Global Play" id="global_play_mp3" style="background:#fd7e14;color:white;border:1px solid #e06b00;">
<br><br>
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

    // MP3 Dropdown
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

    // UL Dropdown
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

    // Delete MP3
    $("#delete_mp3").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) { alert("Select an MP3 file."); return; }
        if (!confirm("Delete '" + mp3 + "'?")) return;
        $.post("custom/delete_file.php", { type: "mp3", file: mp3 }, function(data) {
            alert(data);
            loadMP3Dropdown();
        }).fail(function() { alert("Delete failed."); });
    });

    // Local Play MP3/WAV
    $("#local_play_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f) { alert("Select an MP3/WAV file first"); return; }
        $.post("/supermon/custom/run_announcement.php", {
            file: f,
            source: "mp3"
        }, function(r){ alert(r); })
        .fail(function() { alert("Local play failed."); });
    });

    // Global Play MP3/WAV
    $("#global_play_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f) { alert("Select an MP3/WAV file first"); return; }
        if (!confirm("WARNING: This will play on this node AND all linked nodes.\n\nContinue?")) return;
        $.post("/supermon/custom/globalplay.php", {
            file: f,
            source: "mp3"
        }, function(r){ alert(r); })
        .fail(function() { alert("Global play failed."); });
    });

    // Play Now UL
    $("#run_ul").click(function() {
        let ulFile = $("#ul_select").val();
        if (!ulFile) { alert("Select a UL file."); return; }
        $.post("/supermon/custom/run_announcement.php", {
            file: ulFile,
            source: "ul"
        }, function(data) { alert(data); })
        .fail(function() { alert("Play Now failed."); });
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

    // Generate TTS
    $("#generate_tts").click(function() {
        let text = $("#tts_text").val().trim();
        let filename = $("#tts_filename").val().trim();
        let voice = $("#tts_voice").val();
        if (!text) { alert("Enter some text."); return; }
        if (!filename) { alert("Enter a filename."); return; }
        if (!confirm("Generate '" + filename + ".wav'?")) return;
        $("#tts_status").text("Generating...").css("color", "#28a745");
        $.post("custom/piper_generate.php", {
            text: text,
            filename: filename,
            voice: voice
        })
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

    // Install Announcement
    $("#install_announcement").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) { alert("Select an MP3 file."); return; }
        let min = prompt("Minute:Which minute of the hour do you want this to play?", "45"); if(min===null) return;
        let hour = prompt("Hour:Which hour or range of hours do you want this to play? \n * = every hour \n single= 12 \n selected= 6,10,16 \n range= 7-20 \n every 2 hours between selected = 7-19/2", "7-20"); if(hour===null) return;
        let dom = prompt("DOM:Which day or range of days do you want this to play? \n* = everyday \n 1-31 \n selected= 1,7,23 \n range= 12-18", "*"); if(dom===null) return;
        let month = prompt("Month:Which Months do you want this to play? \n* = every month \n Jan = 1 Dec = 12", "*"); if(month===null) return;
        let dow = prompt("DOW: Which Day or days of the week do you want this to play? \n* = every day \n Sun=1 Sat=7 \nor range/list\n\nSingle number 1-7 = nth-week scheduling", "*");
        if(dow===null) return;
        let week = "*";
        let useNth = false;
        if (/^[1-7]$/.test(dow.trim())) {
            week = prompt("Week of month?\n* = every week \n1–5 = specific week", "*");
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
        $.post("custom/announcement.php", {
            file: mp3,
            min, hour, dom, month, dow, week,
            use_nth: useNth ? 1 : 0,
            desc
        }, function(data) {
            alert(data);
            loadULDropdown();
            loadCronList();
            loadMP3Dropdown();
        }).fail(function() {
            alert("Install failed.");
        });
    });

    // Load Cron List + Edit button with WARNING for advanced jobs
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

                let playBtn = $("<button>").text("Play Now").css({ "background": "#28a745", "color": "white", "margin-right": "8px" }).click(function() {
                    if (!confirm("Play now?")) return;
                    $.post("custom/run_announcement.php", { file: c.file }, function(data) { alert(data); });
                });

                let editBtn = $("<button>").text("Edit").css({ "background": "#ffc107", "color": "black", "margin-right": "8px" }).click(function() {
                    // Stronger detection for advanced/nth-week jobs
                    let isNthJob = c.raw.includes("/bin/bash -c") && 
                                   c.raw.includes("date +\\%d") && 
                                   c.raw.includes("playaudio.sh");

                    if (isNthJob) {
                        alert("Editing is not available for advanced/nth-week scheduled cron jobs.\n\n" +
                              "These jobs use special logic (e.g. '2nd Monday of the month').\n" +
                              "To change this job, please delete it and create a new one.");
                        return; // STOP — do NOT proceed to edit prompts
                    }

                    // Only classic jobs reach this point
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
