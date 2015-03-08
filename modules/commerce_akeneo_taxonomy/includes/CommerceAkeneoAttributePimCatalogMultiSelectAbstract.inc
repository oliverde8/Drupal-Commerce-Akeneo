<?php

/**
 * @file
 * CommerceAkeneoAttributePimCatalogMultiSelectAbstract class file.
 */

/**
 * Class CommerceAkeneoAttributePimCatalogMultiSelectAbstract
 */
class CommerceAkeneoAttributePimCatalogMultiSelectAbstract extends CommerceAkeneoAttributePimCatalogAbstractSelect {
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
   *   Field details.
   */
  public function getFieldSettings($attribute, $bundle, $field_name, $field_label) {
    // Generic initialization.
    $field_details = parent::getFieldSettings($attribute, $bundle, $field_name, $field_label);

    if ($field_details) {
      $field_details['field']['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
    }

    return $field_details;
  }
}
