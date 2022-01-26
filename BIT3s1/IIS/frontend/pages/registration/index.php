<?php
require_once(__DIR__ . '/../../../include/common.php');




// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Registrace', 'css' => [], 'js' => ['./frontend/js/registration.js', './frontend/js/login.js'], ];
include(__DIR__ .'/../../components/html_head.php');



// MENU
include(__DIR__ .'/../../components/common/menu.php');
$cmpnt_menu['active'] = 'Registrace';
include(__DIR__ .'/../../components/menu.php');

?>


<div class="container pt-4">
	<div class="jumbotron">
	  <h3>Registrace</h3>
	  <form method="post" id="registerForm">
		  <div class="form-group">
			<label for="username">*Uživatelské jméno:</label>
			<input type="text" class="form-control" id="username" name='username' required>
		  </div>
		  <div class="form-group">
			<label for="password">*Heslo:</label>
			<input type="password" class="form-control" id="password" name='password' required>
		  </div>
		  <div class="form-group">
			<label for="email">*Email:</label>
			<input type="email" class="form-control" placeholder="xplagiat00@gmail.com" id="email" required>
		  </div>
		  <div class="form-group">
			<label for="firstname">*Jméno:</label>
			<input type="text" class="form-control" placeholder="Pepa" id="firstname" required>
		  </div>
		  <div class="form-group">
			<label for="lastname">*Příjmení:</label>
			<input type="text" class="form-control" placeholder="Borec" id="lastname" required>
		  </div>
		  <div class="form-group">
			<label for="birthdate">*Datum narození:</label>
			<input type="date" class="form-control" id="birthdate" required>
		  </div>
		  <div class="form-group">
			<label for="phone">*Telefon:</label>
			<input type="text" class="form-control" placeholder="420 360 690" maxlength="15" id="phone" required>
		  </div>
		  <div class="form-group">
			<label for="address">*Adresa:</label>
			<input type="text" class="form-control" placeholder="Božetěchova 1" id="address" required>
		  </div>
		  <button type="submit" class="btn btn-primary" name="register_submit">Submit</button>
	  </form>
	</div>
  </div>

