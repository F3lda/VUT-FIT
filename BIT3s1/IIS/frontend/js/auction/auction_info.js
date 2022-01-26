/**
 * @file 
 * @fileoverview 
 * Používána v auctions/info
 */
function formatFullDate(date) {
    var d = new Date(date),
        month = '' + (d.getMonth() + 1),
        day = '' + d.getDate(),
        year = d.getFullYear();

    if (month.length < 2) 
        month = '0' + month;
    if (day.length < 2) 
        day = '0' + day;

    return [year, month, day].join('-');
} 

function getAuctionRequest(data, cmd, type, callback)
{
    jQuery.ajax({
        url: './API/index.php?cmd=' + cmd,
        type: type,
        data: JSON.stringify(data),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (response) {callback(response);},
        error: function () {
            //$('#auction_bid_table').hide();
            //console.log("ERROR");
            //$("body").empty();
            //$("body").append("<br><br><h2 style=\"text-align:center\">Error 400. Přístup zamítnut<br><br><a href=\"https://kajel.net/projects/IIS/\">Domov</a></h2>");
        }
    });		
}

function addBidRequest(data)
{
    jQuery.ajax({
        url: './API/index.php?cmd=participantAddBid',
        type: 'post',
        data: JSON.stringify(data),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (response) {
            //console.log(response);
            location.reload();
        },
        error: function (response) {
            console.log(response.responseText); 
            alert('ERROR: '+JSON.parse(response.responseText)["error"]);
        }
    });		
}


// FUNKCE PRO UZIVATELE, KTERY SE UCASTNI FUNKCE
function joinAuctionRequest(auction_data)
{
    jQuery.ajax({
        url: './API/index.php?cmd=participantGetInfo',
        type: 'POST',
        data: JSON.stringify(auction_data),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (data) {
            if (data['participant_status'] == 'not') {
                div = document.getElementById("auction_info_div");
                document.getElementById("auction_buttons_div").innerHTML += '<span data-toggle="modal" data-target="#join_modal" style="margin-right:1.25em"><button type="button" class="btn btn-primary">Zůčastnit se</button></span>';
                $('body').append(`
                    <div id="join_modal" class="modal fade" role="dialog">
                        <div class="modal-dialog">
                        <!-- Příhoz modal content-->
                            <div class="modal-content">
                                <div class="modal-header">
                                    <h4 class="modal-title">Zůčastnit se aukce?</h4>
                                    <button type="button" class="close" data-dismiss="modal">&times;</button>
                                </div>
                                <div class="modal-body">
                                        <button type="submit" class="btn btn-primary" id="joinAuction">Zůčastnit se</button>
                                </div>
                            </div>
                        </div>
                    </div>           
                `);
                $('#joinAuction').click(function() {
                    auction = new Object();
                    auction.auction_id = auction_data["auction_id"];
                    console.debug(JSON.stringify(auction));
                    sendAjaxRequest(auction, 'participantJoinAuction', 'POST', function(data) {
                        //console.log(data);
                        location.reload();
                    });
                    return false;
                });
            } else if (data['participant_status'] == 'schvaleno'){
                
                //console.log('participantGetInfo', data)
                div = document.getElementById("auction_buttons_div");
                div.innerHTML += '<span data-toggle="modal" data-target="#prihoz_modal" style="margin-right:1.25em">\
                                    <button type="button" class="btn btn-primary">Příhoz</button> \
                                </span> \
                                <button type="button" class="btn btn-primary" id="leaveAuction">Odejít z aukce</button>';
                $('body').append(`
                            <div id="prihoz_modal" class="modal fade" role="dialog">
                            <div class="modal-dialog">
                            <!-- Příhoz modal content-->
                                <div class="modal-content">
                                    <div class="modal-header">
                                        <h4 class="modal-title">Současná cena:</h4>
                                        <button type="button" class="close" data-dismiss="modal">&times;</button>
                                    </div>
                                    <div class="modal-body">
                                        <form id="prihozForm">
                                            <div class="form-group">
                                                <label for="a_price" class="font-weight-bold">Příhoz:</label>
                                                <input type="number" class="form-control" id="a_bid">
                                            </div>
                                            <button type="submit" class="btn btn-primary">Přidat</button>
                                        </form>
                                    </div>
                                </div>
                            </div>
                        </div> 
                `);
                $('#prihozForm').submit(function() {
                    auction = new Object();
                    auction.auction_id = auction_data["auction_id"];
                    auction.bid = $('#a_bid').val();
                    console.debug(JSON.stringify(auction));
                    addBidRequest(auction);
                    return false;
                });
            } else if (data['participant_status'] == 'cekajici') {
                div = document.getElementById("auction_info_div");
                div.innerHTML += '<p><strong>Musíte počkat na schválení licitátorem aukce.</strong></p>';
            }
        },
        error: function () {
            console.log("participantGetInfo ERROR");
        }
    });		
}

function generateTables(auction_id, auction_state) {
    div2 = document.getElementById("table_buttons");
    div2.innerHTML += '<div class="btn-group"><button type="button" class="btn btn-primary" id="triggerBids">Příhozy</button><button type="button" class="btn btn-info" id="triggerParticipants">Účastníci</button></div>';
    $("#triggerBids").click(function() {
        $("#auction_participants_table").hide();
        $("#auction_bid_table").show();
        var tab = $('#auction_bid_data');
        loadBids(auction_id, tab);
    })
    $("#triggerParticipants").click(function() {
        $("#auction_participants_table").show();
        $("#auction_bid_table").hide();
        var tab = $('#auction_participants_data');
        loadParticipants(auction_id, auction_state, tab);
    })
}


function loadBids(id, tab)
{ 
    auction = new Object();
    auction.auction_id = id;
    console.debug(JSON.stringify(auction));
    getAuctionRequest(auction, 'auctionGetAllBids', 'POST',
            function (data) {
                console.log("auctionGetAllBids: ", data);
                tab.empty();
                if (Object.keys(data).length-1 == 0) {
                    $(tab).replaceWith("<p>K aukci zatím není žádný příhoz</p>");
                    return;
                }
                for (var i = 0; i < Object.keys(data).length-1; i++)
                {
                    //console.log(data);
                    var row = "";
                    row += '<td class="a_name">' + getUsername(data[i].uzivatel_id) + '</td>';
                    row += '<td class="a_category">' + data[i].cena + ' Kč</td>';
                    row += '<td class="a_type">' + data[i].cas + '</td>';
                    tab.append("<tr>" + row + '</tr>');
                }
        }
    );
}

function loadParticipants(id, auction_state, tab)
{ 
    auction = new Object();
    auction.auction_id = id;
    console.debug(JSON.stringify(auction));
    getAuctionRequest(auction, 'getAllParticipants', 'POST',
            function (data) {
                console.log("getAllParticipants", data);
                tab.empty();
                if (Object.keys(data).length-1 == 0) {
                    $(tab).replaceWith("<p>K aukci zatím není žádný účastník</p>");
                    return;
                }
                for (var i = 0; i < Object.keys(data).length-1; i++)
                {
                    // TADY GENERUJE HTML TABULKU UŽIVATELŮ
                    state = data[i].stav;
                    var row = "";
                    row += '<td class="user-userid" data>' + data[i].uzivatel_id + '</td>';
                    row += '<td class="user-username" data>' + getUsername(data[i].uzivatel_id) + '</td>';
                    row += '<td class="user-state">' + formatUcastnik(state) + '</td>';
                    if(state == "cekajici"){
                        row += '<td><div class="col text-center"> \
                        <button type="button" class="btn btn-success confirm-user-participant" data-toggle="tooltip" title="Potvrdit účast"><i class="fa fa-check"></i></button> \
                        <button type="button" class="btn btn-danger reject-user-participant" data-toggle="tooltip" title="Odmítnout účast"><i class="fa fa-times"></i></button> \
                        </div>\
                        </td>';
                    }
                    else if (state == "schvaleno") {
                        if (auction_state == 'ukoncena') {
                            row += '<td><div class="col text-center"> \
                            <button type="button" class="btn btn-success make-winner" data-toggle="tooltip" title="Potvrdit účast">Zvolit jako výherce</button> \
                            </div> \
                            </td>';
                        } else {
                            row += '<td><div class="col text-center"> \
                            <p>Nic</p> \
                            </div> \
                            </td>';
                        }
                    }
                    tab.append("<tr>" + row + '</tr>');
                }
                $("#auction_participants_table").on('click', '.confirm-user-participant', function() {
                    participant = new Object();
                    participant.participant_id = $(this).closest('tr').find('.user-userid').text();
                    participant.auction_id = id;
                    participant.licitator_id = logged_user["user_id"];
                    console.debug(JSON.stringify(participant));
                    jQuery.ajax({
                        url: './API/index.php?cmd=authorizeParticipant',
                        type: 'POST',
                        data: JSON.stringify(participant),
                        contentType: 'application/json; charset=utf-8',
                        dataType: 'json',
                        async: true,
                        success: function (data) {
                            console.log(data);
                            location.reload();
                        }
                    });		
                });
                $("#auction_participants_table").on('click', '.reject-user-participant', function() {
                    participant = new Object();
                    participant.participant_id = $(this).closest('tr').find('.user-userid').text();
                    participant.auction_id = id;
                    participant.licitator_id = logged_user["user_id"];
                    console.debug(JSON.stringify(participant));
                    jQuery.ajax({
                        url: './API/index.php?cmd=rejectParticipant',
                        type: 'POST',
                        data: JSON.stringify(participant),
                        contentType: 'application/json; charset=utf-8',
                        dataType: 'json',
                        async: true,
                        success: function (data) {
                            console.log(data);
                            location.reload();
                        }
                    });		
                });
                $("#auction_participants_table").on('click', '.make-winner', function() {
                    participant = new Object();
                    participant.winner_id = $(this).closest('tr').find('.user-userid').text();
                    participant.auction_id = id;
                    //participant.licitator_id = logged_user["user_id"];
                    console.debug(JSON.stringify(participant));
                    jQuery.ajax({
                        url: './API/index.php?cmd=finishAuction',
                        type: 'POST',
                        data: JSON.stringify(participant),
                        contentType: 'application/json; charset=utf-8',
                        dataType: 'json',
                        async: true,
                        success: function (data) {
                            console.log(data);
                            location.reload();
                        }
                    });
                });
            }
    );
}

function getUserInfo(user, callback) {
    jQuery.ajax({
        url: './API/index.php?cmd=getuser',
        type: 'POST',
        data: JSON.stringify(user),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (data) {callback(data);},
        error: function () {
            console.log("User was not found");
        }
    });	
}

function loadAuctionInfo(id)
{
    auction = new Object();
    auction.auction_id = id;
    //console.debug(JSON.stringify(auction));
    getAuctionRequest(auction, 'getAuction', 'POST', function (data) {
                //TVORBA tlacitek
                //Jestli logged uživatel je autorem aukce
                buttons_div = document.getElementById("auction_buttons_div");
                if (parseInt(logged_user["user_id"]) == parseInt(data["autor_id"]) && data['stav'] == 'navrzena') {
                    buttons_div.innerHTML += '<span data-toggle="modal" data-target="#editAuctionModal" style="margin-right:1.25em"><button type="button" class="btn btn-primary" id="editAuctionButton">Upravit</button></span>';
                    buttons_div.innerHTML += '<span data-toggle="modal" data-target="#confirm-delete" style="margin-right:1.25em"><button type="button" class="btn btn-danger" id="removeAuction">Smazat aukci</button></span>';
                } 
                else if (data["licitator_id"] != logged_user["user_id"] && data['stav'] == 'probiha') {
                    // Can participate in auction
                    if (is_logged["logged_in"] == true && (parseInt(logged_user["user_id"]) != parseInt(data["autor_id"]))) {
                        joinAuctionRequest(auction);
                    } else if (is_logged["logged_in"] == true && (parseInt(logged_user["user_id"]) == parseInt(data["autor_id"]))){
                        buttons_div.innerHTML += '<p><strong>Nemůžete přihazovat k vlastní aukci.</strong></p>';
                    } else {
                        buttons_div.innerHTML += '<p><strong>Musíte být přihlášen k zúčastnění se aukce.</strong></p>';
                    }
                }
                if (logged_user["user_type"] == 'licitator' || logged_user["user_type"] == 'admin') {
                    if (logged_user["user_id"] == data["licitator_id"] && logged_user["user_id"] != data["autor_id"]){
                        //var tab = $('#auction_bids_data');
                        generateTables(data['id'], data['stav']);
                    }
                    if (data['stav'] == 'navrzena') {
                        if (parseInt(logged_user["user_id"]) != parseInt(data["autor_id"]) ) {
                            if (data["licitator_id"] == null) {
                                buttons_div.innerHTML += '<button type="button" class="btn btn-primary" id="becomeLicitator">Stát se licitátorem</button>';
                            } else if (parseInt(logged_user["user_id"]) == parseInt(data["licitator_id"])) {
                                buttons_div.innerHTML += '<button type="button" class="btn btn-primary" id="leaveLicitator">Odejít z aukce jako licitátor</button> \
                                                    <span data-toggle="modal" data-target="#confirm-prepare" style="margin-right:1.25em"> \
                                                    <button type="button" class="btn btn-primary" id="prepareAuction">Připravit aukci</button></span>';
                            }
                        }
                    } else if (data['stav'] == 'pripravena') {

                        if (parseInt(logged_user["user_id"]) == parseInt(data["autor_id"])){
                            console.log("");
                            buttons_div.innerHTML += '<p><strong>Aukce je ve stavu připravena.</strong></p>';
                        }
                    } else if (data['stav'] == 'probiha') {
                        if (parseInt(logged_user["user_id"]) == parseInt(data["licitator_id"])) {
                            buttons_div.innerHTML += '<p><strong>K zůčasnění do aukce nesmíte být licitátorem této aukce.</strong></p>';
                            buttons_div.innerHTML += '<p><strong>Nemůžete odejít jako licitátor, pokud aukce probíhá.</strong></p>';
                        }
                    }
                }
                if (data['stav'] == 'vyhodnocena') {
                    buttons_div.innerHTML += '<p><strong>Aukce je již ukončena.</strong></p>';
                    div2 = document.getElementById("auction_winner");
                    div2.innerHTML += "<strong>Výherce:</strong><br>" + getUsername(data["vyherce_id"]);
                } else if (data['stav'] == 'zrusena') {
                    buttons_div.innerHTML += '<p><strong>Aukce byla zrušena.</strong></p>';
                }
                $('body').append(`
                    <!-- AUCTION EDIT -->
                    <div id="editAuctionModal" class="modal fade" role="dialog">
                        <div class="modal-dialog">
                        <!-- Modal content-->
                        <div class="modal-content">
                            <div class="modal-header">
                                <h4 class="modal-title">Upravit aukci</h4>
                                <button type="button" class="close" data-dismiss="modal">&times;</button>
                            </div>
                            <div class="modal-body">
                                <form id="editForm">
                                    <div class="form-group">
                                        <label for="a_name" class="font-weight-bold">Název:</label>
                                        <input type="text" class="form-control" id="a_name" value="${data["nazev"]}" readonly>
                                    </div>
                                    <div class="form-group">
                                        <label for="a_description" class="font-weight-bold">Popis:</label>
                                        <textarea class="form-control" rows="5" id="a_description">${data["popis"]}</textarea>
                                    </div>
                                    <div class="form-group">
                                        <label for="a_category" class="font-weight-bold">Kategorie:</label>
                                        <p>Současná kategorie: ${formatCategory(data["kategorie"])}</p>
                                        <select class="form-control" id="a_category">
                                            <option disabled selected value>-- Zvolit kategorií --</option>
                                            <option value="aktiva">Aktiva</option>
                                            <option value="sluzby">Služby</option>
                                            <option value="majetek">Majetek</option>
                                            <option value="zbozi">Zboží</option>
                                        </select>
                                    </div>
                                    <div class="form-group">
                                        <label for="a_type" class="font-weight-bold">Typ:</label>
                                        <p>Současný typ: ${formatType(data["typ"])}</p>
                                        <select class="form-control" id="a_type">
                                            <option disabled selected value>-- Zvolit typ --</option>
                                            <option value="nabidkova">Nabídková</option>
                                            <option value="poptavkova">Poptávková</option>
                                        </select>
                                    </div>
                                    <div class="form-group pt-2" id="a_type_div">
		  	                        </div>
                                    <div class="form-group">
                                        <label for="a_type" class="font-weight-bold">Pravidlo:</label>
                                        <p>Současné pravidlo: ${formatRule(data["pravidla"])}</p>
                                        <select class="form-control" id="a_rule">
                                            <option disabled selected value>-- Zvolit pravidlo --</option>
                                            <option value="otevrena">Otevřená</option>
                                            <option value="uzavrena">Uzavřená</option>
                                        </select>
                                    </div>
                                    <button type="submit" class="btn btn-primary">Upravit</button>
                                </form>
                            </div>
                        </div>
                        </div>
                    </div>
                    <!-- CONFIRM DELETE -->
                    <div class="modal fade" id="confirm-delete" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
                        <div class="modal-dialog">
                            <div class="modal-content">
                                <div class="modal-body">
                                Jste si jist? Tato akce je nenavratitelná.
                                </div>
                                <div class="modal-footer">
                                    <button type="button" class="btn btn-default" data-dismiss="modal">Zrušit</button>
                                    <a class="btn btn-danger btn-ok" id="removeAuctionConfirm">Smazat</a>
                                </div>
                            </div>
                        </div>
                    </div>     
                    <!-- CONFIRM PREPARE -->
                    <div class="modal fade" id="confirm-prepare" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
                        <div class="modal-dialog">
                            <div class="modal-content">
                                <div class="modal-body">
                                    <form id="prepareAuctionForm">
                                        <div class="form-group">
                                            <label for="end_date">Začátek aukce:</label>
                                            <input type="date" class="form-control" id="start_date" value="2010-10-10">
                                        </div>
                                        <div class="form-group">
                                            <label for="end_date">Konec Aukce:</label>
                                            <input type="date" class="form-control" id="end_date">
                                        </div>
                                        <button type="submit" class="btn btn-primary">Připravit</button>
                                    </form>
                                </div>
                                <div class="modal-footer">
                                    <button type="button" class="btn btn-danger" data-dismiss="modal">Zrušit</button>
                                </div>
                            </div>
                        </div>
                    </div>     
                `
            );
            // Dont accept past dates in calendar
            document.getElementById("start_date").min = new Date().getFullYear() + "-" +  parseInt(new Date().getMonth() + 1 ) + "-" + parseInt(new Date().getDate() + 1);
            document.getElementById("start_date").value = new Date().getFullYear() + "-" +  parseInt(new Date().getMonth() + 1 ) + "-" + parseInt(new Date().getDate() + 1);
            document.getElementById("end_date").min = new Date().getFullYear() + "-" +  parseInt(new Date().getMonth() + 1 ) + "-" + parseInt(new Date().getDate() + 1);
            if (data["obrazek"] != "") {
                document.getElementById("auction_image").src = data["obrazek"];
            }

            $('#a_type').on("change", function(){
                type = jQuery(this).val();
                div = document.getElementById("a_type_div");
                console.log(data['typ']);
                if (type == "poptavkova") {
                    div.innerHTML = `<label for="Typ">*Minimální cena</label>
                    <input type="text" class="form-control" id="a_min_cost" onkeypress="return isNumberKey(event)" min="0" required>`;
                    div.innerHTML += `<label for="Typ">*Maximální cena</label>
                    <input type="text" class="form-control" id="a_max_cost" onkeypress="return isNumberKey(event)" min="0" required>`;
                } else if (type == "nabidkova") {
                    div.innerHTML = `<label for="Typ">*Startovací cena</label>
                    <input type="text" class="form-control" id="a_start_cost" onkeypress="return isNumberKey(event)" min="0" required>`;
                }
            });
            $('#editAuctionButton').click(function(){
                type = data["typ"];
                div = document.getElementById("a_type_div");
                if(type == 'poptavkova') {
                    div.innerHTML =`<label for="Typ">*Minimální cena</label>
                                    <input type="text" class="form-control" id="a_min_cost" onkeypress="return isNumberKey(event)" value="${data["min_cena"]}" required>`;
                    div.innerHTML +=`<label for="Typ">*Maximální cena</label>
                                    <input type="text" class="form-control" id="a_max_cost" onkeypress="return isNumberKey(event)" value="${data["max_cena"]}" required>`;
                } else if (type == 'nabidkova') {
                    div.innerHTML = `<label for="Typ">*Startovací cena</label>
                                    <input type="text" class="form-control" id="a_start_cost" onkeypress="return isNumberKey(event)" value="${data["vyvolavaci_cena"]}" required>`;
                }
            });
            $('#editForm').submit(function() {
                auction = new Object();
                auction.auction_id = data["id"];
                auction.a_description = $('#a_description').val();
                auction.a_category = $('#a_category').val();
                auction.a_type = $('#a_type').val();
                auction.a_rule = $('#a_rule').val();
                if (auction.a_type == 'nabidkova') {
                    auction.a_start_cost = $('#a_start_cost').val();
                } else {
                    auction.a_min_cost = $('#a_min_cost').val();
                    auction.a_max_cost = $('#a_max_cost').val();
                }
                console.debug(JSON.stringify(auction));
                sendAjaxRequest(auction, 'editAuction', 'POST', function(data) {
                    //console.log(data);
                    location.reload();
                });
                return false;
            });
            $('#becomeLicitator').click(function() {
                auction = new Object();
                auction.auction_id = data["id"];
                console.debug(JSON.stringify(auction));
                sendAjaxRequest(auction, 'addLicitator', 'POST', function(data) {
                    //console.log(data);
                    location.reload();
                });
                return false;
            });
            $('#leaveLicitator').click(function() {
                auction = new Object();
                auction.auction_id = data["id"];
                console.debug(JSON.stringify(auction));
                sendAjaxRequest(auction, 'removeLicitator', 'POST', function(data) {
                    //console.log(data);
                    location.reload();
                });
                return false;
            });
            $('#prepareAuctionForm').submit(function() {
                auction = new Object();
                auction.auction_id = data["id"];
                auction.start_time = $('#start_date').val();
                auction.end_time = $('#end_date').val();
                console.debug(JSON.stringify(auction));
                sendAjaxRequest(auction, 'prepareAuction', 'POST', function(data) {
                    console.log("Auction prepared: ", data);
                    location.reload();
                });
                return false;
            });
            $('#removeAuctionConfirm').click(function() {
                auction = new Object();
                auction.auction_id = data["id"];
                console.debug(JSON.stringify(auction));
                sendAjaxRequest(auction, 'removeAuction', 'POST', function(data) {
                    //console.log(data);
                    window.location.replace("https://kajel.net/projects/IIS/user-auctions");
                });
                return false;
            });

            for (const key in data)
            {
                console.log(key, data[key])
                if(key == "nazev"){
                    document.getElementById("auction_name").innerHTML = data[key];
                } else if (key == "popis") {
                    document.getElementById("auction_desc").innerHTML = data[key];
                } else if (key == "autor_id") {
                    document.getElementById("auction_owner").innerHTML += getUsername(data[key]);
                }
                else if (key == "licitator_id") {
                    if (data[key] != null){
                        document.getElementById("auction_licitator").innerHTML += getUsername(data[key]);
                    } else {
                        document.getElementById("auction_licitator").innerHTML += "Žádný";
                    }
                } else if (key == "stav") {
                    document.getElementById("auction_state").innerHTML += formatState(data[key]);
                } else if (key == "cas_zahajeni") {
                    document.getElementById("auction_start_date").innerHTML += formatDate(data[key]);
                } else if (key == "cas_ukonceni") {
                    document.getElementById("auction_end_date").innerHTML += formatDate(data[key]);
                }   else if (key == "typ") {
                    document.getElementById("auction_type").innerHTML += '<strong>Typ aukce: </strong>' + formatType(data[key]);
                }   else if (key == "pravidla") {
                    document.getElementById("auction_rule").innerHTML += formatRule(data[key]);
                }
            }
            if (data["typ"] == "nabidkova") {
                $('#auction_info_div').append(`<h5 id="a_start_price">Vyvolávací cena: ${parseInt(data["vyvolavaci_cena"])} Kč</h5>`);
            } else {
                $('#auction_info_div').append(`
                    <h5 id="a_start_price">Minimální cena: ${parseInt(data["min_cena"])} Kč</h5>
                    <h5 id="a_start_price">Maximální cena: ${parseInt(data["max_cena"])} Kč</h5>
                `);
            }
            if (data["pravidla"] == "uzavrena" && data["licitator_id"] != logged_user["user_id"]) {
                $('#auction_bid_table').hide()
            }
        }
    );
}



/**
 * Tento kód se volá při načtení stránky
 */
$(document).ready(function(){
    var tab = $('#auction_bid_data');
    var url_string = window.location.href;
    var url = new URL(url_string);
    var auction_id = url.searchParams.get("auction_id")
    //console.log(auction_id);
    //loadAttendees(auction_id, tab);
    $('#auction_participants_table').hide();
    var tab = $('#auction_bid_data');
    loadAuctionInfo(auction_id);
    loadBids(auction_id, tab);
});
