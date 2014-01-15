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
 * <div id="ossap-stats-<?php echo $key; ?>"></div>
<? endforeach; ?>
 * @endcode
 *
 * Note: The examples above use DIV tags, but you may use SPAN, P, etc.
 *
 * @see https://github.com/openscholar/ossap
 */

(function(){

<? foreach ($aggregates as $key => $value): ?>
  // Current <?php echo $key; ?>

  var elem = document.getElementById('ossap-stats-<?php echo $key; ?>');
  if (typeof elem !== 'undefined' && elem !== null) {
    <?php $value = ($key == 'filesize_bytes') ? format_size($value) : $value ?>
    elem.innerText = '<?php echo $value; ?>';
    <?php $value = (is_numeric($value)) ? number_format($value) : $value ?>
  }

<? endforeach; ?>
})();