<?php

/**
 * @file
 */

/**
 * Class MigrateAkeneoDestinationCommerceProduct
 */
class MigrateAkeneoDestinationCommerceProduct extends MigrateDestinationCommerceProduct {
  /**
   * @var array
   */
  protected $deferred;

  /**
   * Basic initialization
   *
   * @param string $entity_type
   * @param string $bundle
   * @param array  $options
   *  Options (language, text_format) used for creating fields.
   */
  public function __construct($entity_type, $bundle, array $options = array()) {
    parent::__construct($entity_type, $bundle, $options);

    $this->deferred = array();
  }

  /**
   * {@inheritdoc}
   */
  public function import(stdClass $entity, stdClass $row) {
    // Fetch original IDs array from MigrateDestinationEntityAPI::import().
    $ids = parent::import($entity, $row);

    // Flatten the original IDs array.
    foreach ($ids as $key => $id) {
      if (is_array($id)) {
        $ids[$key] = reset($id);
      }
    }

    // Return flattened array now that it can be successfully processed by
    // MigrateSQLMap::saveIDMapping().
    return $ids;
  }

  /**
   * Give handlers a shot at modifying the object (or taking additional action)
   * after saving it.
   *
   * @param $entity
   *  Entity object to build. This is the complete object after saving.
   * @param $source_row
   *  Raw source data object - passed through to complete handlers.
   */
  public function complete($entity, stdClass $source_row) {


    parent::complete($entity, $source_row);
  }

  /**
   * Add entity to deferred queue to be processed again.
   *
   * @param \stdClass $entity
   *   Entity generated.
   * @param \stdClass $row
   *   Row from source.
   */
  public function addDeferredImports(stdClass $entity, stdClass $row) {
    $deferred_import = new stdClass();

    $deferred_import->entity = clone $entity;
    $deferred_import->row    = clone $row;

    $this->deferred[$entity->product_id] = $deferred_import;
  }

  /**
   * Check whether or not this destination needs to be processed again.
   *
   * @return bool
   *   TRUE if there are entities for which import was deferred.
   *   FALSE otherwise.
   */
  public function hasDeferredImports() {
    return (count($this->deferred) > 0);
  }

  /**
   * Return the SKU of entities for which import was deferred.
   *
   * @return array
   *   An array of SKUs.
   */
  public function getDeferredImportKeys() {
    $keys = array();

    foreach ($this->deferred as $import) {
      $keys[] = $import->entity->sku;
    }

    return $keys;
  }

  /**
   * Attempt to import any entity for which import was previously deferred.
   *
   * This will run import() again for each entity and row.
   *
   * @param string $method
   */
  public function runDeferredImports($method = 'complete') {
    $current = $this->deferred;

    // Clear the list of deferred imports, to be rebuilt during the import.
    $this->deferred = array();

    foreach ($current as $import) {
      // This will automatically re-queue any imports that
      // still need to be deferred.
      if (method_exists($this, $method)) {
        $args = array($import->entity, $import->row);
        call_user_func_array(array($this, $method), $args);
      }
    }
  }
}
