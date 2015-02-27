<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

/**
 * Class CommerceAkeneoFieldPluginPimCatalogImage
 */
class CommerceAkeneoFieldPluginPimCatalogImage extends CommerceAkeneoFieldPluginPimCatalogAbstract {
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
    $field_details = $this->prepareFieldDetails($field_name, $bundle, 'image', 'image_image', $field_label);

    // Custom part based on Akeneo information.
    $parameters = isset($attribute['parameters']) ? $attribute['parameters'] : array();

    if (count($parameters['allowed_extensions'])) {
      $field_details['field_instance']['file_extensions'] = implode(' ', $parameters['allowed_extensions']);
    }

    return $field_details;
  }
}
