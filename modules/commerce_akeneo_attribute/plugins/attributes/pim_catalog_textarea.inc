<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_textarea',
  'title'      => t('Textarea'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogTextarea',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogTextarea
 */
class CommerceAkeneoAttributePimCatalogTextarea extends CommerceAkeneoAttributeAbstract {
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
      'text_long',
      'text_textarea',
      $field_label,
      $localizable
    );

    // Custom part based on Akeneo information.
    if (!empty($parameters['wysiwyg'])) {
      $field_details['field_instance']['settings']['text_processing'] = 1;
    }

    return $field_details;
  }
}
