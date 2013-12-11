<?php

/**
 * @file
 * Javascript source to print the total sites onto the page.
 */

if(empty($stats)){
  $stats = array();
}
?>


/** Updates the div element to contain the latest total number of vsites. */
function ossapStatsGet(stat) {
<?php
foreach (variable_get('os_stats_enabled',array('websites')) as $stat){
  echo "if (stat == '{$stat}') {\nreturn '{$stats[$stat]}'; ?>';\n}\n";
}
?>
}
