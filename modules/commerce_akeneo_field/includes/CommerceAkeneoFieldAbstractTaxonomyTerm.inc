<?php

/**
 * @file
 * Plugin file to handle field.
 */

/**
 * Class CommerceAkeneoFieldAbstractTaxonomyTerm
 */
class CommerceAkeneoFieldAbstractTaxonomyTerm extends CommerceAkeneoFieldAbstract {
  /**
   * Map source and destination fields.
   *
   * @inheritdoc
   */
  public function addFieldMappingToMigrate($migrate, &$source_fields) {
    $field_name = $this->field['field_name'];

    // Default value field.
    $source_fields[$field_name . '_value'] = t('@field value', array('@field' => $this->fieldInstance['label']));
    $migrate->addFieldMapping($field_name, $field_name . '_value');

    // Default settings.
    $migrate->addFieldMapping($field_name . ':source_type')->defaultValue('');
    $migrate->addFieldMapping($field_name . ':create_term')->defaultValue(TRUE);
    $migrate->addFieldMapping($field_name . ':ignore_case')->defaultValue(FALSE);
    $migrate->addFieldMapping($field_name . ':machine_name')->defaultValue(TRUE);
  }

  /**
   * Prepare source row to match mapping.
   *
   * @inheritdoc
   */
  public function prepareRow($migrate, $row, $attributes) {
    $field_name = $this->field['field_name'];

    $properties = array(
      $field_name . '_value'     => array(),
      $field_name . '_languages' => array(),
    );

    foreach ($attributes as $code => $attribute) {
      if (isset($row->values[$code])) {
        foreach ($row->values[$code] as $locale => $values) {
          $language = ($this->field['translatable'] ?
            $language = commerce_akeneo_locale_to_language($locale) :
            LANGUAGE_NONE
          );

          foreach ($values as $value) {
            switch ($attribute['type']) {
              case 'pim_catalog_simpleselect':
              case 'pim_catalog_multiselect':
                $properties[$field_name . '_value'][$language][] = $value['code'];
                break;

              default:
                // Not supported.
            }
          }

          $properties[$field_name . '_languages'][$language] = $language;
        }
      }
    }

    $this->formatPrepareRowProperties($field_name, $row, $properties);

    return TRUE;
  }
}
