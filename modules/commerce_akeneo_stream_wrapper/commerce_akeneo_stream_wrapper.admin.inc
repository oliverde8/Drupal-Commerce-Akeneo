<?php

/**
 * @file
 * Module file for Commerce Akeneo module.
 */

/**
 * Implements hook_form_FORMID_alter().
 */
function commerce_akeneo_stream_wrapper_form_commerce_akeneo_settings_form_alter(&$form, &$form_state, $form_id) {

  $form['commerce_akeneo_stream_wrapper'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Stream Wrapper'),
    '#description' => t(
      'The Stream Wrapper adds support for remote medias using the following pattern: @pattern',
      array('@pattern' => 'akeneo://domainname.tld/path/file.ext')
    ),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );

  $form['commerce_akeneo_stream_wrapper']['commerce_akeneo_stream_wrapper_base_url'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Akeneo base URL'),
    '#description'   => t(
      'Indicate the Akeneo base URL which allows Drupal to retrieve media, etc... Must start by "http" or "https".'
    ),
    '#default_value' => variable_get(
      'commerce_akeneo_stream_wrapper_base_url',
      NULL
    ),
  );

  $form['commerce_akeneo_stream_wrapper']['commerce_akeneo_stream_wrapper_insecure'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Allow insecure connection'),
    '#description'   => t('HTTPS only.'),
    '#size'          => 5,
    '#default_value' => variable_get('commerce_akeneo_stream_wrapper_insecure', FALSE),
  );

  $form['commerce_akeneo_stream_wrapper']['commerce_akeneo_stream_wrapper_timeout'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Connexion timeout'),
    '#description'   => t('duration in seconds'),
    '#size'          => 5,
    '#default_value' => variable_get(
      'commerce_akeneo_stream_wrapper_timeout',
      COMMERCE_AKENEO_STREAM_WRAPPER_DEFAULT_TIMEOUT
    ),
  );

  $form['#validate'][] = 'commerce_akeneo_stream_wrapper_settings_form_validate';
}

/**
 * Implements hook_form_validate().
 */
function commerce_akeneo_stream_wrapper_settings_form_validate(&$form, &$form_state) {
  $timeout = $form_state['values']['commerce_akeneo_stream_wrapper_timeout'];

  // Check if timeout is integer and different than zero.
  if (!preg_match('/^[0-9]*$/', $timeout) || empty($timeout)) {
    form_error(
      $form['commerce_akeneo_stream_wrapper']['commerce_akeneo_stream_wrapper_timeout'],
      t('Duration must be a numeric and positive value.')
    );
  }
}
