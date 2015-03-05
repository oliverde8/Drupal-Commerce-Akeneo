<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

$plugin = array(
  'name'       => 'pim_catalog_multiselect',
  'title'      => t('Multi Select'),
  'class_name' => 'CommerceAkeneoAttributePimCatalogMultiSelect',
  'weight'     => 10,
);

/**
 * Class CommerceAkeneoAttributePimCatalogMultiSelect
 */
class CommerceAkeneoAttributePimCatalogMultiSelect extends CommerceAkeneoAttributePimCatalogMultiSelectAbstract {

}
