<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_simpleselect',
  'title'      => t('Simple select'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogSimpleSelect',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogSimpleSelect
 */
class CommerceAkeneoAttributePimCatalogSimpleSelect extends CommerceAkeneoAttributePimCatalogAbstractSelect {
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
      'taxonomy_term_reference',
      'taxonomy_autocomplete',
      $field_label,
      $localizable
    );

    // Custom part based on Akeneo information.
    $field_details['field']['cardinality'] = 1;

    $vocabulary = $this->createVocabularyIfMissing($attribute['code'], $field_label);

    $field_details['field']['settings']['allowed_values'] = array(
      array('vocabulary' => $vocabulary),
    );

    return $field_details;
  }
}
