<?php

/**
 * @file
 * MigrateSourceQueue class file.
 */

/**
 * Class MigrateSourceQueue
 */
class MigrateSourceQueue extends MigrateSource {
  /**
   * @var \DrupalReliableQueueInterface
   */
  protected $queue;

  /**
   * @var array
   */
  protected $fields;

  /**
   * @var array
   */
  protected $stack;

  /**
   * Constructor.
   *
   * @param string $queue_name
   *   Name of the queue.
   * @param array  $fields
   *   Fields.
   */
  public function __construct($queue_name, $fields) {
    $this->queue  = commerce_akeneo_queue_load($queue_name, FALSE);
    $this->fields = $fields;
    $this->stack  = array();

    parent::__construct();
  }

  /**
   * Destructor.
   */
  public function __destruct() {
    if ($this->queue) {
      $this->releaseItems();
    }
  }

  /**
   * Derived classes must implement fields(), returning a list fields.
   *
   * @return array
   *   Keys: machine names of the fields (to be passed to addFieldMapping)
   *   Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    return $this->fields;
  }

  /**
   * Return total count of items.
   *
   * @param bool $refresh
   *   Refresh.
   *
   * @return int
   *   Count of items.
   */
  public function count($refresh = FALSE) {
    return $this->queue->numberOfItems();
  }

  /**
   * Implementation of MigrateSource::performRewind().
   */
  public function performRewind() {
    $this->releaseItems();
  }

  /**
   * Implementation of MigrateSource::getNextRow().
   *
   * @return null|stdclass
   *   Next row.
   */
  public function getNextRow() {
    $lease_time = variable_get('commerce_akeneo_migrate_lease_time', COMMERCE_AKENEO_MIGRATE_DEFAULT_LEASE_TIME);

    if ($item = $this->queue->claimItem($lease_time)) {
      $this->stack[$item->item_id] = $item;

      $row = $item->data;

      // Set item_id.
      $row['migrate_source_item_id'] = $item->item_id;

      return (object) $row;
    }
    else {
      return NULL;
    }
  }

  /**
   * Give the calling migration a shot at manipulating, and possibly rejecting.
   *
   * @return bool
   *   FALSE if the row is to be skipped.
   */
  protected function prepareRow($row) {
    $return = parent::prepareRow($row);

    if (!$return) {
      // Remove ignored item from queue.
      $this->deleteItem($row->migrate_source_item_id);
    }

    return $return;
  }

  /**
   * Release all items in the current stack.
   */
  public function releaseItems() {
    foreach ($this->stack as $item) {
      $this->queue->releaseItem($item);
    }

    $this->stack = array();
  }

  /**
   * Delete the queue.
   */
  public function deleteQueue() {
    $this->queue->deleteQueue();
  }

  /**
   * Release item in the queue.
   *
   * @param int $item_id
   *   Item id.
   */
  public function releaseItem($item_id) {
    if (isset($this->stack[$item_id])) {
      $this->queue->releaseItem($this->stack[$item_id]);
    }
  }

  /**
   * Delete item in the queue.
   *
   * @param int $item_id
   *   Item id.
   */
  public function deleteItem($item_id) {
    if (isset($this->stack[$item_id])) {
      $this->queue->deleteItem($this->stack[$item_id]);
    }
  }

  /**
   * Magic function for internal use only.
   *
   * @return string
   *   Name of the class.
   */
  public function __toString() {
    return __CLASS__;
  }
}
