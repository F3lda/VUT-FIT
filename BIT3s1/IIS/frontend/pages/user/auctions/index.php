<?php
require_once(__DIR__ . '/../../../../include/common.php');

// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Moje aukce', 'css' => [''], 'js' => ['./frontend/js/user/user_auctions.js']];
include(__DIR__ .'/../../../components/html_head.php');

// MENU
include(__DIR__ .'/../../../components/common/menu.php');
$cmpnt_menu['active'] = 'Admin';
include(__DIR__ .'/../../../components/menu.php');


// ONLY ADMIN CAN ACCESS THIS PAGE
//only_admin();

?>

<div class="container pt-1 pr-4 pl-4">
	<div class="row">
		<div class="mr-auto pt-5" id="user_auctions_count">
		</div>
		<div class="form-group form-inline col-md-1 ml-auto">
			<label for="item_count">Počet: </label>
			<select class="form-control" autocomplete="off" id="select_auctions_count">
				<option selected="selected">5</option>
				<option>10</option>
				<option>25</option>
			</select>
		</div>
	</div>
	<div class="row">
		<table id="auction_table" class="table table-striped table-bordered table-hover">
			<div class="col-12">
				<thead>
					<tr>
						<th>ID</th>
						<th>Název</th>
						<th>Kategorie</th>
						<th>Typ</th>
					</tr>
				</thead>
				<tbody id="data">
					<!-- to be filled using AJAX -->
				</tbody>
			</div>
		</table>
	</div>
</div>


<div class="container">       
  <ul class="pagination justify-content-center" id="user-auction-pagination">
  </ul>
</div>

