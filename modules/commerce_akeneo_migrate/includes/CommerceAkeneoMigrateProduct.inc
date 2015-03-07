<?php

/**
 * @file
 */

/**
 * Class CommerceAkeneoMigrateProduct
 */
class CommerceAkeneoMigrateProduct extends Migration {
  /**
   * @var array
   */
  protected $attributesByFields;

  /**
   * @var array
   */
  protected $pluginsByFields;
  
  /**
   * @param array $arguments
   */
  public function __construct($arguments) {
    global $user;

    parent::__construct($arguments);
    $this->description = t('Import product from Akeneo services.');

    $family = commerce_akeneo_family_load($arguments['code']);
//    $bundle = $family['product_type'];
    $this->addArguments(array('bundle' => $family['product_type']));

    // Create a map object for tracking the relationships between source rows.
    $this->map = new MigrateSQLEntityAPIQueueMap(
      $this->machineName,
      array(
        'sku' => array(
          'type'        => 'varchar',
          'length'      => 255,
          'not null'    => TRUE,
          'description' => 'SKU',
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product', $family['product_type']),
      'default',
      array(
        'entity_type'         => 'commerce_product',
        'bundle'              => $family['product_type'],
        'track_last_imported' => TRUE,
      )
    );

    $sourceFields = array(
      'sku'                  => t('SKU'),
      'label'                => t('Label'),
      'status'               => t('Status'),
      'created'              => t('Created date'),
      'updated'              => t('Updated date'),
//      'categories'           => t('Categories'),
      'commerce_price_value' => t('Commerce price'),
      'path'                 => t('Path'),
    );

    // Common field or properties.
    $this->addFieldMapping('uid')->defaultValue($user->uid);
    $this->addFieldMapping('title', 'label');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'updated');
    $this->addFieldMapping('path', 'path')->defaultValue(NULL);
    $this->addFieldMapping('language')->defaultValue(language_default('language'));

    // Drupal commerce fields.
    $this->addFieldMapping('sku', 'sku');
    // Commerce Price.
    $default_currency = commerce_default_currency();
    // Todo: Point commerce price data to fields.
    $this->addFieldMapping('commerce_price', 'commerce_price_value')->defaultValue(0);
    $this->addFieldMapping('commerce_price:currency_code')->defaultValue($default_currency);
    $this->addFieldMapping('commerce_price:tax_rate')->defaultValue(0);

    $this->attributesByFields = array();
    $this->pluginsByFields     = array();

    // First we only handle generic attributes.
    $this->loadFieldsAndPlugins($sourceFields, $family);
    // Association and categories are handle in second time.
    $this->loadFieldsAndPlugins($sourceFields, $family, 'category', 'product_category');
    $this->loadFieldsAndPlugins($sourceFields, $family, 'association', 'product_association');

    $this->source      = new MigrateSourceQueue('product:' . $arguments['code'], $sourceFields);
    $this->destination = new MigrateAkeneoDestinationCommerceProduct('commerce_product', $family['product_type']);
  }

  /**
   * @param array  $sourceFields
   * @param array  $family
   * @param string $section
   * @param string $plugin_name
   */
  protected function loadFieldsAndPlugins(&$sourceFields, $family, $section = '', $plugin_name = '') {
    $attributes         = commerce_akeneo_attribute_load_all($this->arguments['code'], $section);
    $attributesByFields = commerce_akeneo_attribute_dispatch_by_fields($attributes);

    foreach ($attributesByFields as $field_name => $field_info) {
      $field = field_info_field($field_name);

      if ($field && $plugin = commerce_akeneo_field_get_plugin(
          ($plugin_name ? $plugin_name : $field['type']),
          'commerce_product',
          $family['product_type'],
          $field_name
        )
      ) {
        /** @var CommerceAkeneoFieldInterface $plugin */
        $plugin->addFieldMappingToMigrate($this, $sourceFields);

        $this->attributesByFields[$field_name] = $field_info;
        $this->pluginsByFields[$field_name]     = $plugin;
      }
      elseif ($field === NULL) {
        drupal_set_message(
          t(
            'Destination field name not found: @field_name (bundle: @bundle)',
            array('@bundle' => $family['product_type'], '@field_name' => $field_name)
          ),
          'warning'
        );
      }
      else {
        drupal_set_message(
          t(
            'Destination field type not supported: @type (bundle: @bundle, field: @field)',
            array('@type' => $field['type'], '@bundle' => $family['product_type'], '@field' => $field_name)
          ),
          'warning'
        );
      }
    }
  }

  /**
   * Purge map table from missing destination entities.
   */
  protected function preImport() {
    // Removes map entries pointing at removed entities.
    $this->map->purgeMissingEntities('commerce_product', $this->arguments['bundle']);
  }

  /**
   * Release items from queue which have not been correctly imported.
   */
  protected function postImport() {
    $this->getSource()->releaseItems();

    // Links terms if parent not created before child.
    $limit = 10;

    // Keep going until we have no more orphan terms, or we're stuck in a
    // seemingly infinite loop (only happens with bad data).
    while ($this->destination->hasDeferredImports() && ($limit > 0)) {
      $this->destination->runDeferredImports();
      --$limit;
    }

    // Display an error if we still have orphans left over.
    if ($this->destination->hasDeferredImports()) {
      drupal_set_message(
        t(
          'One or more product(s) could not be fully imported because their associations could not be found: (%skus)',
          array('%skus' => implode(', ', $this->destination->getDeferredImportKeys()))
        ),
        'warning'
      );
    }
  }

  /**
   * @param stdClass $row
   *
   * @return bool
   */
  public function prepareRow($row) {
    global $user;

    // Skip invalid products.
    if (empty($row->sku) || !($valid = parent::prepareRow($row))) {
      return FALSE;
    }

    // Common values.
    $row->uid     = $user->uid;
    $row->created = strtotime($row->created);
    $row->updated = strtotime($row->updated);
    $row->label   = commerce_akeneo_get_language($row->labels);

    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginsByFields[$field_name])) {
        /** @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginsByFields[$field_name];
        $plugin->prepareRow($this, $row, $field_info['attributes']);
      }
    }

    return TRUE;
  }

  /**
   * @param stdClass $entity
   * @param stdClass $row
   */
  public function prepare(&$entity, $row) {
    // Todo: check behavior when type change.

    // Add support for languages.
    $languages = array();

    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginsByFields[$field_name])) {
        /** @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginsByFields[$field_name];
        $plugin->prepare($this, $entity, $row, $field_info['attributes']);
      }

      if (isset($entity->$field_name) && is_array($entity->$field_name)) {
        foreach ($entity->$field_name as $language => $data) {
          if ($language != LANGUAGE_NONE) {
            $languages[$language] = $language;
          }
        }
      }
    }

    if (count($languages)) {
      $languageDefault = language_default('language');

      $entity->language     = $languageDefault;
      $entity->translations = (object) array(
        'original' => $languageDefault,
        'data'     => array(),
      );

      foreach ($languages as $language) {
        $entity->translations->data[$language] = array(
          'entity_type' => 'commerce_product',
          'entity_id'   => isset($entity->nid) ? $entity->nid : 0,
          'language'    => $language,
          'source'      => ($language == $languageDefault ? '' : $languageDefault),
          'uid'         => $row->uid,
          'status'      => '1',
          'translate'   => '0',
        );
      }
    }
  }

  /**
   * @param stdClass $entity
   * @param stdClass $row
   */
  public function complete($entity, $row) {
    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginsByFields[$field_name])) {
        /** @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginsByFields[$field_name];
        $plugin->complete($this, $entity, $row, $field_info['attributes']);
      }
    }

    // Remove processed items from queue.
    // TODO: remove comment.
    $this->getSource()->deleteItem($row->migrate_source_item_id);
  }

  /**
   * @return \MigrateSourceQueue
   */
  public function getSource() {
    return $this->source;
  }

  /**
   * @return \MigrateAkeneoDestinationCommerceProduct
   */
  public function getDestination() {
    return $this->destination;
  }
}
