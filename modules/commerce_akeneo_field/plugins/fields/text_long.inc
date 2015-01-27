<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'text_long',
  'title'      => t('Text rich'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldTextLong',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldTextLong
 */
class CommerceAkeneoFieldTextLong extends CommerceAkeneoFieldText {
  /**
   * @param Migration $migrate
   * @param array     $sourceFields
   */
//  public function addFieldMappingToMigrate($migrate, &$sourceFields) {
//    $field_name = $this->field['field_name'];
//
//    // Default value field.
//    $sourceFields[$field_name . '_value'] = t('@field value', array('@field' => $this->field_instance['label']));
//    $migrate->addFieldMapping($field_name, $field_name . '_value');
//
//    // Support for translations.
//    if ($this->field['translatable']) {
//      $sourceFields[$field_name . '_languages'] = t('@field languages', array('@field' => $this->field_instance['label']));
//      $migrate->addFieldMapping($field_name . ':language', $field_name . '_languages')->defaultValue(array());
//    }
//  }
}
