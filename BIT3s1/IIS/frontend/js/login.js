function loginRequest(data, cmd, type, callback)
{
    jQuery.ajax({
        url: './API/index.php?cmd=' + cmd,
        type: type,
        data: JSON.stringify(data),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (response) {callback(response);},
        error: function (result) {
            $("#status_div").replaceWith(`<div class="alert alert-danger" role="alert">
                                        Neplatné údaje!
                                    </div>`);
            
        }
    });		
}



$(function() {
    $('#loginForm').submit(function(event) {
		
		document.body.style.cursor = 'wait';
		document.getElementsByTagName("button")[0].style.cursor = 'wait';
		
        user = new Object();
        user.username = $('#username').val();
        user.password = $('#pwd').val();
        loginRequest(user, 'login', 'POST', function(data) {
            if (data['status'] == 'OK' && data['logged_in'] == true) {
				
                
				
                setTimeout(() => {window.location.replace("./");}, 333);
                
            }
        });
        return false;
    });
    
    $('#logoutLink').click(function() { 
        sendAjaxRequest('', 'logout', 'GET', function(data) {
            //console.log(data);
            window.location.replace("./");
        });
        return false; 
    });
});
