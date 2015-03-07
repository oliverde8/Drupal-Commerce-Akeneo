<?php

/**
 * @file
 * Plugin file to handle field.
 */

/**
 * Class CommerceAkeneoFieldAbstractEntityReference
 */
class CommerceAkeneoFieldAbstractEntityReference extends CommerceAkeneoFieldAbstract {
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

//    // Support for translations.
//    if ($this->field['translatable']) {
//      $sourceFields[$field_name . '_languages'] = t('@field languages', array('@field' => $this->field_instance['label']));
//      $migrate->addFieldMapping($field_name . ':language', $field_name . '_languages')->defaultValue(array());
//    }

    return TRUE;
  }

  public function prepareRow($migrate, $row, $attributes) {
    $field_name = $this->field['field_name'];

    $properties = array(
      $field_name . '_value'     => array(),
      $field_name . '_languages' => array(),
    );

//    foreach ($attributes as $code => $attribute) {
//      if (isset($row->values[$code])) {
//        foreach ($row->values[$code] as $locale => $values) {
//          $language = ($this->field['translatable'] ?
//            $language = commerce_akeneo_locale_to_language($locale) :
//            LANGUAGE_NONE
//          );
//
//          foreach ($values as $value) {
//            switch ($attribute['type']) {
//              case 'pim_catalog_metric':
//                $unit = units_unit_machine_name_load($value['unit']);
//
//                $properties[$field_name . '_value'][$language][]     = doubleval($value['value']);
//                $properties[$field_name . '_target_id'][$language][] = $unit->umid;
//                break;
//
//              default:
//                $properties[$field_name . '_value'][$language][]     = doubleval($value['value']);
//                $properties[$field_name . '_target_id'][$language][] = NULL;
//                break;
//            }
//          }
//
//          $properties[$field_name . '_languages'][$language] = $language;
//        }
//      }
//    }

    $this->formatPrepareRowProperties($field_name, $row, $properties);

    return TRUE;
  }
}
