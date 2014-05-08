/**
 * Include this JS file to embed aggregated OpenScholar SAP stats on any page.
 * Like this:
 * @code
 * <script type="text/javascript" src="http://dev.openscholar.harvard.edu/ossap/stats.js"></script>
 * @endcode
 *
 * Your HTML markup may use any of the following id attributes to get numbers:
 * @code
 * <span class="ossap-stats-filesize_bytes"></span>
 * <span class="ossap-stats-users"></span>
 * <span class="ossap-stats-new_users"></span>
 * <span class="ossap-stats-websites"></span>
 * <span class="ossap-stats-new_websites"></span>
 * <span class="ossap-stats-posts"></span>
 * <span class="ossap-stats-new_posts"></span>
 * <span class="ossap-stats-publications"></span>
 * <span class="ossap-stats-new_publications"></span>
 * <span class="ossap-stats-files"></span>
 * <span class="ossap-stats-os_version"></span>
 * <span class="ossap-stats-activity-messages"></span>
 * <span class="ossap-stats-gs-recent_active_topics"></span>
 * <span class="ossap-stats-gs-open_topics"></span>
 * <span class="ossap-stats-gs-closed_topics"></span>
 * <span class="ossap-stats-most_visited_sites"></span>
 * <span class="ossap-stats-most_viewed_pages"></span>
 * @endcode
 *
 * Note: The examples above use SPAN tags, but you may use DIV, P, etc.
 *
 * @see https://github.com/openscholar/ossap
 */

(function(){

 // Most visited sites 
  var elements = document.getElementsByClassName('ossap-stats-most_visited_sites');
  for(var i = 0, j = 0; i < elements && j < elements.length ; i++, j++) {
                elements[i].innerHTML = 'fss.finance.harvard.edu';
				elements[j].innerHTML = '16990';
  }
  // Most viewed pages
 var elements = document.getElementsByClassName('ossap-stats-most_viewed_pages');
  for(var i = 0, j = 0; i < elements.length && j < elements.length ; i++, j++) {
                elements[i].innerHTML = 'fss.finance.harvard.edu/applications';
				elements[j].innerHTML = '24109';
  }
  
  // Current filesize_bytes
  var elements = document.getElementsByClassName('ossap-stats-filesize_bytes');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '53.28 GB';
  }

  // Current users
  var elements = document.getElementsByClassName('ossap-stats-users');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '5,482';
  }

  // Current new_users
  var elements = document.getElementsByClassName('ossap-stats-new_users');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '0';
  }

  // Current websites
  var elements = document.getElementsByClassName('ossap-stats-websites');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '3,974';
  }

  // Current new_websites
  var elements = document.getElementsByClassName('ossap-stats-new_websites');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '0';
  }

  // Current posts
  var elements = document.getElementsByClassName('ossap-stats-posts');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '113,037';
  }

  // Current new_posts
  var elements = document.getElementsByClassName('ossap-stats-new_posts');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '194';
  }

  // Current publications
  var elements = document.getElementsByClassName('ossap-stats-publications');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '35,743';
  }

  // Current new_publications
  var elements = document.getElementsByClassName('ossap-stats-new_publications');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '0';
  }

  // Current files
  var elements = document.getElementsByClassName('ossap-stats-files');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '91,622';
  }

  // Current os_version
  var elements = document.getElementsByClassName('ossap-stats-os_version');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '';
  }

  // Current activity-messages
  var elements = document.getElementsByClassName('ossap-stats-activity-messages');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = "<div class=\"item-list\"><ul class=\"ossap-stats-embed-messages\">\
				<li><time class=\"time\" datetime=\"2014-05-07 10:27:40-0400\" title=\"Wed, 05\/07\/2014 - 10:27\">8 hours 2 min ago<\/time>\n<p><a href=\"http:\/\/dev.scholar.harvard.edu\/tcheng2\/content\/tcheng2\">tcheng2<\/a> edited News: <a href=\"http:\/\/dev.scholar.harvard.edu\/tcheng2\/news\/test-private-comment\">Test comment format<\/a><\/p>\n<\/li><li><time class=\"time\" datetime=\"2014-05-07 09:53:31-0400\" title=\"Wed, 05\/07\/2014 - 09:53\">8 hours 36 min ago<\/time>\n<p><a href=\"http:\/\/dev.scholar.harvard.edu\/tcheng2\/content\/tcheng2\">tcheng2<\/a> created News: <a href=\"http:\/\/dev.scholar.harvard.edu\/tcheng2\/node\/164841\">This is the news title<\/a><\/p>\n<\/li><li><time class=\"time\" datetime=\"2014-05-07 09:52:39-0400\" title=\"Wed, 05\/07\/2014 - 09:52\">8 hours 37 min ago<\/time>\n<p><a href=\"http:\/\/dev.scholar.harvard.edu\/tcheng2\/content\/tcheng2\">tcheng2<\/a> edited Class: <a href=\"http:\/\/dev.scholar.harvard.edu\/tcheng2\/classes\/class-2014-na\">class 2014 N\/A<\/a><\/p>\n<\/li><\/ul><\/div>";
  }

  // Current gs-recent_active_topics
  var elements = document.getElementsByClassName('ossap-stats-gs-recent_active_topics');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '21';
  }

  // Current gs-open_topics
  var elements = document.getElementsByClassName('ossap-stats-gs-open_topics');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '61';
  }

  // Current gs-closed_topics
  var elements = document.getElementsByClassName('ossap-stats-gs-closed_topics');
  for(var i = 0; i < elements.length; i++) {
                elements[i].innerHTML = '562';
  }

})();