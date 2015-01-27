<?php

/**
 * @file
 * API file for Commerce Akeneo module.
 */

/**
 * Implements hook_commerce_akeneo_family_alter().
 */
//function hook_commerce_akeneo_family_alter(&$product_type, $request) {
//  // Allow overriding drupal commerce product type before saving into database.
//}

/**
 * Implements hook_commerce_akeneo_family().
 */
function hook_commerce_akeneo_family($family, $request) {
  // Allow to process custom code after commerce product type is saved.
}

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
}
