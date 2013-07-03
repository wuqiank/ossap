<?php

/**
 * Helper function to issue a HTTP request with cURL.
 *
 * @param array $body
 *   Either the body for POST and PUT or additional URL parameters for GET.
 */
function ossap_httpRequest($url, $method, $user_pass = NULL, $body = NULL, $format = 'json') {

  switch ($method) {
  	case 'GET':
  	  // Set query if there are addition GET parameters.
  	  $options = isset($body) ? array('absolute' => TRUE, 'query' => $body) : array('absolute' => TRUE);
  	  return ossap_curlExec(array(
  	    CURLOPT_HTTPGET => TRUE,
  	    CURLOPT_URL => url($url, $options),
  	    CURLOPT_NOBODY => FALSE)
  	  );
  	case 'POST':
  	  return ossap_curlExec(array(
  	  CURLOPT_HTTPGET => FALSE,
  	  CURLOPT_USERPWD => $user_pass,
  	  CURLOPT_POST => TRUE,
  	  CURLOPT_POSTFIELDS => $body,
  	  CURLOPT_URL => url($url, array('absolute' => TRUE)),
  	  CURLOPT_NOBODY => FALSE,
  	  CURLOPT_HTTPHEADER => array('Content-Type: application/' . $format),
  	  ));
  	case 'PUT':
  	  return ossap_curlExec(array(
  	  CURLOPT_HTTPGET => FALSE,
  	  CURLOPT_USERPWD => $user_pass,
  	  CURLOPT_CUSTOMREQUEST => 'PUT',
  	  CURLOPT_POSTFIELDS => $body,
  	  CURLOPT_URL => url($url, array('absolute' => TRUE)),
  	  CURLOPT_NOBODY => FALSE,
  	  CURLOPT_HTTPHEADER => array('Content-Type: application/' . $format),
  	  ));
  	case 'DELETE':
  	  return ossap_curlExec(array(
  	  CURLOPT_HTTPGET => FALSE,
  	  CURLOPT_USERPWD => $user_pass,
  	  CURLOPT_CUSTOMREQUEST => 'DELETE',
  	  CURLOPT_URL => url($url, array('absolute' => TRUE)),
  	  CURLOPT_NOBODY => FALSE,
  	  ));
  }
}

/**
 * Initializes and executes a cURL request.
 *
 * @param $curl_options
 *   An associative array of cURL options to set, where the keys are constants
 *   defined by the cURL library. For a list of valid options, see
 *   http://www.php.net/manual/function.curl-setopt.php
 * @param $redirect
 *   FALSE if this is an initial request, TRUE if this request is the result
 *   of a redirect.
 *
 * @return
 *   The content returned from the call to curl_exec().
 *
 * @see curlInitialize()
 */
function ossap_curlExec($curl_options, $redirect = FALSE) {

  $ch = curl_init($url);

  $default_curl_options = array(
    CURLOPT_COOKIEJAR => '/tmp/cookie.txt',
    CURLOPT_FOLLOWLOCATION => FALSE,
    CURLOPT_RETURNTRANSFER => TRUE,
    CURLOPT_SSL_VERIFYPEER => FALSE, // Required to make the tests run on HTTPS.
    CURLOPT_SSL_VERIFYHOST => FALSE, // Required to make the tests run on HTTPS.
  );

  // cURL incorrectly handles URLs with a fragment by including the
  // fragment in the request to the server, causing some web servers
  // to reject the request citing "400 - Bad Request". To prevent
  // this, we strip the fragment from the request.
  // TODO: Remove this for Drupal 8, since fixed in curl 7.20.0.
  if (!empty($curl_options[CURLOPT_URL]) && strpos($curl_options[CURLOPT_URL], '#')) {
    $original_url = $curl_options[CURLOPT_URL];
    $curl_options[CURLOPT_URL] = strtok($curl_options[CURLOPT_URL], '#');
  }

  $url = empty($curl_options[CURLOPT_URL]) ? curl_getinfo($ch, CURLINFO_EFFECTIVE_URL) : $curl_options[CURLOPT_URL];

  if (!empty($curl_options[CURLOPT_POST])) {
    // This is a fix for the Curl library to prevent Expect: 100-continue
    // headers in POST requests, that may cause unexpected HTTP response
    // codes from some webservers (like lighttpd that returns a 417 error
    // code). It is done by setting an empty "Expect" header field that is
    // not overwritten by Curl.
    $curl_options[CURLOPT_HTTPHEADER][] = 'Expect:';
  }
  curl_setopt_array($ch, $default_curl_options + $curl_options);

  $content = curl_exec($ch);
  curl_close($ch);

  return $content;
}