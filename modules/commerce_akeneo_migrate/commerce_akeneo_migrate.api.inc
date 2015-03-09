<?php

/**
 * @file
 * Commerce Akeneo Migrate API file.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Use this hook to implement your own logic before creating entity.
 *
 * @param object $row
 *   Source row.
 *
 * @return bool
 *   TRUE if the product can be processed, FALSE otherwise.
 */
function hook_commerce_akeneo_migrate_product_prepare_row($row) {
  // Special case for T-Shirt family products.
  if ($row->family == 'tshirt') {
    // Compose product title using 2 fields.
    $label = '';

    if (!empty($row->akeneo_product_line_value)) {
      $label .= commerce_akeneo_get_language($row->akeneo_product_line_value, 'fr-FR');
    }

    if (!empty($row->akeneo_product_short_name_value)) {
      $label .= ' - ' . commerce_akeneo_get_language($row->akeneo_product_short_name_value, 'fr-FR');
    }

    if (trim($label, ' -')) {
      $row->label = trim($label, ' -');
    }

    // Override product price.
    $row->commerce_price_value         = $row->facing_price_value;
    $row->commerce_price_currency_code = 'EUR';
    $row->commerce_price_tax_rate      = 20;
  }

  return TRUE;
}

/**
 * @} End of "addtogroup hooks".
 */
