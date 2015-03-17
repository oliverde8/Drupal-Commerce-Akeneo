<?php

/**
 * @file
 * CommerceAkeneoMigrateProductDisplay class file.
 */

/**
 * Class CommerceAkeneoMigrateProductDisplay
 */
class CommerceAkeneoMigrateProductDisplay extends Migration {
  /**
   * @var string
   */
  protected $bundle;

  /**
   * @var string
   */
  protected $entityType;

  /**
   * @var array
   */
  protected $field;

  /**
   * Constructor.
   *
   * @param array $arguments
   *   Arguments.
   */
  public function __construct($arguments) {
    parent::__construct($arguments);

    $family            = commerce_akeneo_family_load($arguments['code']);
    $this->description = t('Import product display from Akeneo services.');
    $this->entityType  = 'node';

    if (!$this->bundle = $this->getDestinationBundle($family)) {
      throw new CommerceAkeneoException(
        t(
          'The bundle destination field is missing. You need to <a href="@url">setup one</a>.',
          array('@url' => url('admin/config/services/akeneo'))
        )
      );
    }

    if (!$this->field = $this->getDestinationField($family, $this->entityType, $this->bundle)) {
      throw new CommerceAkeneoException(t('The product variant destination field is missing.'));
    }

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
      MigrateDestinationEntityAPI::getKeySchema($this->entityType, $this->bundle),
      'default',
      array(
        'entity_type'         => $this->entityType,
        'bundle'              => $this->bundle,
        'track_last_imported' => TRUE,
        'find_existing'       => FALSE,
      )
    );

    $source_fields = array(
      'label'                 => t('Label'),
      'status'                => t('Status'),
      'created'               => t('Created date'),
      'product_variant_value' => t('Products references'),
    );

    // Common field or properties.
    $this->addFieldMapping('type')->defaultValue($this->bundle);
    $this->addFieldMapping('title', 'label');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('language')->defaultValue(language_default('language'));

    $this->addUnmigratedDestinations(
      array(
        'promote',
        'sticky',
        'author',
        'log',
        'revision',
        'body',
        'body:summary',
        'body:format',
        'body:language',
        'path',
        $this->field['field_name'],
      )
    );

//    $queue  = commerce_akeneo_queue_load('product_display:' . $arguments['code'], FALSE);
//    $queue->deleteQueue();

    $this->source      = new MigrateSourceQueue('product_display:' . $arguments['code'], $source_fields);
    $this->destination = new MigrateAkeneoDestinationEntityAPI($this->entityType, $this->bundle);
  }

  /**
   * Determine destination bundle name.
   *
   * @param object $family
   *   Family.
   *
   * @return string
   *   Bundle name.
   */
  protected function getDestinationBundle($family) {
    // Defined on settings page.
    if ($bundle = variable_get('commerce_akeneo_migrate_product_display_bundle', NULL)) {
      return $bundle;
    }

    // Try to detect same bundle on node entity type.
    if ($node_type = node_type_load($family['product_type'])) {
      return $family['product_type'];
    }

    return NULL;
  }

  /**
   * Determine destination field name which will hold product variants.
   *
   * @param array  $family
   *   Family.
   * @param string $entity_type
   *   Entity type.
   * @param string $bundle
   *   Bundle.
   *
   * @return array
   *   Field.
   */
  protected function getDestinationField($family, $entity_type, $bundle) {
    // Auto detect field.
    if (variable_get('commerce_akeneo_migrate_product_display_field_auto', FALSE)) {
      $field_instances = field_info_instances($entity_type, $bundle);
      $availables      = array();

      foreach ($field_instances as $field_name => $field_instance) {
        $field = field_info_field($field_name);

        if ($field['type'] == 'commerce_product_reference' &&
          isset($field['bundles'][$entity_type]) &&
          array_search($bundle, $field['bundles'][$entity_type], TRUE) !== FALSE
        ) {
          $availables[] = $field;
        }
      }

      if (count($availables) == 1) {
        return $availables[0];
      }
    }

    // Check specified field.
    if ($field_name = variable_get('commerce_akeneo_migrate_product_display_field', '')) {
      $field = field_info_field($field_name);

      if ($field['type'] == 'commerce_product_reference' &&
        isset($field['bundles'][$entity_type]) &&
        array_search($bundle, $field['bundles'][$entity_type], TRUE) !== FALSE
      ) {
        return $field;
      }
    }

    return NULL;
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
   * @return \MigrateAkeneoDestinationEntityAPI
   *   Destination.
   */
  public function getDestination() {
    return $this->destination;
  }

  /**
   * Purge map table from missing destination entities.
   */
  protected function preImport() {
    if (empty($this->field)) {
      throw new CommerceAkeneoException(t('The product variant destination field is missing.'));
    }

    // Removes map entries pointing at removed entities.
    $this->map->purgeMissingEntities($this->entityType, $this->bundle);
  }

  /**
   * Prepare row before import.
   *
   * @param object $row
   *   Row.
   *
   * @return bool
   *   TRUE to import row.
   */
  public function prepareRow($row) {
    // Skip invalid products.
    if (empty($row->sku) || !($valid = parent::prepareRow($row))) {
      return FALSE;
    }

    // Common values.
    $row->created = REQUEST_TIME;
    $row->label   = $row->product_row->label;

    // Skip products parts of group.
    if (!empty($row->groups) && variable_get('commerce_akeneo_migrate_product_display_import_group', FALSE)) {
      return FALSE;
    }

    foreach (module_implements('commerce_akeneo_migrate_product_display_prepare_row') as $module) {
      if (module_invoke($module, 'commerce_akeneo_migrate_product_display_prepare_row', $row) === FALSE) {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Prepare destination entity based on source row.
   *
   * @param object $entity
   *   Entity.
   * @param object $row
   *   Source row.
   */
  public function prepare(&$entity, $row) {
    global $user;

    $entity->uid = $user->uid;
  }

  /**
   * Complete destination entity based on source row.
   *
   * @param object $entity
   *   Entity.
   * @param object $row
   *   Source row.
   */
  public function complete($entity, $row) {
    $wrapper = entity_metadata_wrapper($this->entityType, $entity);
    if ($this->field['cardinality'] == 1) {
      $wrapper->{$this->field['field_name']}->set($row->product_id);
    }
    else {
      $wrapper->{$this->field['field_name']}->set(array($row->product_id));
    }
    $wrapper->save();

    foreach (module_implements('commerce_akeneo_migrate_product_display_complete') as $module) {
      module_invoke($module, 'commerce_akeneo_migrate_product_display_complete', $entity, $row);
    }

    // Remove processed items from queue.
    // $this->getSource()->deleteItem($row->migrate_source_item_id);
  }

  /**
   * Release items from queue which have not been correctly imported.
   */
  protected function postImport() {
    $this->getSource()->releaseItems();
  }
}
