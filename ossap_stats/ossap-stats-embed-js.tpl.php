<?php

/**
 * @file
 * Javascript source to print the total sites onto the page.
 */

if (empty($total)) {
  $total = "0";
}

?>
/** Updates the div element to contain the latest total number of vsites. */
function ossapStatsUpdateDiv() {
  var div = document.getElementById('ossap-stats-sites');
  if (typeof div !== 'undefined' && div !== null) {
    div.innerText = '<?php print $total; ?>';
  }
}
ossapStatsUpdateDiv();
