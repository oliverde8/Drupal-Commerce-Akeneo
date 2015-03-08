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
 * Load attribute plugin.
 *
 * @param string $type
 *   Type.
 *
 * @return CommerceAkeneoAttributeInterface
 *   Plugin class.
 */
function commerce_akeneo_get_attribute_plugin($type) {
  $attributePlugins = &drupal_static(__METHOD__, array());

  if (!isset($attributePlugins[$type])) {
    ctools_include('context');
    ctools_include('plugins');

    if ($class_name = ctools_plugin_load_class('commerce_akeneo', 'attributes', $type, 'class_name')) {
      $attributePlugins[$type] = new $class_name($type);
    }
  }

  return $attributePlugins[$type];
}

/**
 * @param string $type
 * @param string $entity_type
 * @param string $bundle
 * @param string $field_name
 *
 * @return CommerceAkeneoFieldInterface|bool
 */
function commerce_akeneo_get_field_plugin($type, $entity_type, $bundle, $field_name) {
  ctools_include('context');
  ctools_include('plugins');

  if ($class_name = ctools_plugin_load_class('commerce_akeneo', 'fields', $type, 'class_name')) {
    return new $class_name($entity_type, $bundle, $field_name);
  }

  return FALSE;
}
