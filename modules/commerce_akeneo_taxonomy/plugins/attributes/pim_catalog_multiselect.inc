<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

// Add dependency.
require_once dirname(__FILE__) . '/pim_catalog_abstractselect.inc';

$plugin = array(
  'name'       => 'pim_catalog_multiselect',
  'title'      => t('Multi Select'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogMultiSelect',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogMultiSelect
 */
class CommerceAkeneoAttributePimCatalogMultiSelect extends CommerceAkeneoAttributePimCatalogAbstractSelect {
  /**
   * Perform matching for Akeneo attribute type with Drupal fields.
   *
   * @param array  $attribute
   *   Attribute.
   * @param string $bundle
   *   Bundle.
   * @param string $field_name
   *   Field name.
   * @param string $field_label
   *   Field label.
   *
   * @return array|bool
   *   Field details.
   */
  public function getFieldSettings($attribute, $bundle, $field_name, $field_label) {
    // Generic initialization.
    $field_details = parent::getFieldSettings($attribute, $bundle, $field_name, $field_label);

    // Custom part based on Akeneo information.
    $parameters = isset($attribute['parameters']) ? $attribute['parameters'] : array();

    $field_details['field']['cardinality'] = FIELD_CARDINALITY_UNLIMITED;

    return $field_details;
  }
}
