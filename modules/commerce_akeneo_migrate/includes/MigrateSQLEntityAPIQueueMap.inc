<?php

/**
 * @file
 * MigrateSQLEntityAPIQueueMap class file.
 */

/**
 * Class MigrateSQLEntityAPIQueueMap
 */
class MigrateSQLEntityAPIQueueMap extends MigrateSQLEntityAPIMap {
  /**
   * Overrides update mechanism to force it.
   *
   * @param array $source_id
   *   Source ID.
   *
   * @return array
   *   Source row.
   */
  public function getRowBySource(array $source_id) {
    $row = parent::getRowBySource($source_id);

    // Force destination to be updated.
    if ($row) {
      $row['needs_update'] = MigrateMap::STATUS_NEEDS_UPDATE;
    }

    return $row;
  }
}
