<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'mvf_number_integer_entityreference',
  'title'      => t('MVF Integer'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldMVFInteger',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldMVFInteger
 */
class CommerceAkeneoFieldMVFInteger extends CommerceAkeneoFieldAbstract {
  /**
   * Map source and destination fields.
   *
   * @inheritdoc
   */
  public function addFieldMappingToMigrate($migrate, &$source_fields) {
    $field_name = $this->field['field_name'];

    if (!empty($this->fieldInstance['default_value'][0]['target_id'])) {
      $target_id = $this->fieldInstance['default_value'][0]['target_id'];
    }
    elseif ($target_bundle = reset($this->field['settings']['unit']['handler_settings']['target_bundles'])) {
      $units = units_akeneo_family_load_all();

      // Unit machine name are always lowercase.
      $unit_name = strtolower($units[$target_bundle]['standard']);
      $unit      = units_unit_machine_name_load($unit_name);

      $target_id = NULL;
      if ($unit) {
        $target_id = $unit->umid;
      }
    }
    else {
      $target_id = NULL;
    }

    // Default value field.
    $source_fields[$field_name . '_value'] = t('@field value', array('@field' => $this->fieldInstance['label']));
    $migrate->addFieldMapping($field_name, $field_name . '_value');

    // Settings.
    $source_fields[$field_name . '_target_id'] = t('@field unit', array('@field' => $this->fieldInstance['label']));
    $migrate->addFieldMapping($field_name . ':target_id', $field_name . '_target_id')->defaultValue($target_id);

    // Support for translations.
    if ($this->field['translatable']) {
      $source_fields[$field_name . '_languages'] = t('@field languages', array('@field' => $this->fieldInstance['label']));
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
      $field_name . '_target_id' => array(),
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
              case 'pim_catalog_metric':
                // Unit machine name are always lowercase.
                $unit_name = strtolower($value['unit']);
                $unit      = units_unit_machine_name_load($unit_name);

                $properties[$field_name . '_value'][$language][]     = floatval($value['value']);
                $properties[$field_name . '_target_id'][$language][] = $unit->umid;
                break;

              default:
                $properties[$field_name . '_value'][$language][]     = floatval($value['value']);
                $properties[$field_name . '_target_id'][$language][] = NULL;
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
