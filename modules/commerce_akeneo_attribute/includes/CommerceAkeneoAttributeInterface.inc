<?php

/**
 * @file
 * CommerceAkeneoAttributeInterface interface file.
 */

/**
 * Interface CommerceAkeneoAttributeInterface
 */
interface CommerceAkeneoAttributeInterface {
  /**
   * Perform matching for Akeneo attribute type with Drupal fields.
   *
   * @param array  $attribute
   *   Attribute.
   * @param string $bundle
   *   Bundle.
   * @param string $field_name
   *   Field name.
   * @param string $field_label
   *   Field label.
   *
   * @return array|bool
   *   Fields details.
   */
  public function getFieldSettings($attribute, $bundle, $field_name, $field_label);
}
