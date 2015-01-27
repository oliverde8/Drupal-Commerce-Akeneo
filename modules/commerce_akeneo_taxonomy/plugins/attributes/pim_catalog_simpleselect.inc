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
class CommerceAkeneoAttributePimCatalogSimpleSelect extends CommerceAkeneoAttributeAbstract {
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

    $vocabulary = $this->createVocabularyIfMissing($attribute['code'], $field_label);

    $field_details['field']['settings']['allowed_values'] = array(
      array('vocabulary' => $vocabulary),
    );

    return $field_details;
  }

  /**
   * Create taxonomy vocabulary if missing.
   *
   * @param string $code
   *   Code.
   * @param string $field_label
   *   Field label.
   *
   * @return stdClass
   */
  protected function createVocabularyIfMissing($code, $field_label) {
    if ($taxonomy = commerce_akeneo_taxonomy_load($code, 'option')) {
      return $taxonomy->vocabulary;
    }
    else {
      // Load or create new taxonomy entry.
      if (!$taxonomy = commerce_akeneo_taxonomy_load($code, 'option')) {
        $taxonomy = (object) array(
          'code' => $code,
          'type' => 'option',
        );
      }

      $taxonomy->label = $field_label;

      // Save taxonomy entry and generate a vocabulary machine name.
      commerce_akeneo_taxonomy_save($taxonomy);

      // Create vocabulary is not exists.
      if (!taxonomy_vocabulary_machine_name_load($taxonomy->vocabulary)) {
        $vocabulary = (object) array(
          'machine_name' => $taxonomy->vocabulary,
          'name'         => $field_label,
          'description'  => t('Vocabulary created via Akeneo PIM.'),
          'i18n_mode'    => I18N_MODE_LOCALIZE,
        );

        // Write the vocabulary to the database.
        taxonomy_vocabulary_save($vocabulary);
      }

      return $taxonomy->vocabulary;
    }
  }
}
