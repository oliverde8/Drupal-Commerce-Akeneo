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
   * @param Migration $migrate
   * @param array     $sourceFields
   *
   * @return bool
   */
  public function addFieldMappingToMigrate($migrate, &$sourceFields) {
    parent::addFieldMappingToMigrate($migrate, $sourceFields);

    $field_name = $this->field['field_name'];
    $migrate->addFieldMapping($field_name . ':create_term', NULL, FALSE)->defaultValue(FALSE);

    return TRUE;
  }

  /**
   * @param \Migration $migrate
   * @param string     $row
   * @param array      $attributes
   *
   * @return bool
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
