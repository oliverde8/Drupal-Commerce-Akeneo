<?php

/**
 * @file
 * Resource handler for Akeneo product services.
 */

/**
 * Handle service request for family synchronize.
 *
 * @param array $request
 *   Request.
 *
 * @return object
 *   Confirmation message.
 */
function _commerce_akeneo_resource_family($request) {
  $code   = $request['code'];
  $labels = $request['labels'];

  // Build a like commerce product create form like.
  $product_type = array(
    'type'         => 'akeneo_' . $code,
    'name'         => reset($labels),
    'description'  => 'Product type created via Akeneo PIM.',
    'help'         => '',
    'revision'     => 1,
    'multilingual' => 4,
    'is_new'       => TRUE,
  );

  // Load product type from database.
  if ($product_type_old = commerce_product_type_load($product_type['type'])) {
    $product_type['is_new']      = FALSE;
    $product_type['description'] = $product_type_old['description'];
    $product_type['help']        = $product_type_old['help'];

    if (isset($product_type_old['multilingual'])) {
      $product_type['multilingual'] = $product_type_old['multilingual'];
    }
  }

  // Set the multilingual value for the product type
  // if entity translation is enabled.
  if (module_exists('entity_translation')) {
    variable_set('language_product_type_' . $product_type['type'], $product_type['multilingual']);
  }

  try {
    // Allow override.
    drupal_alter('commerce_akeneo_family', $product_type, $request);

    // Write the product type to the database.
    commerce_product_ui_product_type_save($product_type);

    module_invoke_all('commerce_akeneo_family', $product_type, $request);
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
