<?php

/**
 * @file
 */

/**
 * Class MigrateSQLEntityAPIMap
 */
class MigrateSQLEntityAPIMap extends MigrateSQLMap {
  /**
   * @var string
   */
  protected $destinationEntityType;

  /**
   * @var string
   */
  protected $destinationBundle;

  /**
   * Constructor.
   *
   * @param string $machine_name
   *   The unique reference to the migration that we are mapping.
   * @param array  $source_key
   *   The database schema for the source key.
   * @param array  $destination_key
   *   The database schema for the destination key.
   * @param string $connection_key
   *   Optional - The connection used to create the mapping tables. By default
   *   this is the destination (Drupal). If it's not possible to make joins
   *   between the destination database and your source database you can specify
   *   a different connection to create the mapping tables on.
   * @param array  $options
   *   Optional - Options applied to this source.
   *
   * @throws Exception
   */
  public function __construct(
    $machine_name,
    array $source_key,
    array $destination_key,
    $connection_key = 'default',
    $options = array()
  ) {
    parent::__construct($machine_name, $source_key, $destination_key, $connection_key, $options);

    if (!empty($options['entity_type'])) {
      $this->destinationEntityType = $options['entity_type'];
    }
    else {
      throw new Exception('Missing entity type.');
    }

    if (!empty($options['bundle'])) {
      $this->destinationBundle = $options['bundle'];
    }
    else {
      throw new Exception('Missing bundle.');
    }
  }


  /**
   * Remove any map entry with missing destination entity.
   */
  public function purgeMissingEntities() {
    reset($this->destinationKey);
    $field_name      = key($this->destinationKey);
    $entity_info     = entity_get_info($this->destinationEntityType);
    $sourceKeys      = drupal_map_assoc(array_values($this->sourceKeyMap));
    $destinationKeys = drupal_map_assoc(array_values($this->destinationKeyMap));

    // Select map entries which point at missing entities.
    $query = "SELECT m.*
              FROM {$this->mapTable} AS m
              LEFT JOIN {$entity_info['base table']} AS e ON m.destid1 = e.{$field_name}
              WHERE e.{$field_name} IS NULL";

    $results = db_query($query);

    // Loop over map entries.
    while ($result = $results->fetchAssoc()) {
      $sourceIds      = array_intersect_key($result, $sourceKeys);
      $destinationIds = array_intersect_key($result, $destinationKeys);

      // Remove map entry.
      $this->delete($sourceIds);

      watchdog(
        'migrate',
        t(
          "The destination @entity_type entity @destid1 (source: @source) doesn't exist anymore. The corresponding map entry has been removed."
        ),
        array(
          '@entity_type' => $this->destinationEntityType,
          '@destid1'     => reset($destinationIds),
          '@source'      => implode(', ', $sourceIds),
        ),
        WATCHDOG_NOTICE
      );
    }
  }

  /**
   * Overrides default mechanism to support existing entities
   * not referenced in map table.
   *
   * @param array $source_id
   *   Source id map.
   *
   * @return mixed
   *   Map entry.
   */
  public function getRowBySource(array $source_id) {
    $row = parent::getRowBySource($source_id);

    if ($row === FALSE && ($entities = entity_load($this->destinationEntityType, FALSE, $source_id))) {
      $entity = reset($entities);
      $row    = array();

      foreach ($this->destinationKeyMap as $destination_col => $map_col) {
        $row[$map_col] = $entity->$destination_col;
      }

      // Fix values for new entry.
      $dest_ids = $row;

      // Add default values.
      $row += array(
        'needs_update'    => MigrateMap::STATUS_NEEDS_UPDATE,
        'rollback_action' => MigrateMap::ROLLBACK_DELETE,
        'last_imported'   => REQUEST_TIME,
        'hash'            => NULL,
      );

      foreach ($this->sourceKeyMap as $source_col => $map_col) {
        $row[$map_col] = $source_id[$source_col];
      }

      // Save new map.
      $this->saveIDMapping($entity, $dest_ids, $row['needs_update'], $row['rollback_action'], $row['hash']);
    }

    return $row;
  }
}
