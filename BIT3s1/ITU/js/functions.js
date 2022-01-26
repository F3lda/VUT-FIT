/**
* @file functions.js (FITeduler)
* 
* @brief Dotazy na API pomocí asynchronních funkcí
* @date 2022-01-13 (YYYY-MM-DD)
* @author Karel Jirgl, Tereza Buchníčková, Tran Thanh Quang M.
* @update 2022-01-23 (YYYY-MM-DD)
*/

/* Karel - start */
var iframe_origin = global_iframe_origin;
var iframe_url = "./js/API";

function initBackend(callback)
{
    fetchIframe(iframe_origin, iframe_url, ["init"], callback);
}

function changePageFrame(frame)
{
    fetchIframe(iframe_origin, iframe_url, ["changePageFrame", JSON.stringify(frame)], function(data) {
        showPageFrame(data);

        document.body.style.cursor = 'default';
        document.getElementById("create").style.cursor = 'pointer';
    });
}

function loadTimetable()
{
    fetchIframe(iframe_origin, iframe_url, ["loadTimetable"], function(data) {
        data = JSON.parse(data);
        renderTimetable(data[0], data[1]);
    });
}

function timetableAddLesson(lesson, callback)
{
    fetchIframe(iframe_origin, iframe_url, ["timetableAddLesson", JSON.stringify(lesson)], callback);
}

function timetableGetItemAttribute(item, attribute, ifnull, callback)
{
    fetchIframe(iframe_origin, iframe_url, ["timetableGetItemAttribute", JSON.stringify([item, attribute, ifnull])], function(data) {
        callback(JSON.parse(data));
    });
}

async function timetableChangeItemAttribute(item, attribute, value)
{
    var promise = new Promise((resolve, reject) => {
        fetchIframe(iframe_origin, iframe_url, ["timetableChangeItemAttribute", JSON.stringify([item, attribute, value])], function(data) {
            resolve("done");
        });
    });
    var result = await promise;
    return result;
}

function timetableGetItemAndLessonInfo(lessonID)
{
    fetchIframe(iframe_origin, iframe_url, ["timetableGetItemInfo", JSON.stringify(lessonID)], function(data) {
        data = JSON.parse(data);
        
        if (data.subject_id == -1) {
            showLessonInfo(data, null);
        } else {
            subject = new Object();
            subject.id = data.subject_id;

            fetch('./js/API/subjects/?cmd=getLessonInfo', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(subject),
            })
            .then(response => response.json())
            .then(response => {
                showLessonInfo(data, response);
            })
            .catch((error) => {
                console.error('Error:', error);
            });
        }
    });
}

function timetableClearTrash(callback)
{
    fetchIframe(iframe_origin, iframe_url, ["timetableClearTrash"], callback);
}

function timetableClear(callback)
{
    fetchIframe(iframe_origin, iframe_url, ["timetableClear"], callback);
}
/*// replaced with PHP API
function optionsLoadSubjects(study, semester, year)
{
    fetchIframe(iframe_origin, iframe_url, ["optionsLoadSubjects", JSON.stringify([study, semester, year])], function(data) {
        data = JSON.parse(data);
        showSubjectsOptions(data[0], data[1]);
    });
}
// replaced with PHP API
function optionsCreateTimetable(compulsory_subjects, optional_subjects, callback)
{
    fetchIframe(iframe_origin, iframe_url, ["optionsCreateTimetable", JSON.stringify([compulsory_subjects, optional_subjects])], callback);
}
*/
/* Karel - end */



/* Tereza - start */
async function getSports(filter)
{
    sports = new Object();
    sports.filter = filter;
    
    jQuery.ajax({
        url: './js/API/subjects/?cmd=getSports',
        type: 'GET',
        data: sports,
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (data) {
            //console.log(data)
            $('#myUL').empty()
            for (var i = 0; i < data.length; i++) {
                //$('#myUL').append('<li><a href="'+ data[i]['link'] + '" target="_blank" onclick="return false;" style="cursor: default;"><table id="myTable"><tbody><tr><td id="sportName">'+ data[i]['name'] + '</td><td id="sportAction"><i class="fa fa-link" aria-hidden="true"></i></td><td id="sportTimeDay">'+ data[i]['day'] +'</td><td id="sportTimeDay">'+ data[i]['time'] +'</td></tr></tbody></table></a></li>');
                $('#myUL').append('<li><table id="myTable"><tbody><tr><td id="sportName">'+ data[i]['name'] + '</td> \
                <td id="sportAction"> \
                <a href="'+ data[i]['link'] + '" target="_blank" data-toggle="tooltip" title="Odkaz ke sportu"><i class="fa fa-link buttonOptions" aria-hidden="true"></i></a> \
                <a style="padding-left:25px;" href="#" class="addSport" data-toggle="tooltip" title="Přidat termín k pracovnímu rozvrhu"><i class="fa fa-plus buttonOptions" aria-hidden="true"></i></td></a> \
                <td id="sportDay">'+ data[i]['day'] +'</td> \
                <td id="sportTime" value="'+ data[i]['from_time'] + '_' + data[i]['to_time'] + '">'+ data[i]['time'] +'</td> \
                </tr></tbody></table></li>');

            }
            $('.addSport').on('click',function (){
                time = $(this).closest('li').find('#sportTime').attr('value');
                array = time.split('_');
                from_time = array[0];
                to_time = array[1];
        
                lesson = new Object();
                lesson.name = $(this).closest('li').find('#sportName').text();
                day = $(this).closest('li').find('#sportDay').text();
                if (day == "Pondělí") {
                    lesson.day = 0;
                } else if (day == "Úterý") {
                    lesson.day = 1;
                } else if (day == "Středa") {
                    lesson.day = 2;
                } else if (day == "Čtvrtek") {
                    lesson.day = 3;
                } else if (day == "Pátek") {
                    lesson.day = 4;
                }
                lesson.from = Number(from_time);
                lesson.to = Number(to_time);
                lesson.week = "both";
                lesson.color = 0;
                lesson.room = "CESA";
                lesson.description = '';
                lesson.type = 'sport';
                lesson.status = "displayed"; // displayed, hidden, removed, starred
                lesson.selected = false; // false, true
                lesson.subject_id = -1;
                console.log(lesson)
                timetableAddLesson(lesson, () => {});
                loadTimetable();
                changePageFrame("timetable");
            });
        }
    });
}

/* Tereza - end */



/* Tran - start */
async function optionsLoadSubjects(study, semester, year)
{
    subjects = new Object();
    subjects.study = study;
    subjects.semester = semester;
    subjects.year = year;
    
    jQuery.ajax({
        url: './js/API/subjects/?cmd=loadAllSubjects',
        type: 'GET',
        data: subjects,
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (data) {
            var compulsory_subjects = "";
            var optional_subjects = "";
            for (var i = 0; i < data.length; i++)
            {
                if (data[i].compulsory == 1) {
                    compulsory_subjects += '<option value="'+data[i].nickname+'">'+data[i].nickname+'</option>';
                } else {
                    optional_subjects += '<option value="'+data[i].nickname+'">'+data[i].nickname+'</option>';
                }
            }

            $("#p-p").html(compulsory_subjects).selectpicker('refresh');
            $("#v-p").html(optional_subjects).selectpicker('refresh');

            // avoid doubleclick on selects
            $(".dropdown-toggle").each(function(index, element) {
                $(element).trigger("click");
            });
            $("label[for='p-p']").trigger("click");
        }
    });
}

async function optionsCreateTimetable(compulsory_subjects, optional_subjects)
{
    document.body.style.cursor = 'wait';
    document.getElementById("create").style.cursor = 'wait';

    subjects = new Object();
    subjects.compulsory = compulsory_subjects;
    subjects.optional = optional_subjects;
    jQuery.ajax({
        url: './js/API/subjects/?cmd=loadSelectedSubjects',
        type: 'POST',
        data: JSON.stringify(subjects),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: async function (data) {
            //console.log(data);
            for (var i = 0; i < data.length; i++)
            {
                var colors = {"přednáška": 1, "cvičení": 2};
                    
                var lesson = new Object();
                lesson.fullname = data[i].name;
                lesson.day = Number(data[i].day);
                lesson.from = Number(data[i].from_time);
                lesson.to = Number(data[i].to_time);
                lesson.week = data[i].week;
                lesson.color = 0;
                if (colors[data[i].type] !== undefined) {lesson.color = colors[data[i].type];}
                lesson.name = data[i].nickname;
                lesson.room = data[i].room;
                //lesson.description = element["name"]+'<br /><a href='+element["link"]+'>Karta předmětu</a>';
                lesson.description = '';
                lesson.type = data[i].type;
                lesson.status = "displayed"; // displayed, hidden, removed, starred
                lesson.selected = false; // false, true
                lesson.subject_id = data[i].index;
                await timetableAddLesson(lesson, () => {});
            }
            loadTimetable();
            changePageFrame("timetable");
        }
    });
}
/* Tran - end */
