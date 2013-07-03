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

  $form['actions'] = array(
    '#type' => 'actions',
    'create' => array(
      '#type' => 'submit',
      '#value' => t('Create Your Site'),
      '#attributes' => array(
        //'class' => array('use-ajax-submit')
      )
    )
  );

  return $form;
}

function ossap_register_site_register_form_submit($form, &$form_state) {
}
