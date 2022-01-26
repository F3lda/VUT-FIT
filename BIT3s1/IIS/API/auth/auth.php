<?php

class Auth {
    
    private $session = null;
    
    
    
    public function __construct($session)
    {
        $this->session = $session;
    }
    
    public function login($username, $password)
    {
        if (!empty($username) && !empty($password)) {
            $return_data = $this->session->login($username, $password);
            if (is_string($return_data)) {
                return return_internal_error($return_data);
            } else {
                if ($return_data == true) {
                    return return_ok(['logged_in' => true]);
                } else {
                    return return_error('invalid data');
                }
            }
        } else {
            return return_error('empty fields');
        }
    }
    
    public function logout()
    {
        $this->session->logout();
        return return_ok(['logged_in' => false]);
    }
    
    public function is_logged_in()
    {
        if ($this->session->is_logged_in()) {
            return return_ok(['logged_in' => true]);
        }
        return return_ok(['logged_in' => false]);
    }
    
    public function get_logged_in()
    {
        return return_ok($this->session->get_logged_in());
    }
}

?>
