<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'product_association',
  'title'      => t('Product Association (Entity Reference)'),
  'class_name' => 'CommerceAkeneoAttributeProductAssociation',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributeProductAssociation
 */
class CommerceAkeneoAttributeProductAssociation extends CommerceAkeneoAttributeAbstract {
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
      'commerce_product_reference',
      'inline_entity_form',
      $field_label,
      $localizable
    );

    // Target entity.
    $field_details['field']['cardinality']             = FIELD_CARDINALITY_UNLIMITED;
    $field_details['field']['settings']['target_type'] = 'commerce_product';

    // Allow users to add existing products.
    $field_details['field_instance']['widget']['settings']['type_settings']['allow_existing'] = TRUE;
    $field_details['field_instance']['widget']['settings']['type_settings']['match_operator'] = 'CONTAINS';

    return $field_details;
  }
}
