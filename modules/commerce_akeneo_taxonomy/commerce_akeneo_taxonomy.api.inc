<?php

/**
 * @file
 * Commerce Akeneo Taxonomy API file.
 */

/**
 * Allow override of taxonomy vocabulary before database update.
 *
 * @param stdClass $vocabulary
 *   Vocabulary.
 * @param array    $request
 *   Service request.
 */
function hook_commerce_akeneo_taxonomy_category_alter(&$vocabulary, $request) {

}

/**
 * Allow override of taxonomy vocabulary before database update.
 *
 * @param stdClass $vocabulary
 *   Vocabulary.
 * @param array    $request
 *   Service request.
 */
function hook_commerce_akeneo_taxonomy_category($vocabulary, $request) {

}

/**
 * Allow override of taxonomy vocabulary before database update.
 *
 * @param $term
 *   Term.
 * @param $option
 *   Options of the term as sent by Akeneo.
 */
function hook_commerce_akeneo_taxonomy_term_save($term, $option) {

}
