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

  module_load_include('form.inc', 'vsite_register');
  if (!user_is_logged_in()) {
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
  ctools_include('user', 'os');
  ctools_include('vsite', 'vsite');
  $values = $state['values'];

  // If the specified user account already exists...
  if ($values['vicarious_user'] && $values['existing_username']) {
    // Loads that user account as site owner.
    /**
     * @todo Allow users to create site for existing users
     */
    //$site_owner = user_load_by_name($values['existing_username']);
    error_log("Unable to create account for existing users.");
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
    $site_owner = os_user_create($user_options);

    // Send out an email to notify the user of the newly created account.
    // Email only sent if user requested to be notified of new account.
    // if ($values['notify']) {
    //  _user_mail_notify('register_no_approval_required', $site_owner);
    // }

    // Logs in as the new user, if we're not already logged in.
    $uli = "USER_LOGIN_LINK";
  }
  else {
    /**
     * @todo Allow Authenticated users to create site for existing users, or as Current user
     */
    // Creates site for current logged in user. No need to create a new user.
    //global $user;
    //$site_owner = $user;
    error_log("Unable to create account for logged in Users.");
    return FALSE;
  }

  // Creates the vsite node.
  $name = $purl = $values['domain'];
  $author = $site_owner->uid;
  $bundle = $values['bundle'];
  $preset = $values['preset'];
  $parent = $_POST['parent'] ? $_POST['parent'] : FALSE;
  $visibility = isset($values['vsite_private']) ? $values['vsite_private'] : FALSE;
  $vsite = vsite_create_vsite($name, $purl, $author, $bundle, $preset, $parent, $visibility);
  if ($vsite) {
    // Adds the user to the new vsite.
    module_load_include('inc', 'vsite', 'includes/user');
    vsite_user_add($site_owner);

    os_role_grant($site_owner->uid, 'vsite admin', $vsite->id);

    $message = vsite_register_message($form, $values['domain']);
    $commands[] = ajax_command_replace('#submit-suffix', $message);
    $commands[] = ajax_command_remove('#edit-submit');

    // Emails user to inform them of new created site, email is always sent.
    // This function may additionally send a second email to a specified
    // administrator's email address.
    // @see vsite_register_admin_form()
    vsite_register_notify($vsite);

    // If we have gotten to this point, then the vsite registration was success.
    // Clears the errors.
    drupal_get_messages('error');
  }
  else {
    $commands[] = _vsite_register_form_error();
  }

  // Calls any other submit functions here.
  foreach ($form['#submit'] as $fnc) {
    if (__FUNCTION__ !== $fnc) {
      call_user_func($fnc, $form, $state);
    }
  }

  // Check for a present queued og_tasks batch.
  $batch =& batch_get();
  if ($vsite && $batch) {
    //Run all the batch commands right now.
    $batch['progressive'] = FALSE;
		batch_process();
  }

  print ajax_render($commands);
  exit;
}
