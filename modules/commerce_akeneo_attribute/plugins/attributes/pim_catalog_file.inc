<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_file',
  'title'      => t('File'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogFile',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogFile
 */
class CommerceAkeneoAttributePimCatalogFile extends CommerceAkeneoAttributeAbstract {
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
    $parameters  = isset($attribute['settings']['parameters']) ? $attribute['settings']['parameters'] : array();
    $localizable = !empty($parameters['localizable']);

    // Generic initialization.
    $field_details = $this->prepareFieldSettings(
      $field_name,
      $bundle,
      'file',
      'file_generic',
      $field_label,
      $localizable
    );

    // Custom part based on Akeneo information.
    if (count($parameters['allowed_extensions'])) {
      $field_details['field_instance']['file_extensions'] = implode(' ', $parameters['allowed_extensions']);
    }

    return $field_details;
  }
}
