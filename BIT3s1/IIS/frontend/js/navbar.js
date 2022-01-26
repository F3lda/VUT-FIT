/**
 * @file generate_page.js
 * @fileoverview Skript generující navigační panel podle typu uživatele
 * Používána každou html stránkou
 */


/**
 * Request požadující údaje od uživatele
 * @param cmd - is_logged_in nebo get_logged_in
 */
function getSessionInfo(cmd)
{
    var tmp;
    jQuery.ajax({
        url: './API/index.php?cmd=' + cmd,
        type: 'GET',
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: false,
        success: function (data) {
            tmp = data;
        }
    });
    return tmp;
}

/**
 * Generuje horní navigační panel podle typu uživatele
 * @param is_logged - Jestli je někdo přihlášen
 * @param user_data - Údaje o uživateli (neregistrovaný, registrovaný, licitátor, admin)
 */
function generateNavbar(is_logged, user_data)
{
    //Aukce (Hlavní stránka)
    var li = document.createElement('li');
    li.setAttribute('class', 'nav-item')
    var a = document.createElement('a');
    a.setAttribute('href', './');
    a.setAttribute('class', 'nav-link');
    a.innerHTML = 'Aukce';
    li.appendChild(a);
    document.getElementById('ulNavbarLeft').appendChild(li);
    

    //Pravá strana
    //Pokud není nikdo přihlášen
    if(is_logged['logged_in'] == false) {
        //LOGIN
        li = document.createElement('li');
        li.setAttribute('class', 'nav-item')
        //li.setAttribute('class', 'active')
        a = document.createElement('a');
        a.setAttribute('class', 'nav-link');
        a.setAttribute('href', './login');
        a.innerHTML = '<i class="fa fa-sign-in"></i> Přihlásit se';
        li.appendChild(a);
        document.getElementById('ulNavbarRight').appendChild(li);

        //Registrace tlacitko
        li = document.createElement('li');
        li.setAttribute('class', 'nav-item')
        //li.setAttribute('class', 'active')
        a = document.createElement('a');
        a.setAttribute('class', 'nav-link');
        a.setAttribute('href', './registration');
        a.innerHTML = '<i class="fa fa-user"></i> Registrovat se';
        li.appendChild(a);
        document.getElementById('ulNavbarRight').appendChild(li);
    }
    //Toto se vygeneruje pokud je uzivatel prihlasen
    else {
        //Navrhnout aukci
        var li = document.createElement('li');
        li.setAttribute('class', 'nav-item')
        //li.setAttribute('class', 'active')
        var a = document.createElement('a');
        a.setAttribute('class', 'nav-link');
        a.setAttribute('href', './auctions-create');
        a.innerHTML = 'Navrhnout aukci';
        li.appendChild(a);
        document.getElementById('ulNavbarLeft').appendChild(li);
        //Prihlasen jako:
        
        
        li = document.createElement('li');
        li.setAttribute('class', 'nav-item')
        a = document.createElement('a');
        a.setAttribute('class', 'nav-link');
        a.innerHTML = 'Přihlášen jako: ' + user_data['user_name'] + ' (' + user_data['user_type'] + ')';
        li.appendChild(a);
        document.getElementById('ulNavbarRight').appendChild(li);
        
        if(user_data['user_type'] == 'uzivatel') {
            
            li = document.createElement('li');
            li.setAttribute('class', 'nav-item dropdown');
            li.innerHTML = `
            <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown" role="button" data-toggle="dropdown" aria-expanded="false"></a>
            <div class="dropdown-menu dropdown-menu-right" aria-labelledby="navbarDropdown">
                <a class="dropdown-item" href="./user-auctions">Moje aukce</a>
                <a class="dropdown-item" href="./user-attended">Účastním se aukcí</a>
                <div class="dropdown-divider"></div>
                <a class="dropdown-item" href="./user">Profil</a>
                <a class="dropdown-item" id="logoutLink" style="cursor: pointer;">Odhlásit se</a>
            </ul>`;
            document.getElementById('ulNavbarRight').appendChild(li);
        }
        if(user_data['user_type'] == 'licitator' || user_data['user_type'] == 'admin') {
            var li = document.createElement('li');
            li.setAttribute('class', 'nav-item')
            //li.setAttribute('class', 'active')
            var a = document.createElement('a');
            a.setAttribute('class', 'nav-link');
            a.setAttribute('href', 'auctions-approve');
            a.innerHTML = 'Schvalovat aukce';
            li.appendChild(a);
            document.getElementById('ulNavbarLeft').appendChild(li);

            var li = document.createElement('li');
            li.setAttribute('class', 'nav-item')
            //li.setAttribute('class', 'active')
            var a = document.createElement('a');
            a.setAttribute('class', 'nav-link');
            a.setAttribute('href', 'auctions-managed');
            a.innerHTML = 'Spravované aukce';
            li.appendChild(a);
            document.getElementById('ulNavbarLeft').appendChild(li);

            li = document.createElement('li');
            li.setAttribute('class', 'nav-item')
            //li.setAttribute('class', 'active')
            a = document.createElement('a');
            a.setAttribute('class', 'nav-link');
            a.setAttribute('href', './auctions-create');
            a.innerHTML = 'Navrhnout aukci';
            li.appendChild(a);
            li = document.createElement('li');
            li = document.createElement('li');
            li.setAttribute('class', 'nav-item dropdown');
            li.innerHTML = `
            <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown" role="button" data-toggle="dropdown" aria-expanded="false"></a>
            <div class="dropdown-menu dropdown-menu-right" aria-labelledby="navbarDropdown" id="dropdown-div">
                <a class="dropdown-item" href="./user-auctions">Moje aukce</a>
                <a class="dropdown-item" href="./user-attended">Účastním se aukcí</a>
                <div class="dropdown-divider"></div>
                <a class="dropdown-item" href="./user">Profil</a>
                <a class="dropdown-item" id="logoutLink" style="cursor: pointer;">Odhlásit se</a>
            </ul>`;
            document.getElementById('ulNavbarRight').appendChild(li);
            if(user_data['user_type'] == 'admin') {
                a = document.createElement('a');
                a.innerHTML = '<a class="dropdown-item" href="./admin-users">Spravovat uživatele</a>';
                document.getElementById("dropdown-div").prepend(a)
            }
        }
    }
}


$(function() {
    
});

/**
 * Tento kód se volá při načtení stránky
 */
$(document).ready(function(){
    is_logged = getSessionInfo('is_loggedin');
    console.log(is_logged);
    logged_user = getSessionInfo('get_loggedin');
    console.log(logged_user);
    generateNavbar(is_logged, logged_user);
    document.getElementById('mainNavList');
});