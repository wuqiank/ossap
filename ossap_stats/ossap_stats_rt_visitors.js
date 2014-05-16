(function ($) {
  Drupal.behaviors.refresh_rt_visitors = {
    attach: function (context, settings) {

	  setInterval(function() {
	  $( ".ossap-stats-real-time-visitors" ).load( "ossap/stats.js .ossap-stats-real-time-visitors", function() {
   
})
       }, 1000); 
    }
  };
})(jQuery);