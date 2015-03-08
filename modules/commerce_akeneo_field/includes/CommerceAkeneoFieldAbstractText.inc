<?php

/**
 * @file
 * Plugin file to handle field.
 */

/**
 * Class CommerceAkeneoFieldAbstractText
 */
class CommerceAkeneoFieldAbstractText extends CommerceAkeneoFieldAbstract {
  /**
   * @param Migration $migrate
   * @param array     $sourceFields
   *
   * @return bool
   */
  public function addFieldMappingToMigrate($migrate, &$sourceFields) {
    $field_name = $this->field['field_name'];

    // Default value field.
    $sourceFields[$field_name . '_value'] = t('@field value', array('@field' => $this->field_instance['label']));
    $migrate->addFieldMapping($field_name, $field_name . '_value');

    // Settings.

    // Support for translations.
//    if ($this->field['translatable']) {
    $sourceFields[$field_name . '_languages'] = t(
      '@field languages',
      array('@field' => $this->field_instance['label'])
    );
    $migrate->addFieldMapping($field_name . ':language', $field_name . '_languages')->defaultValue(array());
//    }

    return TRUE;
  }

  /**
   * @param \Migration $migrate
   * @param string     $row
   * @param array      $attributes
   *
   * @return bool
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
              default:
                $properties[$field_name . '_value'][$language][] = $value['value'];
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
