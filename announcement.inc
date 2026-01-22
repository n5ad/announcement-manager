<?php
// announcement.inc
// Loaded only after Supermon login
?>

<?php if (isset($_SESSION['sm61loggedin']) && $_SESSION['sm61loggedin'] === true) { ?>

<hr style="margin-top:30px;">

<div id="bottom_cron_section" style="margin-top:20px;">

<br>

<h3>Announcements Manager</h3>

<!-- MP3 Section -->
<select id="mp3_select" class="submit" style="border:2px solid #444; width: 250px;">
    <option value="">-- Select MP3 --</option>
</select>

<input type="button" class="submit" value="Install Announcement" id="install_announcement">
<input type="button" class="submit" value="Delete MP3" id="delete_mp3" style="background-color:#dc3545; color:white; border:1px solid #c82333;">

<br><br>

<!-- UL Section -->
<select id="ul_select" class="submit" style="border:2px solid #444; width: 250px;">
    <option value="">-- Select Announcement --</option>
</select>

<input type="button" class="submit" value="Play Now" id="run_ul">
<input type="button" class="submit" value="Delete UL" id="delete_ul" style="background-color:#dc3545; color:white; border:1px solid #c82333;">

<br><br>

<h3>Cron Manager</h3>

<table id="cron_table" border="1" cellpadding="5">
    <thead>
        <tr>
            <th>Min Hr Dom Mon Dow</th>
            <th>Name of File</th>
            <th>Description</th>
            <th>Actions</th>
        </tr>
    </thead>
    <tbody>
        <!-- Cron entries will be populated here -->
    </tbody>
</table>

</div>

<!-- Begin JavaScript to implement all the announcement functions -->

<script>

$(document).ready(function () {

    // ====== MP3 Dropdown ======
    function loadMP3Dropdown() {
        $.getJSON("custom/list_mp3.php", function(files) {
            $("#mp3_select").empty().append(
                $("<option>").val("").text("-- Select MP3 --")
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

    // ====== Delete MP3 Button ======
    $("#delete_mp3").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) {
            alert("Please select an MP3 file to delete.");
            return;
        }
        if (!confirm("Delete MP3 file '" + mp3 + "'? This cannot be undone.")) return;

        $.post("custom/delete_file.php", { type: "mp3", file: mp3 }, function(data) {
            alert(data);
            loadMP3Dropdown(); // Refresh MP3 dropdown
        }).fail(function() {
            alert("Failed to delete MP3 file. Check server logs.");
        });
    });

    // ====== Delete UL Button ======
  // ====== Delete UL Button ======
$("#delete_ul").click(function() {
    let ul = $("#ul_select").val();
    if (!ul) {
        alert("Please select a UL file to delete.");
        return;
    }
    if (!confirm("Delete UL file '" + ul + ".ul'? This cannot be undone.")) return;

    // FIXED: Send the base name only (no extra .ul)
    $.post("custom/delete_file.php", { type: "ul", file: ul }, function(data) {
        alert(data);
        loadULDropdown(); // Refresh UL dropdown
    }).fail(function() {
        alert("Failed to delete UL file. Check server logs.");
    });
});

    // ====== Cron List ======
    function loadCronList() {
        $.getJSON("custom/list_cron.php", function(crons) {
            let table = $("#cron_table tbody");
            table.empty();

            crons.forEach(function(c) {
                let row = $("<tr>");

                row.append($("<td>").text(c.time));
                row.append($("<td>").text(c.file));
                row.append($("<td>").text(c.desc || ""));

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

                        $this.prop("disabled", true).text("Playing…");

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

                        let min   = prompt("Minute:", parts[0]); if(min===null) return;
                        let hour  = prompt("Hour:", parts[1]); if(hour===null) return;
                        let dom   = prompt("Day of Month:", parts[2]); if(dom===null) return;
                        let month = prompt("Month:", parts[3]); if(month===null) return;
                        let dow   = prompt("Day of Week:", parts[4]); if(dow===null) return;

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

        let min   = prompt("Minute:", "45"); if(min===null) return;
        let hour  = prompt("Hour:", "8-21"); if(hour===null) return;
        let dom   = prompt("DOM:", "1-24"); if(dom===null) return;
        let month = prompt("Month:", "*"); if(month===null) return;
        let dow   = prompt("DOW:", "*"); if(dow===null) return;
        let desc  = prompt("Description:", "Scheduled announcement");

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

    // ====== Play Now (top dropdown) ======
    $("#run_ul").click(function() {
        let ulFile = $("#ul_select").val();
        if (!ulFile) { alert("Select a UL file."); return; }

        $.post("custom/run_announcement.php", { file: ulFile }, function(data) {
            alert(data);
        });
    });

});

</script>

<?php } ?>