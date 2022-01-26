/**
 * @file 
 * @fileoverview 
 * Používána v auctions/info
 */

/**
 * Funkce posílající request na zobrazení uživatelů přes API 
 * @param tab - HTML tabulka v admin/users/
 */
var page = 1;
var item_count = 5;
function loadUsers(tab, page)
{
    jQuery.getJSON("./API/index.php?cmd=loadusers&page_number="+page+"&item_count="+item_count,
            function (data) {
            tab.empty();
            //console.log(data);
            for (var i = 0; i < Object.keys(data).length-1; i++)
            {
                // TADY GENERUJE HTML TABULKU UŽIVATELŮ
                var row = "";
                row += '<td class="userid">' + data[i].id + '</td>';
                row += '<td class="username">' + data[i].uzivatelske_jmeno + '</td>';
                row += '<td class="email">' + filternull(data[i].email) + '</td>';
                row += '<td class="firstname">' + filternull(data[i].jmeno) + '</td>';
                row += '<td class="lastname">' + filternull(data[i].prijmeni) + '</td>';
                row += '<td class="birthdate">' + filternull(data[i].datum_narozeni) + '</td>';
                row += '<td class="phone">' + filternull(data[i].telefon) + '</td>';
                row += '<td class="address">' + filternull(data[i].adresa) + '</td>';
                row += '<td class="privileges">' + formatRole(data[i].prava) + '</td>';
                row += '<td class="ico">' + filternull(data[i].ico) + '</td>';
                row += '<td><div class="col text-center"> \
                <button type="button" class="btn btn-danger delete-user" data-toggle="tooltip" title="Odstranit uživatele"><i class="fa fa-trash"></i></button> \
                <span data-toggle="modal" data-target="#editUserModal"> \
                    <button type="button" class="btn btn-info edit-user" data-toggle="tooltip" title="Upravit uživatelské údaje" value="Upravit uživatelské údaje"><i class="fa fa-edit"></i></button> \
                </span> \
                </div>\
                </td>';
                tab.append('<tr>' + row + '</tr>');
            }
        }
    );
}

function pagination(tab, pages) {
    var pagination = document.getElementById('user-table-pagination');
    
    pagination.innerHTML = '';
    for (var page = 1; page <= pages; page++){
        pagination.innerHTML += `<li class="page-item" value=${page}><a class="page-link" href="javascript:void(0);" >${page}</a></li>`
    }
    $('.page-item').on('click', function(){
        $(this).attr('class', 'page-item active');
        $(this).siblings().attr('class', 'page-item');
    })
    $('.page-link').on('click', function(){
        //$(tab).empty();
        page = $(this).text();
        loadUsers(tab, page);
    })
}

function generateMenu(tab)
{
    loadUsers(tab, page);
    var pages;
    jQuery.ajax({
        url: './API/index.php?cmd=getUsersCount',
        type: "POST",
        data: null,
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: false,
        success: function (response) {
            //console.log(response)
            pages = Math.ceil(response["item_count"]/item_count);
            pagination(tab, pages);
        }
    });		
    
}


/**
 * Tento kód se volá při načtení stránky
 */
$(document).ready(function(){
    var tab = $('#data');

    //var page = document.getElementById("user-table-pagination").value;
    //generatePagination(page);
    generateMenu(tab);

    $("#user_table").on('click', '.delete-user', function() {
        
        if (confirm("Opravdu chcete uživatele odstranit?")) {
            user = new Object();
            user.userid = $(this).closest('tr').find('.userid').text();
            user.privileges = $(this).closest('tr').find('.privileges').text();
            console.debug(JSON.stringify(user));
            
            sendAjaxRequest(user, 'removeuser', 'POST', function(data) {
                var tab = $('#data');
            });
            loadUsers(tab, 1);

        }
      });

    $("#user_table").on('click', '.edit-user', function() {
        user = new Object();
        user.userid = $(this).closest('tr').find('.userid').text();
        user.username = $(this).closest('tr').find('.username').text();
        document.getElementById("edit_id").value = $(this).closest('tr').find('.userid').text();
        document.getElementById("edit_username").value = $(this).closest('tr').find('.username').text();
        document.getElementById("edit_email").value = $(this).closest('tr').find('.email').text();
        document.getElementById("edit_firstname").value = $(this).closest('tr').find('.firstname').text();
        document.getElementById("edit_lastname").value = $(this).closest('tr').find('.lastname').text();
        document.getElementById("edit_birthdate").value = $(this).closest('tr').find('.birthdate').text();
        document.getElementById("edit_phone").value = $(this).closest('tr').find('.phone').text();
        document.getElementById("edit_address").value = $(this).closest('tr').find('.address').text();
        document.getElementById("edit_privileges").value = $(this).closest('tr').find('.privileges').text();
        $("#edit_privileges").trigger('change');
        console.debug(JSON.stringify(user));
      });

    $("#edit_privileges").change( function() {
        edit_ico_div = document.getElementById("edit_ico_div");
        edit_ico = document.getElementById("edit_ico");
        current_ico = $('#user_table tr td:first-child:contains("' + user.userid + '")').filter(function(){
            return $(this).text() === user.userid;
        }).parent().find('.ico').text();
        
        if( $("#edit_privileges").val() == "uzivatel" )
        {
            edit_ico_div.style.display = "none";
            edit_ico.required = false;
        }
        else
        {
            edit_ico_div.style.display = "block";
            edit_ico.required = true;
            edit_ico.value = current_ico;
        }
    }); 
    $('#addForm').submit(function() {
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
            var tab = $('#data');
            generateMenu(tab);
        });
        
        return false;
    });

    $('#deleteForm').submit(function() {
        return false;
    });

    $('#editForm').submit(function() {
        user = new Object();
        user.userid = $('#edit_id').val();
        user.email = $('#edit_email').val();
        user.firstname = $('#edit_firstname').val();
        user.lastname = $('#edit_lastname').val();
        user.birthdate = $('#edit_birthdate').val();
        user.phone = $('#edit_phone').val();
        user.address = $('#edit_address').val();
        user.privileges = $('#edit_privileges').val();
        
        if( user.privileges != "uzivatel" ) {user.ico = $('#edit_ico').val();}
        
        console.debug(JSON.stringify(user));
        sendAjaxRequest(user, 'edituser', 'POST', function(data) {
            
            if (data['status'] == 'OK') {
                var tab = $('#data');
                generateMenu(tab);
            } else if (data['error'] == 'empty fields') {
                alert('Doplňte chybějící údaje!');
            } else {
                alert(data['error']);
            }
            
        });
        return false;
    });
    $('.page-item:first').attr('class', 'page-item active');
    $('#select_users_count').on("change", function(){
        item_count = $(this).val();
        generateMenu(tab);
    });
});
