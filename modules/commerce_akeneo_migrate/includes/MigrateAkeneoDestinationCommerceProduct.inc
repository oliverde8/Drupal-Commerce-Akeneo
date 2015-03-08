<?php

/**
 * @file
 * MigrateAkeneoDestinationCommerceProduct class file.
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
   * Basic initialization.
   *
   * @param string $entity_type
   *   Entity type.
   * @param string $bundle
   *   Bundle.
   * @param array  $options
   *   Options (language, text_format) used for creating fields.
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
   * Add entity to deferred queue to be processed again.
   *
   * @param string $method
   *   Method to call in deferred process.
   * @param string $id
   *   Id.
   */
  public function addDeferredImports($method, $id) {
    $args = func_get_args();
    // Remove method.
    array_shift($args);
    // Remove id.
    array_shift($args);

    $args_cloned = array();
    foreach ($args as $arg) {
      $args_cloned[] = clone $arg;
    }

    $deferred_import         = new stdClass();
    $deferred_import->method = $method;
    $deferred_import->args   = $args_cloned;

    $this->deferred[$id] = $deferred_import;
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
   * @return bool
   *   TRUE if an item as been successfully processed.
   */
  public function runDeferredImports() {
    $current = $this->deferred;

    // Clear the list of deferred imports, to be rebuilt during the import.
    $this->deferred = array();

    foreach ($current as $import) {
      // This will automatically re-queue any imports that
      // still need to be deferred.
      if (method_exists($this, $import->method)) {
        $args = array($import->entity, $import->row);
        call_user_func_array(array($this, $import->method), $args);
      }
    }

    return count($current) != count($this->deferred);
  }
}
