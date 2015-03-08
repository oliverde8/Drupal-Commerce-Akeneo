<?php

/**
 * @file
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * @return mixed
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
 * @param array $attributes
 */
function hook_commerce_akeneo_attribute_mapping_info_alter(&$attributes) {

}

/**
 * @} End of "addtogroup hooks".
 */
