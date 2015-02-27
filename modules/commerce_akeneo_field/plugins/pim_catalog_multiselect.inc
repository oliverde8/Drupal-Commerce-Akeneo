<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

// Include class dependency.
module_load_include('inc', 'commerce_akeneo_field', 'plugins/pim_catalog_simpleselect');

/**
 * Class CommerceAkeneoFieldPluginPimCatalogMultiselect
 */
class CommerceAkeneoFieldPluginPimCatalogMultiselect extends CommerceAkeneoFieldPluginPimCatalogSimpleselect {
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
  public function getFieldDetails($attribute, $bundle, $field_name, $field_label) {
    // Generic initialization.
    $field_details = parent::getFieldDetails($attribute, $bundle, $field_name, $field_label);

    // Custom part based on Akeneo information.
    $parameters = isset($attribute['parameters']) ? $attribute['parameters'] : array();

    $field_details['field']['cardinality'] = FIELD_CARDINALITY_UNLIMITED;

    return $field_details;
  }
}
