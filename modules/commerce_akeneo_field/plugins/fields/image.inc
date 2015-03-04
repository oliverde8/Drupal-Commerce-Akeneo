<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'image',
  'title'      => t('Image'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldImage',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldImage
 */
class CommerceAkeneoFieldImage extends CommerceAkeneoFieldAbstract {
  /**
   * @param Migration $migrate
   * @param array     $sourceFields
   */
  public function addFieldMappingToMigrate($migrate, &$sourceFields) {
    $field_name = $this->field['field_name'];

    // Default value field.
    $sourceFields[$field_name . '_value'] = t('@field value', array('@field' => $this->field_instance['label']));
    $migrate->addFieldMapping($field_name, $field_name . '_value');

    // Settings.
    $migrate->addFieldMapping($field_name . ':file_class')->defaultValue(NULL);
    $migrate->addFieldMapping($field_name . ':preserve_files')->defaultValue(TRUE);
    $migrate->addFieldMapping($field_name . ':file_replace')->defaultValue(FILE_EXISTS_REPLACE);
    $migrate->addFieldMapping($field_name . ':source_dir')->defaultValue(NULL);
    $migrate->addFieldMapping($field_name . ':urlencode')->defaultValue(TRUE);

    $uri_scheme      = $this->field['settings']['uri_scheme'];
    $file_directory  = $this->field_instance['settings']['file_directory'];
    $destination_dir = file_stream_wrapper_uri_normalize($uri_scheme . '://' . $file_directory);
    $migrate->addFieldMapping($field_name . ':destination_dir')->defaultValue($destination_dir);

    $sourceFields[$field_name . '_destination_file'] = t(
      '@field destination file',
      array('@field' => $this->field_instance['label'])
    );
    $migrate->addFieldMapping($field_name . ':destination_file', $field_name . '_destination_file')->defaultValue(NULL);

    $sourceFields[$field_name . '_alt'] = t('@field alt', array('@field' => $this->field_instance['label']));
    $migrate->addFieldMapping($field_name . ':alt', $field_name . '_alt')->defaultValue(NULL);

    $sourceFields[$field_name . '_title'] = t('@field title', array('@field' => $this->field_instance['label']));
    $migrate->addFieldMapping($field_name . ':title', $field_name . '_title')->defaultValue(NULL);

    // Support for translations.
    $sourceFields[$field_name . '_languages'] = t(
      '@field languages',
      array('@field' => $this->field_instance['label'])
    );
    $migrate->addFieldMapping($field_name . ':language', $field_name . '_languages')->defaultValue(array());
  }

  public function prepareRow($migrate, $row, $attributes) {
    $field_name = $this->field['field_name'];

    $default_properties = array(
      $field_name . '_value'            => '',
      $field_name . '_destination_file' => '',
      $field_name . '_alt'              => '',
      $field_name . '_title'            => '',
    );

    $properties = array(
      $field_name . '_value'            => array(),
      $field_name . '_destination_file' => array(),
      $field_name . '_alt'              => array(),
      $field_name . '_title'            => array(),
      $field_name . '_languages'        => array(),
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
              case 'pim_catalog_file':
              case 'pim_catalog_image':
                // Change url protocol to enable Akeneo stream wrapper.
                $url = preg_replace('/^http/i', 'akeneo', $value['rest_url']);

                $properties[$field_name . '_value'][$language][]            = $url;
                $properties[$field_name . '_destination_file'][$language][] = $value['filename_original'];
                $properties[$field_name . '_alt'][$language][]              = $value['filename_original'];
                $properties[$field_name . '_title'][$language][]            = $value['filename_original'];
                break;

              default:
                // Not supported.
            }
          }

          $properties[$field_name . '_languages'][$language] = $language;
        }
      }
    }

    $this->formatPrepareRowProperties($field_name, $row, $properties, $default_properties);

    return TRUE;
  }

  public function prepare($migrate, $entity, $row, $attributes) {
    return TRUE;
//
//    global $user;
//
//    $field_name = $this->field['field_name'];
//
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
//              case 'pim_catalog_image':
//                $uri = preg_replace('/^http/i', 'akeneo', $value['rest_url']);
//
////                file_save()
//
////                file_save()
//
//                if ($files = file_load_multiple(NULL, array('uri' => $uri))) {
//                  $file = reset($files);
//                }
//                else {
//                  $file      = new stdClass();
//                  $file->fid = NULL;
//                  $file->uri = $uri;
//                }
//
//                $file->filename = drupal_basename($value['filename_original']);
//                $file->filemime = DrupalLocalStreamWrapper::getMimeType($value['filename']);
//                $file->filesize = (int) $value['length'];
//                $file->uid      = $user->uid;
//                $file->status   = FILE_STATUS_PERMANENT;
//
//                // Avoid warning about file size call.
//                $image = file_save_akeneo($file);
//
//                $entity->{$field_name}[$language][] = array(
//                  'fid' => $image->fid,
//                );
//                break;
//
//              default:
//                // Not supported.
//            }
//          }
//        }
//      }
//    }
//
////    var_dump($properties);die;
////    var_dump($row);die;
//
//    return TRUE;
  }
}
