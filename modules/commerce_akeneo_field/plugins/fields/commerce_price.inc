<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'commerce_price',
  'title'      => t('Commerce price'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldCommercePrice',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldCommercePrice
 */
class CommerceAkeneoFieldCommercePrice extends CommerceAkeneoFieldAbstract {
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

    // Settings.
    $source_fields[$field_name . '_currency_code'] = t(
      '@field currency code',
      array('@field' => $this->fieldInstance['label'])
    );
    $migrate->addFieldMapping($field_name . ':currency_code', $field_name . '_currency_code')->defaultValue(
      commerce_default_currency()
    );

    $source_fields[$field_name . '_tax_rate'] = t('@field tax rate', array('@field' => $this->fieldInstance['label']));
    $migrate->addFieldMapping($field_name . ':tax_rate', $field_name . '_tax_rate')->defaultValue(0);

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
      $field_name . '_value'         => array(),
      $field_name . '_currency_code' => array(),
      $field_name . '_languages'     => array(),
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
                if (isset($value['amount']) && $value['currency']) {
                  $properties[$field_name . '_value'][$language][]         = floatval($value['amount']);
                  $properties[$field_name . '_currency_code'][$language][] = $value['currency'];
                }
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
