<?php

/**
 * @file
 * API file for Commerce Akeneo module.
 */

/**
 * Implements hook_commerce_akeneo_attribute_AKENEO_TYPE().
 */
function hook_commerce_akeneo_attribute_AKENEO_TYPE($code, $labels = array(), $settings = array()) {
}

/**
 * Implements hook_commerce_akeneo_field_alter().
 */
function hook_commerce_akeneo_field_alter(&$field, $settings, $request) {
}

/**
 * Implements hook_commerce_akeneo_field_instance().
 */
function hook_commerce_akeneo_field_instance_alter(&$field_instance, $settings, $request) {
}

/**
 * Implements hook_commerce_akeneo_locale_to_language_alter().
 */
function hook_commerce_akeneo_locale_to_language_alter(&$language, $locale) {
  switch ($locale) {
    case 'fr_CA':
    case 'fr_FR':
      $language = 'fr-FR';
      break;

    case 'en_GB':
    case 'en_US':
      $language = 'en-US';
      break;
  }
}
