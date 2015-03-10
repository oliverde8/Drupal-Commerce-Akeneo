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
    '#title'       => t('Machine names prefix'),
    '#description' => t("Generally prefix added to code can't exceed 32 chars length."),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_product'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Product type prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_product', ''),
    '#description'   => t('Corresponds to the product type.'),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_field'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Field prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_field', ''),
    '#description'   => t('Corresponds to the default field.'),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_field_category'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Field association prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_field_category', ''),
    '#description'   => t("Corresponds to the category's catalog field."),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_field_association'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Field association prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_field_association', ''),
    '#description'   => t('Corresponds to the product association field.'),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_group'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Group prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_group', ''),
    '#description'   => t('Corresponds to the field group used to create vertical tabs.'),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_category'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Category prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_category', ''),
    '#description'   => t('Corresponds to the category taxonomy vocabulary.'),
  );

  $form['commerce_akeneo_machine_name']['commerce_akeneo_prefix_option'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Option prefix'),
    '#size'          => 20,
    '#default_value' => variable_get('commerce_akeneo_prefix_option', ''),
    '#description'   => t('Corresponds to the default taxonomy vocabulary.'),
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
