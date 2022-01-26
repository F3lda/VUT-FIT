<?php
require_once(__DIR__ . '/../../../../include/common.php');




// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Navrhnout aukci', 'css' => [], 'js' => ['./frontend/js/auction/create_auction.js']];
include(__DIR__ .'/../../../components/html_head.php');



// MENU
include(__DIR__ .'/../../../components/common/menu.php');
$cmpnt_menu['active'] = 'Registrace';
include(__DIR__ .'/../../../components/menu.php');

?>


<div class="container container pt-4">
	<div class="jumbotron">
	  <h3>Navrhnout aukci</h3>
	  <form id="createAuctionForm" onSubmit="return false;">
		  <div class="form-group">
			<label for="a_name">*Název:</label>
			<input type="text" class="form-control" id="a_name" required>
		  </div>
		  <div class="form-group">
			<label for="form-group">Popis:</label>
			<textarea class="form-control" id="a_description" rows="3" style="resize: none;"></textarea>
		  </div>
		  <div class="form-group">
			<label for="category">*Kategorie</label>
			<select class="form-control" id="a_category">
                <option value="aktiva">Aktiva</option>
                <option value="sluzby">Služby</option>
                <option value="nemovitosti">Nemovitosti</option>
                <option value="zbozi">Zboží</option>
            </select>
		  </div>
		  <div class="form-group">
			<label for="Typ">*Typ</label>
			<select class="form-control" id="a_type">
                <option value="nabidkova" selected="selected">Nabídková</option>
                <option value="poptavkova">Poptávková</option>
            </select>
			<div class="form-group pt-2" id="a_type_div">
				<label for="Typ">*Startovací cena</label>
				<input type="text" class="form-control" id="a_start_cost" onkeypress="return isNumberKey(event)" value="1" required>
		  	</div>
		  </div>
		  <div class="form-group">
			<label for="rule">*Pravidlo</label>
			<select class="form-control" id="a_rule">
                <option value="otevrena">Otevřená</option>
                <option value="uzavrena">Uzavřená</option>
            </select>
		  	</div>
			<!--
		  	<div class="form-group">
				<label for="a_end_date">Čas ukončení:</label>
				<input type="date" class="form-control" id="a_end_date">
		  	</div>-->
			<div class="form-group">
				<label for="a_image">*Obrázek</label>
				<input type="file" class="form-control-file" id="a_image" required>
			</div>
		  	<button type="submit" class="btn btn-primary" name="auction_submit">Navrhnout</button>
	  </form>
	</div>
  </div>

<div class="modal" id="successModal">
	<div class="modal-dialog">
		<div class="modal-content">
	
		<!-- Modal body -->
		<div class="modal-body">
			<div class="alert alert-success" id="modal-body-success">

			</div>
		</div>
		
		<!-- Modal footer -->
		<div class="modal-footer">
			<a href="user-auctions"><button type="button" class="btn btn-primary">Ok</button></a>
		</div>
		
		</div>
	</div>
</div>

<div class="modal" id="failModal">
	<div class="modal-dialog">
		<div class="modal-content">
		
		<!-- Modal body -->
		<div class="modal-body">
			<div class="alert alert-danger">
    			<strong>Neůspěch!</strong> Aukce už asi existuje.
  			</div>
		</div>
		
		<!-- Modal footer -->
		<div class="modal-footer">
			<button type="button" class="btn btn-danger" data-dismiss="modal">Zavřít</button>
		</div>
		</div>
	</div>
</div>
