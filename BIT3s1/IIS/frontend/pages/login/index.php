<?php
require_once(__DIR__ . '/../../../include/common.php');




// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Login', 'css' => [], 'js' => ['./frontend/js/login.js']];
include(__DIR__ .'/../../components/html_head.php');



// MENU
include(__DIR__ .'/../../components/common/menu.php');
$cmpnt_menu['active'] = 'Login';
include(__DIR__ .'/../../components/menu.php');

?>


<div class="container pt-4">
	<div id="status_div">

	</div>
	<div class="jumbotron">
		<h3>Login</h3>
	  	<form id="loginForm" >
			<div class="form-group">
				<label for="text">Uživatelské jméno</label>
				<input type="text" class="form-control" id="username" name='username' value="" placeholder="Uživatelské jméno">
			</div>
			<div class="form-group">
				<label for="pwd">Heslo</label>
				<input type="password" class="form-control" id="pwd" name='password' value="" placeholder="Heslo">
			</div>
			<button type="submit" class="btn btn-primary">Submit</button>
		</form>
	</div>
</div>
