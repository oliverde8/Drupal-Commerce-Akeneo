<?php

/**
 * @file
 * Plugin file to handle field.
 */

$plugin = array(
  'name'       => 'product_association',
  'title'      => t('Product Association'),
  'weight'     => 10,
  'class_name' => 'CommerceAkeneoFieldProductAssociation',
  'settings'   => array(),
);

/**
 * Class CommerceAkeneoFieldProductAssociation
 */
class CommerceAkeneoFieldProductAssociation extends CommerceAkeneoFieldAbstractEntityReference {
  /**
   * Map source and destination fields.
   *
   * @inheritdoc
   */
  public function addFieldMappingToMigrate($migrate, &$source_fields) {
    $field_name = $this->field['field_name'];

    // Default value field.
    $source_fields[$field_name . '_value'] = t('@field value', array('@field' => $this->fieldInstance['label']));
    $migrate->addFieldMapping($field_name, $field_name . '_value')->defaultValue(array());

    return TRUE;
  }

  /**
   * Complete destination entity process.
   *
   * @inheritdoc
   */
  public function complete($migrate, $entity, $row, $attributes) {

    $field_name  = $this->field['field_name'];
    $deferred    = FALSE;
    $product_ids = array();

    // Lowercase keys.
    $row->associations = array_change_key_case($row->associations);

    foreach ($attributes as $code => $attribute) {
      // Import only products association.
      // (groups are not supported by this connector).
      if (isset($row->associations[$code]['products'])) {
        foreach ($row->associations[$code]['products'] as $sku) {
          if ($product_id = $this->getProductIdFromSku($sku)) {
            $product_ids[] = $product_id;
          }
          else {
            $deferred = TRUE;
          }
        }
      }
    }

    // Register entity to be processed again.
    if ($deferred) {
      /* @var MigrateAkeneoDestinationCommerceProduct $destination */
      $destination = $migrate->getDestination();
      $destination->addDeferredImports('complete', $entity->product_id, $entity, $row);
    }

    // Save product associations.
    $wrapper = entity_metadata_wrapper('commerce_product', $entity);
    $wrapper->{$field_name}->set($product_ids);
    $wrapper->save();

    return TRUE;
  }

  /**
   * Internal function to obtain product_id from sku.
   *
   * @param string $sku
   *   Product SKU.
   *
   * @return int
   *   Product id.
   */
  protected function getProductIdFromSku($sku) {
    static $sku_list = array();

    if (!isset($sku_list[$sku])) {
      $product = db_select('commerce_product', 'p')
        ->fields('p', array('product_id'))
        ->condition('sku', $sku)
        ->execute()
        ->fetchAssoc();

      if ($product) {
        $sku_list[$sku] = $product['product_id'];
      }
    }

    return isset($sku_list[$sku]) ? $sku_list[$sku] : NULL;
  }
}
