<?php

// announcement.inc

// Loaded only after Supermon login

?>


<?php if (isset($_SESSION['sm61loggedin']) && $_SESSION['sm61loggedin'] === true) { ?>


<hr style="margin-top:30px;">


<div id="bottom_cron_section" style="margin-top:20px;">


<br>

<h3>Announcements Manager</h3>


<select id="mp3_select" class="submit" style="border:2px solid #444;">

    <option value="">-- Select MP3 --</option>

</select>


<input type="button" class="submit" value="Install Announcement" id="install_announcement">


<select id="ul_select" class="submit" style="border:2px solid #444;">

    <option value="">-- Select Announcement --</option>

</select>


<input type="button" class="submit" value="Play Now" id="run_ul">


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


    // ====== MP3 Dropdown for Conversion ======

    $.getJSON("custom/list_mp3.php", function(files) {

        files.forEach(function(f) {

            $("#mp3_select").append($("<option>").val(f).text(f));

        });

    });


    // ====== UL Dropdown ======

    function loadULDropdown() {

        $.getJSON("custom/list_ul.php", function(files) {

            $("#ul_select").empty().append(

                $("<option>").val("").text("-- Select UL File --")

            );

            files.forEach(function(f) {

                $("#ul_select").append($("<option>").val(f).text(f));

            });

        });

    }

    loadULDropdown();


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


                let editBtn = $("<button>").text("Edit").click(function() {

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


                let delBtn = $("<button>").text("Delete").click(function() {

                    if (!confirm("Delete cron for " + c.file + "?")) return;

                    $.post("custom/delete_announcement.php", {

                        raw_line: c.raw

                    }, function(data) {

                        alert(data);

                        loadCronList();

                    });

                });


                row.append($("<td>").append(editBtn).append(" ").append(delBtn));

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


    // ====== Play Now ======

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

