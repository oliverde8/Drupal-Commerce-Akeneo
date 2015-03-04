<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_simpleselect',
  'title'      => t('Simple select'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogSimpleSelect',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogSimpleSelect
 */
class CommerceAkeneoAttributePimCatalogSimpleSelect extends CommerceAkeneoAttributePimCatalogAbstractSelect {
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

    return $field_details;
  }
}
