<?php

/**
 * @file
 * CommerceAkeneoMigrateProduct class file.
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
   * @var DrupalReliableQueueInterface
   */
  protected $productDisplayQueue;

  /**
   * Constructor.
   *
   * @param array $arguments
   *   Arguments.
   */
  public function __construct($arguments) {
    global $user;

    parent::__construct($arguments);
    $this->description = t('Import product from Akeneo services.');

    $family = commerce_akeneo_family_load($arguments['code']);
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

    $source_fields = array(
      'sku'                          => t('SKU'),
      'label'                        => t('Label'),
      'status'                       => t('Status'),
      'created'                      => t('Created date'),
      'updated'                      => t('Updated date'),
      'commerce_price_value'         => t('Commerce price'),
      'commerce_price_currency_code' => t('Commerce price currency'),
      'commerce_price_tax_rate'      => t('Commerce price tax rate'),
      'path'                         => t('Path'),
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
    $this->addFieldMapping('commerce_price:currency_code', 'commerce_price_currency_code')
      ->defaultValue($default_currency);
    $this->addFieldMapping('commerce_price:tax_rate', 'commerce_price_tax_rate')->defaultValue(0);

    $this->attributesByFields = array();
    $this->pluginsByFields    = array();

    // First we only handle generic attributes.
    $this->loadFieldsAndPlugins($source_fields, $family);
    // Association and categories are handle in second time.
    $this->loadFieldsAndPlugins($source_fields, $family, 'category', 'product_category');
    $this->loadFieldsAndPlugins($source_fields, $family, 'association', 'product_association');

    $this->source      = new MigrateSourceQueue('product:' . $arguments['code'], $source_fields);
    $this->destination = new MigrateAkeneoDestinationCommerceProduct('commerce_product', $family['product_type']);
  }

  /**
   * Provide source casted.
   *
   * @return \MigrateSourceQueue
   *   Source.
   */
  public function getSource() {
    return $this->source;
  }

  /**
   * Provide destination casted.
   *
   * @return \MigrateAkeneoDestinationCommerceProduct
   *   Destination.
   */
  public function getDestination() {
    return $this->destination;
  }

  /**
   * Load fields and plugins.
   *
   * @param array  $source_fields
   *   Source fields.
   * @param array  $family
   *   Family.
   * @param string $section
   *   Section.
   * @param string $plugin_name
   *   Plugin name.
   */
  protected function loadFieldsAndPlugins(&$source_fields, $family, $section = '', $plugin_name = '') {
    $attributes           = commerce_akeneo_attribute_load_all($this->arguments['code'], $section);
    $attributes_by_fields = commerce_akeneo_attribute_dispatch_by_fields($attributes);

    foreach ($attributes_by_fields as $field_name => $field_info) {
      $field = field_info_field($field_name);

      if ($field && $plugin = commerce_akeneo_get_field_plugin(
          ($plugin_name ? $plugin_name : $field['type']),
          'commerce_product',
          $family['product_type'],
          $field_name
        )
      ) {
        /* @var CommerceAkeneoFieldInterface $plugin */
        $plugin->addFieldMappingToMigrate($this, $source_fields);

        $this->attributesByFields[$field_name] = $field_info;
        $this->pluginsByFields[$field_name]    = $plugin;
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
            array(
              '@type'   => $field['type'],
              '@bundle' => $family['product_type'],
              '@field'  => $field_name,
            )
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

    // Create a queue to prepare product display migration.
    $this->productDisplayQueue = commerce_akeneo_queue_load('product_display:' . $this->arguments['code'], TRUE);
  }

  /**
   * Prepare row before import.
   *
   * @param stdclass $row
   *   Row.
   *
   * @return bool
   *   TRUE to import row.
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
    $row->label   = filter_xss(commerce_akeneo_get_language($row->labels));

    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginsByFields[$field_name])) {
        /* @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginsByFields[$field_name];

        // Skip this row if requested by plugin.
        if (!$plugin->prepareRow($this, $row, $field_info['attributes'])) {
          $args = array('@sku' => $row->sku, '@plugin' => get_class($plugin));
          drupal_set_message(t('Product @sku skipped by plugin @plugin.', $args), 'warning');

          return FALSE;
        }
      }
    }

    foreach (module_implements('commerce_akeneo_migrate_product_prepare_row') as $module) {
      if (module_invoke($module, 'commerce_akeneo_migrate_product_prepare_row', $row) === FALSE) {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Prepare destination entity based on source row.
   *
   * @param stdclass $entity
   *   Entity.
   * @param stdclass $row
   *   Source row.
   */
  public function prepare(&$entity, $row) {
    // Add support for languages.
    $languages = array();

    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginsByFields[$field_name])) {
        /* @var CommerceAkeneoFieldInterface $plugin */
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
      $language_default = language_default('language');

      $entity->language     = $language_default;
      $entity->translations = (object) array(
        'original' => $language_default,
        'data'     => array(),
      );

      foreach ($languages as $language) {
        $entity->translations->data[$language] = array(
          'entity_type' => 'commerce_product',
          'entity_id'   => isset($entity->nid) ? $entity->nid : 0,
          'language'    => $language,
          'source'      => ($language == $language_default ? '' : $language_default),
          'uid'         => $row->uid,
          'status'      => '1',
          'translate'   => '0',
        );
      }
    }
  }

  /**
   * Complete destination entity based on source row.
   *
   * @param stdclass $entity
   *   Entity.
   * @param stdclass $row
   *   Source row.
   */
  public function complete($entity, $row) {
    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginsByFields[$field_name])) {
        /* @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginsByFields[$field_name];
        $plugin->complete($this, $entity, $row, $field_info['attributes']);
      }
    }

    // Register product to be processed as product display.
    $new_row = array(
      'sku'         => $row->sku,
      'product_id'  => $entity->product_id,
      'product_row' => $row,
    );
    $this->productDisplayQueue->createItem($new_row);

    // Remove processed items from queue.
    $this->getSource()->deleteItem($row->migrate_source_item_id);
  }

  /**
   * Release items from queue which have not been correctly imported.
   */
  protected function postImport() {
    $this->getSource()->releaseItems();

    // Links products if not created before.
    $limit = 10;

    while ($this->destination->hasDeferredImports() && ($limit > 0)) {
      if (!$this->destination->runDeferredImports()) {
        break;
      }

      // Loop security.
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
}
