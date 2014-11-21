<?php

/**
 * @file
 * Resource handler for Akeneo product services.
 */

/**
 * Handle service request for product.
 *
 * @param mixed $product
 *   Product.
 *
 * @return object
 *   Confirmation message.
 */
function _commerce_akeneo_resource_product($product) {
  return (object) array(
    'id' => '42',
  );
}
