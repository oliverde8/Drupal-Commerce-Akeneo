<?php

/**
 * @file
 * Plugin file to expose service.
 */

$plugin = array(
  'name'     => 'group',
  'title'    => t('Group'),
  'weight'   => 10,
  'settings' => array(
    'help'     => t('Store product groups.'),
    'callback' => 'commerce_akeneo_migrate_services_group',
  ),
);

/**
 * Handle service request for product association.
 *
 * @param array $request
 *   Request.
 *
 * @return object
 *   Confirmation message.
 */
function commerce_akeneo_migrate_services_group($request) {
//  if (!isset($request['sku']) || !is_scalar($request['sku'])) {
//    return services_error('Missing SKU.', 406);
//  }
//
//  if (!isset($request['family']) || !is_string($request['family'])) {
//    return services_error('Missing family.', 406);
//  }
//
//  if (!isset($request['associations']) || !is_array($request['associations'])) {
//    return services_error('Missing associations.', 406);
//  }

  try {
    // Secure family name.
//    $family = check_plain($request['family']);

    // Save product to Drupal Queue for future Migrate handle.
    $queue = commerce_akeneo_queue_load('group', TRUE);

    if (!$queue->createItem($request)) {
      throw new CommerceAkeneoException('Error while trying to add product group to queue.', 500);
    }

    // Reload migrate job list.
    migrate_static_registration();
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
