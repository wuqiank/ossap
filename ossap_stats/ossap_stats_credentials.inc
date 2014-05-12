<?php

/**
 * @file of type inc for credentials
 * Statistics for OpenScholar Single Access Point (OSSAP).
 */

/* 
* ossap_stats_check_credentials()
* authenticate credentials by creating a new Google_client object and 
* sending it to google analytics
* ossap_stats_check_credentials
* no parameters
* creates the google_client object
* returns the validated service object
*/

function ossap_stats_check_credentials(){
    //declare and set the variables 
    $output ='';
    $access_type = 'offline_access';
    $scope = 'https://www.googleapis.com/auth/analytics.readonly';
    $service_gmail = variable_get('ossap_stats_service_gmail');
    $client_id = variable_get('ossap_stats_client_id');
    $fid = variable_get('os_board_private_key_fid', '');
	
    //create a new instance of Google_client
    $client = new Google_Client();
    // set the application name
    $client->setApplicationName('OSSAP Stats');   
   
    //check if private_key is uploaded and read from it 
    if (!empty($fid)) {
      $uri = db_query('SELECT uri FROM {file_managed} WHERE fid = :fid', array(':fid' => $fid))->fetchColumn();
      $fhandle = fopen($uri, 'r');
      $result = fread($fhandle, filesize($uri));
      fclose($fhandle);
    }
    
    //create the credentials object to be sent to google analytics
    $client->setAssertionCredentials(new Google_AssertionCredentials($service_gmail, array($scope), $result));                              
    $client->setClientId($client_id);
    $client->setAccessType($access_type);
    $client->getAccessToken();
    
    //create a new instance of google_analyticsservice and send the credentials object
    $service = new Google_AnalyticsService($client);
    
    //return the object returned by google analytics
    return $service;    
    }
    
/* 
* ossap_stats_design_output
* parameters = $type which could be a pageview or visits
* taps into google analytics and retrieves the results for 
* most frequent page views and page visited
* returns the pageviews and visit object
*/
function ossap_stats_design_output($type){
    //declare and set the variables 
    $output ='';
    $app_name = 'OSSAP Statistics';
    $metrics_pageviews = 'ga:pageviews';
    $metrics_visits = 'ga:visits';
    $sort_pageviews = '-ga:pageviews';
    $sort_visits = '-ga:visits';
    $analytics_id = variable_get('ossap_stats_analytics_id');
    //changed date to auto go back 1 day.
    $start_date = $end_date = date('Y-m-d', strtotime('- 1 day'));
	
    //get credentials 
    $service = ossap_stats_check_credentials();
	 
    //set values depending on the type of the parameter
    if ($type == 'visits'){
        $sort = $sort_visits;
        $metrics = $metrics_visits;
        $dimensions = 'ga:hostname';
        $max_results = 6;
     }
    else {
        $sort = $sort_pageviews;
        $metrics = $metrics_pageviews;
        $dimensions = 'ga:hostname,ga:pagePath';
        $max_results = 5;
    }

    //set optional parameters like sort, dimensions etc
    $optparams = array ('dimensions' => $dimensions,'sort' => $sort,'max-results' => $max_results);
    //get results from the service object
    $results = $service->data_ga->get($analytics_id, $start_date, $end_date, $metrics, $optparams);
    
    //return the results
    return $results;
}

/* 
* ossap_stats_return_users()
* no parameters 
* taps into google analytics and retrieves the results for 
* users that are currently active
*/
function ossap_stats_return_users(){
        //declare variables
        $dimensions = 'rt:source';
        $rt_users = 'rt:activeUsers';
	$analytics_id = variable_get('ossap_stats_analytics_id');
        
        //validate the credentials
        $service = ossap_stats_check_credentials();
	//set the optional parameters
        $optparams = array ('dimensions' => $dimensions);
        
        //get result 
        $results = $service->data_realtime->get($analytics_id, $rt_users);
        
        //return the results
        return $results;
}

/* 
* ossap_stats_return_pages()
* no parameters 
* taps into google analytics and retrieves the results for 
* pages that are currently active
*/
function ossap_stats_return_pages(){
        //declare variables
        $dimensions = 'rt:pagePath,rt:pageTitle';
        $sort = '-rt:pageviews';
        $max_results = 5;
        $page_views = 'rt:pageviews';
        $analytics_id = variable_get('ossap_stats_analytics_id')
		
        //validate the credentials
        $service = ossap_stats_check_credentials();
	//set the optional parameters
        $optparams = array ('dimensions' => $dimensions , 'sort' =>  $sort, 'max-results' => $max_results );
        
        //get result 
        $results = $service->data_realtime->get($analytics_id,$page_views, $optparams);
        
        //return the results
        return $results;
}
