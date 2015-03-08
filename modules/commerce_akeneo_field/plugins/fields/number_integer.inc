<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'number_integer',
  'title'      => t('Number integer'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldNumberInteger',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldNumberInteger
 */
class CommerceAkeneoFieldNumberInteger extends CommerceAkeneoFieldAbstract {
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

    // Support for translations.
    if ($this->field['translatable']) {
      $source_fields[$field_name . '_languages'] = t(
        '@field languages',
        array('@field' => $this->fieldInstance['label'])
      );
      $migrate->addFieldMapping($field_name . ':language', $field_name . '_languages')->defaultValue(array());
    }

    return TRUE;
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
              case 'pim_catalog_price_collection':
                $properties[$field_name . '_value'][$language][] = floatval($value['amount']);
                break;

              default:
                $properties[$field_name . '_value'][$language][] = floatval($value['value']);
                break;
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
