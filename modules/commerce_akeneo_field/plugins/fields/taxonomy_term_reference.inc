<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'taxonomy_term_reference',
  'title'      => t('Taxonomy term'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldTaxonomyTerm',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldTaxonomyTerm
 */
class CommerceAkeneoFieldTaxonomyTerm extends CommerceAkeneoFieldAbstractTaxonomyTerm {

}
