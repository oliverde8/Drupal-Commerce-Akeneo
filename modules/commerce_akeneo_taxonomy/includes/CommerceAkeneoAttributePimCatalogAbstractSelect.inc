<?php

/**
 * @file
 * CommerceAkeneoAttributePimCatalogAbstractSelect class file.
 */

/**
 * Class CommerceAkeneoAttributePimCatalogAbstractSelect
 */
class CommerceAkeneoAttributePimCatalogAbstractSelect extends CommerceAkeneoAttributeAbstract {
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
    $parameters  = isset($attribute['parameters']) ? $attribute['parameters'] : array();
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

    if ($vocabulary = $this->loadVocabulary($attribute['code'])) {
      $field_details['field']['settings']['allowed_values'] = array(
        array('vocabulary' => $vocabulary, 'parent' => NULL),
      );

      return $field_details;
    }

    return NULL;
  }

  /**
   * Create taxonomy vocabulary if missing.
   *
   * @param string $code
   *   Code.
   * @param string $type
   *   Type.
   *
   * @return string
   *   Vocabulary machine name.
   */
  protected function loadVocabulary($code, $type = 'option') {
    if ($taxonomy = commerce_akeneo_taxonomy_load($code, $type)) {
      return $taxonomy['vocabulary'];
    }

    return NULL;
  }
}
