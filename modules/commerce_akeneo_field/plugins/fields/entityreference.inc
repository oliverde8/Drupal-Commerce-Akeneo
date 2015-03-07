<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'entityreference',
  'title'      => t('Entity Reference'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldEntityReference',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldEntityReference
 */
class CommerceAkeneoFieldEntityReference extends CommerceAkeneoFieldAbstractEntityReference {

}
