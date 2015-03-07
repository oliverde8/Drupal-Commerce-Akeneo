<?php

/**
 * @file
 * Commerce Akeneo Field Class file.
 */
abstract class CommerceAkeneoFieldAbstract implements CommerceAkeneoFieldInterface {
  /**
   * @var array
   */
  protected $field;

  /**
   * @var array
   */
  protected $field_instance;

  /**
   * @param string $entity_type
   * @param string $bundle
   * @param string $field_name
   */
  public function __construct($entity_type, $bundle, $field_name) {
    $this->field          = field_info_field($field_name);
    $this->field_instance = field_info_instance($entity_type, $field_name, $bundle);
  }

  /**
   * @param Migration $migrate
   * @param array     $sourceFields
   *
   * @return bool
   */
  public function addFieldMappingToMigrate($migrate, &$sourceFields) {
    return TRUE;
  }

  /**
   * @param Migration $migrate
   * @param string    $row
   * @param array     $attributes
   *
   * @return bool
   */
  public function prepareRow($migrate, $row, $attributes) {
    return TRUE;
  }

  /**
   * @param Migration $migrate
   * @param stdClass  $entity
   * @param string    $row
   * @param array     $attributes
   *
   * @return bool
   */
  public function prepare($migrate, $entity, $row, $attributes) {
    return TRUE;
  }

  /**
   * @param Migration $migrate
   * @param stdClass  $entity
   * @param string    $row
   * @param array     $attributes
   *
   * @return bool
   */
  public function complete($migrate, $entity, $row, $attributes) {
    return TRUE;
  }

  /**
   * @param string   $field_name
   * @param stdClass $row
   * @param array    $properties
   */
  protected function formatPrepareRowProperties($field_name, $row, $properties) {
    if ($this->field['cardinality'] == 1) {
      foreach ($properties as $property => $values) {
        if ($property != $field_name . '_languages') {
          $properties[$property] = array_map('reset', $properties[$property]);
        }
      }
    }

    // Handle translation support.
    if (!$this->field['translatable']) {
      unset($properties[$field_name . '_languages']);

      // Removes language level.
      foreach ($properties as $property => $values) {
        $properties[$property] = reset($properties[$property]);
      }
    }
    else {
      $properties[$field_name . '_languages'] = array_values($properties[$field_name . '_languages']);
    }

    // Map properties on final row.
    foreach ($properties as $property => $values) {
      $row->$property = $values;
    }
  }
}
