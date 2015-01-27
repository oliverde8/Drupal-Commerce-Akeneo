<?php

/**
 * @file
 * Module file for Commerce Akeneo module.
 */

/**
 * Implements hook_form_FORMID_alter().
 */
function commerce_akeneo_migrate_form_commerce_akeneo_settings_form_alter(&$form, &$form_state, $form_id) {

  $form['commerce_akeneo_migrate'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Migrate'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );

  $form['commerce_akeneo_migrate']['commerce_akeneo_migrate_lease_time'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Lease time'),
    '#default_value' => variable_get('commerce_akeneo_migrate_lease_time', COMMERCE_AKENEO_MIGRATE_DEFAULT_LEASE_TIME),
    '#description'   => t('Time in second during which queue items are locked.'),
  );

  $form['#validate'][] = 'commerce_akeneo_migrate_settings_form_validate';
}

/**
 * Implements hook_form_validate().
 */
function commerce_akeneo_migrate_settings_form_validate(&$form, &$form_state) {
  $timeout = $form_state['values']['commerce_akeneo_migrate_lease_time'];

  // Check if timeout is integer and different than zero.
  if (!preg_match('/^[0-9]*$/', $timeout) || empty($timeout)) {
    form_error(
      $form['commerce_akeneo_migrate']['commerce_akeneo_migrate_lease_time'],
      t('Duration must be a numeric and positive value.')
    );
  }
}
