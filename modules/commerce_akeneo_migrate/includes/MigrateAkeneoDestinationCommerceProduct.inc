<?php

/**
 * @file
 */

/**
 * Class MigrateAkeneoDestinationCommerceProduct
 */
class MigrateAkeneoDestinationCommerceProduct extends MigrateDestinationCommerceProduct {
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
}