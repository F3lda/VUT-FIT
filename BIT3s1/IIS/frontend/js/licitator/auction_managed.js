/**
 * @file auction_methods.js
 * @fileoverview Skript na přímou editaci a správu aukcí
 * Používána v auctions/edit?
 */

var page = 1;
var item_count = 5;
function loadAuctions(tab, item_count, page_number)
{
    jQuery.getJSON("./API/index.php?cmd=getAuctionsManagedByLicitator&page_number="+page_number+"&item_count="+item_count,
            function (data) {
                //console.log(data);
                tab.empty();
                for (var i = 0; i < Object.keys(data).length-1; i++)
                {
                    // TADY GENERUJE HTML TABULKU UŽIVATELŮ
                    var row = "";
                    row += '<td class="a_name">' + data[i].nazev + '</td>';
                    row += '<td class="a_category">' + getUsername(data[i].autor_id) + '</td>';
                    row += '<td class="a_type">' + filternull(formatCategory(data[i].kategorie)) + '</td>';
                    row += '<td class="a_type">' + filternull(formatType(data[i].typ)) + '</td>';
                    row += '<td class="a_type">' + filternull(formatState(data[i].stav)) + '</td>';
                    tab.append("<tr onclick=\"window.location='auctions-info?auction_id="+ data[i].id + "'\">" + row + '</tr>');
                }
        }
    );
}

function pagination(tab, pages, item_count, page) {
    var pagination = document.getElementById('user-auction-pagination');
    pagination.innerHTML = '';
    for (var page = 1; page <= pages; page++){
        pagination.innerHTML += `<li class="page-item" value=${page}><a class="page-link" href="javascript:void(0);" >${page}</a></li>`;
    }
    $('.page-item').on('click', function(){
        $(this).attr('class', 'page-item active');
        $(this).siblings().attr('class', 'page-item');
    });
    $('.page-link').on('click', function(){
        //$(tab).empty();
        page = $(this).text();
        //console.log(page)
        loadAuctions(tab, item_count, page);
    });
}


function generateMenu(tab, item_count, page)
{
    loadAuctions(tab, item_count, page);
    
    var pages;
    jQuery.ajax({
        url: './API/index.php?cmd=getAuctionsManagedByLicitatorCount',
        type: "POST",
        data: null,
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (response) {
            $('#licitator_managed_count').append(`<p class="text-secondary">Celkově ${response["item_count"]} aukcí, které organizujete</p>`);
            pages = Math.ceil(response["item_count"]/item_count);
            pagination(tab, pages, item_count, page);
        }
    });
}


/**
 * Tento kód se volá při načtení stránky
 */
$(document).ready(function(){
    var tab = $('#data');
    //var page = document.getElementById("user-table-pagination").value;
    generateMenu(tab, item_count, page);
    $('.page-item:first').attr('class', 'page-item active');
    $('#select_auctions_count').on("change", function(){
        item_count = $(this).val();
        console.log(item_count)
        generateMenu(tab, item_count, page);
    });
});
