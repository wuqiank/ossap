<?php

/**
 * @file
 * Javascript source to print the total sites onto the page.
 *
 * Available variables:
 *
 * $aggregates - the value of the 'ossap_stats_aggregates' variable, an indexed
 * array where keys are the statistic's description, and values are numeric
 * values.
 */

$src = url('ossap/stats.js', array('absolute' => TRUE));

if (isset($os_version)) {
  $aggregates['os_version'] = $os_version;
}
if (isset($messages) && !empty($messages)) {
  $aggregates['activity-messages'] = $messages;
}
if (isset($getsatisfaction) && !empty($getsatisfaction)) {
  foreach ($getsatisfaction as $key => $value) {
    $aggregates["gs-{$key}"] = $value;
  }
}
if (isset($visited_sites) && !empty($visited_sites)) {
  foreach ($visited_sites as $key => $value) {
    $aggregates["most-visited-sites{$key}"] = "{$value[0]} {$value[1]}";
  }
}
if (isset($viewed_pages) && !empty($viewed_pages)) {
  foreach ($viewed_pages as $key => $value) {
    $aggregates["most-viewed-pages{$key}"] = "{$value[0]} {$value[1]}";
  }
}
if (isset($rt_visitors) && !empty($rt_visitors)) {
  $aggregates['real-time-visitors'] = $rt_visitors;
}


?>
/**
 * Include this JS file to embed aggregated OpenScholar SAP stats on any page.
 * Like this:
 * @code
 * <script type="text/javascript" src="<?php echo $src; ?>"></script>
 * @endcode
 *
<?php
if (empty($aggregates)) {
  echo " * Something is wrong. Either no child domains are configured, or the\n";
  echo " * ossap_stats cron job needs to be run. Contact a system administrator.\n";
  echo " */";
  die();
}
?>
 * Your HTML markup may use any of the following id attributes to get numbers:
 * @code
<? foreach ($aggregates as $key => $value): ?>
 * <span class="ossap-stats-<?php echo $key; ?>"></span>
<? endforeach; ?>
 * @endcode
 *
 * Note: The examples above use SPAN tags, but you may use DIV, P, etc.
 *
 * @see https://github.com/openscholar/ossap
 */

var jqueryRefresh;

$().ready(function(){
  //jQuery function
  jqueryRefresh = function(){
    setInterval(function(){
      $("#realtime-vis").load( "http://dev.openscholar.harvard.edu/ossap/real-time-visitors")  
    }, 3000);
  }
})
 
(function(){

<? foreach ($aggregates as $key => $value): ?>
  // Current <?php echo $key; ?>

  var elements = document.getElementsByClassName('ossap-stats-<?php echo $key; ?>');
  for(var i = 0; i < elements.length; i++) {
    <?php $value = ($key == 'filesize_bytes') ? format_size($value) : $value ?>
    <?php $value = ($key != "os_version" && is_numeric($value)) ? number_format($value) : $value ?>
    <?php $value = ($key == 'activity-messages') ? $value : "'{$value}'" ?>
    elements[i].innerHTML = <?php echo $value; ?>;
  }

<? endforeach; ?>
  jqueryRefresh();
})();

