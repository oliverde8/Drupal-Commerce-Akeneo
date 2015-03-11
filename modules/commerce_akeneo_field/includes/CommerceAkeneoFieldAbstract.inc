<?php

/**
 * @file
 * CommerceAkeneoFieldAbstract class file.
 */

/**
 * Class CommerceAkeneoFieldAbstract
 */
abstract class CommerceAkeneoFieldAbstract implements CommerceAkeneoFieldInterface {
  /**
   * @var array
   */
  protected $field;

  /**
   * @var array
   */
  protected $fieldInstance;

  /**
   * Constructor.
   *
   * @param string $entity_type
   *   Entity type.
   * @param string $bundle
   *   Bundle.
   * @param string $field_name
   *   Field name.
   */
  public function __construct($entity_type, $bundle, $field_name) {
    $this->field         = field_info_field($field_name);
    $this->fieldInstance = field_info_instance($entity_type, $field_name, $bundle);
  }

  /**
   * Map source and destination fields.
   *
   * @param Migration $migrate
   *   Migrate script.
   * @param array     $source_fields
   *   Source fields.
   *
   * @return bool
   *   TRUE if valid.
   */
  public function addFieldMappingToMigrate($migrate, &$source_fields) {
    return TRUE;
  }

  /**
   * Prepare source row to match mapping.
   *
   * @param Migration $migrate
   *   Migrate script.
   * @param string    $row
   *   Source row.
   * @param array     $attributes
   *   Attributes.
   *
   * @return bool
   *   TRUE if valid.
   */
  public function prepareRow($migrate, $row, $attributes) {
    return TRUE;
  }

  /**
   * Prepare destination entity.
   *
   * @param Migration $migrate
   *   Migrate script.
   * @param stdclass  $entity
   *   Destination entity.
   * @param string    $row
   *   Source row.
   * @param array     $attributes
   *   Attributes.
   *
   * @return bool
   *   TRUE if valid.
   */
  public function prepare($migrate, $entity, $row, $attributes) {
    return TRUE;
  }

  /**
   * Complete destination entity process.
   *
   * @param Migration $migrate
   *   Migrate script.
   * @param stdclass  $entity
   *   Destination entity.
   * @param string    $row
   *   Source row.
   * @param array     $attributes
   *   Attributes.
   *
   * @return bool
   *   TRUE if valid.
   */
  public function complete($migrate, $entity, $row, $attributes) {
    return TRUE;
  }

  /**
   * Internal helper.
   *
   * @param string   $field_name
   *   Field name.
   * @param stdclass $row
   *   Source row.
   * @param array    $properties
   *   Properties.
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
      $properties[$field_name . '_languages'] = array(LANGUAGE_NONE);

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
