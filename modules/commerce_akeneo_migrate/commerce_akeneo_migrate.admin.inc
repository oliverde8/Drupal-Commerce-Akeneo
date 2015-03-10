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

  $form['commerce_akeneo_migrate']['product_display'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Product Display'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );

  $form['commerce_akeneo_migrate']['product_display']['commerce_akeneo_migrate_product_display_import_group'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Import the products being part of a group'),
    '#default_value' => variable_get('commerce_akeneo_migrate_product_display_import_group', FALSE),
    '#description'   => t('Usually products part of group are only displayed on group.'),
  );

  $form['commerce_akeneo_migrate']['product_display']['commerce_akeneo_migrate_product_display_bundle_auto'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Automatic content type mapping'),
    '#default_value' => variable_get('commerce_akeneo_migrate_product_display_bundle_auto', ''),
    '#description'   => t(
      '"Automatic" will try to map product types with content types using same machine name. If not found, the default content type will be used.'
    ),
  );

  if (!$nodes = node_type_get_names()) {
    $options = array(
      '' => t('- Missing -'),
    );
  }
  else {
    $options = array('' => t('- None -'));
    $options += $nodes;
  }

  $form['commerce_akeneo_migrate']['product_display']['commerce_akeneo_migrate_product_display_bundle'] = array(
    '#type'          => 'select',
    '#title'         => t('Default content type'),
    '#default_value' => variable_get('commerce_akeneo_migrate_product_display_bundle', ''),
    '#options'       => $options,
    '#description'   => t('Define the default content type used to create/update product display.'),
  );

  $form['commerce_akeneo_migrate']['product_display']['commerce_akeneo_migrate_product_display_field_auto'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Auto detect destination field'),
    '#default_value' => variable_get('commerce_akeneo_migrate_product_display_field_auto', FALSE),
    '#description'   => t(
      '"Automatic" will detect the field based on it\'s type (commerce_product_reference). If not found, or more than one please choose the exact field below.'
    ),
  );

  $fields = field_info_fields();
  foreach ($fields as $field_name => $field) {
    if ($field['type'] == 'commerce_product_reference' && !empty($field['bundles']['node'])) {
      $fields[$field_name] = $field_name;
    }
    else {
      unset($fields[$field_name]);
    }
  }

  if (!$fields) {
    $options = array(
      '' => t('- Missing -'),
    );
  }
  else {
    asort($fields);
    $options = array('' => t('- None -'));
    $options += $fields;
  }

  $form['commerce_akeneo_migrate']['product_display']['commerce_akeneo_migrate_product_display_field'] = array(
    '#type'          => 'select',
    '#title'         => t('Default destination field'),
    '#default_value' => variable_get('commerce_akeneo_migrate_product_display_field', ''),
    '#options'       => $options,
    '#description'   => t('Define the field used to store product references.'),
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
