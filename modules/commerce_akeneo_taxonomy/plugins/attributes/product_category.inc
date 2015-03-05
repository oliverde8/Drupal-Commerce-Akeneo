<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'product_category',
  'title'      => t('Product Category'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogProductCategory',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogProductCategory
 */
class CommerceAkeneoAttributePimCatalogProductCategory extends CommerceAkeneoAttributePimCatalogMultiSelectAbstract {
  /**
   * Create taxonomy vocabulary if missing.
   *
   * @param string $code
   *   Code.
   * @param string $type
   *   Type.
   *
   * @return string|NULL
   */
  protected function loadVocabulary($code, $type = 'option') {
    return parent::loadVocabulary($code, 'category');
  }
}
