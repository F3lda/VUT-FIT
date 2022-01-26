<?php
require_once(__DIR__ . '/../../../include/common.php');




// HEAD + START BODY TAG
$cmpnt_html_head = ['title' => 'Aukce', 'css' => [], 'js' => ['./frontend/js/auction/auctions.js']];
include(__DIR__ .'/../../components/html_head.php');



// MENU
include(__DIR__ .'/../../components/common/menu.php');
$cmpnt_menu['active'] = 'Aukce';
include(__DIR__ .'/../../components/menu.php');

?>

<div class="container pt-4">
  <div class="form-row">
    <div class="form-group form-inline col-md-1 ml-auto">
      <label for="item_count">Počet: </label>
      <select class="form-control" autocomplete="off" id="select_item_count">
        <option>4</option>
        <option selected="selected">8</option>
        <option>12</option>
     </select>
    </div>
  </div>
    <div class="row" id="auction_data">
        <!-- Generuje auctions.js -->
    </div>
</div>

<div class="container">       
  <ul class="pagination justify-content-center" id="pagination_wrapper">
  </ul>
</div>