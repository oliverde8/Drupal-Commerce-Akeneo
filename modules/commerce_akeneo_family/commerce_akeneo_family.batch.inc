<?php

/**
 * @file
 * Commerce Akeneo Family batch import.
 */

/**
 * Implements hook_queue_info().
 */
function commerce_akeneo_family_queue_info() {
  $queues = array();

  $queues['commerce_akeneo:family'] = array(
    'title' => 'Commerce Akeneo : Family',
    'batch' => array(
      'operations'       => array(
        array('commerce_akeneo_family_batch_process', array()),
      ),
      'init_message'     => t('About to begin Commerce Akeneo Family import'),
      'title'            => t('Processing'),
      'progress_message' => t('Completed @current of @total, Estimate: @estimate, Elapsed: @elapsed'),
    ),
  );

  return $queues;
}

/**
 * Batch process for family synchronization.
 *
 * @param DrupalQueueInterface|string $queue
 *   Queue to process.
 * @param array                       $context
 *   Running context.
 */
function commerce_akeneo_family_batch_process($queue, &$context) {
  if (is_string($queue)) {
    /* @var DrupalQueueInterface $queue */
    $queue = DrupalQueue::get($queue);
  }

  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max']      = $queue->numberOfItems();
  }

  try {
    if ($item = $queue->claimItem(300)) {
      // Shortcut.
      $request = $item->data;

      // Create or update family from request.
      $family = commerce_akeneo_family_load_from_request($request);

      // Create or update destination commerce product type.
      if (commerce_akeneo_family_product_type_save($family, $request)) {


        // Handle fields, categories and associations.
        commerce_akeneo_family_services_family_attributes($family, $request);

        // Remote item from queue due to correct handle.
        $queue->deleteItem($item);

        $context['message'] = t(
          'Processed family "@family" as [@product_type] product type (!count/!total).',
          array(
            '@family' => $family['label'],
            '@product_type' => $family['product_type'],
            '!count' => $context['sandbox']['progress'] + 1,
            '!total' => $context['sandbox']['max'],
          )
        );
      }
      else {
        $context['message'] = t(
          'Skipped family "@family" as [@product_type] product type (!count/!total). No modifications detected!',
          array(
            '@family' => $family['label'],
            '@product_type' => $family['product_type'],
            '!count' => $context['sandbox']['progress'] + 1,
            '!total' => $context['sandbox']['max'],
          )
        );
      }
    }
  }
  catch (Exception $e) {
    // In case of exception log it and leave the item in the queue
    // to be processed again later.
    drupal_set_message($e->getMessage(), 'error', FALSE);
    watchdog_exception('queue_ui', $e, NULL, WATCHDOG_ERROR);
  }

  // Cache clear for entities because we handle entity structure.
  entity_flush_caches();

  // Cache clear for all info about fields.
  field_cache_clear();

  $context['sandbox']['progress']++;

  // If the last attempt to get an item produced an empty result, then no more
  // claimable items remain in the queue and we can tell Batch API we are done.
  // Otherwise, items may remain in the queue and we should tell Batch API to
  // call us again.
  if (empty($item)) {
    $context['finished'] = 1;

    // Clear all caches.
    cache_clear_all();

    if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
      $delta = $context['sandbox']['max'] - $context['sandbox']['progress'];
      drupal_set_message(
        t(
          '@count items in the queue are locked. You need to remove leases before processing again those items.',
          array('@count' => $delta)
        ),
        'warning'
      );
    }

    // Reset cache for commerce product types.
    commerce_product_types_reset();
    variable_set('menu_rebuild_needed', TRUE);

    // All statically defined migrations have been (re)registered.
    if (module_exists('migrate')) {
      migrate_static_registration();
    }
  }
  else {
    // We can't use numberOfItems() to know how many items we must process,
    // because that does not take claims on items into account.
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Create or update family from Akeneo request.
 *
 * @param array $request
 *   Request from Akeneo.
 *
 * @return array
 *   Family.
 */
function commerce_akeneo_family_load_from_request($request) {
  $code = check_plain($request['code']);

  // Create or update family from request.
  if (!$family = commerce_akeneo_family_load($code)) {
    $family = array(
      'code'         => $code,
      'product_type' => commerce_akeneo_get_machine_name($code, 'product'),
    );
  }

  $family['label']    = filter_xss(commerce_akeneo_get_language($request['labels']));
  $family['settings'] = $request;

  // Store family in database for future purpose.
  commerce_akeneo_family_save($family);

  return $family;
}

/**
 * Save product type based on family.
 *
 * @param array $family
 *   Family.
 * @param array $request
 *   Request.
 *
 * @return bool
 *   If the familly was modified or not
 *
 * @throws \CommerceAkeneoException
 */
function commerce_akeneo_family_product_type_save($family, $request) {
  if (!isset($request['update_time'])) {
    $request['update_time'] = 0;
  }

  // Load destination commerce product type.
  if ($settings = commerce_product_type_load($family['product_type'])) {
    $settings['name']   = filter_xss(commerce_akeneo_get_language($request['labels']));
    $settings['is_new'] = FALSE;

    $update_time_variable = "commerce_akeneo_familly_update_time_" . strtolower($request['akeneo_code']);
    $last_update_time = variable_get($update_time_variable, 0);

    if ($last_update_time == $request['update_time']) {
      // No modifications no need to make changes.
      return FALSE;
    }
    variable_set($update_time_variable, $request['update_time']);
  }
  else {
    die("WTF");
    // Create settings for new commerce product type.
    $settings = array(
      'type'         => $family['product_type'],
      'name'         => $family['label'],
      'description'  => time(),
      'help'         => '',
      'revision'     => 1,
      'multilingual' => 4,
      'is_new'       => TRUE,
    );
  }

  // Set the multilingual value for the product type
  // if entity translation is enabled.
  if (module_exists('entity_translation')) {
    $multilingual = isset($settings['multilingual']) ? $settings['multilingual'] : 4;
    variable_set('language_product_type_' . $settings['type'], $multilingual);
  }

  // Configure but skip cache reset.
  if (!commerce_product_ui_product_type_save($settings, TRUE, TRUE)) {
    throw new CommerceAkeneoException(t('Unable to create or update product type.'));
  }

  return TRUE;
}

/**
 * Prepare attribute to create/update destination field.
 *
 * @param array $family
 *   Family.
 * @param array $request
 *   Request.
 *
 * @throws \CommerceAkeneoException
 */
function commerce_akeneo_family_services_family_attributes($family, $request) {
  // Build full attributes list.
  $attributes = array();
  $groups     = $request['attribute_groups'];

  foreach ($groups as $group_code => $group_details) {
    if (!empty($group_details['attributes'])) {
      foreach ($group_details['attributes'] as $attribute_code => $attribute_details) {
        // Create label from code if not available.
        $label = filter_xss(commerce_akeneo_get_language($attribute_details['labels']));
        if (empty($label) || preg_match('/^\[[a-z0-9_]+\]$/i', $label)) {
          $label = drupal_ucfirst(str_replace('_', ' ', $attribute_code));
        }

        $attribute = array(
          'family'     => $family['code'],
          'section'    => '',
          'code'       => $attribute_code,
          'type'       => $attribute_details['type'],
          'required'   => $attribute_details['required'],
          'group_code' => $group_code,
          'label'      => $label,
          'settings'   => array(
            'labels'     => $attribute_details['labels'],
            'parameters' => $attribute_details['parameters'],
          ),
        );

        $attributes[$attribute_code] = $attribute;
      }
    }
  }

  // Add fields for category references.
  if ($categories = commerce_akeneo_taxonomy_load_all('category')) {
    $groups['categories'] = array(
      'labels' => array(
        LANGUAGE_NONE => t('Categories'),
      ),
    );

    foreach ($categories as $category) {
      $attribute_code = strtolower($category['code']);

      $attribute = array(
        'family'     => $family['code'],
        'section'    => 'category',
        'code'       => $attribute_code,
        'type'       => 'product_category',
        'required'   => FALSE,
        'group_code' => 'categories',
        'label'      => $category['label'],
        'settings'   => array(
          'labels'     => array(LANGUAGE_NONE => $category['label']),
          'parameters' => array(),
        ),
      );

      $attributes[$attribute_code] = $attribute;
    }
  }

  // Add entity references for product associations.
  if (!empty($request['associations'])) {
    $groups['associations'] = array(
      'labels' => array(
        LANGUAGE_NONE => t('Associations'),
      ),
    );

    foreach ($request['associations'] as $code => $association) {
      $attribute_code = strtolower($code);
      $label          = filter_xss(commerce_akeneo_get_language($association['labels']));

      $attribute = array(
        'family'     => $family['code'],
        'section'    => 'association',
        'code'       => $attribute_code,
        'type'       => 'product_association',
        'required'   => FALSE,
        'group_code' => 'associations',
        'label'      => $label,
        'settings'   => array(
          'labels'     => $association['labels'],
          'parameters' => array(),
        ),
      );

      $attributes[$attribute_code] = $attribute;
    }
  }

  // Save attributes (create / update).
  foreach ($attributes as $pos => $values) {
    // Load existing attribute record if exists.
    $attribute     = commerce_akeneo_attribute_load($values['family'], $values['section'], $values['code']);
    $values['aid'] = ($attribute ? $attribute['aid'] : 0);

    // Set destination field_name not already defined.
    if (empty($values['field_name'])) {
      $type = ($values['section'] ? 'field_' . $values['section'] : 'field');

      $values['field_name'] = commerce_akeneo_get_machine_name($values['code'], $type);
    }

    if ($values['field_name']) {
      // Save updates.
      commerce_akeneo_attribute_save($values);
      $attributes[$pos] = $values;
    }
    else {
      unset($attributes[$pos]);
    }
  }

  // Contains only mapped attributes / fields.
  $attributes_by_field = commerce_akeneo_attribute_dispatch_by_fields($attributes);

  // Build destination fields.
  $fields_by_group = array();
  foreach ($attributes_by_field as $field_name => $field_settings) {
    // Use first attribute as reference for destination field.
    $attribute = reset($field_settings['attributes']);

    commerce_akeneo_family_services_family_attribute($family, $attribute, $field_name);

    $fields_by_group[$attribute['group_code']][$field_name] = $field_settings;
  }

  if (module_exists('field_group')) {
    commerce_akeneo_family_services_family_attribute_groups($family, $groups, $fields_by_group);
    cache_clear_all('field_groups', 'cache_field');
  }
}

/**
 * Get field settings.
 *
 * @param string $bundle
 *   Bundle.
 * @param string $field_name
 *   Field name.
 * @param array  $attribute
 *   Attribute.
 *
 * @return array|bool
 *   Settings.
 */
function commerce_akeneo_attribute_get_settings($bundle, $field_name, $attribute) {
  /* @var CommerceAkeneoAttributeInterface $plugin */
  if ($plugin = commerce_akeneo_get_attribute_plugin($attribute['type'])) {
    $settings = $plugin->getFieldSettings($attribute, $bundle, $field_name, $attribute['label']);

    return $settings;
  }

  return FALSE;
}

/**
 * Handle field and instance field update and return settings.
 *
 * @param array  $family
 *   Family.
 * @param array  $attribute
 *   Attribute.
 * @param string $field_name
 *   Field name.
 *
 * @return array|bool
 *   FALSE if error or array for settings.
 *
 * @throws \CommerceAkeneoException
 * @throws \Exception
 * @throws \FieldException
 */
function commerce_akeneo_family_services_family_attribute($family, $attribute, $field_name) {
  // Handle special case like identifier: SKU.
  if (!$field_settings = commerce_akeneo_attribute_get_settings($family['product_type'], $field_name, $attribute)) {
    return FALSE;
  }

  // Load field if exists.
  if (!$field = field_info_field($field_name)) {
    // Create field if missing.
    $field = field_create_field($field_settings['field']);
  }
  elseif ($field['type'] != $field_settings['field']['type']) {
    drupal_set_message(
      t(
        'Field type for field "@field" has been changed: "@old" => "@new", which is not supported.',
        array(
          '@field' => $attribute['code'],
          '@old'   => $field['type'],
          '@new'   => $field_settings['field']['type'],
        )
      ),
      'warning'
    );
  }
  elseif ($field['translatable'] != $field_settings['field']['translatable']) {
    // Update field properties.
    $field['translatable'] = $field_settings['field']['translatable'];

    field_update_field($field);
  }

  // Create field instance if missing.
  $field_instance = field_read_instance('commerce_product', $field_name, $family['product_type']);

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

    $md5 = md5(serialize($field_instance));

    if ($md5 != $attribute['checksum']) {
      field_update_instance($field_instance);

      $attribute['checksum'] = $md5;

      // Store new checksum.
      commerce_akeneo_attribute_save($attribute);
    }
  }

  return array(
    'field'          => $field,
    'field_instance' => $field_instance,
  );
}

/**
 * Manage and save field groups.
 *
 * @param array $family
 *   Family.
 * @param array $groups
 *   Attribute groups.
 * @param array $fields_by_group
 *   Fields by group.
 */
function commerce_akeneo_family_services_family_attribute_groups($family, $groups, $fields_by_group) {
  $group_all_code = commerce_akeneo_get_machine_name('all', 'group');

  // Create or update the group tab.
  commerce_akeneo_family_services_family_group_save(
    $family['product_type'],
    $group_all_code,
    t('Tabs'),
    50,
    'tabs',
    array()
  );

  $weight   = 0;
  $children = array();

  foreach ($fields_by_group as $code => $fields) {
    $group_code            = commerce_akeneo_get_machine_name($code, 'group');
    $attribute_group_label = filter_xss(commerce_akeneo_get_language($groups[$code]['labels']));

    $group = commerce_akeneo_family_services_family_group_save(
      $family['product_type'],
      $group_code,
      $attribute_group_label,
      $weight++,
      'tab',
      array_keys($fields),
      'tabs'
    );

    $children[] = $group->group_name;
  }

  // Set children for tab group.
  commerce_akeneo_family_services_family_group_save(
    $family['product_type'],
    $group_all_code,
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
function commerce_akeneo_family_services_family_group_save(
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

    $new_group->format_settings = array(
      'formatter' => isset($formatter['default_formatter']) ? $formatter['default_formatter'] : '',
    );

    if (isset($formatter['instance_settings'])) {
      $new_group->format_settings['instance_settings'] = $formatter['instance_settings'];
    }

    $classes = _field_group_get_html_classes($new_group);

    $new_group->format_settings['instance_settings']['classes'] = implode(' ', $classes->optional);

    // Save and enable it in ctools.
    ctools_export_crud_save('field_group', $new_group);
    ctools_export_crud_enable('field_group', $new_group->identifier);

    return $new_group;
  }
}
