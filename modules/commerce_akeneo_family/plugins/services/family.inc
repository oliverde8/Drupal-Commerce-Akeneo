<?php

/**
 * @file
 * Plugin file to expose service.
 */

$plugin = array(
  'name'     => 'family',
  'title'    => t('Family'),
  'weight'   => 10,
  'settings' => array(
    'help'     => t('Synchronize a family.'),
    'callback' => 'commerce_akeneo_family_services_family',
  ),
);

/**
 * Handle service request for family synchronize.
 *
 * @param array $request
 *   Request.
 *
 * @return object
 *   Confirmation message.
 */
function commerce_akeneo_family_services_family($request) {
  if (!isset($request['code']) || !is_scalar($request['code'])) {
    return services_error('Missing code.', 406);
  }

  if (!isset($request['labels']) || !is_array($request['labels'])) {
    return services_error('Missing labels.', 406);
  }

  if (!isset($request['attribute_groups']) || !is_array($request['attribute_groups'])) {
    return services_error('Missing attribute groups.', 406);
  }

  try {
    // Save family details into Drupal Queue for future Migrate handle.
    $queue = commerce_akeneo_queue_load('family', TRUE);

    if (!$queue->createItem($request)) {
      throw new CommerceAkeneoException('Error while trying to add item to queue.', 500);
    }
  } catch (FieldException $e) {
    watchdog_exception('akeneo', $e);

    return services_error($e->getMessage(), 500);
  } catch (CommerceAkeneoException $e) {
    watchdog_exception('akeneo', $e);

    return services_error($e->getMessage(), $e->getCode() ? $e->getCode() : 500);
  } catch (Exception $e) {
    watchdog_exception('akeneo', $e);

    return services_error('Internal Server Error', 500);
  }

  return (object) $request;
}
