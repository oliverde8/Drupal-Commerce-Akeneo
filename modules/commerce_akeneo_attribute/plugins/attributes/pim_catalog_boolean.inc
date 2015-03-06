<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_boolean',
  'title'      => t('Boolean'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogBoolean',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogBoolean
 */
class CommerceAkeneoAttributePimCatalogBoolean extends CommerceAkeneoAttributeAbstract {
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
      'list_boolean',
      'options_onoff',
      $field_label,
      $localizable
    );

    // Custom part based on Akeneo information.
    $field_details['field_instance']['widget']['settings']['display_label']  = TRUE;
    $field_details['field_instance']['widget']['settings']['allowed_values'] = array(
      0 => '',
      1 => $field_label,
    );

    return $field_details;
  }
}
