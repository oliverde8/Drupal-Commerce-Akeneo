<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'text_with_summary',
  'title'      => t('Text with summary'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldTextWithSummary',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldTextWithSummary
 */
class CommerceAkeneoFieldTextWithSummary extends CommerceAkeneoFieldAbstractText {
  /**
   * Map source and destination fields.
   *
   * @inheritdoc
   */
  public function addFieldMappingToMigrate($migrate, &$source_fields) {
    $return = parent::addFieldMappingToMigrate($migrate, $source_fields);

    $field_name = $this->field['field_name'];

    // Default value field.
    $source_fields[$field_name . '_format'] = t('@field format', array('@field' => $this->fieldInstance['label']));
    $migrate->addFieldMapping($field_name . ':format', $field_name . '_format')->defaultValue('full_html');

    return $return;
  }
}
