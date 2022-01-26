/**
* @file callbacks.js (FITeduler)
* 
* @brief Funkce pro zobrazování dat získaných z API na frontendu
* @date 2022-01-13 (YYYY-MM-DD)
* @author Karel Jirgl
* @update 2022-01-23 (YYYY-MM-DD)
*/

function showPageFrame(frame)
{
    $('.page_frame').hide();
    $('#frame_'+frame).show();
    $('.nav-link').removeClass("text-dark");
    $(".nav-link[href='#"+frame+"']").addClass("text-dark");
}

function renderTimetable(table, items)
{
    var this_ = this;
    console.log("VIEW: timetable rendering...");
    


    // clear timetable
    $('#timetable tbody').html(`
    <!--Pondeli-->
                    <tr id="rowMonday">
                        <td class="align-middle" rowspan="1">Pondělí</td>
                    </tr>
    <!--Utery-->
                    <tr id="rowThuesday">
                        <td class="align-middle" rowspan="1">Úterý </td>
                    </tr>
    <!-- Streda -->
                    <tr id="rowWednesday">
                        <td class="align-middle" rowspan="1">Středa</td>
                    </tr>
    <!--Ctvrtek-->
                    <tr id="rowThursday">
                        <td class="align-middle" rowspan="1">Čtvrtek</td>
                    </tr>
    <!--Patek-->
                    <tr id="rowFriday">
                        <td class="align-middle" rowspan="1">Pátek</td>
                    </tr>
                `);
    
    // clear timetable
    $('#timetable_result tbody').html(`
    <!--Pondeli-->
                    <tr id="rowMonday_result">
                        <td class="align-middle" rowspan="1">Pondělí</td>
                    </tr>
    <!--Utery-->
                    <tr id="rowThuesday_result">
                        <td class="align-middle" rowspan="1">Úterý </td>
                    </tr>
    <!-- Streda -->
                    <tr id="rowWednesday_result">
                        <td class="align-middle" rowspan="1">Středa</td>
                    </tr>
    <!--Ctvrtek-->
                    <tr id="rowThursday_result">
                        <td class="align-middle" rowspan="1">Čtvrtek</td>
                    </tr>
    <!--Patek-->
                    <tr id="rowFriday_result">
                        <td class="align-middle" rowspan="1">Pátek</td>
                    </tr>
                `);

    // clear bin
    $("#panel3").html("");
    


    // day
    for (var i = 0; i < 5; i++) {
        var dayId = ["rowMonday", "rowThuesday", "rowWednesday", "rowThursday", "rowFriday"];
        var maxRowCount = 0; // current max row count

        // get max row count in current day
        for (var j = 0; j < 14; j++) {
            if (table[i][j].length > maxRowCount) {
                maxRowCount = table[i][j].length;
            }
        }
        

        // if maxRowCount is 0 -> add 1 empty row
        if (maxRowCount == 0) {
            maxRowCount = 1;
        }

        
        // hour
        for (var j = 0; j < 14; j++) {
            
            // row
            var currentRow = $('#'+dayId[i]);
            var currentRowResult = $('#'+dayId[i]+'_result');
            for (var k = 0; k < maxRowCount; k++) {
                
                // IF it is the first hour of the day -> add a new row AND set is as current row OR move on the next existing row
                if (k != 0) {
                    if (j == 0)  {
                        currentRow = $('\
                        <tr>\
                            <td style="display: none;">Day</td>\
                        </tr>').insertAfter(currentRow);
                        currentRowResult = $('\
                        <tr>\
                            <td style="display: none;">Day</td>\
                        </tr>').insertAfter(currentRowResult);
                        
                    } else {
                        currentRow = currentRow.next();
                        currentRowResult = currentRowResult.next();
                    }
                }
                

                // add lesson
                if (table[i][j][k] != null && (j == 0 || table[i][j-1][k] != table[i][j][k])) {
                    
                    if (items[table[i][j][k]].status == 'removed') {

                        currentRow.append('\
                        <td class="bg-light-gray"></td>');
                        currentRowResult.append('\
                        <td class="bg-light-gray"></td>');

                        var days = ["Po", "Út", "St", "Čt", "Pá"];
                        var from_time = ["7:00", "8:00", "9:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00", "20:00"];
                        var to_time = ["7:50", "8:50", "9:50", "10:50", "11:50", "12:50", "13:50", "14:50", "15:50", "16:50", "17:50", "18:50", "19:50", "20:50"];

                        $("#panel3").append('<button type="button" style="margin:5px;" id="restore_'+table[i][j][k]+'" data-lesson-id="'+table[i][j][k]+'" title="Obnovit položku" class="restore text-white btn dark-sky">'+items[table[i][j][k]].name+': '+days[items[table[i][j][k]].day]+' '+from_time[items[table[i][j][k]].from]+' - '+to_time[items[table[i][j][k]].to]+'&nbsp; <i class="fas fa-trash-restore"></i></button>');

                    } else {

                        var currentItemId = table[i][j][k];
                        var currentItem = items[table[i][j][k]];
                        var colspan = (currentItem.to-currentItem.from)+1;
                        var colors = ["#ffc107", "#02c2c7", "#FA6C38"];


                        var color = colors[currentItem.color];
                        if (color == null) {
                            color = currentItem.color;
                        }
                        
                        
                        var classes = "";
                        if (currentItem.selected == true) {
                            classes = " lesson-selected";
                        }



                        var stylesTD = "opacity: 1; border: 1px solid #343a40 !important;";
                        var stylesDIV = "";

                        if (currentItem.selected == true) {
                            stylesDIV = "background-color: #20abffd6;";
                            stylesTD = "opacity: 1; border: 1px solid #007bff !important;";
                        }
                        if (currentItem.status == 'hidden') {
                            if (currentItem.selected == false) {
                                stylesTD = "opacity: 0.4; border: 1px solid #dee2e6 !important;";
                            } else {
                                stylesTD = "opacity: 0.4; border: 1px solid #007bff !important;";
                            }
                        } else if (currentItem.status == 'starred') {
                            if (currentItem.selected == false) {
                                stylesTD = "opacity: 1; border: 3px solid #343a40 !important;";
                            } else {
                                stylesTD = "opacity: 1; border: 3px solid #007bff !important;";
                            }
                        }/* else if (currentItem.status == 'removed') {
                            stylesDIV = "background-color: red;";
                        }*/
                        

                        // add lesson to timetable
                        currentRow.append('\
                        <td id="lesson'+currentItemId+'" data-lesson-id="'+currentItemId+'" class="lesson '+classes+'" style="cursor: pointer; '+stylesTD+' background-color:'+color+';" colspan="'+colspan+'">\
                            <a href="#" style="text-decoration: none;">\
                                <div class="text-white font-size16" style="height:100%; width:100%; '+stylesDIV+'"><strong>'+currentItem.name+'</strong> - '+currentItem.room+'</div>\
                            </a>\
                        </td>');
                        if (currentItem.status == 'starred') {
                            currentRowResult.append('\
                            <td id="lesson'+currentItemId+'" data-lesson-id="'+currentItemId+'" class="" style="opacity: 1; border: 1px solid #343a40 !important; background-color:'+color+';" colspan="'+colspan+'">\
                                <a href="#" style="text-decoration: none; cursor: default;" onclick="return false;">\
                                    <div class="text-white font-size16" style="height:100%; width:100%;"><strong>'+currentItem.name+'</strong> - '+currentItem.room+'</div>\
                                </a>\
                            </td>');
                        } else {
                            currentRowResult.append('\
                            <td class="bg-light-gray"></td>');
                        }

                    }

                // add empty hour
                } else if (table[i][j][k] == null) {
                    
                    currentRow.append('\
                    <td class="bg-light-gray"></td>');
                    currentRowResult.append('\
                    <td class="bg-light-gray"></td>');
                } else if (items[table[i][j][k]].status == 'removed') {

                    currentRow.append('\
                    <td class="bg-light-gray"></td>');
                    currentRowResult.append('\
                    <td class="bg-light-gray"></td>');
                } else if (items[table[i][j][k]].status != 'starred') {

                    currentRowResult.append('\
                    <td class="bg-light-gray"></td>');
                }


                // set new maxRowCount
                if (k > maxRowCount) {
                    maxRowCount = k;
                }
            }
        }

        // set day rowspan
        $('#'+dayId[i]+' :first-child').first().attr("rowspan", maxRowCount);
        $('#'+dayId[i]+'_result :first-child').first().attr("rowspan", maxRowCount);
    }


    // remove empty rows from both timetables
    $("#timetable_result, #timetable").each(function(index, element) {

        $('#'+element.id+' tr:not(.thead-dark)').each(function(index, element) {
            
            // if row is empty remove current row
            if ($(element).children(".bg-light-gray").length == 14) {

                if ($(element).next().children().first().html() == "Day") {
                    // day name
                    $(element).next().children().first().html($(element).children().first().html());
                    // rowspan
                    $(element).next().children().first().attr("rowspan", Number($(element).children().first().attr("rowspan"))-1);
                    // display block
                    $(element).next().children().first().show();
                }


                var currentRowId = element.id;
                if ($(element).next().length != 0 && $(element).next().attr("id") == null) {
                    $(element).next().attr("id", currentRowId);
                    
                    // remove current element row 
                    if ($(element).next().length != 0) {
                        $(element).remove();
                    }
                } else if ($(element).children().first().html() == "Day") {
                    $(element).remove();
                }
                
            } else {
                if ($(element).children().first().html() == "Day") {
                    $(element).children().first().hide();
                }
            }
            
        })
    });
    $("#timetable_result, #timetable").each(function(index, element) {

        $('#'+element.id+' tr:not(.thead-dark)').each(function(index, element) {
            
            // change rowspans
            if ($(element).attr("id") != null && $(element).attr("id") != "") {
                // count number of rows
                var rowspanCoount = 1;
                var currentRow = $(element).next();
                while(currentRow.length != 0 && (currentRow.attr("id") == null || currentRow.attr("id") == "")) {
                    currentRow = currentRow.next();
                    rowspanCoount++;
                }
                // change rowspan
                $(element).children().first().attr("rowspan", rowspanCoount);
                $(element).children().first().addClass("align-middle");
            }
        })
    });


    // set border for timtable days
    $("#timetable tr[id]").each(function(index, element) { $(element).children().first().css("border", "1px solid black")});


    // panel3
    if ($("#panel3").html() == "" || $("#panel3").html() == "Koš je prázdný.") {
        $("#panel3").html("Koš je prázdný.");
        $('#clear_trash').prop("disabled", true);
    } else {
        $('#clear_trash').prop("disabled", false);
    }

    // clear timetable button
    if (items.length == 0) {
        $('#clear').prop("disabled", true);
    } else {
        $('#clear').prop("disabled", false);
    }

    // if more than 0 selected -> enable buttons ELSE disable buttons
    if ($(".lesson-selected").length > 0) {
        $('.mass_event').prop("disabled", false);
    } else {
        $('.mass_event').prop("disabled", true);
    }



    console.log("VIEW: timetable rendered!");
}

function showLessonInfo(item, lesson)
{
    // show panel lesson
    if ($("#panel_lesson").hasClass("active") == false && $("#panel_subject").hasClass("active") == false) {
        $("#panel_lesson").trigger("click");
    }

    $('.tab_panels').show();
    $('#form_add').css({'display' : 'none'});
    $(this).html('Přidat vlastní hodinu <i class="fas fa-plus"></i>');
    var days = ["Pondělí", "Úterý", "Středa", "Čtvrtek", "Pátek"];
    var start_times = [
        "7:00", 
        "8:00", 
        "9:00", 
        "10:00", 
        "11:00", 
        "12:00", 
        "13:00", 
        "14:00", 
        "15:00", 
        "16:00", 
        "17:00",
        "18:00",
        "19:00",
        "20:00"
    ];
    var end_times = [
        "7:50", 
        "8:50", 
        "9:50", 
        "10:50", 
        "11:50", 
        "12:50", 
        "13:50", 
        "14:50", 
        "15:50", 
        "16:50", 
        "17:50",
        "18:50",
        "19:50",
        "20:50"
    ];
    var semester = ["Zimní", "Letní"];
    var years = ["První", "Druhý", "Třetí"];
    var compuls = ["Volitelný", "Povinný"];
    

    var panel1Content = "";
    var panel2Content = "";
    $.each(item, function(key, value) {
        switch(key) {
            case 'day':
                panel1Content += "Den: <strong>" + days[value] + "</strong><br />";
                break;
            case 'from':
                panel1Content += "Od: <strong>" + start_times[value] + "</strong><br />";
                break;
            case 'to':
                panel1Content += "Do: <strong>" + end_times[value] + "</strong><br />";
                break;
            case 'name':
                panel1Content += "Zkratka: <strong>" + value + "</strong><br />";
                break;
            case 'room':
                panel1Content += "Místnost: <strong>" + value + "</strong><br />";
                break;
            case 'description':
                if(value != '') {
                    panel1Content += "Popis:<br /> <strong>" + value + "</strong><br />";
                }
                break;
            case 'fullname':
                if(value != null) {
                    panel1Content += "Předmět:<br /> <strong>" + value + "</strong><br />";
                }
                break;
            case 'type':
                panel1Content += "Typ: <strong>" + value + "</strong><br />";
                break;
            case 'week':
                panel1Content += "Týdny: <strong>" + value + "</strong><br />";
                break;
        }
    });

    $.each(lesson, function(key, value) {
        switch(key) {
            case 'study':
                panel2Content += "Studium: <strong>" + value + "</strong><br />";
                break;
            case 'semester':
                panel2Content += "Semestr: <strong>" + semester[value] + "</strong><br />";
                break;
            case 'year':
                panel2Content += "Ročník: <strong>" + years[value] + "</strong><br />";
                break;
            case 'compulsory':
                panel2Content += "Povinnost: <strong>" + compuls[value] + "</strong><br />";
                break;
            case 'name':
                panel2Content += "Jméno:<br /> <strong>" + value + "</strong><br />";
                break;
            case 'nickname':
                panel2Content += "Zkratka: <strong>" + value + "</strong><br />";
                break;
            case 'week':
                panel2Content += "Týdny: <strong>" + value + "</strong><br />";
                break;
        }
    });
    if (lesson != null && lesson.link != null) {
        panel2Content += '<a href="'+lesson.link+'" target="_blank">Karta předmětu</a>';
    }

    if (panel2Content == "") {
        panel2Content = "Informace nenalezeny. Nejedná se o oficiální předmět.";
    }


    $("#panel1").html(panel1Content);
    $("#panel2").html(panel2Content);
}

/* // replaced with: functions.js --> optionsLoadSubjects()
function showSubjectsOptions(compulsory, optional)
{
    var compulsory_subjects = "";
    var optional_subjects = "";

    $(compulsory).each(function(index, element) {
        compulsory_subjects += '<option value="'+element+'">'+element+'</option>';
    });
    $(optional).each(function(index, element) {
        optional_subjects += '<option value="'+element+'">'+element+'</option>';
    });

    $("#p-p").html(compulsory_subjects).selectpicker('refresh');
    $("#v-p").html(optional_subjects).selectpicker('refresh');

    // avoid doubleclick on selects
    $(".dropdown-toggle").each(function(index, element) {
        $(element).trigger("click");
    });
    $("label[for='p-p']").trigger("click");
}
*/
