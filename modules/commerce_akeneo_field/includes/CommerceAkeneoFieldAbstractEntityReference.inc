<?php

/**
 * @file
 * Plugin file to handle field.
 */

/**
 * Class CommerceAkeneoFieldAbstractEntityReference
 */
class CommerceAkeneoFieldAbstractEntityReference extends CommerceAkeneoFieldAbstract {
  /**
   * @param Migration $migrate
   * @param array     $sourceFields
   *
   * @return bool
   */
  public function addFieldMappingToMigrate($migrate, &$sourceFields) {
    $field_name = $this->field['field_name'];

    // Default value field.
    $sourceFields[$field_name . '_value'] = t('@field value', array('@field' => $this->field_instance['label']));
    $migrate->addFieldMapping($field_name, $field_name . '_value');

    return TRUE;
  }
}
