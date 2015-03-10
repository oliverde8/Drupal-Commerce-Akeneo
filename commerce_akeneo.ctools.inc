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
 * Load attribute plugin instance.
 *
 * @param string $type
 *   Type.
 *
 * @return CommerceAkeneoAttributeInterface
 *   Plugin instance.
 */
function commerce_akeneo_get_attribute_plugin($type) {
  $attribute_plugins = &drupal_static(__METHOD__, array());

  if (!isset($attribute_plugins[$type])) {
    ctools_include('context');
    ctools_include('plugins');

    if ($class_name = ctools_plugin_load_class('commerce_akeneo', 'attributes', $type, 'class_name')) {
      $attribute_plugins[$type] = new $class_name($type);
    }
  }

  return $attribute_plugins[$type];
}

/**
 * Load field plugin instance.
 *
 * @param string $type
 *   Type.
 * @param string $entity_type
 *   Entity type.
 * @param string $bundle
 *   Bundle.
 * @param string $field_name
 *   Field name.
 *
 * @return CommerceAkeneoFieldInterface|bool
 *   Plugin instance.
 */
function commerce_akeneo_get_field_plugin($type, $entity_type, $bundle, $field_name) {
  ctools_include('context');
  ctools_include('plugins');

  if ($class_name = ctools_plugin_load_class('commerce_akeneo', 'fields', $type, 'class_name')) {
    return new $class_name($entity_type, $bundle, $field_name);
  }

  return FALSE;
}
