<?php

/**
 * @file
 */

require_once 'MigrateSQLEntityAPIQueueMap.inc';
require_once 'MigrateSourceQueue.inc';

/**
 * Class CommerceAkeneoMigrateProduct
 */
class CommerceAkeneoMigrateProduct extends Migration {
  /**
   * @var array
   */
  protected $attributesByFields;

  /**
   * @param array $arguments
   */
  public function __construct($arguments) {
    global $user;

    parent::__construct($arguments);
    $this->description = t('Import product from Akeneo services.');

    $bundle = variable_get('commerce_akeneo_product_type_prefix', '') . $arguments['type'];
    $this->addArguments(array('bundle' => $bundle));

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
      MigrateDestinationEntityAPI::getKeySchema('commerce_product', $bundle),
      'default',
      array('entity_type' => 'commerce_product', 'bundle' => $bundle, 'track_last_imported' => TRUE)
    );

    $sourceFields = array(
      'sku'                  => t('SKU'),
      'label'                => t('Label'),
      'status'               => t('Status'),
      'created'              => t('Created date'),
      'updated'              => t('Updated date'),
      'categories'           => t('Categories'),
//      'title_field_value'    => t('Product title'),
//      'title_field_language' => t('Product title languages'),
      'commerce_price_value' => t('Commerce price'),
      'path'                 => t('Path'),
    );

    // Common field or properties.
    $this->addFieldMapping('uid')->defaultValue($user->uid);
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'updated');
    $this->addFieldMapping('path', 'path')->defaultValue(NULL);
    $this->addFieldMapping('language')->defaultValue(language_default('language'));

    // Drupal commerce fields.
    $this->addFieldMapping('sku', 'sku');
//    $this->addFieldMapping('commerce_price', 'commerce_price_value')->defaultValue(0);

    // Akeneo fields.
    $attributes               = commerce_akeneo_attribute_load_all(NULL, $arguments['type']);
    $this->attributesByFields = commerce_akeneo_attribute_dispatch_by_fields($attributes);
    $this->pluginFields       = array();

    foreach ($this->attributesByFields as $field_name => $field_info) {
      $field = field_info_field($field_name);

      /** @var CommerceAkeneoFieldInterface $plugin */
      if ($plugin = commerce_akeneo_field_get_plugin($field['type'], 'commerce_product', $bundle, $field_name)) {
        $plugin->addFieldMappingToMigrate($this, $sourceFields);

        $this->pluginFields[$field_name] = $plugin;
      }
      else {
        drupal_set_message(
          t(
            'Destination field type not supported: @type (bundle: @bundle, field: @field)',
            array('@type' => $field['type'], '@bundle' => $bundle, '@field' => $field_name)
          ),
          'warning'
        );
      }
    }

    $this->source      = new MigrateSourceQueue($arguments['type'], $sourceFields);
    $this->destination = new MigrateDestinationCommerceProduct('commerce_product', $bundle);
  }

  /**
   *
   */
  protected function preImport() {
    // Removes map entries pointing at removed entities.
    $this->map->purgeMissingEntities('commerce_product', $this->arguments['bundle']);
  }

  /**
   *
   */
  protected function postImport() {
    $this->getSource()->releaseItems();
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
    $row->title   = commerce_akeneo_get_language($row->labels);

    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginFields[$field_name])) {
        /** @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginFields[$field_name];
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
      if (isset($this->pluginFields[$field_name])) {
        /** @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginFields[$field_name];
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
    // Todo: delete imported items.
//    $this->getSource()->deleteItem($row->migrate_source_item_id);

    // Loop over plugins to map attributes vs fields/properties.
    foreach ($this->attributesByFields as $field_name => $field_info) {
      if (isset($this->pluginFields[$field_name])) {
        /** @var CommerceAkeneoFieldInterface $plugin */
        $plugin = $this->pluginFields[$field_name];
        $plugin->complete($this, $entity, $row, $field_info['attributes']);
      }
    }
  }

  /**
   * @return \MigrateSourceQueue
   */
  public function getSource() {
    return $this->source;
  }
}
