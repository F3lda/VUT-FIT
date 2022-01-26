<?php
require_once(__DIR__ . '/../../../include/common.php');




// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Profil', 'css' => [], 'js' => ['./frontend/js/user/profile.js']];
include(__DIR__ .'/../../components/html_head.php');



// MENU
include(__DIR__ .'/../../components/common/menu.php');
$cmpnt_menu['active'] = 'Aukce';
include(__DIR__ .'/../../components/menu.php');


?>


<div class="container pt-4">
	<div class="jumbotron">
	  	<div class="col-md-12">
		  	<h3>Profil</h3>
		  	<div class="row">
				<div class="col-md-6">
					<label>Uživatelské jméno:</label>
				</div>
				<div class="col-md-6" id="uzivatelske_jmeno">
					<p>Expert</p>
				</div>
			</div>
			<div class="row">
				<div class="col-md-6">
					<label>Jméno:</label>
				</div>
				<div class="col-md-6" id="jmeno">
					<!-- To be filled by profile.js -->
				</div>
			</div>
			<div class="row">
				<div class="col-md-6">
					<label>Příjmení:</label>
				</div>
				<div class="col-md-6" id="prijmeni">
					<!-- To be filled by profile.js -->
				</div>
			</div>
			<div class="row">
				<div class="col-md-6">
					<label>Datum narození:</label>
				</div>
				<div class="col-md-6" id="datum_narozeni">
					<!-- To be filled by profile.js -->
				</div>
			</div>
			<div class="row">
				<div class="col-md-6">
					<label>Email:</label>
				</div>
				<div class="col-md-6" id="email">
					<!-- To be filled by profile.js -->
				</div>
			</div>
			<div class="row">
				<div class="col-md-6">
					<label>Telefon:</label>
				</div>
				<div class="col-md-6" id="telefon">
					<!-- To be filled by profile.js -->
				</div>
			</div>
			<div class="row">
				<div class="col-md-6">
					<label>Adresa:</label>
				</div>
				<div class="col-md-6" id="adresa">
					<!-- To be filled by profile.js -->
				</div>
			</div>
			<span data-toggle="modal" data-target="#editUserModal">
                <button type="button" class="btn btn-info edit-user" data-toggle="tooltip" title="Upravit uživatelské údaje">Upravit údaje</button>
            </span>
		</div>
	</div>
</div>


<div id="editUserModal" class="modal fade" role="dialog">
	<div class="modal-dialog">
	  <!-- Modal content-->
	  <div class="modal-content">
		<div class="modal-header">
		  <h4 class="modal-title">Upravit uživatelské</h4>
		  <button type="button" class="close" data-dismiss="modal">&times;</button>
		</div>
		<div class="modal-body">
			<form action="#" method="post" id="editForm">
				<div class="form-group">
					<label for="edit_id">ID:</label>
					<input type="text" class="form-control" id="edit_id" readonly>
				</div>
				<div class="form-group">
					<label for="edit_username">Uživatelské jméno:</label>
					<input type="text" class="form-control" id="edit_username" readonly>
				  </div>
				<div class="form-group">
					<label for="edit_email">Email:</label>
					<input type="email" class="form-control" id="edit_email">
				</div>
				<div class="form-group">
					<label for="edit_firstname">Jméno:</label>
					<input type="text" class="form-control" id="edit_firstname">
				</div>
				<div class="form-group">
					<label for="edit_lastname">Příjmení:</label>
					<input type="text" class="form-control" id="edit_lastname">
				</div>
				<div class="form-group">
					<label for="edit_birthdate">Datum narození:</label>
					<input type="date" class="form-control" id="edit_birthdate" readonly>
				</div>
				<div class="form-group">
					<label for="edit_phone">Telefon:</label>
					<input type="text" class="form-control" maxlength="15" id="edit_phone">
				</div>
				<div class="form-group">
					<label for="edit_address">Adresa:</label>
					<input type="text" class="form-control" id="edit_address">
				</div>
				<button type="submit" class="btn btn-primary">Upravit</button>
			</form>
		</div>
		<div class="modal-footer">
		  	<button type="button" class="btn btn-primary" data-dismiss="modal">Zavřít</button>
		</div>
	  </div>
	</div>
</div>
