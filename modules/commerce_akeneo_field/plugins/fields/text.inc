<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'text',
  'title'      => t('Text'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldText',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldText
 */
class CommerceAkeneoFieldText extends CommerceAkeneoFieldAbstractText {
}
