<?php

class Session {
    
    private $database = null;
    
    
    
    public function __construct($database)
    {
        $this->database = $database;
        
        if (session_status() === PHP_SESSION_NONE) {
            session_start();
        }
        
        if (!isset($_SESSION['user_id'])) {
            $_SESSION['user_id'] = "";
            $_SESSION['user_name'] = "";
            $_SESSION['user_type'] = "";
        }
    }
    
    public function login($username, $password)
    {
        $data = $this->database->fetch("SELECT id, uzivatelske_jmeno, heslo, prava FROM uzivatel WHERE uzivatelske_jmeno = :username AND prava != 'vyrazen' LIMIT 1", ['username' => $username]);
        if ($this->database->lastError() !== 'OK') {
            return 'SQL ERROR: ' . $this->database->lasterror();
        }
        
        if (isset($data['heslo'])) {
            if (password_verify($password, $data['heslo'])) {
                $_SESSION['user_id'] = $data['id'];
                $_SESSION['user_name'] = $data['uzivatelske_jmeno'];
                $_SESSION['user_type'] = $data['prava'];
            } else {
                $_SESSION['user_id'] = "";
                $_SESSION['user_name'] = "";
                $_SESSION['user_type'] = "";
            }
        } else {
            $_SESSION['user_id'] = "";
            $_SESSION['user_name'] = "";
            $_SESSION['user_type'] = "";
        }
        
        return $this->is_logged_in();
    }

    public function logout()
    {
        $_SESSION['user_id'] = "";
        $_SESSION['user_name'] = "";
        $_SESSION['user_type'] = "";
        
        //session_destroy();
    }
    
    public function is_logged_in()
    {
        if (!empty($_SESSION['user_id'])) {
            return true;
        }
        return false;
    }
    
    public function get_logged_in()
    {
        return ['user_id' => $_SESSION['user_id'], 'user_name' => $_SESSION['user_name'], 'user_type' => $_SESSION['user_type']];
    }
    
    
    
    
    
    public function is_user_or_up_in()
    {
        if ($this->is_logged_in()) {
            return true;
        }
        return false;
    }
    
    public function is_licitator_or_up_in()
    {
        if ($this->is_logged_in() && in_array($this->get_logged_in()['user_type'], ['licitator', 'admin'])) {
            return true;
        }
        return false;
    }
    
    public function is_admin_in()
    {
        if ($this->is_logged_in() && $this->get_logged_in()['user_type'] == 'admin') {
            return true;
        }
        return false;
    }
}

?>
