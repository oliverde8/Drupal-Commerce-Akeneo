<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_number',
  'title'      => t('Number'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogNumber',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogNumber
 */
class CommerceAkeneoAttributePimCatalogNumber extends CommerceAkeneoAttributeAbstract {
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
    $parameters     = isset($attribute['settings']['parameters']) ? $attribute['settings']['parameters'] : array();
    $localizable    = !empty($parameters['localizable']);
    $allow_decimale = isset($parameters['allow_decimale']) ? $parameters['allow_decimale'] : FALSE;
    $type           = $allow_decimale ? 'number_decimale' : 'number_integer';

    // Generic initialization.
    $field_details = $this->prepareFieldSettings(
      $field_name,
      $bundle,
      $type,
      'number',
      $field_label,
      $localizable
    );

    // Custom part based on Akeneo information.
    $field_details['field_instance']['settings']['min'] = isset($parameters['min_value']) ? $parameters['min_value'] : '';
    $field_details['field_instance']['settings']['max'] = isset($parameters['max_value']) ? $parameters['max_value'] : '';

    $field_details['field_instance']['default'] = array(
      array('value' => isset($parameters['default_value']) ? $parameters['default_value'] : ''),
    );

    return $field_details;
  }
}
