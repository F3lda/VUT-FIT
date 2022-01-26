
function addAuctionRequest(data, cmd, type, callback)
{
    jQuery.ajax({
        url: './API/index.php?cmd=' + cmd,
        type: type,
        data: JSON.stringify(data),
        contentType: 'application/json; charset=utf-8',
        dataType: 'json',
        async: true,
        success: function (response) {callback(response);},
        error: function (response) {$("#failModal").modal('show');;}
    });		
}

$(function() {
    $('#createAuctionForm').submit(function() {
		
		document.body.style.cursor = 'wait';
		document.getElementsByTagName("button")[0].style.cursor = 'wait';
		
        console.log("HELLO");
        auction = new Object();
        auction.a_name = $('#a_name').val();
        auction.a_description= $('#a_description').val();
        auction.a_start_cost = $('#a_start_cost').val();
        auction.a_min_cost = $('#a_min_cost').val();
        auction.a_max_cost = $('#a_max_cost').val();
        auction.a_category = $('#a_category').val();
        auction.a_rule = $('#a_rule').val()
        auction.a_type = $('#a_type').val();
        auction.a_end_date =  $('#a_end_date').val();
        


        file = $('#a_image')[0].files[0];
        if (file != undefined) {
            formData = new FormData();
            if (file.type.match(/image.*/)) {
                formData.append("json", JSON.stringify(auction));
                formData.append("image", file);
                $.ajax({
                    url: "./API/index.php?cmd=addAuction",
                    type: "POST",
                    data: formData,
                    processData: false,
                    contentType: false,
                    dataType: 'json',
                    async: true,
                    success: function(data){
                        $("#modal-body-success").append("<strong>Úspěch!</strong> Aukce s ID "+ data['inserted_id'] + " byla přidána.");
                        $("#successModal").modal('show');

                        console.log("SUCCESS");
                        console.log(data);
                    },
                    error: function(data){
                        $("#failModal").modal('show');

                        console.log("ERROR");
                        console.log(data);
                    }
                });
            } else {
                alert('Not a valid image!');
            }
        } else {
            alert('File not included!');
        }
        
        
        
        /*
        console.debug(JSON.stringify(auction));
        addAuctionRequest(auction, 'addAuction', 'POST', function(data){
            $("#modal-body-success").append("<strong>Úspěch!</strong> Aukce s ID "+ data['inserted_id'] + "byla přidána.");
            $("#successModal").modal('show');
        } );*/
        return false;
    });
    $('#a_type').on("change", function(){
        type = jQuery(this).val();
        div = document.getElementById("a_type_div");
        if (type == "poptavkova") {
            div.innerHTML = `<label for="Typ">*Minimální cena</label>
            <input type="text" class="form-control" id="a_min_cost" onkeypress="return isNumberKey(event)" value="1" required>`;
            div.innerHTML += `<label for="Typ">*Maximální cena</label>
            <input type="text" class="form-control" id="a_max_cost" onkeypress="return isNumberKey(event)" value="2" required>`;
        } else if (type == "nabidkova") {
            div.innerHTML = `<label for="Typ">*Startovací cena</label>
            <input type="text" class="form-control" id="a_start_cost" onkeypress="return isNumberKey(event)" value="1" required>`;
        }
    });
});