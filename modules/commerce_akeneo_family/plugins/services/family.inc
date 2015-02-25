<?php

/**
 * @file
 * Plugin file to expose service.
 */

$plugin = array(
  'name'     => 'family',
  'title'    => t('Family'),
  'weight'   => 10,
  'settings' => array(
    'help'     => t('Synchronize a family.'),
    'callback' => 'commerce_akeneo_family_services_family',
  ),
);

/**
 * Handle service request for family synchronize.
 *
 * @param array $request
 *   Request.
 *
 * @return object
 *   Confirmation message.
 */
function commerce_akeneo_family_services_family($request) {
  if (!isset($request['code']) || !is_scalar($request['code'])) {
    return services_error('Missing code.', 406);
  }

  if (!isset($request['labels']) || !is_array($request['labels'])) {
    return services_error('Missing labels.', 406);
  }

  if (!isset($request['attribute_groups']) || !is_array($request['attribute_groups'])) {
    return services_error('Missing attribute groups.', 406);
  }
  $code = $request['code'];
  if (strlen($code) > 25) {
    $code = substr($code, 0, 24) . '_';
    $code = preg_replace('/_$/', '', $code);
  }


  try {
    if (!$family = commerce_akeneo_family_load($code)) {
      $family = array(
        'code' => $code,
      );
    }

    $family['label']    = commerce_akeneo_get_language($request['labels']);
    $family['settings'] = $request;

    // Store family in database for future purpose.
    commerce_akeneo_family_save($family);

    // Create product type.
    $bundle = variable_get('commerce_akeneo_product_type_prefix', '') . $code;

    if ($settings = commerce_product_type_load($bundle)) {
      $settings['name']   = commerce_akeneo_get_language($request['labels']);
      $settings['is_new'] = FALSE;
    }
    else {
      $settings = array(
        'type'         => $bundle,
        'name'         => commerce_akeneo_get_language($request['labels']),
        'description'  => t('Product type created via Akeneo PIM.'),
        'help'         => '',
        'revision'     => 1,
        'multilingual' => 4,
        'is_new'       => TRUE,
      );
    }

    // Set the multilingual value for the product type
    // if entity translation is enabled.
    if (module_exists('entity_translation')) {
      $multilingual = (isset($settings['multilingual']) ? $settings['multilingual'] : 4);
      variable_set('language_product_type_' . $settings['type'], $multilingual);
    }

    if (!commerce_product_ui_product_type_save($settings)) {
      throw new CommerceAkeneoException(t('Unable to create product type.'));
    }

    // Enable title field.
    if (module_exists('title')) {
      // Replace default title property by title field.
      $entity_info = entity_get_info('commerce_product');
      $info        = $entity_info['field replacement']['title'];
      $instance    = field_info_instance('commerce_product', $info['field']['field_name'], $bundle);

      // If not already done.
      if (empty($instance)) {
        title_field_replacement_toggle('commerce_product', $bundle, 'title');
        title_field_replacement_batch_set('commerce_product', $bundle, 'title');
      }
    }

    // Todo: ajouter nom du catalogue dans le field name ?
    $field_name = commerce_akeneo_get_machine_name('categories', 'field');
    $attribute  = array(
      'code'       => 'categories',
      'family'     => $bundle,
      'group_code' => 'categories',
      'type'       => 'pim_catalog_multiselect',
      'label'      => t('Catalog'),
      'labels'     => array(
        LANGUAGE_NONE => t('Catalog'),
      ),
      'parameters' => array(),
    );

    _commerce_akeneo_family_services_family_attribute($bundle, $field_name, $attribute);

    // Handle fields.
    _commerce_akeneo_family_services_family_attributes($code, $bundle, $request['attribute_groups']);

    module_invoke_all('commerce_akeneo_family', $request);

    // Flush multilingual cache.
    field_multilingual_settings_changed();

    // Cache clear for entities because we handle entity structure.
    entity_flush_caches();

    // Cache clear for all info about fields.
    field_cache_clear();

    // All statically defined migrations have been (re)registered.
    if (module_exists('migrate')) {
      migrate_static_registration();
    }
  }
  catch (FieldException $e) {
    watchdog_exception('akeneo', $e);

    return services_error($e->getMessage(), 500);
  }
  catch (CommerceAkeneoException $e) {
    watchdog_exception('akeneo', $e);

    return services_error($e->getMessage(), $e->getCode() ? $e->getCode() : 500);
  }
  catch (Exception $e) {
    watchdog_exception('akeneo', $e);

    return services_error('Internal Server Error', 500);
  }

  return (object) $request;
}

/**
 * @param string $family_code
 * @param string $bundle
 * @param array  $attribute_groups
 *
 * @throws \CommerceAkeneoException
 */
function _commerce_akeneo_family_services_family_attributes($family_code, $bundle, $attribute_groups) {
  // Build full attributes list.
  $attributes = array();

  foreach ($attribute_groups as $group_code => $group_details) {
    if (!empty($group_details['attributes'])) {
      foreach ($group_details['attributes'] as $attribute_code => $attribute_details) {
        // Create label from code if not available.
        $label = commerce_akeneo_get_language($attribute_details['labels']);
        if (empty($label)) {
          $label = drupal_ucfirst(str_replace('_', ' ', $attribute_code));
        }

        $attributes[$attribute_code]               = $attribute_details;
        $attributes[$attribute_code]['code']       = $attribute_code;
        $attributes[$attribute_code]['family']     = $family_code;
        $attributes[$attribute_code]['group_code'] = $group_code;
        $attributes[$attribute_code]['label']      = $label;
        $attributes[$attribute_code]['settings']   = $attribute_details;
      }
    }
  }

  // Contains only mapped attributes / fields.
  $attributes_by_field = commerce_akeneo_attribute_dispatch_by_fields($attributes);

  // Build destination fields.
  $fields = array();

  foreach ($attributes_by_field as $field_name => $field_settings) {
    $attribute = reset($field_settings['attributes']);

    _commerce_akeneo_family_services_family_attribute($bundle, $field_name, $attribute);

    $fields[$attribute['group_code']][] = $field_name;
  }

  if (module_exists('field_group')) {
    _commerce_akeneo_family_services_family_attribute_groups(
      $bundle,
      $attribute_groups,
      $fields
    );

    cache_clear_all('field_groups', 'cache_field');
  }
}

/**
 * @param string $bundle
 * @param string $field_name
 * @param array  $attribute
 *
 * @return array|bool
 */
function commerce_akeneo_attribute_get_settings($bundle, $field_name, $attribute) {
  /** @var CommerceAkeneoAttributeInterface $plugin */
  if ($plugin = commerce_akeneo_attribute_get_plugin($attribute['type'])) {
    $settings = $plugin->getFieldSettings($attribute, $bundle, $field_name, $attribute['label']);

    return $settings;
  }

  return FALSE;
}

/**
 * @param string $type
 *
 * @return CommerceAkeneoAttributeInterface
 */
function commerce_akeneo_attribute_get_plugin($type) {
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
 * @param string $bundle
 * @param string $field_name
 * @param array  $attribute
 *
 * @return array|bool
 *
 * @throws \CommerceAkeneoException
 * @throws \Exception
 * @throws \FieldException
 */
function _commerce_akeneo_family_services_family_attribute($bundle, $field_name, $attribute) {
  // Handle special case like identifier: SKU.
  if (!$field_settings = commerce_akeneo_attribute_get_settings($bundle, $field_name, $attribute)) {
    return FALSE;
  }

  _commerce_akeneo_family_services_family_attribute_save(
    $attribute['code'],
    $attribute['family'],
    $attribute['group_code'],
    $attribute
  );

  // Load field if exists.
  if (!$field = field_info_field($field_name)) {
    // Create field if missing.
    $field = field_create_field($field_settings['field']);
  }
  elseif ($field['type'] != $field_settings['field']['type']) {
    throw new CommerceAkeneoException(
      t(
        'Field type for field "@field" has been changed: "@old" => "@new", which is not supported.',
        array(
          '@field' => $attribute['code'],
          '@old'   => $field['type'],
          '@new'   => $field_settings['field']['type'],
        )
      ),
      406
    );
  }
  else {
    // Update field properties.
    $field['translatable'] = $field_settings['field']['translatable'];

    field_update_field($field);
  }

  // Create field instance if missing.
  $field_instance = field_read_instance('commerce_product', $field_name, $bundle);

  if (!$field_instance) {
    $field_instance = field_create_instance($field_settings['field_instance']);
  }
  else {
    $field_instance['label']       = $field_settings['field_instance']['label'];
    $field_instance['description'] = $field_settings['field_instance']['description'];
    $field_instance['required']    = $field_settings['field_instance']['required'];

    if (isset($field_settings['field_instance']['default_values'])) {
      $field_instance['default_values'] = $field_settings['field_instance']['default_values'];
    }

    field_update_instance($field_instance);
  }

  return array(
    'field'          => $field,
    'field_instance' => $field_instance,
  );
}

/**
 * @param string $attribute_code
 * @param string $family_code
 * @param string $group_code
 * @param array  $details
 *
 * @return array|mixed
 * @throws \Exception
 */
function _commerce_akeneo_family_services_family_attribute_save($attribute_code, $family_code, $group_code, $details) {
  // Store locally attributes for future purpose.
  if (!$attribute = commerce_akeneo_attribute_load($attribute_code, $family_code)) {
    $attribute = array(
      'family' => $family_code,
      'code'   => $attribute_code,
    );
  }

  $attribute['group_code'] = $group_code;
  $attribute['type']       = $details['type'];
  $attribute['label']      = $details['label'];

  unset($details['code']);
  unset($details['type']);
  unset($details['family']);
  unset($details['group_code']);
  unset($details['label']);

  $attribute['settings'] = $details;

  commerce_akeneo_attribute_save($attribute);

  return $attribute;
}

/**
 * @param string $bundle
 * @param array  $attribute_groups
 * @param array  $fields
 */
function _commerce_akeneo_family_services_family_attribute_groups($bundle, $attribute_groups, $fields) {
  // Create or update the group tab.
  _commerce_akeneo_family_services_family_group_save(
    $bundle,
    'group_tabs',
    t('Tabs'),
    50,
    'tabs',
    array()
  );

  $weight   = 0;
  $children = array();

  foreach ($attribute_groups as $code => $attribute_group) {
    if (!empty($fields[$code])) {
      $attribute_group_code  = 'group_' . $attribute_group['code'];
      $attribute_group_label = commerce_akeneo_get_language($attribute_group['labels']);

      $group = _commerce_akeneo_family_services_family_group_save(
        $bundle,
        $attribute_group_code,
        $attribute_group_label,
        $weight++,
        'tab',
        $fields[$code],
        'tabs'
      );

      $children[] = $group->group_name;
    }
  }

  // Set children for tab group.
  _commerce_akeneo_family_services_family_group_save(
    $bundle,
    'group_tabs',
    t('Tabs'),
    50,
    'tabs',
    $children
  );
}

/**
 * Save field group.
 *
 * @param string $bundle
 *   Bundle.
 * @param string $name
 *   Group name.
 * @param string $label
 *   Group label.
 * @param int    $weight
 *   Weight.
 * @param string $type
 *   Type.
 * @param array  $children
 *   Children.
 * @param string $parent
 *   Parent name.
 *
 * @return object
 *   The group saved.
 */
function _commerce_akeneo_family_services_family_group_save(
  $bundle,
  $name,
  $label,
  $weight = 0,
  $type = 'tab',
  $children = array(),
  $parent = ''
) {
  // Prepare storage with ctools.
  ctools_include('export');

  $entity_type = 'commerce_product';
  $mode        = 'form';

  $identifier = $name . '|' . $entity_type . '|' . $bundle . '|' . $mode;

  $groups = field_group_info_groups($entity_type, $bundle, $mode, TRUE);

  // Update existing group.
  if (isset($groups[$name]) && $group = $groups[$name]) {
    if (is_array($children)) {
      $group->children = $children;
    }

    $group->label       = $label;
    $group->parent_name = $parent;
    $group->weight      = $weight;

    ctools_export_crud_save('field_group', $group);

    return $group;
  }
  else {
    $field_group_types = field_group_formatter_info();
    $formatter         = $field_group_types[$mode];

    $new_group = (object) array(
      'identifier'  => $identifier,
      'group_name'  => $name,
      'entity_type' => $entity_type,
      'bundle'      => $bundle,
      'mode'        => $mode,
      'children'    => is_array($children) ? $children : array(),
      'parent_name' => $parent,
      'weight'      => (int) $weight,
      'label'       => $label,
      'format_type' => $type,
      'disabled'    => FALSE,
    );

    $new_group->format_settings = array('formatter' => isset($formatter['default_formatter']) ? $formatter['default_formatter'] : '');

    if (isset($formatter['instance_settings'])) {
      $new_group->format_settings['instance_settings'] = $formatter['instance_settings'];
    }

    $classes = _field_group_get_html_classes($new_group);

    $new_group->format_settings['instance_settings']['classes'] = implode(' ', $classes->optional);

    // Save and enable it in ctools.
    ctools_export_crud_save('field_group', $new_group);
    ctools_export_crud_enable('field_group', $new_group->identifier);

    drupal_set_message(t('New group %label successfully created.', array('%label' => $new_group->label)));

    return $new_group;
  }
}
