<?php

/**
 * @file
 * MigrateSourceQueue class file.
 */

/**
 * Class MigrateSourceGroupsQueue
 */
class MigrateSourceGroupsQueue extends MigrateSource {
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

  protected $groups;
  protected $groupKeys;
  protected $currentGroup;

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

    // Group elements.
    $this->groups = array();
    while ($item = $this->queue->claimItem(30)) {
      $group_id = $this->getGroupeIdfromQueueItem($item);
      if (!isset($this->groups[$group_id])) {
        $this->groups[$group_id] = (object) $item->data;
        $this->groups[$group_id]->product_ids = array();
        $this->groups[$group_id]->queue_items = array();
        $this->groupKeys[] = $group_id;
      }
      $this->groups[$group_id]->product_ids[] = $item->data['product_id'];
      $this->groups[$group_id]->queue_items[] = $item;
    }

    $this->releaseItems();

    $this->currentGroup = 0;
    parent::__construct();
  }

  /**
   * Get the grouping identifier for a queue item.
   *
   * @param object $item
   *   The item.
   *
   * @return string
   *   The id.
   */
  protected function getGroupeIdfromQueueItem($item) {
    if (empty($item->data['product_row']->groups)) {
      return $item->data['product_id'];
    }
    else {
      // Only work with first group.
      $keys = array_keys($item->data['product_row']->groups);
      return $item->data['product_row']->groups[$keys[0]]['code'];
    }
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
    return count($this->groups);
  }

  /**
   * Implementation of MigrateSource::performRewind().
   */
  public function performRewind() {
    $this->releaseItems();
    $this->currentGroup = 0;
  }

  /**
   * Implementation of MigrateSource::getNextRow().
   *
   * @return null|stdclass
   *   Next row.
   */
  public function getNextRow() {

    if (isset($this->groupKeys[$this->currentGroup])) {
      $row = $this->groups[$this->groupKeys[$this->currentGroup]];
      $this->currentGroup++;
      return $row;
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
      foreach ($row->queue_items as $item) {
        // Remove ignored item from queue.
        $this->deleteItem($item);
      }
    }

    return $return;
  }

  /**
   * Release all items in the current stack.
   */
  public function releaseItems() {
    foreach ($this->groups as $group) {
      foreach ($group->queue_items as $item) {
        $this->queue->releaseItem($item);
      }
    }

    $this->currentGroup = 0;
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
   * @param object $group
   *   Item id.
   */
  public function releaseItem($group) {
    foreach ($group->queue_items as $item) {
      $this->queue->releaseItem($item);
    }
  }

  /**
   * Delete item in the queue.
   *
   * @param object $group
   *   Item id.
   */
  public function deleteItem($group) {
    foreach ($group->queue_items as $item) {
      $this->queue->deleteItem($item);
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
