function sendAjaxRequest(data, cmd, type, callback)
{
    jQuery.ajax({
        url: './API/index.php?cmd=' + cmd,
        type: type,
        data: JSON.stringify(data),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (response) {callback(response);},
        error: function (response) {console.log(response.responseText); alert('ERROR: '+JSON.parse(response.responseText)["error"]);}
    });		
}

function filternull(entry)
{
    return (entry == null ? '' : entry);
}


function isNumberKey(evt)
{
    var charCode = (evt.which) ? evt.which : evt.keyCode;
    if (charCode != 46 && charCode > 31 
    && (charCode < 48 || charCode > 57))
    return false;
    return true;
}  

function formatType(type){
    switch(type) {
        case 'poptavkova':
          return 'Poptávková'

        case 'nabidkova':
          return 'Nabídková';
      } 
}

function formatCategory(category){
    switch(category) {
        case 'zbozi':
            return 'Zboží';

        case 'nemovitosti':
            return 'Nemovitosti';
        
        case 'Služby':
            return 'Služby';

        case 'aktiva':
            return 'Aktiva';
      } 
}

function formatRule(rule){
    switch(rule) {
        case 'otevrena':
            return 'Otevřená';

        case 'uzavrena':
            return 'Uzavřená';
      } 
}

function formatState(state){
    switch(state) {
        case 'navrzena':
            return 'Navržena';

        case 'pripravena':
            return 'Připravena';
        
        case 'probiha':
            return 'Probíhá';

        case 'vyhodnocena':
            return 'Vyhodnocena';
        
        case 'ukoncena':
            return 'Ukončena';

        case 'zrusena':
            return 'Zrušena';
      } 
}

function formatDate(date){
    if(date == null) {
        return 'Žádný';
    } else {
        return date;
    }
}

function formatUcastnik(state){
    switch(state) {
        case 'cekajici':
            return 'Čekající';

        case 'schvaleno':
            return 'Schváleno';
        
        case 'zamitnuto':
            return 'Zamítnuto';

        case 'opustil':
            return 'Opustil';
    } 
}

function formatRole(role){
    switch(role) {
        case 'vyrazen':
            return 'Vyřazen';

        case 'uzivatel':
            return 'Uživatel';
        
        case 'licitator':
            return 'Licitátor';

        case 'admin':
            return 'Admin';
    } 
}

function getUsername(id){
    var user = new Object();
    user.userid = id;
    var username;
    jQuery.ajax({
        url: './API/index.php?cmd=getusername',
        type: 'POST',
        data: JSON.stringify(user),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: false,
        success: function (response) {
            username = response['uzivatelske_jmeno'];
        },
        error: function (response) {alert(response);}
    });
    return username;
}
