<?php

/**
 * @file
 * CTools file for Commerce Akeneo module.
 */

/**
 * Implements hook_ctools_plugin_type().
 */
function commerce_akeneo_ctools_plugin_type() {
  // Used to declare services.
  $plugins['services'] = array();
  // Used to synchronize families.
  $plugins['attributes'] = array();
  // Used to migrate products.
  $plugins['fields'] = array();

  return $plugins;
}

/**
 * @return array
 */
function commerce_akeneo_get_attributes() {
  ctools_include('context');
  ctools_include('plugins');

  return ctools_get_plugins('commerce_akeneo', 'attributes');
}

/**
 * @param string $name
 *
 * @return mixed
 */
function commerce_akeneo_get_attribute($name) {
  ctools_include('context');
  ctools_include('plugins');

  return ctools_get_plugins('commerce_akeneo', 'attributes', $name);
}

/**
 * @param string $type
 * @param string $entity_type
 * @param string $bundle
 * @param string $field_name
 *
 * @return CommerceAkeneoFieldInterface|bool
 */
function commerce_akeneo_field_get_plugin($type, $entity_type, $bundle, $field_name) {
  ctools_include('context');
  ctools_include('plugins');

  if ($class_name = ctools_plugin_load_class('commerce_akeneo', 'fields', $type, 'class_name')) {
    return new $class_name($entity_type, $bundle, $field_name);
  }

  return FALSE;
}
