<?php

class Users {
    
    private $database = null;
    private $session = null;
    
    
    
    public function __construct($database, $session)
    {
        $this->database = $database;
        $this->session = $session;
    }
    // TODO maybe - search user
    
    
    
    
    // REGISTRATION - all
    public function addUser($data)
    {
        // unset extra query fields
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);
        
        if (!empty($data["username"]) && !empty($data["password"]) && !empty($data["email"]) && !empty($data["firstname"]) 
        && !empty($data["lastname"]) && !empty($data["birthdate"]) && !empty($data["phone"]) && !empty($data["address"])) {
                
            $return_data = $this->database->execute('SELECT * FROM uzivatel WHERE uzivatelske_jmeno = :username OR email = :email LIMIT 1', ['username' => $data['username'], 'email' => $data['email']]);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            
            if ($return_data['row_count'] != 0) {
                return return_error('username or email exists');
            }
            
            $data['password'] = password_hash($data["password"], PASSWORD_DEFAULT); // hash password
            
            $query = "INSERT INTO uzivatel (uzivatelske_jmeno, heslo, email, jmeno, prijmeni, datum_narozeni, telefon, adresa, prava) VALUES (";
            $query .= ':username, :password, :email, :firstname, :lastname, :birthdate, :phone, :address, \'uzivatel\')';
            
            $return_data = $this->database->insert($query, $data);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            
            return return_ok($return_data);
            
        } else {
            return return_error('empty fields');
        }
    }
    
    // ADMIN, author
    public function getUser($data)
    {
        if (!empty($data["userid"])) {
            if ($this->session->is_admin_in() == true || $this->session->get_logged_in()['user_id'] == $data["userid"]) {
                
                $return_data = $this->database->fetch("SELECT uzivatelske_jmeno, email, jmeno, prijmeni, datum_narozeni, telefon, adresa, prava, ico FROM uzivatel WHERE id = ?", [$data["userid"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                
                if ($return_data == false) {
                    return return_error('user not found');
                } else {
                    return return_ok($return_data);
                }
                
            } else {
                return return_error('unauthorized access');
            }
        } else {
            return return_error('empty fields');
        }
    }

    public function getUserName($data)
    {
        if (!empty($data["userid"])) {
             
            $return_data = $this->database->fetch("SELECT uzivatelske_jmeno FROM uzivatel WHERE id = ?", [$data["userid"]]);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            
            if ($return_data == false) {
                return return_error('user not found');
            } else {
                return return_ok($return_data);
            }

        } else {
            return return_error('empty fields');
        }
    }
    
    // ADMIN, author
    public function editUser($data)
    {
        // unset extra query fields
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);
        
        if (!empty($data["userid"]) && !empty($data["email"]) && !empty($data["firstname"]) && !empty($data["lastname"]) && !empty($data["birthdate"]) && !empty($data["phone"]) && !empty($data["address"])) {
            if ($this->session->is_admin_in() == true || $this->session->get_logged_in()['user_id'] == $data["userid"]) {
                
                if ($this->session->is_admin_in() == true && empty($data["privileges"])) {
                    return return_error('empty fields');
                }
                
                // LOAD DATA
                $return_data = $this->database->fetch("SELECT * FROM uzivatel WHERE id = ?", [$data["userid"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                
                if ($return_data == false) {
                    return return_error('user not found');
                }
                


                // EDIT ICO
                if ($this->session->is_admin_in() == true) {

                    if ($data['privileges'] != $return_data['prava']) {

                        if (($return_data['prava'] == 'uzivatel' || $return_data['prava'] == 'vyrazen') && ($data['privileges'] != 'vyrazen' && $data['privileges'] != 'uzivatel')) {
                            // ADD ico
                            if (!empty($data["ico"])) {
                                
                                $return_data_temp = $this->database->execute('UPDATE uzivatel SET ico = ? WHERE id = ?', [$data['ico'], $data['userid']]);
                                if ($this->database->lastError() !== 'OK') {
                                    return return_internal_error($this->database->lasterror());
                                }
                                
                            } else {
                                return return_error('empty field ico');
                            }
                            
                        } else {
                            // DELETE ico
                            $return_data_temp = $this->database->fetchAll('SELECT COUNT(*) FROM aukce WHERE licitator_id = ?', [$data["userid"]]);
                            if ($this->database->lastError() !== 'OK') {
                                return return_internal_error($this->database->lasterror());
                            }

                            if ($return_data_temp[0]['COUNT(*)'] > 0) {
                                return return_error('can\'t remove ICO when it\'s connected to AUCTIONS');
                            }

                            $return_data_temp = $this->database->execute('UPDATE uzivatel SET ico = NULL WHERE id = ?', [$data['userid']]);
                            if ($this->database->lastError() !== 'OK') {
                                return return_internal_error($this->database->lasterror());
                            }

                        }

                    } else if ($data['privileges'] == 'licitator' || $data['privileges'] == 'admin') {
                        // EDIT ico
                        if ($data["ico"] != $return_data["ico"]) {
                            if (!empty($data["ico"])) {

                                $return_data_temp = $this->database->fetchAll('SELECT COUNT(*) FROM aukce WHERE licitator_id = ?', [$data["userid"]]);
                                if ($this->database->lastError() !== 'OK') {
                                    return return_internal_error($this->database->lasterror());
                                }

                                if ($return_data_temp[0]['COUNT(*)'] > 0) {
                                    return return_error('can\'t change ICO when it\'s connected to AUCTIONS');
                                }

                                $return_data_temp = $this->database->execute('UPDATE uzivatel SET ico = ? WHERE id = ?', [$data['ico'], $data['userid']]);
                                if ($this->database->lastError() !== 'OK') {
                                    return return_internal_error($this->database->lasterror());
                                }

                            } else {
                                return return_error('empty field ico');
                            }
                        }

                    }

                }
                
                

                // UNSET ico
                if (isset($data["ico"])) {unset($data["ico"]);}

                // UPDATE DATA
                $query = 'UPDATE uzivatel SET email = :email, jmeno = :firstname, prijmeni = :lastname, datum_narozeni = :birthdate, telefon = :phone, adresa = :address, prava = :privileges WHERE id = :userid';
                
                $return_data = $this->database->execute($query, $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                
                $return_data['row_count_licitator'] = (isset($return_data_temp['row_count'])) ? $return_data_temp['row_count'] : 0;
                if ($return_data['row_count'] == 0 && $return_data['row_count_licitator'] == 0) {
                    return return_error('nothing changed');
                } else {
                    return return_ok($return_data);
                }
                
            } else {
                return return_error('unauthorized access');
            }
        } else {
            return return_error('empty fields');
        }
    }
    
    // ADMIN, author
    public function removeUser($data)
    {
        if (!empty($data["userid"])) {
            if ($this->session->is_admin_in() == true || $this->session->get_logged_in()['user_id'] == $data["userid"]) {
                
                if ($this->session->is_admin_in() == true && !empty($data["privileges"]) && $data["privileges"] == 'vyrazen') {
                    
                    // DELETE USER
                    $return_data = $this->database->execute('DELETE FROM uzivatel WHERE id = ?', [$data["userid"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }
                    
                    if ($return_data['row_count'] == 0) {
                        return return_error('user not found');
                    } else {
                        return return_ok($return_data);
                    }
                    
                } else {
                    
                    $return_data = $this->database->execute('UPDATE uzivatel SET prava = \'vyrazen\' WHERE id = ?;', [$data["userid"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }
                    
                    if ($return_data['row_count'] == 0) {
                        return return_error('user not found');
                    } else {
                        return return_ok($return_data);
                    }
                    
                }
                
            } else {
                return return_error('unauthorized access');
            }
        } else {
            return return_error('empty fields');
        }
    }
    
    
    
    
    
    // ADMIN
    public function getAll($data)
    {
        if ($this->session->is_admin_in() == true) {
            
            $return_data = $this->database->fetchAll('SELECT id, uzivatelske_jmeno, email, jmeno, prijmeni, datum_narozeni, telefon, adresa, prava, ico
            FROM uzivatel ORDER BY prava DESC, uzivatelske_jmeno LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            
            $count = count($return_data);
            if ($count > $data['item_count']) {
                $slice_count = $count - $data['item_count'];
                $return_data = array_slice($return_data, $slice_count);
            }
            return return_ok($return_data);
            
        } else {
            return return_error('unauthorized access');
        }
    }
    
    // ADMIN
    public function getAllCount($data)
    {
        if ($this->session->is_admin_in() == true) {
            
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM uzivatel', []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            return return_ok(['item_count' => $return_data[0]['COUNT(*)']]);
            
        } else {
            return return_error('unauthorized access');
        }
    }
    
}

?>
