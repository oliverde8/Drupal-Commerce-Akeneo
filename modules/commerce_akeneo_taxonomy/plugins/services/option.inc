<?php

/**
 * @file
 * Plugin file to expose service.
 */

$plugin = array(
  'name'     => 'option',
  'title'    => t('Option'),
  'weight'   => 10,
  'settings' => array(
    'help'     => t('Synchronize an attribute option list.'),
    'callback' => 'commerce_akeneo_taxonomy_services_option',
  ),
);

/**
 * Handle service request for option synchronize.
 *
 * @param array $request
 *   Request.
 *
 * @return object
 *   Confirmation message.
 */
function commerce_akeneo_taxonomy_services_option($request) {
  if (!isset($request['code']) || !is_scalar($request['code'])) {
    return services_error('Missing code.', 406);
  }

  if (!isset($request['labels']) || !is_array($request['labels'])) {
    return services_error('Missing labels.', 406);
  }

  if (!isset($request['options']) || !is_array($request['options'])) {
    return services_error('Missing options.', 406);
  }

  try {
    commerce_akeneo_taxonomy_handle_service($request['code'], 'option', $request['labels'], $request['options']);
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
