/**
 * @file auctions.js
 * @fileoverview Skript na načtení aukcí na hlavní stránce
 * Zatím používána pouze hlavní stránkou
 */
var page = 1;
var item_count = 8;
function loadAuctions(tab, item_count, page_number)
{
    jQuery.getJSON("./API/index.php?cmd=getAuctions&page_number="+page_number+"&item_count="+item_count,
            function (data) {
            tab.empty()
            console.log(data)
            for (var i = 0; i < Object.keys(data).length-1; i++)
            {   
                var obrazek_url = "https://via.placeholder.com/700x400";
                if (data[i].obrazek != "") {
                    obrazek_url = data[i].obrazek;
                }
                var box = "";
                box += `
                <div class="col-lg-3 mb-3 d-flex align-items-stretch" id="aukce-${data[i].id}">
                    <div class="card" style="width: 18rem;">
                        <a href="./auctions-info?auction_id=${data[i].id}">
                            <img src="${obrazek_url}" class="card-img-top" alt="https://via.placeholder.com/700x400">
                        </a>
                        <div class="card-body d-flex flex-column" id="card_body_${data[i].id}">
                            <h5 class="card-title"><a href="./auctions-info?auction_id=${data[i].id}">${data[i].nazev}</a></h5>
                            <p class="card-text mb-4">${data[i].popis}</p>
                        </div>
                    </div>
                </div> `;
                tab.append(box);
                if(logged_user["user_type"] == 'licitator' || logged_user["user_type"] == 'admin') {
                    card_body_id = "#card_body_" + data[i].id;
                    $(card_body_id).append(`<p class="card-text mb-4"><strong>Stav: ${formatState(data[i].stav)}</strong></p>`);
                }
            }
            //number_of_auctions = $('#auction_data .col-lg-3').length;
            //console.log(number_of_auctions);   
        }
    );
}

function pagination(tab, pages, item_count, page)
{
    var pagination = document.getElementById('pagination_wrapper');
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
        //console.log(page)
        loadAuctions(tab, item_count, page);
    })

}

function generateMainMenu(tab, item_count, page)
{
    loadAuctions(tab, item_count, page);
    var pages;
    jQuery.ajax({
        url: './API/index.php?cmd=getAuctionsCount',
        type: "POST",
        data: null,
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (response) {
            //console.log("getAuctionsCount", response);
            pages = Math.ceil(response["item_count"]/item_count);
            pagination(tab, pages, item_count, page)
        }
    });		
    
}

$(document).ready(function(){
    var tab = $('#auction_data');
    generateMainMenu(tab, item_count, page);
    $('.page-item:first').attr('class', 'page-item active');
    $('#select_item_count').on("change", function(){
        item_count = $(this).val();;
        //console.log(item_count)
        generateMainMenu(tab, item_count, page);
    });
});