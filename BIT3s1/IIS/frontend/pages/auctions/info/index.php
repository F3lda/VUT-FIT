<?php
require_once(__DIR__ . '/../../../../include/common.php');




// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Info o aukci', 'css' => [], 'js' => ['./frontend/js/auction/auction_info.js']];
include(__DIR__ .'/../../../components/html_head.php');



// MENU
include(__DIR__ .'/../../../components/common/menu.php');
$cmpnt_menu['active'] = 'Aukce';
include(__DIR__ .'/../../../components/menu.php');

?>

<div class="container pt-4" id="auction_info_container">	
	<div class="row">

		<div class="col-lg-9 mb-3 d-flex">
			<div class="card bg-light">
				<div class="row">
					<div class="col-lg-6">
						<ul class="preview-thumbnail nav nav-tabs">
							<img id="auction_image" src="https://via.placeholder.com/700x400" class="card-img-top border-dark" alt="https://via.placeholder.com/700x400">
						</ul>
					</div>
					<div class="col-lg-6 pt-3">
						<h3 id="auction_name"><!-- To be filled by auction_info.js --></h3>
						<p id="auction_type"><!-- To be filled by auction_info.js --></p>
						<p id="auction_desc"><!-- To be filled by auction_info.js --></p>
					</div>
				</div>
				<div class="row pt-4 pb-2 pr-3 pl-3">
					<div class="col-lg-6 pt-2" id="auction_buttons_div">
					
					</div>
					<div class="col-lg-6" id="auction_info_div">
					</div>
				</div>
			</div>
		</div>
		
		<div class="col-lg-3 mb-3 d-flex">
			<div class="card bg-light">
				<div class="col-md-auto" id="auction_info_div2">
					<h3 id="auction_name"><!-- To be filled by auction_info.js --></h3>
					<p id="auction_state"><strong>Stav:</strong> <br></p>
					<p id="auction_rule"><strong>Pravidlo:</strong> <br></p>
					<p id="auction_owner"><strong>Autor:</strong> <br></p>
					<p id="auction_licitator"><strong>Licitátor aukce:</strong> <br></p>
					<p id="auction_start_date"><strong>Čas zahájení:</strong> <br><!-- To be filled by auction_info.js --></p>
					<p id="auction_end_date"><strong>Čas konce:</strong><br><!-- To be filled by auction_info.js --></p>
					<p id="auction_winner"><!-- To be filled by auction_info.js --></p>
				</div>
			</div>
		</div>

	</div>
</div>

<div class="container pt-4 pr-4 pl-4">
	<div class="row" id="table_buttons">
	</div>
	<div class="row">
		<table id="auction_bid_table" class="table table-striped table-bordered table-hover">
			<div class="col-12">
				<thead>
					<tr>
						<th>Uživatelské jméno</th>
						<th>Příhoz</th>
						<th>Čas</th>
					</tr>
				</thead>
				<tbody id="auction_bid_data">
					<!-- to be filled using AJAX -->
				</tbody>
			</div>
		</table>
		<table id="auction_participants_table" class="table table-striped table-bordered table-hover">
			<div class="col-12">
				<thead>
					<tr>
						<th>ID uživatele</th>
						<th>Uživatelské jméno</th>
						<th>Stav</th>
						<th>Akce</th>
					</tr>
				</thead>
				<tbody id="auction_participants_data">
					<!-- to be filled using AJAX -->
				</tbody>
			</div>
		</table>
	</div>
</div>