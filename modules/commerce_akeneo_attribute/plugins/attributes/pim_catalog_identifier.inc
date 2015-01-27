<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_identifier',
  'title'      => t('Identifier'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogIdentifier',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogIdentifier
 */
class CommerceAkeneoAttributePimCatalogIdentifier extends CommerceAkeneoAttributeAbstract {
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
    // Identifier in Drupal Commerce is a property not a field.
    return FALSE;
  }
}
