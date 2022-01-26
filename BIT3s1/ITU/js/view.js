/**
* @file view.js (FITeduler)
* 
* @brief Funkce interaktivního frontendu aplikace
* @date 2021-11-16 (YYYY-MM-DD)
* @author Karel Jirgl, Tereza Buchníčková, Tran Thanh Quang M.
* @update 2022-01-23 (YYYY-MM-DD)
*/

$( document ).ready(function() {
/* Karel - start */
    // --------- init backend and frontend ---------
    initBackend((data) => {
        console.log(data);

        // load page
        changePageFrame(null);

        // load timetable
        console.log("VIEW: calling loadTimetable()...");
        loadTimetable();
    });


    // --------- link click - change page frame ---------
    $('.nav-link').click(function(event) {
        event.preventDefault();
        changePageFrame(event.currentTarget.href.split("#")[1]);
    });


    
    // --------- form button add lesson ---------
    $('#addLesson').click(function() {

        if ($('#form_add .div9_form input').val() != "" && $('#form_add .div10_form input').val() != "") {

            var lesson = new Object();
            lesson.name = $('#form_add .div9_form input').val();
            lesson.day = Number($('#form_add .div1_form select').val());
            lesson.from = Number($('#form_add .div2_form select').val());
            lesson.to = Number($('#form_add .div3_form select').val());
            lesson.week = $('#form_add .div6_form select').val();
            if ($("#form_add .div11_form select").children("option:selected").length > 0) {
                
                // clear selectpicker selection
                //$("#form_add .div11_form select").val('default');
                //$("#form_add .div11_form select").selectpicker('refresh');

                //lesson.week = [];
                lesson.week = "";

                $("#form_add .div11_form select").children("option:selected").each(function(index, element) {
                    //lesson.week.push($(element).val());
                    lesson.week += $(element).val()+", ";
                });
            }
            lesson.color = Number($('#form_add .div7_form select').val());
            lesson.room = $('#form_add .div10_form input').val();
            lesson.description = $('#form_add .div12_form input').val();
            lesson.status = "displayed"; // displayed, hidden, removed, starred
            lesson.selected = false; // false, true
            lesson.subject_id = -1;

            timetableAddLesson(lesson, (data) => {
                loadTimetable();
            });

        } else {
            alert("Pole 'Název' a 'Místnost' jsou povinná!");
        }
    });



    // --------- click on lesson ---------
    window.lessonClicked = false;
    $(document).on('click', async function(event) {
        // don't clear selection when clicked on lesson
        if (window.lessonClicked == false) {
            // clear selection
            if ($(".text-dark").attr("href") == "#timetable" && $(".lesson-selected").length > 0) {
                if (!($(event.target).attr("type") == 'button' || $(event.target).parent().attr("type") == 'button' || $(event.target).attr("type") == 'color' || $(event.target).parent().attr("type") == 'color')) {
                    
                    var promises = [];
                    $.when($(".lesson-selected").each(function(index, element) {
                        promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'selected', false));
                    })).then(function() {
                        Promise.all(promises).then(() => {
                            loadTimetable();
                        });
                    });

                }
            }
        }
        window.lessonClicked = false;            
    });
    $(document).on('click','.lesson', async function(event) {
        window.lessonClicked = true;
        
        var lessonID = event.currentTarget.dataset.lessonId;

        // change item selected status
        timetableGetItemAttribute(lessonID, 'selected', false, (currentStatus) => {
            if (currentStatus == false) {currentStatus = true;} else {currentStatus = false;}
            timetableChangeItemAttribute(lessonID, 'selected', currentStatus).then(() => {
                loadTimetable();
            });
        });

        event.preventDefault();
    });



    // --------- button show ---------
    $('#show').click(async function() {
        var promises = [];
        $.when($(".lesson-selected").each(function(index, element) {
            promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'status', 'displayed'));
        })).then(function() {
            Promise.all(promises).then(() => {
                loadTimetable();
            });
        });
    });


    // --------- button hide ---------
    $('#hide').click(async function() {
        var promises = [];
        $.when($(".lesson-selected").each(function(index, element) {
            promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'status', 'hidden'));
        })).then(function() {
            Promise.all(promises).then(() => {
                loadTimetable();
            });
        });
    });


    // --------- button star ---------
    $('#star').click(async function() {
        var promises = [];
        $.when($(".lesson-selected").each(function(index, element) {
            promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'status', 'starred'));
            promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'selected', false));
        })).then(function() {
            Promise.all(promises).then(() => {
                loadTimetable();
            });
        });
    });


    // --------- button html5colorpicker ---------
    $('#html5colorpicker').change(async function() {
        var promises = [];
        $.when($(".lesson-selected").each(function(index, element) {
            promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'color', $('#html5colorpicker').val()));
        })).then(function() {
            Promise.all(promises).then(() => {
                loadTimetable();
            });
        });
    });


    // --------- button trash ---------
    $('#trash').click(async function() {
        var promises = [];
        $.when($(".lesson-selected").each(function(index, element) {
            promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'status', 'removed'));
            promises.push(timetableChangeItemAttribute(element.dataset.lessonId, 'selected', false));
        })).then(function() {
            Promise.all(promises).then(() => {
                loadTimetable();
            });
        });
    });


    // --------- button clear_trash ---------
    $('#clear_trash').click(function() {
        if (confirm("Opravdu chcete vysypat koš?")) {
            timetableClearTrash((data) => {
                loadTimetable();
            });
        }
    });


    // --------- button clear ---------
    $('#clear').click(function() {
        if (confirm("Opravdu chcete smazat celý rozvrh?")) {
            timetableClear((data) => {
                loadTimetable();
            });
        }
    });

    
    // --------- OPTIONS ---------
    // avoid doubleclick on selects
    $(".dropdown-toggle").each(function(index, element) {
        $(element).trigger("click");
    });
    $("label[for='p-p']").trigger("click");
    // clear selects
    $("#p-p").val([]).trigger('change');
    $("#v-p").val([]).trigger('change');


    // --------- subjects logic update ---------
    $('.studium, .btn_semestr, #r').on('click change', function(event) {
        if ((event.type == "change") || (event.type == "click" && event.currentTarget.id != "mit" && event.currentTarget.id != "r")) {
            setTimeout(() => {
                var study = "BIT";
                var semester = 0;
                var year = 0;

                // study
                if ($("#bit").hasClass("disabled") && $("#mit").val() != "default") {
                    study = $("#mit").val();
                }

                // semester
                if (!$("#l_s").hasClass("disabled") && $("#z_s").hasClass("disabled")) {
                    semester = 1;
                }

                // year
                year = $("#r").val();

                optionsLoadSubjects(study, semester, year);
            }, 33);
        }
    });


    // --------- create timetable ---------
    $('#create').on('click', function() {
        
        var compulsory_subjects = [];
        var optional_subjects = [];
    
        $("#p-p").children("option:selected").each(function(index, element) {
            compulsory_subjects.push($(element).val());
        });
        $("#v-p").children("option:selected").each(function(index, element) {
            optional_subjects.push($(element).val());
        });
    
        timetableClear(() => {
            optionsCreateTimetable(compulsory_subjects, optional_subjects);
        });
    });
    

    // restore from bin
    $(document).on('click','.restore', function(event) {
        timetableChangeItemAttribute(event.currentTarget.dataset.lessonId, 'status', 'displayed').then(() => {
            loadTimetable();
        });
    });



    // RIGHT CLICK CONTEXT MENU
    // --------- button show ---------
    $('#show_dd').click(function() {
        timetableChangeItemAttribute($(".lesson-dropdown-selected")[0].dataset.lessonId, 'status', 'displayed').then(() => {
            loadTimetable();
        });
    });


    // --------- button hide ---------
    $('#hide_dd').click(function() {
        timetableChangeItemAttribute($(".lesson-dropdown-selected")[0].dataset.lessonId, 'status', 'hidden').then(() => {
            loadTimetable();
        });
    });


    // --------- button star ---------
    $('#star_dd').click(function() {
        timetableChangeItemAttribute($(".lesson-dropdown-selected")[0].dataset.lessonId, 'status', 'starred').then(() => {
            loadTimetable();
        });
    });


    // --------- button html5colorpicker_dropdown ---------
    $('#html5colorpicker_dropdown').change(function() {
        timetableChangeItemAttribute($(".lesson-dropdown-selected")[0].dataset.lessonId, 'color', $('#html5colorpicker_dropdown').val()).then(() => {
            loadTimetable();
        });
    });


    // --------- button trash ---------
    $('#trash_dd').click(function() {
        timetableChangeItemAttribute($(".lesson-dropdown-selected")[0].dataset.lessonId, 'selected', false).then(() => {
            timetableChangeItemAttribute($(".lesson-dropdown-selected")[0].dataset.lessonId, 'status', 'removed').then(() => {
                loadTimetable();
            });
        });
    });
/* Karel - end */



/* Tran - start */
    // --------- show/hide add new lessson form ---------
    $('#create_lesson').on('click', function() {
        if ($('#form_add').css('display') == 'none') {
            $('.tab_panels').hide();
            $('#form_add').css({'display' : 'grid'});
            $(this).html('Přidat vlastní­ hodinu <i class="fas fa-times"></i>');
        } else {
            $('.tab_panels').show();
            $('#form_add').css({'display' : 'none'});
            $(this).html('Přidat vlastní­ hodinu <i class="fas fa-plus"></i>');
        }
    });


    // --------- lesson info panels ---------
    $('.tab_panels').show();
    $('.tab_panels .tabs li').on('click', function() {
        //this je zalozka na kterou uzivatel klikl
        var $panel = $(this).closest('.tab_panels');

        $panel.find('.tabs li.active').removeClass('active');
        $(this).addClass('active');

        //ktery panel se ukaze po kliknuti
        var panelToShow = $(this).attr('rel');

        //skryje se predchozi panel
        $('.tab_panels .panel.active').fadeOut(showNextPanel);

        //ukaze se novy panel
        function showNextPanel() {
            $(this).removeClass('active');

            $('#'+panelToShow).fadeIn(function() {
                $(this).addClass('active');
            });
        }
    });


    // --------- RIGHT CLICK CONTEXT MENU ------------
    $(document).bind("contextmenu", '#timetable', function(e){
        if ( $("#menu").css('display') == 'block' ){
            $(" #menu ").hide();
        }
        $('td').css('box-shadow', 'none');
        $(this).removeClass("lesson-dropdown-selected");
        return false;
    });
    $(document).on('contextmenu', ".lesson", function(e) {
        //console.log($(this).attr('id'))
        $('td').css('box-shadow', 'none');
        var top = e.pageY - 50;
        var left = e.pageX - 120;
        $(this).css('box-shadow', 'inset 2px 2px 0px 0px rgb(0, 123, 255), inset -2px -2px 0px 0px rgb(0, 123, 255)');
        $("#menu").css({
            display: "block",
            top: top,
            left: left
        });
        $(".lesson-dropdown-selected").each(function(index, element) {
            $(element).removeClass("lesson-dropdown-selected");
        });
        $(this).addClass("lesson-dropdown-selected");
        timetableGetItemAndLessonInfo($(".lesson-dropdown-selected")[0].dataset.lessonId);
        return false; //blocks default Webbrowser right click menu
    });
        
    $(document).on("click", "", function() {
        if ( $("#menu").css('display') == 'block' ){
            $(" #menu ").hide();
        }
        $('td').css('box-shadow', 'none');
    });

    $(document).on("click", "#menu a", function() {
        $(this).parent().hide();
    });

    hideInputs();
    $('#bit').click(function(){
        $('.div4').show(); // nadpis semestru
        $('.div5').show();
        $('.div6').show();
    })
    $('#l_s').click(function(event){
        $('.div7').show();
        $('.div8').show(); // nadpis semestru
        $('.div9').show();
        $('.div10').show(); // Povinne predmety
        $('.div11').show(); // Volitelno predmety
        $('.div12').show(); // Voliteln predmety
        $('#create').prop("disabled", false); // Element(s) are now enabled.
    })
    $('#z_s').click(function(event){
        $('.div7').show();
        $('.div8').show(); // nadpis semestru
        $('.div9').show();
        $('.div10').show(); // Povinne predmety
        $('.div11').show(); // Volitelno predmety
        $('.div12').show(); // Voliteln predmety
        $('#create').prop("disabled", false); // Element(s) are now enabled.
    })
    $("#save_as_image").click(function(){
        const target = document.getElementById("timetable_result");
        html2canvas(target).then((canvas) => {
            const base64image = canvas.toDataURL("image/png");
            var anchor = document.createElement('a');
            anchor.setAttribute('href', base64image);
            anchor.setAttribute('download', 'rozvrh.png');
            anchor.click();
        });
    });


    function hideInputs(){
        $('.div4').hide(); // nadpis semestru
        $('.div5').hide(); // semestr
        $('.div6').hide(); // Letni
        $('.div7').hide(); // zimni
        $('.div8').hide(); // rocnik nadpis
        $('.div9').hide(); // rocnik select  
        $('.div10').hide(); // Povinne predmety
        $('.div11').hide(); // Volitelno predmety
        $('.div12').hide(); // Voliteln predmety
        $('#create').prop("disabled",true); 
    }
/* Tran - end */



/* Tereza - start */
    // if more than 0 selected -> enable buttons ELSE disable buttons
    if ($(".lesson-selected").length > 0) {
        $('.mass_event').prop("disabled", false);
    } else {
        $('.mass_event').prop("disabled", true);
    }

    // --------- OPTIONS ---------
    // --------- disable items ---------
    $('.btn_semestr').on('click', function() {
        $(this).removeClass('disabled');
        if (this.id === 'l_s'){
            $('#z_s').addClass('disabled');
        } else {
            $('#l_s').addClass('disabled');
        }
    });

    $('.studium').on('click', function() {
        $(this).removeClass('disabled');
        $('#mit').css({'opacity': '1'});
        if (this.id === 'bit'){
            $('#mit').css({'opacity': '0.5'});
            $('#year3').show();
        } else {
            if ($('#mit').val() !== 'default') {
                $('#bit').addClass('disabled');
            }
            $('#year3').hide();
        }
    });

    // sporty - start
    $('#myInput').keyup(function() {
        // Declare variables
        var filter = $('#myInput').val().toUpperCase();
        getSports(filter)    
    });

    $('.showAll').click(function (){
        var filter = 'all';
        getSports(filter)
    });

    // sporty - end
/* Tereza - end */
});
