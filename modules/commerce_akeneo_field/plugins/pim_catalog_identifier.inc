<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

/**
 * Class CommerceAkeneoFieldPluginPimCatalogIdentifier
 */
class CommerceAkeneoFieldPluginPimCatalogIdentifier extends CommerceAkeneoFieldPluginPimCatalogAbstract {
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
   * @return array|bool|bool
   *   Field details.
   */
  public function getFieldDetails($attribute, $bundle, $field_name, $field_label) {
    // Identifier in Drupal Commerce is a property not a field.
    return FALSE;
  }
}
