<?php

/**
 * @file
 * Form functions for registering sites across installs
 */
function ossap_register_site_register_form($form, $form_state) {
  $groups = og_get_all_group_bundle();
  dpm($groups);

  $form['site_type'] = array(
    '#type' => 'select',
    '#title' => t('Type of Site'),
    '#options' => array(
      '' => t('Select type of Site'),
      'personal' => t('Personal'),
      'project' => t('Project'),
      'department' => t('Department'),
    )
  );

  $presets = spaces_preset_load();
  dpm($presets);

  $form['preset'] = array(
    '' => t('Select preset'),
    'personal' => t('Personal'),
    'project' => t('Project'),
    'department_standard' => t('Department Default'),
    'department_minimal' => t('Department Minimal'),
  );

  return $form;
}
