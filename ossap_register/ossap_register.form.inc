<?php

/**
 * @file
 * Form functions for registering sites across installs
 */
function ossap_register_site_register_form($form, $form_state) {
  $groups = og_get_all_group_bundle();

  $form['site_type'] = array(
    '#type' => 'select',
    '#title' => t('Type of Site'),
    '#options' => array(
      '' => t('Select type of Site'),
    )+$groups['node']
  );

  $presets = spaces_preset_load();

  $settings = array();
  $options = array();
  foreach ($presets as $type => $d) {
    if (!empty($d->value['bundles'])) {
      foreach ($d->value['bundles'] as $b) {
        $settings[$b][] = $type;
      }
      $options[$type] = $d->title;
    }
  }

  $servers = variable_get('ossap_child_domains', array());
  $domains = array_keys($servers);
  $domains = array_combine($domains, $domains);

  $form['#attached'] = array(
    'library' => array(
      array('system', 'ui.tabs'),
    ),
    'js' => array(
      drupal_get_path('module', 'ossap_register').'/js/ossap_register.form.js',
      array(
        'type' => 'setting',
        'data' => array(
          'ossap' => array(
            'servers' => $servers,
            'preset' => $settings,
          )
        )
      )
    )
  );

  $form['preset'] = array(
    '#type' => 'select',
    '#title' => t('Select Preset'),
    '#options' => array(
      '' => t('Select Preset'),
    )+$options
  );

  $form['domain'] = array(
    '#type' => 'select',
    '#options' => array('' => t('Select Domain')) + $domains,
  );

  $form['purl'] = array(
    '#field_prefix' => '<span id="domain">http://placeholder.com/</span>',
    '#type' => 'textfield',
    '#title' => t('Your URL'),
    '#size' => 30,
  );

  if (module_exists('pinserver') && pinserver_check_status()) {
    $huid = pinserver_get_user_huid();

    $pins = array();
    // get user id
    $derp = 0;
    foreach ($servers as $d => $temp) {
      // get user id from path
      $path = 'http://'.$d.'/something/something';

      $uid = $derp++;
      $pins[$d] = $uid;

    }

    $form['#attached']['js'][1]['data']['ossap']['pins'] = $pins;
  }

  module_load_include('form.inc', 'vsite_register');
  //@todo allow anything other than anon users
  // Anonymous users must always create new accounts.
  $form['tabs'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'user-tabs',
    ),
    'links' => array(
      '#theme' => 'links',
      '#links' => array(
        array(
          'href' => '',
          'external' => TRUE,
          'fragment' => 'create',
          'title' => t('Create User'),
        ),
        array(
          'href' => '',
          'external' => TRUE,
          'fragment' => 'existing',
          'title' => t('Existing User'),
        )
      ),
    ),
    'create' => array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'create',
      ),
      'user' => _vsite_register_user_form()
    ),
    'existing' => array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'existing'
      ),
      'desc' => array(
        '#markup' => t('Login credentials for <span id="target-domain"></span>'),
      ),
      'username' => array(
        '#type' => 'textfield',
        '#title' => t('Username'),
      ),
    ),
    'pin_user' => array(
      '#type' => 'hidden',
      '#default_value' => 0,
      '#attributes' => array(
        'id' => 'pin-user',
      )
    )
  );
  unset($form['tabs']['create']['user']['mail']['#ajax'], $form['tabs']['create']['user']['name']['#ajax']);

  $form['vsite_private'] = array(
    '#type' => 'radios',
    '#title' => t('Site visibility'),
    '#default_value' => VSITE_ACCESS_PUBLIC,
    '#options' => vsite_access_privacy_available_values(),
    '#description' => t('Choose who has access to view your site.'),
    '#access' => module_exists('vsite_access'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create your site'),
    '#weight' => 40,
    '#prefix' => '<div id="form-errors"></div>',
    '#suffix' => '<div id="submit-suffix"></div>',
    '#attributes' => array('class' => array('use-ajax-submit')),
    '#ajax' => array('callback' => 'ossap_register_site_register_form_submit'),
  );

  $form['#validate'][] = 'ossap_register_site_register_form_user_validate';

  return $form;
}

/**
 * Validates that a user entered with username and password exist on the target server
 */
function ossap_register_site_register_form_user_validate($form, &$form_state) {
  $values = &$form_state['values'];
  // they're trying to use an existing account
  if (isset($values['username'])) {
    $uid = ossap_user_exists($values['domain'], $values['username']);
     if (!$uid) {
       form_set_error($form['tabs']['existing'], t('Unrecognized username or email on %domain. Please reenter and try again.', array('%domain' => $values['domain'])));
     }
    else {
      $values['uid'] = $uid;
    }
  }

  return true;
}

/**
 * Form AJAX submit callback; attempts vsite register and sends messages.
 *
 * Creates user and vsite when site/register form is submitted
 */
function ossap_register_site_register_form_submit(&$form, &$state) {
  module_load_include('form.inc', 'vsite_register');

  $values = $state['values'];

  /**
   * @todo Change to be the purl_base_domain of the server
   */
  $server = $values['domain'];

  //Holds true if we are creating a new user.
  $new_user = false;

  if ($values['pin_user']) {
    $site_owner = new stdClass();
    $site_owner->uid = $values['pin_user'];
    $uli = "USER_LOGIN_LINK";
  }
  // If the specified user account already exists...
  elseif (isset($values['uid']) && $values['uid']) {
    $site_owner = new stdClass();
    $site_owner->uid = $values['uid'];
    $uli = "USER_LOGIN_LINK";
  }
  elseif (($values['vicarious_user'] && !$values['existing_username']) || (!$values['vicarious_user'] && $values['name'])) {
    // Create user for current logged in user or on someone else's behalf.
    $user_options = array(
      'name' => $values['name'],
      'pass' => $values['password'],
      'mail' => $values['mail'],
      'status' => 1,
      'field_first_name' => $values['first_name'],
      'field_last_name' => $values['last_name'],
    );

    $site_owner = ossap_user_create($server, $user_options);
		if(!isset($site_owner->id)){
		  watchdog('ossap_site_register',"Unable to create user account via callback.");
		  drupal_set_message("Unable to create user account, support has been notified. Please contact customer support for assistance.",'error');
		}else{
		  $new_user = true;
		  //Create user object
		  $site_owner->uid = $site_owner->id;
		  $site_owner->name = $user_options['name'];
		}

    // Logs in as the new user, if we're not already logged in.
    $user_token = "USER_LOGIN_TOKEN";
  }
  else {
    /**
     * @todo Allow Authenticated users to create site for existing users, or as Current user
     */
    // Creates site for current logged in user. No need to create a new user.
    //global $user;
    //$site_owner = $user;
    watchdog('ossap_site_register',"Unable to create account for logged in Users.");
  }
  dpm($site_owner);

  if($site_owner->uid){
	  // Creates the vsite node.
	  $name = $purl = $values['purl'];
	  $author = $site_owner->uid;
	  $bundle = $values['site_type'];
	  $preset = $values['preset'];
	  $parent = $_POST['parent'] ? $_POST['parent'] : FALSE;
	  $visibility = isset($values['vsite_private']) ? $values['vsite_private'] : FALSE;

	  $vsite = ossap_create_vsite($server ,$name, $purl, $author, $bundle, $preset, $parent, $visibility);
    dpm($vsite);
	  if(!$vsite){
	    drupal_set_message("Unable to create site, support has been notified.  Please contact customer support for assistance.",'error');
	  }
  }
  else {
    $commands[] = ajax_command_replace('#submit-suffix', 'No user found to make site owner.');
  }

  if ($vsite) {

    $now = time();
    $login_destination = "site/login/once/{$vsite->nid}/{$site_owner->uid}/{$now}/{$user_token}";

    $message = ossap_vsite_register_message($form, $values['domain'], $values['purl'], $login_destination, $new_user, $values['vicarious_user']);
    $commands[] = ajax_command_replace('#submit-suffix', $message);
    $commands[] = ajax_command_remove('#edit-submit');

    // If we have gotten to this point, then the vsite registration was success.
    // Clears the errors.
    drupal_get_messages('error');
  }
  else {
    $commands[] = _vsite_register_form_error();
  }

  print ajax_render($commands);
  exit;
}
