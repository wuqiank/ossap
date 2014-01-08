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

global $base_url;
$src = "$base_url/ossap/stats.js";
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
  // Total <?php echo $key; ?>

  var elem = document.getElementById('ossap-stats-<?php echo $key; ?>');
  if (typeof div !== 'undefined' && div !== null) {
    <?php $value = ($key == 'filesize_bytes') ? format_size($value) : $value ?>
    div.innerText = '<?php echo $value; ?>';
  }

<? endforeach; ?>
})();