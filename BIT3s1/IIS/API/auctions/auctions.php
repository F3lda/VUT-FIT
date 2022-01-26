<?php

class Auction {
    
    private $database = null;
    private $session = null;
    
    private $listOfCategories = ['zbozi', 'nemovitosti', 'sluzby', 'aktiva'];
    /*  1	aktiva	Aukce, jejíž předmětem jsou aktiva
        2	nemovitosti	Aukce, jejíž předmětem je nemovitosti (místo majetku)
        3	zbozi	Aukce, jejíž předmětem je zboží
        4	sluzby	Aukce, jejíž předmětem jsou služby */
    private $listOfRules = ['otevrena', 'uzavrena'];
    
    
    public function __construct($database, $session)
    {
        $this->database = $database;
        $this->session = $session;
    }
    

    

    
    public function getAll($data)
    {
        // LICITATOR and ADMIN
        if ($this->session->is_licitator_or_up_in() == true) {
            $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce ORDER BY stav LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
        // USER
        } else if ($this->session->is_logged_in() == true) {
            $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE (aukce.stav != \'navrzena\' AND aukce.stav != \'zrusena\') OR aukce.autor_id = '. $this->session->get_logged_in()['user_id'] .' ORDER BY stav LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
        // ALL
        } else {
            $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.stav != \'navrzena\' AND aukce.stav != \'zrusena\' ORDER BY stav LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
        }
        
        if ($data['page_number'] > 1) {
            $slice_count = ($data['page_number']-1)*$data['item_count'];
            $return_data = array_slice($return_data, $slice_count);
        }
        
        return return_ok($return_data);
    }
    
    public function getAllCount($data)
    {
        // LICITATOR and ADMIN
        if ($this->session->is_licitator_or_up_in() == true) {
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM aukce', []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
        // USER
        } else if ($this->session->is_logged_in() == true) {
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM aukce WHERE (aukce.stav != \'navrzena\' AND aukce.stav != \'zrusena\') OR aukce.autor_id = '. $this->session->get_logged_in()['user_id'] .'', []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
        } else {
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM aukce WHERE aukce.stav != \'navrzena\' AND aukce.stav != \'zrusena\'', []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
        }
        
        return return_ok(['item_count' => $return_data[0]['COUNT(*)']]);
    }

    public function getAllMyCreated($data)
    {
        // AUTHOR
        if ($this->session->is_logged_in() == true) {
            $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.autor_id = '. $this->session->get_logged_in()['user_id'] .' LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            
            if ($data['page_number'] > 1) {
                $slice_count = ($data['page_number']-1)*$data['item_count'];
                $return_data = array_slice($return_data, $slice_count);
            }

            return return_ok($return_data);
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllMyCreatedCount($data)
    {
        // AUTHOR
        if ($this->session->is_logged_in() == true) {
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM aukce WHERE aukce.autor_id = '. $this->session->get_logged_in()['user_id'] .'', []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            return return_ok(['item_count' => $return_data[0]['COUNT(*)']]);
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllMyAttended($data)
    {
        // AUTHOR
        if ($this->session->is_logged_in() == true) {
            $return_data = $this->database->fetchAll('SELECT aukce.*, ucastnici.uzivatel_id as ucastnik_id FROM aukce JOIN ucastnici ON ucastnici.aukce_id = aukce.id WHERE ucastnici.uzivatel_id = '. $this->session->get_logged_in()['user_id'] .' LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            
            if ($data['page_number'] > 1) {
                $slice_count = ($data['page_number']-1)*$data['item_count'];
                $return_data = array_slice($return_data, $slice_count);
            }

            return return_ok($return_data);
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllMyAttendedCount($data)
    {
        // AUTHOR
        if ($this->session->is_logged_in() == true) {
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM aukce JOIN ucastnici ON ucastnici.aukce_id = aukce.id WHERE ucastnici.uzivatel_id = '. $this->session->get_logged_in()['user_id'] .'', []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            return return_ok(['item_count' => $return_data[0]['COUNT(*)']]);
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllAuctionsNavrzena($data)
    {
        // LICITATOR and ADMIN
        if ($this->session->is_licitator_or_up_in() == true) {
            $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.stav = \'navrzena\' LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }

            if ($data['page_number'] > 1) {
                $slice_count = ($data['page_number']-1)*$data['item_count'];
                $return_data = array_slice($return_data, $slice_count);
            }
            
            return return_ok($return_data);
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllAuctionsNavrzenaCount($data)
    {
        // LICITATOR and ADMIN
        if ($this->session->is_licitator_or_up_in() == true) {
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM aukce WHERE aukce.stav = \'navrzena\' LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }

            return return_ok(['item_count' => $return_data[0]['COUNT(*)']]);
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllManagedByLicitator($data)
    {
        // LICITATOR and ADMIN
        if ($this->session->is_licitator_or_up_in() == true) {
            $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.licitator_id = '. $this->session->get_logged_in()['user_id'] .' LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }

            if ($data['page_number'] > 1) {
                $slice_count = ($data['page_number']-1)*$data['item_count'];
                $return_data = array_slice($return_data, $slice_count);
            }
            
            return return_ok($return_data);
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllManagedByLicitatorCount($data)
    {
        // LICITATOR and ADMIN
        if ($this->session->is_licitator_or_up_in() == true) {
            $return_data = $this->database->fetchAll('SELECT COUNT(*) FROM aukce WHERE aukce.licitator_id = '. $this->session->get_logged_in()['user_id'] .' LIMIT '.$data['item_limit'], []);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }

            return return_ok(['item_count' => $return_data[0]['COUNT(*)']]);
        } else {
            return return_error('unauthorized access');
        }
    }




    
    public function checkAuctionDateAndStatus($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        if (!empty($data["auction_id"])) {
            
            // ALL
            $return_data = $this->database->fetchAll('SELECT stav, cas_zahajeni <= NOW() AS probiha, cas_ukonceni < NOW() AS ukoncena FROM aukce WHERE aukce.id = :auction_id LIMIT 1', $data);
            if ($this->database->lastError() !== 'OK') {
                return return_internal_error($this->database->lasterror());
            }
            
            if (empty($return_data[0])) {
                return return_error('auction not found');
            }
            
            // check if auction started and set status to 'probiha' or check if auction ended and set status to 'ukoncena'
            $auction_status = NULL;
            if ($return_data[0]["stav"] == 'pripravena' && $return_data[0]["probiha"] == 1) {
                $auction_status = 'probiha';
            } else if ($return_data[0]["stav"] == 'probiha' && $return_data[0]["ukoncena"] == 1) {
                $auction_status = 'ukoncena';
            }
            
            if ($auction_status != NULL) {
                $query = 'UPDATE aukce SET stav = :stav WHERE id = :auction_id';
                                        
                $return_data = $this->database->execute($query, ["stav" => $auction_status, "auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
            }
            
            return return_ok([]);
        } else {
            return return_error('empty fields');
        }
    }

    public function getAuction($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        if (!empty($data["auction_id"])) {
            
            // LICITATOR and ADMIN
            if ($this->session->is_licitator_or_up_in() == true) {
                $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.id = :auction_id LIMIT 1', $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
            // USER
            } else if ($this->session->is_logged_in() == true) {
                $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.id = :auction_id AND ((aukce.stav != \'navrzena\' AND aukce.stav != \'zrusena\') OR aukce.autor_id = '. $this->session->get_logged_in()['user_id'] .') LIMIT 1', $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
            // ALL
            } else {
                $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.stav != \'navrzena\' AND aukce.stav != \'zrusena\' AND aukce.id = :auction_id LIMIT 1', $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
            }
            
            if (!empty($return_data[0])) {
                return return_ok($return_data[0]);
            }
            return return_error('auction not found');
        } else {
            return return_error('empty fields');
        }
    }

    public function auctionCheckAccess($data)
    {
        $auction = $this->getAuction($data);
        
        if (isset($auction["status"]) && $auction["status"] == 'OK') {
            if (isset($auction["id"]) && isset($data["auction_id"]) && $auction["id"] == $data["auction_id"]) {
                return true;
            }
            return false;
        } else {
            return $auction;
        }
    }





    /* 
    function: addAuction()
    INPUT:
        a_name - názave aukce
        a_description - popis aukce (nepovinný)
        a_category - id kategorie
        a_type - typ aukce ('poptavkova','nabidkova')
        a_rule - pravidlo aukce ('otevrena','uzavrena')
        a_start_cost - počáteční cena [typ = nabidkova]
        a_min_cost - minimální cena [typ = poptavkova]
        a_max_cost - maximální cena [typ = poptavkova]
        image - obrázek
    */
    public function addAuction($data)
    {
        // unset extra query fields
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);
        
        if ($this->session->is_user_or_up_in() == true) {
            if (!empty($data["a_name"]) && !empty($data["a_category"]) && !empty($data["a_type"]) && !empty($data["a_rule"])) {
            
                // Set autor id
                $data['autor_id'] = $this->session->get_logged_in()['user_id'];



                // Check the duplicate (name and author)
                $return_data = $this->database->execute('SELECT id FROM aukce WHERE nazev = :a_name AND autor_id = :autor_id LIMIT 1', ['a_name' => $data['a_name'], 'autor_id' => $data['autor_id']]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                
                if ($return_data['row_count'] != 0) {
                    return return_error('auction with the same name already exists');
                }



                /*if ($data["a_rule"] == 'uzavrena') { // TODO remove
                    return return_error('this type of auction is currently not available');
                }*/

                // Check category
                if (!in_array($data["a_category"], $this->listOfCategories)) {
                    return return_error('wrong value in a_category field');
                }

                // Check rule
                if (!in_array($data["a_rule"], $this->listOfRules)) {
                    return return_error('wrong value in a_rule field');
                }

                // Check type of auction
                if (($data["a_type"] == 'nabidkova' && !empty($data["a_start_cost"])) && $data["a_start_cost"] >= 0) {
                    $data["a_min_cost"] = NULL;
                    $data["a_max_cost"] = NULL;
                } else if ($data["a_type"] == 'poptavkova' && !empty($data["a_min_cost"]) && !empty($data["a_max_cost"]) && $data["a_min_cost"] >= 0 && $data["a_max_cost"] > 0 && $data["a_min_cost"] < $data["a_max_cost"]) {
                    $data["a_start_cost"] = NULL;
                } else {
                    return return_error('wrong or empty fields');
                }

                // Set empty descriptin if not set
                if (!isset($data["a_description"])) {
                    $data["a_description"] = "";
                }

                // UPLOAD IMAGE
                $return_data = $this->database->fetch('SELECT MAX(id) FROM aukce;', []);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                
                if ($return_data['MAX(id)'] == NULL) {
                    $return_data['MAX(id)'] = 0;
                }

                $image_name = hash('sha256', $data['autor_id'] . $return_data['MAX(id)'] . strval(time()));
                $image_path = $this->uploadImage($image_name);
                if ($image_path["status"] == "ERROR") {
                    return $image_path["error"];
                }

                $data["a_image_path"] = $image_path["path"];


                // INSERT
                $query = "INSERT INTO aukce (nazev, popis, obrazek, kategorie, typ, pravidla, cas_navrzeni, stav, vyvolavaci_cena, min_cena, max_cena, autor_id) VALUES (";
                $query .= ':a_name, :a_description, :a_image_path, :a_category, :a_type, :a_rule, NOW(), \'navrzena\', :a_start_cost, :a_min_cost, :a_max_cost, :autor_id)';
                
                $return_data = $this->database->insert($query, $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                return return_ok($return_data);

            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function uploadImage($file_name)
    {
        $dir = "./../uploaded_images/";

        // CHECK IF FILE IS INCLUDED
        if (empty($_FILES["image"]) || empty($_FILES["image"]["tmp_name"]) || empty($_FILES["image"]["name"]) || empty($_FILES["image"]["type"]) || !isset($_FILES["image"]["error"]) || empty($_FILES["image"]["size"])) {
            return ["status" => "ERROR", "error" => "image not included"];
        }
        
        // CHECK IF DIR EXISTS
        if (!file_exists($dir) && !is_dir($dir)) {
            mkdir($dir, 0777);    
        }
        
        // CHECK FOR ERROR
        if ($_FILES["image"]["error"] != 0) {
            return ["status" => "ERROR", "error" => "uplod image - error occurred: " . $_FILES["image"]["error"]];
        }
        
        // CHECK FILE TYPE
        if (getimagesize($_FILES["image"]["tmp_name"]) == false) {
            return ["status" => "ERROR", "error" => "file is not an image"];
        }
        
        // CHECK FILE SIZE (in bytes)
        if ($_FILES["image"]["size"] > 1024*1024*2) { // 2 MB
            return ["status" => "ERROR", "error" => "file is too big"];
        }
        
        // UPLOAD FILE
        
        $file = explode(".", $_FILES["image"]["name"]);
        $file_path = $dir.$file_name. '.' .end($file);
        move_uploaded_file($_FILES["image"]["tmp_name"], $file_path);
        
        return ["status" => "OK", "path" => "./uploaded_images/" .$file_name. '.' .end($file)];
    }

    

    public function onEventWhileNAVRZENA($data, $event)
    {
        // unset extra query fields
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        if ($this->session->is_user_or_up_in() == true) {

            if (!empty($data["auction_id"])) {
                
                // check if user is author or admin
                $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                // ADMIN and AUTHOR
                if (!(!empty($return_data[0]) && ($this->session->is_admin_in() == true || $this->session->get_logged_in()['user_id'] == $return_data[0]['autor_id']))) {
                    return return_error('auction not found or unauthorized access');
                }

                // check if status is 'navrzena'
                if ($return_data[0]["stav"] != 'navrzena') {
                    return return_error('can\'t remove or edit auction in different status than \'navrzena\'');
                }



                // EVENT
                // remove
                if ($event == 'remove') {

					// DELETE image
					unlink('./.' .$return_data[0]["obrazek"]);
					
                    // DELETE auction
                    $return_data = $this->database->execute('DELETE FROM aukce WHERE id = ?', [$data["auction_id"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }

                    if ($return_data['row_count'] == 0) {
                        return return_error('couldn\'t delete auction');
                    }

                // edit
                } else if ($event == 'edit') {

                    /* 
                    function: edit auction
                    INPUT:
                        a_description - popis aukce
                        a_category - id kategorie
                        a_type - typ aukce ('poptavkova','nabidkova')
                        a_rule - pravidlo aukce ('otevrena','uzavrena')
                        a_start_cost - počáteční cena [typ = nabidkova]
                        a_min_cost - minimální cena [typ = poptavkova]
                        a_max_cost - maximální cena [typ = poptavkova]
                        image - obrázek
                    */
                    
                    $new_data["id"] = $data["auction_id"];

                    if (!empty($data["a_name"])) {
                        return return_error('can\'t change the name of the auction');
                    }
                    if (!empty($data["a_description"])) {
                        $new_data["popis"] = $data["a_description"];
                    } else {
                        $new_data["popis"] = $return_data[0]["popis"];
                    }
                    if (!empty($data["a_category"])) {
                        if (!in_array($data["a_category"], $this->listOfCategories)) {
                            return return_error('wrong value in a_category field');
                        } else {
                            $new_data["kategorie"] = $data["a_category"];
                        }
                    } else {
                        $new_data["kategorie"] = $return_data[0]["kategorie"];
                    }
                    if (!empty($data["a_type"])) {
                        if ($data["a_type"] == 'nabidkova' || $data["a_type"] == 'poptavkova') {
                            $new_data["typ"] = $data["a_type"];
                        } else {
                            return return_error('wrong value in a_type field');
                        }
                    } else {
                        $new_data["typ"] = $return_data[0]["typ"];
                    }
                    if (!empty($data["a_rule"])) {
                        if (!in_array($data["a_rule"], $this->listOfRules)) {
                            return return_error('wrong value in a_rule field');
                        }/* else if ($data["a_rule"] == 'uzavrena') {// TODO remove
                            return return_error('this type of auction is currently not available');
                        }*/ else {
                            $new_data["pravidla"] = $data["a_rule"];
                        }
                    } else {
                        $new_data["pravidla"] = $return_data[0]["pravidla"];
                    }
                    if (!empty($data["a_start_cost"])) {
                        if ($data["a_start_cost"] < 0) {
                            return return_error('start cost can\'t be smaller than 0');
                        }
                        if ($new_data["typ"] == 'nabidkova') {
                            $new_data["vyvolavaci_cena"] = $data["a_start_cost"];
                            $new_data["min_cena"] = NULL;
                            $new_data["max_cena"] = NULL;
                        } else {
                            return return_error('can change start_cost only if auction type is nabidkova');
                        }
                    } else {
                        $new_data["vyvolavaci_cena"] = $return_data[0]["vyvolavaci_cena"];
                    }
                    if (!empty($data["a_min_cost"]) && !empty($data["a_max_cost"])) {
                        if ($data["a_min_cost"] < 0 || $data["a_max_cost"] <= 0 || $data["a_min_cost"] >= $data["a_max_cost"]) {
                            return return_error('a_min_cost and a_max_cost can\'t less than 0 and a_min_cost can\'t be bigger than a_max_cost');
                        }
                        if ($new_data["typ"] == 'nabidkova') {
                            $new_data["min_cena"] = $data["a_min_cost"];
                            $new_data["max_cena"] = $data["a_max_cost"];
                            $new_data["vyvolavaci_cena"] = NULL;
                        } else {
                            return return_error('can change min_cost and max_cost only if auction type is poptavkova');
                        }
                    } else {
                        $new_data["min_cena"] = $return_data[0]["min_cena"];
                        $new_data["max_cena"] = $return_data[0]["max_cena"];
                    }
                    // UPDATE IMAGE
                    $image_name = explode('/', $return_data[0]["obrazek"]);
                    $image_name = end($image_name);
                    $image_name = explode('.', $image_name);
                    $image_name = end($image_name);
                    $image_path = $this->uploadImage($image_name);
                    if ($image_path["status"] == "ERROR" && $image_path["error"] != "image not included") {
                        return $image_path["error"];
                    }



                    // UPDATE DATA
                    $query = 'UPDATE aukce SET popis = :popis, kategorie = :kategorie, typ = :typ, pravidla = :pravidla, vyvolavaci_cena = :vyvolavaci_cena, min_cena = :min_cena, max_cena = :max_cena WHERE id = :id';
                                    
                    $return_data = $this->database->execute($query, $new_data);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }

                    if ($return_data['row_count'] == 0) {
                        return return_error('nothing changed');
                    }
                }



                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }



    public function onEventWhilePRIPRAVENA($data, $event)
    {
        // unset extra query fields
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        if ($this->session->is_user_or_up_in() == true) {

            if (!empty($data["auction_id"])) {
                
                // check if user is author or admin
                $return_data = $this->database->fetchAll('SELECT aukce.* FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                // ADMIN and AUTHOR and LICITATOR ID
                if (!
                    (!empty($return_data[0]) && ($this->session->is_admin_in() == true
                    || $this->session->get_logged_in()['user_id'] == $return_data[0]['autor_id']
                    || ($this->session->get_logged_in()['user_id'] == $return_data[0]['licitator_id'] && $return_data[0]['stav'] != 'ukoncena' && $return_data[0]['stav'] != 'vyhodnocena' && $return_data[0]['stav'] != 'zrusena')
                    ))
                 ) {
                    return return_error('auction not found or unauthorized access or wrong state of auction');
                }

                // check if status is 'pripravena' OR ADMIN OR LICITATOR ID and status is not 'ukoncena' or 'vyhodnocena' or 'zrusena'
                if (!($return_data[0]["stav"] == 'pripravena' || $this->session->is_admin_in() == true || ($this->session->get_logged_in()['user_id'] == $return_data[0]['licitator_id'] && $return_data[0]['stav'] != 'ukoncena' && $return_data[0]['stav'] != 'vyhodnocena' && $return_data[0]['stav'] != 'zrusena'))) {
                    return return_error('can\'t cancel auction in different status than \'pripravena\'');
                }



                // EVENT
                // cancel
                if ($event == 'cancel') {

                    // UPDATE DATA
                    $query = 'UPDATE aukce SET stav = \'zrusena\' WHERE id = :auction_id';
                                    
                    $return_data = $this->database->execute($query, ["auction_id" => $data["auction_id"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }

                    if ($return_data['row_count'] == 0) {
                        return return_error('nothing changed');
                    }

                }



                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }





    public function addLicitator($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            if (!empty($data["auction_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                if ($return_data[0]['licitator_id'] != NULL) {
                    return return_error('auction already has licitator');
                }

                if ($return_data[0]['autor_id'] == $this->session->get_logged_in()['user_id']) {
                    return return_error('author of the auction can\'t be it\'s licitator');
                }

                // SET auction's licitator
                $query = 'UPDATE aukce SET licitator_id = :licitator_id WHERE id = :auction_id';

                $return_data = $this->database->execute($query, ["auction_id" => $data["auction_id"], "licitator_id" => $this->session->get_logged_in()['user_id']]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
                
                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function removeLicitator($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            if (!empty($data["auction_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id, aukce.stav FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                if ($return_data[0]['licitator_id'] == NULL) {
                    return return_error('auction has no licitator');
                }

                // LICITATOR ID or ADMIN
                if ($return_data[0]['licitator_id'] != $this->session->get_logged_in()['user_id'] && $this->session->is_admin_in() != true) {
                    return return_error('unauthorized access 2');
                }

                if ($return_data[0]['stav'] != 'navrzena') {
                    return return_error('can\'t remove licitator from auction in current state');
                }

                // REMOVE auction's licitator
                $query = 'UPDATE aukce SET licitator_id = NULL WHERE id = :auction_id';

                $return_data = $this->database->execute($query, ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
                
                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }
    
    public function licitatorPrepareAuction($data)
    {
        // SET auction start and end
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR ID
        if ($this->session->is_licitator_or_up_in() == true) {
            if (!empty($data["auction_id"]) && !empty($data["start_time"]) && !empty($data["end_time"])) {
                
                $return_data = $this->database->fetchAll('SELECT NOW() < "' .$data["start_time"]. '" AS start_time, "' .$data["start_time"]. '" < "' .$data["end_time"]. '" AS end_time, aukce.autor_id, aukce.licitator_id, aukce.stav FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                if ($return_data[0]['licitator_id'] == NULL) {
                    return return_error('auction has no licitator');
                }

                // check if start_time > NOW() and end_time > start_time
                if ($return_data[0]["start_time"] != 1 || $return_data[0]["end_time"] != 1) {
                    return return_error('start_time is outdated or end_time is not after start_time');
                }

                // LICITATOR ID
                if ($return_data[0]['licitator_id'] != $this->session->get_logged_in()['user_id']) {
                    return return_error('unauthorized access 2');
                }

                if (!($return_data[0]['stav'] == 'navrzena' || $return_data[0]['stav'] == 'pripravena')) {
                    return return_error('can\'t prepare auction in current state');
                }

                // PREPARE auction
                $query = 'UPDATE aukce SET stav = \'pripravena\', cas_zahajeni = :start_time, cas_ukonceni = :end_time WHERE id = :auction_id';

                $return_data = $this->database->execute($query, $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
                
                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }
    
    /*public function licitatorStartAuction($data)// TODO remove
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            if (!empty($data["auction_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id, aukce.stav FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                if ($return_data[0]['licitator_id'] == NULL) {
                    return return_error('auction has no licitator');
                }

                // LICITATOR ID
                if ($return_data[0]['licitator_id'] != $this->session->get_logged_in()['user_id']) {
                    return return_error('unauthorized access 2');
                }

                if (!($return_data[0]['stav'] == 'pripravena')) {
                    return return_error('can\'t start auction in current state');
                }

                // PREPARE auction
                $query = 'UPDATE aukce SET stav = \'probiha\' WHERE id = :auction_id';

                $return_data = $this->database->execute($query, $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
                
                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function licitatorEndAuction($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            if (!empty($data["auction_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id, aukce.stav FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                if ($return_data[0]['licitator_id'] == NULL) {
                    return return_error('auction has no licitator');
                }

                // LICITATOR ID
                if ($return_data[0]['licitator_id'] != $this->session->get_logged_in()['user_id']) {
                    return return_error('unauthorized access 2');
                }

                if (!($return_data[0]['stav'] == 'probiha')) {
                    return return_error('can\'t end auction in current state');
                }

                // PREPARE auction
                $query = 'UPDATE aukce SET stav = \'ukoncena\' WHERE id = :auction_id';

                $return_data = $this->database->execute($query, $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
                
                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }*/

    public function licitatorFinishAuction($data)
    {
        //SET winner and status vyhodnocena
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            if (!empty($data["auction_id"]) && !empty($data["winner_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id, aukce.stav FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                if ($return_data[0]['licitator_id'] == NULL) {
                    return return_error('auction has no licitator');
                }

                // LICITATOR ID
                if ($return_data[0]['licitator_id'] != $this->session->get_logged_in()['user_id']) {
                    return return_error('unauthorized access 2');
                }

                if (!($return_data[0]['stav'] == 'ukoncena')) {
                    return return_error('can\'t finish auction in current state');
                }

                // CHECK - vyherce neni autor ani licitator [a zaroven vyherce prihazoval (a mel nejvetsi prihoz)]
                if ($return_data[0]['licitator_id'] == $data["winner_id"] || $return_data[0]['autor_id'] == $data["winner_id"]) {
                    return return_error('licitator or author of auction can\'t be a winner');
                }

                // PREPARE auction
                $query = 'UPDATE aukce SET stav = \'vyhodnocena\', vyherce_id = :winner_id  WHERE id = :auction_id';

                $return_data = $this->database->execute($query, $data);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
                
                return return_ok($return_data);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }



    public function getAllParticipants($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            
            if (!empty($data["auction_id"])) {

                $auction = $this->getAuction($data);
            
                // CHECK AUCTION AND LICITATOR ID
                if (isset($auction["status"]) && $auction["status"] == 'OK' && isset($auction["id"]) && $auction["id"] == $data["auction_id"] && isset($auction["licitator_id"]) && ($auction["licitator_id"] == $this->session->get_logged_in()['user_id'] || $this->session->is_admin_in() == true)) {
                    
                    $return_data = $this->database->fetchAll('SELECT * FROM ucastnici WHERE aukce_id = :auction_id ORDER BY stav', ["auction_id" => $data["auction_id"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }
                    return return_ok($return_data);

                } else {
                    return return_error('unauthorized access 2');
                }
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function getAllParticipantsCEKAJICI($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {

            if ($this->session->is_admin_in() == true) {
                $return_data = $this->database->fetchAll('SELECT * FROM ucastnici WHERE stav = \'cekajici\'', []);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                return return_ok($return_data);
            } else {
                $return_data = $this->database->fetchAll('SELECT * FROM ucastnici WHERE licitator_id = :licitator_id AND stav = \'cekajici\'', ["licitator_id" => $this->session->get_logged_in()['user_id']]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                return return_ok($return_data);
            }

        } else {
            return return_error('unauthorized access');
        }
    }

    public function authorizeParticipant($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            
            if (!empty($data["auction_id"]) && !empty($data["participant_id"])) {

                $auction = $this->getAuction(["auction_id" => $data["auction_id"]]);
            
                // CHECK AUCTION AND LICITATOR ID
                if (isset($auction["status"]) && $auction["status"] == 'OK' && isset($auction["id"]) && $auction["id"] == $data["auction_id"] && isset($auction["licitator_id"]) && ($auction["licitator_id"] == $this->session->get_logged_in()['user_id'] || $this->session->is_admin_in() == true)) {
                    
                    $return_data = $this->database->fetchAll('SELECT * FROM ucastnici WHERE aukce_id = :auction_id AND uzivatel_id = :uzivatel_id LIMIT 1', ["auction_id" => $data["auction_id"], "uzivatel_id" => $data["participant_id"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }

                    if (!empty($return_data[0])) {
                        // UPDATE DATA
                        $query = 'UPDATE ucastnici SET stav = \'schvaleno\', licitator_id = :licitator_id WHERE uzivatel_id = :participant_id AND aukce_id = :auction_id';
                                            
                        $return_data = $this->database->execute($query, ["participant_id" => $data["participant_id"], "auction_id" => $data["auction_id"], "licitator_id" => $this->session->get_logged_in()['user_id']]);
                        if ($this->database->lastError() !== 'OK') {
                            return return_internal_error($this->database->lasterror());
                        }

                        if ($return_data['row_count'] == 0) {
                            return return_error('nothing changed');
                        }
                        return return_ok(["changed" => "OK"]);

                    } else {
                        return return_error('participant not found');
                    }
                } else {
                    return return_error('unauthorized access 2');
                }
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function rejectParticipant($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // ADMIN and LICITATOR
        if ($this->session->is_licitator_or_up_in() == true) {
            
            if (!empty($data["auction_id"]) && !empty($data["participant_id"])) {

                $auction = $this->getAuction(["auction_id" => $data["auction_id"]]);
            
                // CHECK AUCTION AND LICITATOR ID
                if (isset($auction["status"]) && $auction["status"] == 'OK' && isset($auction["id"]) && $auction["id"] == $data["auction_id"] && isset($auction["licitator_id"]) && ($auction["licitator_id"] == $this->session->get_logged_in()['user_id'] || $this->session->is_admin_in() == true)) {
                    
                    $return_data = $this->database->fetchAll('SELECT * FROM ucastnici WHERE aukce_id = :auction_id AND uzivatel_id = :uzivatel_id LIMIT 1', ["auction_id" => $data["auction_id"], "uzivatel_id" => $data["participant_id"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }
                    
                    if (!empty($return_data[0])) {
                        
                        if (isset($return_data[0]["stav"]) && $return_data[0]["stav"] != "opustil") {
                            // UPDATE DATA
                            $query = 'UPDATE ucastnici SET stav = \'zamitnuto\', licitator_id = :licitator_id WHERE uzivatel_id = :participant_id AND aukce_id = :auction_id';
                                                
                            $return_data = $this->database->execute($query, ["participant_id" => $data["participant_id"], "auction_id" => $data["auction_id"], "licitator_id" => $this->session->get_logged_in()['user_id']]);
                            if ($this->database->lastError() !== 'OK') {
                                return return_internal_error($this->database->lasterror());
                            }

                            if ($return_data['row_count'] == 0) {
                                return return_error('nothing changed');
                            }
                            return return_ok(["changed" => "OK"]);
                        } else {
                            return return_error('can\'t reject participant in status \'opustil\'');
                        }

                    } else {
                        return return_error('participant not found');
                    }
                } else {
                    return return_error('unauthorized access 2');
                }
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }



    /*
    participant_status:
        not - nepožádal o připojení do aukce
        cekajici - čeká na schválení připojení do aukce
        schvaleno - je účástníkem
        zamitnuto - připojení do aukce bylo zamítnuto
        opustil - účastník sám aukci opustil
    */
    public function participantGetInfo($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // USER
        if ($this->session->is_logged_in() == true && $this->auctionCheckAccess($data) == true) {
            if (!empty($data["auction_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT * FROM ucastnici WHERE aukce_id = :auction_id AND uzivatel_id = :uzivatel_id LIMIT 1', ["auction_id" => $data["auction_id"], "uzivatel_id" => $this->session->get_logged_in()['user_id']]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_ok(["participant_status" => "not"]);
                }
                return return_ok(["participant_status" => $return_data[0]["stav"]]);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function participantJoinAuction($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // USER
        if ($this->session->is_logged_in() == true && $this->auctionCheckAccess($data) == true) {
            if (!empty($data["auction_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id, aukce.stav FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                // LICITATOR ID and AUTHOR ID
                if ($return_data[0]['licitator_id'] == $this->session->get_logged_in()['user_id'] || $return_data[0]['autor_id'] == $this->session->get_logged_in()['user_id']) {
                    return return_error('author and licitator can\'t join auction as participants');
                }

                // CHECK IF user ALREADY IS participant 
                if ($this->participantGetInfo($data)["participant_status"] != "not") {
                    return return_error('participant can\'t join auction in his current state');
                }

                $query = "INSERT INTO ucastnici (uzivatel_id, aukce_id, licitator_id, stav) VALUES (";
                $query .= ':user_id, :auction_id, NULL, \'cekajici\')';
                
                $return_data = $this->database->insert($query, ["user_id" => $this->session->get_logged_in()['user_id'], "auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                return return_ok(["inserted" => "OK"]);

            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function participantQuitAuction($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // USER
        if ($this->session->is_logged_in() == true && $this->auctionCheckAccess($data) == true) {
            if (!empty($data["auction_id"])) {
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id, aukce.stav FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                // LICITATOR ID and AUTHOR ID
                if ($return_data[0]['licitator_id'] == $this->session->get_logged_in()['user_id'] || $return_data[0]['autor_id'] == $this->session->get_logged_in()['user_id']) {
                    return return_error('author and licitator can\'t join and quit auction as participants');
                }

                // CHECK IF user ALREADY IS participant 
                $participant_status = $this->participantGetInfo($data)["participant_status"];
                if ($participant_status == "not" || $participant_status == "zamitnuto") {
                    return return_error('participant can\'t quit auction in his current state');
                }

                // UPDATE DATA
                $query = 'UPDATE ucastnici SET stav = \'opustil\' WHERE uzivatel_id = :user_id AND aukce_id = :auction_id';
                                    
                $return_data = $this->database->execute($query, ["user_id" => $this->session->get_logged_in()['user_id'], "auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if ($return_data['row_count'] == 0) {
                    return return_error('nothing changed');
                }
                return return_ok(["changed" => "OK"]);

            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function participantAddBid($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        // USER
        if ($this->session->is_logged_in() == true && $this->auctionCheckAccess($data) == true) {
            if (!empty($data["auction_id"]) && !empty($data["bid"])) {

                if (is_numeric($data["bid"])) {
                    $data["bid"] = $data["bid"] + 0;
                } else {
                    return return_error('bid is not numeric');
                }
                
                $return_data = $this->database->fetchAll('SELECT aukce.autor_id, aukce.licitator_id, aukce.stav, aukce.typ, vyvolavaci_cena, min_cena, max_cena FROM aukce WHERE aukce.id = :auction_id LIMIT 1', ["auction_id" => $data["auction_id"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }

                if (empty($return_data[0])) {
                    return return_error('auction not found');
                }

                // LICITATOR ID and AUTHOR ID
                if ($return_data[0]['licitator_id'] == $this->session->get_logged_in()['user_id'] || $return_data[0]['autor_id'] == $this->session->get_logged_in()['user_id']) {
                    return return_error('author and licitator can\'t bid in auction');
                }

                // CHECK IF user ALREADY IS participant
                if ($this->participantGetInfo($data)["participant_status"] != "schvaleno") {
                    return return_error('participant can\'t bid in auction in his current state');
                }

                if ($return_data[0]['stav'] != 'probiha') {
                    return return_error('participants can\'t bid in current state of the auction');
                }

                
                if ($return_data[0]['typ'] == 'nabidkova') {
                    // check start_cost
                    if ($return_data[0]['vyvolavaci_cena'] > $data["bid"]) {
                        return return_error('bid is not bigger than start cost');
                    }
                    
                    // check if BID is THE greatest
                    $return_data = $this->database->fetchAll('SELECT cena FROM nabidka WHERE aukce_id = :auction_id ORDER BY cena DESC LIMIT 1', ["auction_id" => $data["auction_id"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }

                    if (!empty($return_data[0]) && ($return_data[0]["cena"] + 0) >= $data["bid"]) {
                        return return_error('bid is not bigger than latest bid');
                    }
                } else if ($return_data[0]['typ'] == 'poptavkova') {
                    // check min and max cost
                    if ($return_data[0]['min_cena'] <= $data["bid"] && $data["bid"] <= $return_data[0]['max_cena']) {
                        return return_error('bid is not between min cost and max cost');
                    }

                    // check if BID is THE smallest
                    $return_data = $this->database->fetchAll('SELECT cena FROM nabidka WHERE aukce_id = :auction_id ORDER BY cena LIMIT 1', ["auction_id" => $data["auction_id"]]);
                    if ($this->database->lastError() !== 'OK') {
                        return return_internal_error($this->database->lasterror());
                    }

                    if (!empty($return_data[0]) && ($return_data[0]["cena"] + 0) <= $data["bid"]) {
                        return return_error('bid is not bigger than latest bid');
                    }
                } else {
                    return return_error('auction type error');
                }


                // add bid
                $query = "INSERT INTO nabidka (aukce_id, uzivatel_id, cena, cas) VALUES (";
                $query .= ':auction_id, :user_id, :bid, NOW())';
                
                $return_data = $this->database->insert($query, ["user_id" => $this->session->get_logged_in()['user_id'], "auction_id" => $data["auction_id"], "bid" => $data["bid"]]);
                if ($this->database->lastError() !== 'OK') {
                    return return_internal_error($this->database->lasterror());
                }
                return return_ok(["inserted" => "OK"]);
            } else {
                return return_error('empty fields');
            }
        } else {
            return return_error('unauthorized access');
        }
    }

    public function auctionGetAllBids($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        if (!empty($data["auction_id"])) {

            $auction = $this->getAuction($data);
        
            if (isset($auction["status"]) && $auction["status"] == 'OK' && isset($auction["id"]) && $auction["id"] == $data["auction_id"]) {
                
                // otevrena or LICITATOR ID or ADMIN
                if ($auction["pravidla"] == 'otevrena' || (($this->session->is_logged_in() == true && $auction["licitator_id"] == $this->session->get_logged_in()['user_id']) || $this->session->is_admin_in() == true)) {

                    if ($auction["typ"] == 'nabidkova') {

                        $return_data = $this->database->fetchAll('SELECT * FROM nabidka WHERE aukce_id = :auction_id ORDER BY cena DESC', ["auction_id" => $data["auction_id"]]);
                        if ($this->database->lastError() !== 'OK') {
                            return return_internal_error($this->database->lasterror());
                        }

                        return return_ok($return_data);

                    } else {

                        $return_data = $this->database->fetchAll('SELECT * FROM nabidka WHERE aukce_id = :auction_id ORDER BY cena', ["auction_id" => $data["auction_id"]]);
                        if ($this->database->lastError() !== 'OK') {
                            return return_internal_error($this->database->lasterror());
                        }
                        
                        return return_ok($return_data);

                    }

                } else {
                    return return_error('auction is not open type');
                }
            } else {
                return return_error('unauthorized access');
            }
        } else {
            return return_error('empty fields');
        }
    }

    public function auctionGetWinner($data)
    {
        unset($data['page_number']);
        unset($data['item_count']);
        unset($data['item_limit']);

        if (!empty($data["auction_id"])) {

            $auction = $this->getAuction($data);
        
            if (isset($auction["status"]) && $auction["status"] == 'OK' && isset($auction["id"]) && $auction["id"] == $data["auction_id"]) {
            
                if ($auction["stav"] == 'vyhodnocena') {

                    if ($auction["typ"] == 'nabidkova') {

                        $return_data = $this->database->fetchAll('SELECT aukce_id, uzivatel_id, cena FROM nabidka WHERE aukce_id = :auction_id AND uzivatel_id = :winner_id ORDER BY cena DESC LIMIT 1', ["auction_id" => $data["auction_id"], "winner_id" => $auction["vyherce_id"]]);
                        if ($this->database->lastError() !== 'OK') {
                            return return_internal_error($this->database->lasterror());
                        }

                        return return_ok($return_data[0]);

                    } else {

                        $return_data = $this->database->fetchAll('SELECT aukce_id, uzivatel_id, cena FROM nabidka WHERE aukce_id = :auction_id AND uzivatel_id = :winner_id ORDER BY cena LIMIT 1', ["auction_id" => $data["auction_id"], "winner_id" => $auction["vyherce_id"]]);
                        if ($this->database->lastError() !== 'OK') {
                            return return_internal_error($this->database->lasterror());
                        }
                        
                        return return_ok($return_data[0]);

                    }

                } else {

                    return return_error('auction is not finished');
                }
            } else {
                return return_error('unauthorized access');
            }
        } else {
            return return_error('empty fields');
        }
    }

}

?>
