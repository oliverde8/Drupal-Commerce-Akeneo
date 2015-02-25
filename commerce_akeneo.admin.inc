<?php

/**
 * @file
 * Module file for Commerce Akeneo module.
 */

/**
 * Menu callback for config page.
 */
function commerce_akeneo_settings_page() {
  $form = drupal_get_form('commerce_akeneo_settings_form');
  return drupal_render($form);
}

/**
 * Implements hook_form().
 */
function commerce_akeneo_settings_form($form, &$form_state) {

  $form['commerce_akeneo_machine_name'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Machine names'),
    '#description' => t("Prefix added to code can't exceed 32 chars."),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_product'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Product type prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_product', ''),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_field'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Field prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_field', ''),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_category'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Category prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_category', ''),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_option'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Option prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_option', ''),
  );

  $form['#validate'][] = 'commerce_akeneo_settings_form_validate';

  return system_settings_form($form);
}

/**
 * Implements hook_form_validate().
 */
function commerce_akeneo_settings_form_validate(&$form, &$form_state) {
  if (!preg_match('/^[a-z0-9\_]*$/', $form_state['values']['commerce_akeneo_prefix_product'])) {
    form_error(
      $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_product'],
      t('The product type prefix must contain only lowercase letters, numbers, and underscores.')
    );
  }

  if (!preg_match('/^[a-z0-9\_]*$/', $form_state['values']['commerce_akeneo_prefix_field'])) {
    form_error(
      $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_field'],
      t('The field prefix must contain only lowercase letters, numbers, and underscores.')
    );
  }

  if (!preg_match('/^[a-z0-9\_]*$/', $form_state['values']['commerce_akeneo_prefix_category'])) {
    form_error(
      $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_category'],
      t('The category prefix must contain only lowercase letters, numbers, and underscores.')
    );
  }

  if (!preg_match('/^[a-z0-9\_]*$/', $form_state['values']['commerce_akeneo_prefix_option'])) {
    form_error(
      $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_option'],
      t('The option prefix must contain only lowercase letters, numbers, and underscores.')
    );
  }
}
