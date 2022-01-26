function loadUserProfile(id)
{
    user = new Object();
    user.userid = id;
    console.debug(JSON.stringify(user));
    sendAjaxRequest(user, 'getuser', 'POST', function (data) {
            //tab.empty();
            document.getElementById("uzivatelske_jmeno").innerHTML = `<p>${data["uzivatelske_jmeno"]}</p>`;
            document.getElementById("jmeno").innerHTML = `<p>${data["jmeno"]}</p>`;
            document.getElementById("prijmeni").innerHTML = `<p>${data["prijmeni"]}</p>`;
            document.getElementById("datum_narozeni").innerHTML = `<p>${data["datum_narozeni"]}</p>`;
            document.getElementById("email").innerHTML = `<p>${data["email"]}</p>`;
            document.getElementById("telefon").innerHTML = `<p>${data["telefon"]}</p>`;
            document.getElementById("adresa").innerHTML = `<p>${data["adresa"]}</p>`;
        }
    );
}


$(document).ready(function(){
    //var tab = $('#view_profile_div');
    userid = logged_user['user_id'];
    loadUserProfile(userid);
    $('#editForm').submit(function() {
        user = new Object();
        user.userid = $('#edit_id').val();
        user.email = $('#edit_email').val();
        user.firstname = $('#edit_firstname').val();
        user.lastname = $('#edit_lastname').val();
        user.birthdate = $('#edit_birthdate').val();
        user.phone = $('#edit_phone').val();
        user.address = $('#edit_address').val();
        user.privileges = logged_user['user_type'];
        
        console.debug(JSON.stringify(user));
        sendAjaxRequest(user, 'edituser', 'POST', function(data) {
            //console.log(data);
            location.reload();
        });
        
        return false;
    });
    $(".edit-user").on('click', function() {
        user = new Object();
        user.userid = userid;
        sendAjaxRequest(user, 'getuser', 'POST', function (data) {
            //Disable date after today
            var now = new Date();
            minDate = now.toISOString().substring(0,10);
            $("#edit_birthdate").prop('max', minDate);
            //console.log(data)
            document.getElementById("edit_id").value = userid;
            document.getElementById("edit_username").value = data["uzivatelske_jmeno"];
            document.getElementById("edit_email").value = data["email"];
            document.getElementById("edit_firstname").value = data["jmeno"];
            document.getElementById("edit_lastname").value = data["prijmeni"];
            document.getElementById("edit_birthdate").value = data["datum_narozeni"];
            document.getElementById("edit_phone").value = data["telefon"];
            document.getElementById("edit_address").value = data["adresa"];
        }
        );
    });

});