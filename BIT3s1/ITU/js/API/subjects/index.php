<?php
/**
* @file index.php [API] (FITeduler)
* 
* @brief PHP API
* @date 2022-01-19 (YYYY-MM-DD)
* @author Tran Thanh Quang M., Tereza Buchníčková, Karel Jirgl
* @update 2022-01-23 (YYYY-MM-DD)
*/

// Tran - start
//allow access from any client (public API)
header('Content-Type: application/json; charset=UTF-8');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST');
header('Access-Control-Allow-Headers: origin, x-csrftoken, content-type, accept, x-requested-with');

$return_data = [];
$return_code = 0;

$string = file_get_contents("./data.json");
$data = json_decode($string, true);
$keys = array_keys($data);

if (!empty($_GET["cmd"])) {
    switch ($_GET["cmd"]) {

        
        case 'loadAllSubjects':
            $return_data = array();
            foreach($data["subjects"] as $index=>$value){
                if($_GET["study"] == $value['study'] && $_GET["semester"] == $value['semester'] && $_GET["year"] == $value['year']) {
                    $return_data[$index]['nickname'] = $value['nickname'];
                    $return_data[$index]['semester'] = $value['semester'];
                    $return_data[$index]['year'] = $value['year'];
                    $return_data[$index]['study'] = $value['study'];
                    if(!empty($value["compulsory"])) {
                        $return_data[$index]["compulsory"] = $value["compulsory"];
                    } else {
                        $return_data[$index]["compulsory"] = '0';
                    }
                }
            }
            $return_data = array_map("unserialize", array_unique(array_map("serialize", $return_data)));
            $return_data = array_values($return_data);
            break;

        case 'loadSelectedSubjects':
            $received_data = json_decode(file_get_contents('php://input'), true);
            foreach($data["subjects"] as $index=>$value){
                if( in_array($value['nickname'], $received_data['compulsory']) || in_array($value['nickname'], $received_data['optional']) ) {
                    $value["index"] = $index;
                    $return_data[$index] = $value;
                }
            }
            $return_data = array_map("unserialize", array_unique(array_map("serialize", $return_data)));
            $return_data = array_values($return_data);
            break;
        
        // Karel - start
        case 'getLessonInfo':
            $received_data = json_decode(file_get_contents('php://input'), true);
            $return_data = $data['subjects'][(int)$received_data['id']];
            break;
        // Karel - end

        // Tereza - start
        case 'getSports':
            $return_data = array();
            $sports =  json_decode(file_get_contents("./sports.json"), true);
            foreach ($sports["sports"] as $index=>$value) { 
                if ($_GET["filter"] == "all") {
                    $return_data[] = $value; 
                }
                if (strpos(mb_strtoupper($value['name'], 'UTF-8'), $_GET["filter"]) !== false or
                    strpos(mb_strtoupper($value['day'], 'UTF-8'), $_GET["filter"]) !== false or
                    strpos(mb_strtoupper($value['time'], 'UTF-8'), $_GET["filter"]) !== false)  {
                    $return_data[] = $value;
                }
            }
            break;
        // Tereza - end
    }
}

http_response_code($return_code);
//echo $return_data;
echo json_encode($return_data);
// Tran - end
?>
