<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title><?php echo $cmpnt_html_head['title']; ?></title>
    
    <?php 
    foreach ($cmpnt_html_head['css'] as $link) {
        echo '<link rel="stylesheet" href="'. $link .'">'. PHP_EOL .'   ';
    }
    ?>
    <!-- BOOTSTRAP 4 -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.16.0/umd/popper.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
    <!-- Bootstrap 3
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js" type="text/javascript"></script>
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js" type="text/javascript"></script> -->
    <script src="./frontend/js/COMMON.js" type="text/javascript"></script>
    <script src="./frontend/js/navbar.js" type="text/javascript"></script>
    <script src="./frontend/js/login.js" type="text/javascript"></script>
    
    <?php 
    foreach ($cmpnt_html_head['js'] as $link) {
        echo '<script src="'. $link .'" type="text/javascript"></script>'. PHP_EOL .'   ';
    }
    ?>

</head>
<body>
