<?php
require_once(__DIR__ . '/../../../../include/common.php');

// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Správa uživatelů', 'css' => [''], 'js' => ['./frontend/js/admin/user_table.js']];
include(__DIR__ .'/../../../components/html_head.php');

// MENU
include(__DIR__ .'/../../../components/common/menu.php');
$cmpnt_menu['active'] = 'Admin';
include(__DIR__ .'/../../../components/menu.php');


// ONLY ADMIN CAN ACCESS THIS PAGE
only_admin();

?>

<div class="container-fluid pt-4 pr-4 pl-4">
	<div class="row">
		<table id="user_table" class="table table-striped table-bordered">
			<div class="form-group">
				<input type="button" class="btn btn-primary add-new" value="Přidat uživatele" data-toggle="modal" data-target="#userModal" id="add_user_button">
				<!-- <input type="button" class="btn btn-info" value="Show Users" id="reloadButton" > -->
			</div>
			<div class="form-group form-inline col-md-1 ml-auto">
				<label for="item_count">Počet: </label>
				<select class="form-control" autocomplete="off" id="select_users_count">
					<option selected="selected">5</option>
					<option>10</option>
					<option>15</option>
					<option>20</option>
				</select>
			</div>
			<div class="col-12 pt-2">
				
				<thead>
					<tr>
						<th>ID</th>
						<th>Uživatelské jméno</th>
						<th>Email</th>
						<th>Jméno</th>
						<th>Příjmení</th>
						<th>Datum narození</th>
						<th>Telefon</th>
						<th>Adresa</th>
						<th>Práva</th>
						<th>IČO</th>
						<th class="text-center">Akce</th>
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
  <ul class="pagination justify-content-center" id="user-table-pagination">
  </ul>
</div>


<!-- Modal content -->
<div id="userModal" class="modal fade" role="dialog">
	<div class="modal-dialog">
	  <!-- Modal content-->
	  <div class="modal-content">
		<div class="modal-header">
			<h4 class="modal-title">Přidat uživatele</h4>
		  	<button type="button" class="close" data-dismiss="modal">&times;</button>
		</div>
		<div class="modal-body">
			<form action="#" method="post" id="addForm">
				<div class="form-group">
				  <label for="username">Uživatelské jméno:</label>
				  <input type="text" class="form-control" id="username" required>
				</div>
				<div class="form-group">
				  <label for="password">Heslo:</label>
				  <input type="text" class="form-control" id="password" required>
				</div>
				<div class="form-group">
					<label for="email">Email:</label>
					<input type="email" class="form-control" id="email">
				</div>
				<div class="form-group">
					<label for="firstname">Jméno:</label>
					<input type="text" class="form-control" placeholder="Pepa" id="firstname">
				</div>
				<div class="form-group">
					<label for="lastname">Příjmení:</label>
					<input type="text" class="form-control" placeholder="Borec" id="lastname">
				</div>
				<div class="form-group">
					<label for="birthdate">Datum narození:</label>
					<input type="date" class="form-control" id="birthdate">
				</div>
				<div class="form-group">
					<label for="phone">Telefon:</label>
					<input type="text" class="form-control" placeholder="420 360 690" maxlength="15" id="phone">
				</div>
				<div class="form-group">
					<label for="address">Adresa:</label>
					<input type="text" class="form-control" placeholder="Božetěchova 1" id="address">
				</div>
				<button type="submit" class="btn btn-default">Přidat</button>
			</form>
		</div>
		<div class="modal-footer">
		  	<button type="button" class="btn btn-default" data-dismiss="modal">Zavřít</button>
		</div>
	  </div>

	</div>
</div>

<div id="editUserModal" class="modal fade" role="dialog">
	<div class="modal-dialog">
	  <!-- Modal content-->
	  <div class="modal-content">
		<div class="modal-header">
		  <button type="button" class="close" data-dismiss="modal">&times;</button>
		  <h4 class="modal-title">Upravit údaje uživatele</h4>
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
					<input type="date" class="form-control" id="edit_birthdate">
				</div>
				<div class="form-group">
					<label for="edit_phone">Telefon:</label>
					<input type="text" class="form-control" maxlength="15" id="edit_phone">
				</div>
				<div class="form-group">
					<label for="edit_address">Adresa:</label>
					<input type="text" class="form-control" id="edit_address">
				</div>
				<div class="form-group">
					<label for="edit_privileges">Role:</label>
					<select name="edit_privileges" class="form-control" id="edit_privileges">
						<option value="uzivatel">Uživatel</option>
						<option value="licitator">Licitátor</option>
						<option value="admin">Admin</option>
					</select>
				</div>
				<div class="form-group" id="edit_ico_div">
					<label for="edit_ico">IČO:</label>
					<input type="text" class="form-control" id="edit_ico" pattern=[0-9]{8}>
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