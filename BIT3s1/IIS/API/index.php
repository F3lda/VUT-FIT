<?php
require_once(__DIR__ . "/../include/common.php");



//allow access from any client (public API)
header('Content-Type: application/json; charset=UTF-8');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST');
header('Access-Control-Allow-Headers: origin, x-csrftoken, content-type, accept, x-requested-with');



require_once(__DIR__ . '/DB/database.php');
require_once(__DIR__ . '/auth/auth.php');
require_once(__DIR__ . '/auth/session.php');
require_once(__DIR__ . '/auctions/auctions.php');
require_once(__DIR__ . '/users/users.php');



define('ITEMS_PER_PAGE', 20);



//response data
$return_data = ['status' => 'ERROR', 'error' => 'bad request'];
$return_code = 0;


$database = new Database();
$database->connect();



$session = new Session($database);
$auth = new Auth($session);
$users = new Users($database, $session);
$auctions = new Auction($database, $session);



$received_data = json_decode(file_get_contents('php://input'), true);
if ($received_data == NULL) {
    if (isset($_POST["json"])) {
        $received_data = json_decode($_POST["json"], true);
    }
}



$received_data['page_number'] = 1;
$received_data['item_count'] = ITEMS_PER_PAGE;

if (isset($_GET['page_number'])) {
    $_GET['page_number'] += 0;
    if (is_int($_GET['page_number']) && $_GET['page_number'] > 0) {
        $received_data['page_number'] = $_GET['page_number'];
    }
}

if (isset($_GET['item_count'])) {
    $_GET['item_count'] += 0;
    if (is_int($_GET['item_count']) && $_GET['item_count'] > 0 && $_GET['item_count'] < 51 ) {
        $received_data['item_count'] = $_GET['item_count'];
    }
}

$received_data['item_limit'] = $received_data['page_number']*$received_data['item_count'];



//process the request
if (!empty($_GET['cmd'])) {
    switch ($_GET["cmd"]) {
        
        /* ********** LOGIN ********** */
        case 'login':
            if (!empty($_POST['username']) && !empty($_POST['password'])) {
                $received_data = ['username' => $_POST['username'], 'password' => $_POST['password']];
            } else if (empty($received_data['username']) || empty($received_data['password'])) {
                $received_data = ['username' => '', 'password' => ''];
            }
            $return_data = $auth->login($received_data['username'], $received_data['password']);
            break;
        
        case 'logout':
            $return_data = $auth->logout();
            break;
            
        case 'is_loggedin':
            $return_data = $auth->is_logged_in();
            break;

        case 'get_loggedin':
            $return_data = $auth->get_logged_in();
            break;
        


        /* ********** USERS ********** */
        case 'adduser':
            $return_data = $users->addUser($received_data);
            break;
            
        case 'getuser':
            $return_data = $users->getUser($received_data);
            break;
            
        case 'getusername':
            $return_data = $users->getUserName($received_data);
            break;
            
         case 'edituser':
            $return_data = $users->editUser($received_data);
            break;

        case 'removeuser':
            $return_data = $users->removeUser($received_data);
            break;
            
        case 'loadusers':
            $return_data = $users->getAll($received_data);
            break;
            
        case 'getUsersCount':
            $return_data = $users->getAllCount($received_data);
            break;
        


        /* ********** AUCTIONS ********** */
        // getAuctions (All + User or Licitator + Admin)
        case 'getAuctions':
            $return_data = $auctions->getAll($received_data);
            break;
        
        case 'getAuctionsCount':
            $return_data = $auctions->getAllCount($received_data);
            break;

        // getAuctionsCreatedByMe (User + UP)
        case 'getAuctionsCreatedByMe':
            $return_data = $auctions->getAllMyCreated($received_data);
            break;

        case 'getAuctionsCreatedByMeCount':
            $return_data = $auctions->getAllMyCreatedCount($received_data);
            break;

        // getAuctionsAttendedByMe (User + UP)
        case 'getAuctionsAttendedByMe':
            $return_data = $auctions->getAllMyAttended($received_data);
            break;

        case 'getAuctionsAttendedByMeCount':
            $return_data = $auctions->getAllMyAttendedCount($received_data);
            break;
        
        // getAuctionsNavrzena (Licitator + Admin)
        case 'getAuctionsNavrzena':
            $return_data = $auctions->getAllAuctionsNavrzena($received_data);
            break;

        case 'getAuctionsNavrzenaCount':
            $return_data = $auctions->getAllAuctionsNavrzenaCount($received_data);
            break;
        
        // getAuctionsManagedByLicitator (Licitator + Admin)
        case 'getAuctionsManagedByLicitator':
            $return_data = $auctions->getAllManagedByLicitator($received_data);
            break;

        case 'getAuctionsManagedByLicitatorCount':
            $return_data = $auctions->getAllManagedByLicitatorCount($received_data);
            break;

        // Get one auction (All + User or Licitator + Admin)
        case 'getAuction':
            // check if auction started and set status to 'probiha' or check if auction ended and set status to 'ukoncena'
            $return_data = $auctions->checkAuctionDateAndStatus($received_data);
            if ($return_data["status"] == "OK") {
                $return_data = $auctions->getAuction($received_data);
            }
            break;

        // Add auction (User + UP)
        case 'addAuction':
            $return_data = $auctions->addAuction($received_data);
            break;

        // Remove auction (Author + Admin)
        case 'removeAuction':
            $return_data = $auctions->onEventWhileNAVRZENA($received_data, 'remove');
            break;

        // Edit auction (Author + Admin)
        case 'editAuction':
            $return_data = $auctions->onEventWhileNAVRZENA($received_data, 'edit');
            break;
            
        // Cancel auction (Author or Licitator or Admin)
        case 'cancelAuction':
            $return_data = $auctions->onEventWhilePRIPRAVENA($received_data, 'cancel');
            break;



        /* ********** LICITATOR ********** */
        // join auction as licitator
        case 'addLicitator':
            $return_data = $auctions->addLicitator($received_data);
            break;

        // quit auction as licitator
        case 'removeLicitator':
            $return_data = $auctions->removeLicitator($received_data);
            break;



        // prepare auction where i am licitator - licitátor připraví aukci, nastaví začátek a konec podávání nabídek
        case 'prepareAuction':
            $return_data = $auctions->licitatorPrepareAuction($received_data);
            break;
            
        // start auction where i am licitator - licitátor // TODO remove
        /*case 'startAuction':
            $return_data = $auctions->licitatorStartAuction($received_data);
            break;
            
        // end auction where i am licitator
        case 'endAuction':
            $return_data = $auctions->licitatorEndAuction($received_data);
            break;*/
            
        // finish auction where i am licitator - licitátor vyhlásí vítěze
        case 'finishAuction':
            $return_data = $auctions->licitatorFinishAuction($received_data);
            break;



        // getAllParticipants
        case 'getAllParticipants':
            $return_data = $auctions->getAllParticipants($received_data);
            break;
        
        // getAllParticipantsCEKAJICI
        case 'getAllParticipantsCEKAJICI':
            $return_data = $auctions->getAllParticipantsCEKAJICI($received_data);
            break;
        
        // authorizeParticipant
        case 'authorizeParticipant':
            $return_data = $auctions->authorizeParticipant($received_data);
            break;

        // rejectParticipant
        case 'rejectParticipant':
            $return_data = $auctions->rejectParticipant($received_data);
            break;



        /* ********** PARTICIPANT ********** */
        // participantGetInfo
        case 'participantGetInfo':
            $return_data = $auctions->participantGetInfo($received_data);
            break;

        // participantJoinAuction
        case 'participantJoinAuction':
            $return_data = $auctions->participantJoinAuction($received_data);
            break;

        // participantQuitAuction
        case 'participantQuitAuction':
            $return_data = $auctions->participantQuitAuction($received_data);
            break;

        // participantAddBid
        case 'participantAddBid':
            $return_data = $auctions->participantAddBid($received_data);
            break;

        

        /* ********** BIDS ********** */
        // auctionGetAllBids
        case 'auctionGetAllBids':
            $return_data = $auctions->auctionGetAllBids($received_data);
            break;

        // auctionGetWinner
        case 'auctionGetWinner':
            $return_data = $auctions->auctionGetWinner($received_data);
            break;

    }
}


$database->disconnect();


// set exit code
if ($return_code == 0) {
    if ($return_data['status'] == 'OK') {
        $return_code = 200;
    } else if ($return_data['status'] == 'ERROR' && $return_data['error'] == 'INTERNAL_ERROR') {
        $return_code = 500;
    } else {
        $return_code = 400;
    }
}


//send the JSON result
http_response_code($return_code);
echo json_encode($return_data);

?>
