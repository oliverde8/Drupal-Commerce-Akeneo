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
    commerce_akeneo_taxonomy_handle_service($request['code'], 'category', $request['labels'], $request['children']);
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
