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

  $form['vsite_private'] = array(
    '#type' => 'radios',
    '#title' => t('Site visibility'),
    '#default_value' => VSITE_ACCESS_PUBLIC,
    '#options' => vsite_access_privacy_available_values(),
    '#description' => t('Choose who has access to view your site.'),
    '#access' => module_exists('vsite_access'),
  );


  if (module_exists('pinserver') && pinserver_check_status()) {
    $huid = pinserver_get_user_huid();

    $user_settings = array(
      'type' => 'setting',
      'data' => array(
        'ossap' => array(
          'pins' => array()
        )
      )
    );
    // get user id
    foreach ($servers as $d => $temp) {
      // get user id from path
      $path = 'http://'.$d.'/something/something';

      $uid = 0;
      $user_settings['data']['ossap']['pins'][$d] = $uid;

    }

    $form['#attached']['js'][] = $user_settings;
  }

  module_load_include('form.inc', 'vsite_register');
  //@todo allow anything other than anon users
  if (TRUE || !user_is_logged_in()) {
    // Anonymous users must always create new accounts.
    $form += _vsite_register_user_form();
    $form['vicarious_user'] = array(
      '#type' => 'hidden',
      '#value' => FALSE,
    );
    // Prepares flag for JS to set when create new user is clicked.
    $form['user_fieldset']['create_new_user'] = array(
      '#type' => 'hidden',
      '#default_value' => TRUE,
    );

    unset($form['mail']['#ajax'], $form['name']['#ajax']);
  }
  elseif (user_access('create vsite content on behalf') && user_access('administer users')) {
    // Managers may:
    // - create a new site,
    // - give a site to another user, or
    // - give a site to a new account.
    $form['vicarious_user'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create this OS Site on behalf of another user.'),
    );

    $form['user_fieldset'] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#states' => array('visible' => array(':input[name="vicarious_user"]' => array('checked' => TRUE))),
    );

    // $ajax_link = l(t('create a new user'), 'javascript:void(0)', array('attributes'=>array('id'=>array('new-user-link'))));
    // @todo figure out l() later
    $ajax_link = '<a href="javascript:void(0)" id="new-user-link">' . t('create a new user') . '</a>';

    $form['user_fieldset']['existing_username'] = array(
      '#type' => 'textfield',
      '#title' => t('Find Existing User'),
      '#description' => t('Enter the name of the user who will manage this new site or !create_new.',
        array('!create_new' => $ajax_link)
      ),
      '#autocomplete_path' => 'vsite/autocomplete/user',
    );

    // Prepares flag for JS to set when create new user is clicked.
    $form['user_fieldset']['create_new_user'] = array(
      '#type' => 'hidden',
      '#default_value' => FALSE,
    );

    $form['user_fieldset']['new_user_div'] = array(
      '#markup' => '<div id="new_user_div">',
    );

    $form['user_fieldset'] += _vsite_register_user_form();

    $form['user_fieldset']['huid'] = array();

    $form['user_fieldset']['new_user_close_div'] = array(
      '#markup' => '</div>',
    );
  }
  else {
    // Regular users may only create sites for themselves.
    $form['vicarious_user'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create your site'),
    '#weight' => 40,
    '#prefix' => '<div id="form-errors"></div>',
    '#suffix' => '<div id="submit-suffix"></div>',
    '#attributes' => array('class' => array('use-ajax-submit')),
    '#ajax' => array('callback' => 'ossap_register_site_register_form_submit'),
  );

  return $form;
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

  // If the specified user account already exists...
  if ($values['vicarious_user'] && $values['existing_username']) {
    // Loads that user account as site owner.
    /**
     * @todo Allow users to create site for existing users
     */
    //$site_owner = user_load_by_name($values['existing_username']);
    watchdog('ossap_site_register',"Unable to create account for existing users.");
    return FALSE;
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

  if($site_owner->uid){
	  // Creates the vsite node.
	  $name = $purl = $values['purl'];
	  $author = $site_owner->uid;
	  $bundle = $values['site_type'];
	  $preset = $values['preset'];
	  $parent = $_POST['parent'] ? $_POST['parent'] : FALSE;
	  $visibility = isset($values['vsite_private']) ? $values['vsite_private'] : FALSE;

	  $vsite = ossap_create_vsite($server ,$name, $purl, $author, $bundle, $preset, $parent, $visibility);
	  if(!$vsite){
	    drupal_set_message("Unable to create site, support has been notified.  Please contact customer support for assistance.",'error');
	  }
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
