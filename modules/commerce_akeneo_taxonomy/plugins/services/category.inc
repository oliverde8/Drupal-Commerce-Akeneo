<?php

/**
 * @file
 * Plugin file to expose service.
 */

$plugin = array(
  'name'     => 'category',
  'title'    => t('Category'),
  'weight'   => 10,
  'settings' => array(
    'help'     => t('Synchronize categories from a catalog.'),
    'callback' => 'commerce_akeneo_taxonomy_services_category',
  ),
);

/**
 * Handle service request for category synchronize.
 *
 * @param array $request
 *   Request.
 *
 * @return object
 *   Confirmation message.
 */
function commerce_akeneo_taxonomy_services_category($request) {
  if (!isset($request['code']) || !is_scalar($request['code'])) {
    return services_error('Missing code.', 406);
  }

  if (!isset($request['labels']) || !is_array($request['labels'])) {
    return services_error('Missing labels.', 406);
  }

  if (!isset($request['children']) || !is_array($request['children'])) {
    return services_error('Missing children.', 406);
  }

  try {
    if (!$taxonomy = commerce_akeneo_taxonomy_load($request['code'], 'category')) {
      $taxonomy = (object) array(
        'code' => $request['code'],
        'type' => 'category',
      );
    }

    $taxonomy->label = commerce_akeneo_get_language($request['labels']);

    commerce_akeneo_taxonomy_save($taxonomy);

    if (!$vocabulary = taxonomy_vocabulary_machine_name_load($taxonomy->vocabulary)) {
      $vocabulary = (object) array(
        'machine_name' => $taxonomy->vocabulary,
        'description'  => t('Vocabulary created via Akeneo PIM.'),
      );
    }

    $vocabulary->name = $taxonomy->label;

    // Allow override.
    module_invoke_all('commerce_akeneo_taxonomy_vocabulary_save', $vocabulary, $request);

    // Write the vocabulary to the database.
    taxonomy_vocabulary_save($vocabulary);

    drupal_alter('commerce_akeneo_taxonomy_vocabulary_save', $vocabulary, $request);
  }
  catch (CommerceAkeneoException $e) {
    watchdog_exception('akeneo', $e);

    return services_error($e->getMessage(), $e->getCode() ? $e->getCode() : 500);
  }
  catch (Exception $e) {
    watchdog_exception('akeneo', $e);

    return services_error('Internal Server Error', 500);
  }

  return (object) $request;
}
