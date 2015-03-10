<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'product_category',
  'title'      => t('Product Category'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldProductCategory',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldProductCategory
 */
class CommerceAkeneoFieldProductCategory extends CommerceAkeneoFieldAbstractTaxonomyTerm {
  /**
   * Add destination fields.
   *
   * @param Migration $migrate
   *   Migrate script.
   * @param array     $source_fields
   *   Source fields.
   *
   * @return bool
   *   Valid.
   */
  public function addFieldMappingToMigrate($migrate, &$source_fields) {
    parent::addFieldMappingToMigrate($migrate, $source_fields);

    $field_name = $this->field['field_name'];
    $migrate->addFieldMapping($field_name . ':create_term', NULL, FALSE)->defaultValue(FALSE);

    return TRUE;
  }

  /**
   * Prepare row before importing.
   *
   * @param \Migration $migrate
   *   Migrate script.
   * @param string     $row
   *   Source row.
   * @param array      $attributes
   *   Attributes.
   *
   * @return bool
   *   TRUE if allowed to be imported.
   */
  public function prepareRow($migrate, $row, $attributes) {
    $field_name = $this->field['field_name'];

    $properties = array(
      $field_name . '_value'     => array(),
      $field_name . '_languages' => array(),
    );

    foreach ($attributes as $code => $attribute) {
      if (isset($row->categories[$code])) {
        foreach ($row->categories[$code] as $value) {
          $properties[$field_name . '_value'][LANGUAGE_NONE][] = $value;
        }

        $properties[$field_name . '_languages'][LANGUAGE_NONE] = LANGUAGE_NONE;
      }
    }

    $this->formatPrepareRowProperties($field_name, $row, $properties);

    return TRUE;
  }
}
