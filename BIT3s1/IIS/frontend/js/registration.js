
$(function() {
    $('#registerForm').submit(function() {
        user = new Object();
        user.username = $('#username').val();
        user.password = $('#password').val();
        user.email = $('#email').val();
        user.firstname = $('#firstname').val();
        user.lastname = $('#lastname').val();
        user.birthdate = $('#birthdate').val();
        user.phone = $('#phone').val();
        user.address = $('#address').val();
        console.debug(JSON.stringify(user));
        sendAjaxRequest(user, 'adduser', 'POST', function(data) {
            //console.log(data);
            if (data['status'] == 'OK') {
                window.location.replace("https://kajel.net/projects/IIS/");
            } else if (data['error'] == 'username or email exists') {
                alert('Email nebo uživatelské jméno již existuje!');
            } else {
                alert('Neplatné údaje!');
            }
        });
        return false;
    });
});