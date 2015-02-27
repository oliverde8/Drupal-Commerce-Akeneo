<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

/**
 * Class CommerceAkeneoFieldPluginPimCatalogSimpleselect
 */
class CommerceAkeneoFieldPluginPimCatalogSimpleselect extends CommerceAkeneoFieldPluginPimCatalogAbstract {
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
    $field_details = $this->prepareFieldDetails(
      $field_name,
      $bundle,
      'taxonomy_term_reference',
      'taxonomy_autocomplete',
      $field_label
    );

    // Custom part based on Akeneo information.
    $parameters = isset($attribute['parameters']) ? $attribute['parameters'] : array();

    $field_details['field']['cardinality']                = 1;
    $field_details['field']['settings']['allowed_values'] = array(
      array('vocabulary' => $field_name),
    );

    return $field_details;
  }
}
