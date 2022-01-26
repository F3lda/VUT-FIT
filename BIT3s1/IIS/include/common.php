<?php
require_once(__DIR__ . '/ini.php');





function execute_php($path)
{
    ob_start();
    include($path);
    $content = ob_get_contents(); 
    ob_end_clean();
    return $content;
}

function return_error($error_text)
{
    return ['status' => 'ERROR', 'error' => $error_text];
}

function return_ok($data)
{
    return array_merge(['status' => 'OK'], $data);
}

function return_internal_error($error_text)
{
    //return ['status' => 'ERROR', 'error' => 'INTERNAL_ERROR'];
    return ['status' => 'ERROR', 'error' => 'INTERNAL_ERROR', 'error_desc' => $error_text]; // for debug
}

function only_admin()
{
    session_start();
    if ($_SESSION['user_type'] != "admin") header('Location: ./');
}

function only_user()
{
    
}

function only_licitator()
{
    
}

?>
