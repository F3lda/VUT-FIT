<?php
require_once(__DIR__ . "/include/common.php");



// SAVE CMD
$get_cmd = '';
if (isset($_GET['cmd'])) {
    $get_cmd = $_GET['cmd'];
}



// PAGE
$pages = [
    'auctions', 
    'auctions-create', 
    'auctions-info', 
    'auctions-approve',
    'auctions-managed',
    'login', 
    'registration', 
    'user', 
    'user-auctions', 
    'user-attended', 
    'admin-users'
];

$PAGE = 'auctions'; // default page
if (isset($_GET['page'])) {
    if (in_array($_GET['page'], $pages)) {
        $PAGE = $_GET['page'];
    }
}

$PAGE = str_replace("-", "/", $PAGE);

include('./frontend/pages/'.$PAGE.'/index.php');



// PHP FILES
$_GET['cmd'] = $get_cmd;

// BODY END
echo PHP_EOL . '</body>' . PHP_EOL . '</html>' . PHP_EOL;
?>
