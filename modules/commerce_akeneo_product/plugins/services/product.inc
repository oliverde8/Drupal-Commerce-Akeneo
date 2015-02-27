<?php

/**
 * @file
 * Plugin file to expose service.
 */

$plugin = array(
  'name'     => 'product',
  'title'    => t('Product'),
  'weight'   => 10,
  'settings' => array(
    'help'     => t("Save a product based on it's SKU."),
    'callback' => 'commerce_akeneo_product_services_product',
  ),
);

/**
 * Handle service request for product synchronize.
 *
 * @param array $request
 *   Request.
 *
 * @return object
 *   Confirmation message.
 */
function commerce_akeneo_product_services_product($request) {
  if (!isset($request['sku']) || !is_scalar($request['sku'])) {
    return services_error('Missing SKU.', 406);
  }

  if (!isset($request['family']) || !is_string($request['family'])) {
    return services_error('Missing family.', 406);
  }

  if (!isset($request['labels']) || !is_array($request['labels'])) {
    return services_error('Missing labels.', 406);
  }

  if (!isset($request['values']) || !is_array($request['values'])) {
    return services_error('Missing values.', 406);
  }

  $family = check_plain($request['family']);

  if (strlen($family) > 25) {
    $family = substr($family, 0, 24) . '_';
    $family = preg_replace('/_$/', '', $family);
  }

  file_put_contents('/tmp/mylogs.log', $family."\n", FILE_APPEND);

  try {
    // Save product to Drupal Queue for future Migrate handle.
    $queue = commerce_akeneo_product_queue_load($family, TRUE);

    if (!$queue->createItem($request)) {
      throw new CommerceAkeneoException('Error while trying to add product to queue.', 500);
    }
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
