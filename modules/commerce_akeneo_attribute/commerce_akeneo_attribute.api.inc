<?php

/**
 * @file
 * Commerce Akeneo Attribute API file.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Declare attributes mapping to destination field.
 *
 * @return mixed
 *   Attributes.
 */
function hook_commerce_akeneo_attribute_mapping_info() {
  $attributes['sku'] = array(
    'names' => array('sku'),
    'skip'  => TRUE,
  );

  $attributes['pictures'] = array(
    'match'        => '/^pictures_[0-9]+$/',
    'type'         => 'pim_akeneo_image',
    'field_name'   => 'pictures',
    'field_locked' => TRUE,
  );

  return $attributes;
}

/**
 * Alter attributes mapping.
 *
 * @param array $attributes
 *   Attributes.
 */
function hook_commerce_akeneo_attribute_mapping_info_alter(&$attributes) {

}

/**
 * @} End of "addtogroup hooks".
 */
