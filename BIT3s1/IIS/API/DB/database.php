<?php

class Database {
    
    private $db_host = 'localhost';
    private $db_name = 'iisprojekt';
    private $db_username = 'iisdb';
    private $db_password = 'GvidTeam';
    private $pdo_connection = null;
    private $pdo_lasterror = 'OK';
    
    
    
    public function connect()
    {
        $this->pdo_connection = null;

        try {
            $this->pdo_connection = new PDO('mysql:host=' . $this->db_host . ';dbname=' . $this->db_name, $this->db_username, $this->db_password);
            $this->pdo_connection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        } catch(PDOException $e) {
            $this->pdo_lasterror = 'SQL connection error: ' . $e->getMessage();
            return 'SQL connection error: ' . $e->getMessage();
        }
        $this->pdo_lasterror = 'OK';
        return 'OK';
    }
    
    public function get_pdoConnection()
    {
        return $this->pdo_connection;
    }
    
    public function disconnect()
    {
        $this->pdo_connection = null;
        $this->pdo_lasterror = 'OK';
        return 'OK';
    }
    
    public function execute($request, $data)
    {
        try {
            $sql_stmt = $this->pdo_connection->prepare($request);
            $sql_stmt->execute($data);
            $row_count = $sql_stmt->rowCount();
            
            $this->pdo_lasterror = 'OK';
            return ['row_count' => $row_count];
        } catch(PDOException $e) {
            $this->pdo_lasterror = 'SQL request execute error: ' . $e->getMessage();
            return 'SQL request execute error: ' . $e->getMessage();
        }
        
    }
    
    public function fetchAll($request, $data)
    {
        try {
            $sql_stmt = $this->pdo_connection->prepare($request);
            $sql_stmt->execute($data);
            $return_data = $sql_stmt->fetchAll(PDO::FETCH_ASSOC); // $return_data = $sql_stmt->fetch();
            
            $this->pdo_lasterror = 'OK';
            return $return_data;
        } catch(PDOException $e) {
            $this->pdo_lasterror = 'SQL request fetch error: ' . $e->getMessage();
            return 'SQL request fetch error: ' . $e->getMessage();
        }
    }

    public function fetch($request, $data)
    {
        try {
            $sql_stmt = $this->pdo_connection->prepare($request);
            $sql_stmt->execute($data);
            $return_data = $sql_stmt->fetch(PDO::FETCH_ASSOC); // $return_data = $sql_stmt->fetch();
            
            $this->pdo_lasterror = 'OK';
            return $return_data;
        } catch(PDOException $e) {
            $this->pdo_lasterror = 'SQL request fetch error: ' . $e->getMessage();
            return 'SQL request fetch error: ' . $e->getMessage();
        }
    }
    
    public function insert($request, $data)
    {
        try {
            $sql_stmt = $this->pdo_connection->prepare($request);
            $sql_stmt->execute($data);
            $return_data = ['inserted_id' => $this->pdo_connection->lastInsertId()];
            
            $this->pdo_lasterror = 'OK';
            return $return_data;
        } catch(PDOException $e) {
            $this->pdo_lasterror = 'SQL request insert error: ' . $e->getMessage();
            return 'SQL request insert error: ' . $e->getMessage();
        }
    }
    
    public function lastError()
    {
        return $this->pdo_lasterror;
    }
    
}
?>
