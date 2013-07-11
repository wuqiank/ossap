

function ossapStatsGetSitesUrl() {
    var pathArray = window.location.href.split( '/' );
    var protocol = pathArray[0];
    var host = pathArray[2];
    var url = protocol + '//' + host + '/ossap/stats/sites';
    return url;
}

function ossapStatsUpdateDiv() {
    var div = document.getElementById('ossap-stats-sites');
    var request = new XMLHttpRequest();
    var url = ossapStatsGetSitesUrl();

    request.open("GET", url,true);
    request.onreadystatechange = function() {
        if (request.readyState == 4) {
            data = (request.responseText);
            div.innerText = data;
        }
    }
    request.send(null);
}

console.log('loaded!!!');
ossapStatsUpdateDiv();
/*jQuery(document).ready(function($) {
    // Returns the local menu URL to query for the total number.
    function getOssapStatsSitesUrl() {
        var pathArray = window.location.href.split( '/' );
        var protocol = pathArray[0];
        var host = pathArray[2];
        var url = protocol + '://' + host + '/ossap/stats/sites';
        return url;
    }

    var selector = '#ossap-stats-sites';
    if ($(selector).length) {
        var url = getOssapStatsSitesUrl();
        $.ajax({
            'url' : url,
        }).done(function(data) {
            $(selector).text(data);
        });
    }
});*/