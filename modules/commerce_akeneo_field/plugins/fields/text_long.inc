<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'text_long',
  'title'      => t('Text rich'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldTextLong',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldTextLong
 */
class CommerceAkeneoFieldTextLong extends CommerceAkeneoFieldAbstractText {
}
