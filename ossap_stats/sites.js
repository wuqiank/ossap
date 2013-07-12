/**
 * Prepares the local URL to fetch the number of sites from.
 * @returns {string}
 */
function ossapStatsGetSitesUrl() {
    var pathArray = window.location.href.split( '/' );
    var protocol = pathArray[0];
    var host = pathArray[2];
    var url = protocol + '//' + host + '/ossap/stats/sites';
    return url;
}

/**
 * Fetches the total number of sites via AJAX and updates the element.
 */
function ossapStatsUpdateDiv() {
    // Only continues if there is a properly ID'd div tag to update.
    var div = document.getElementById('ossap-stats-sites');
    if (typeof div === 'undefined') {
        return;
    }

    // Finds the URL of the local menu item to query for the number.
    var url = ossapStatsGetSitesUrl();

    // Makes an AJAX GET request.
    var request = new XMLHttpRequest();
    request.open("GET", url, true);
    request.onreadystatechange = function() {
        if (request.readyState == 4) {
            data = (request.responseText);
            div.innerText = data;
        }
    }
    request.send(null);
}

// Runs our function.
ossapStatsUpdateDiv();
