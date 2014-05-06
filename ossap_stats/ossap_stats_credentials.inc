<?php

/* authenticate credentials by creating a new Google_client object and 
* sending it to google analytics
* ossap_stats_check_credentials
* no parameters
* creates the google_client object
* returns the validated service object
*/

function ossap_stats_check_credentials(){
    //variables definition block starts
    $output ='';
	$access_type = 'offline_access';
    $scope = 'https://www.googleapis.com/auth/analytics.readonly';
	
    //create a new instance of Google_client
    $client = new Google_Client();
    // set the application name
    $client->setApplicationName('OS Statistics');
	
    //set the credentials, clientid and accesstype
	$fid = variable_get('ossap_stats_private_key_fid', '');
    if (!empty($fid)) {
	  $uri = db_query('SELECT uri FROM {file_managed} WHERE fid = :fid', array(':fid' => $fid))->fetchColumn();
	  $fhandle = fopen($uri, 'r');
	  $result = fread($fhandle, filesize($uri));
	  fclose($fhandle);
	}
	
	$client->setAssertionCredentials(new Google_AssertionCredentials(variable_get('ossap_stats_service_gmail'), array($scope),
	                          $result));
							  
    $client->setClientId(variable_get('ossap_stats_client_id'));
    $client->setAccessType($access_type);
	$client->getAccessToken();
	
    //create a new instance of google_analyticsservice
    $service = new Google_AnalyticsService($client);
    //set optional parameters like sort, dimensions etc
	return $service;
	
	}	
	
/* 
* os_design_output
* parameters = $type which could be a pageview or visits
* taps into google analytics and retrieves the results for 
* most frequent page views and page visited
* returns the pageviews and visit object
*/
function ossap_stats_design_output($type){
    //variables definition block starts
    $output ='';
	$app_name = 'OS Board';
	$metrics_pageviews = 'ga:pageviews';
    $metrics_visits = 'ga:visits';
    $sort_pageviews = '-ga:pageviews';
    $sort_visits = '-ga:visits';
    $dimensions = 'ga:hostname';
	$start_date = '2014-04-20' ;
	$end_date = '2014-04-25'  ;
	
    $max_results = 5;
    //get credentials 
     $service = ossap_stats_check_credentials();
    if ($type == 'visits'){
        $sort = $sort_visits;
        $metrics = $metrics_visits;
     }
    else {
        $sort = $sort_pageviews;
        $metrics = $metrics_pageviews;
    }

    //set optional parameters like sort, dimensions etc
    $optparams = array ('dimensions' => $dimensions,'sort' => $sort,'max-results' => $max_results);
    //get result (analytics object
    $results = $service->data_ga->get(variable_get('ossap_stats_analytics_id'), $start_date, $end_date, $metrics, $optparams);
    //create the header for the table
    return $results;
}

/* 
* return_users
* no parameters 
* taps into google analytics and retrieves the results for 
* users that are currently active
*/
function ossap_stats_return_users(){
        // declare variables
        $dimensions = 'rt:source';
		$rt_users = 'rt:activeUsers';
		
		//validate the credentials
        $service = ossap_stats_check_credentials();
		$optparams = array ('dimensions' => $dimensions);
		
		//get result 
		$results = $service->data_realtime->get(variable_get('ossap_stats_analytics_id'), $rt_users);
		
		//return the results
		return $results;	
}

/* 
* return_pages
* no parameters 
* taps into google analytics and retrieves the results for 
* pages that are currently active
*/
function ossap_stats_return_pages(){
        // declare variables
        $dimensions = 'rt:pagePath,rt:pageTitle';
		$sort = '-rt:pageviews';
		$max_results = 5;
		$page_views = 'rt:pageviews';
		
		//validate the credentials
	    $service = ossap_stats_check_credentials();
        $optparams = array ('dimensions' => $dimensions , 'sort' =>  $sort, 'max-results' => $max_results );
		
		//get result 
		$results = $service->data_realtime->get(variable_get('ossap_stats_analytics_id'),$page_views, $optparams);
		
		//return the results
        return $results;
}