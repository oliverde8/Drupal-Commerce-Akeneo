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
   * @param Migration $migrate
   * @param array     $sourceFields
   *
   * @return bool
   */
  public function addFieldMappingToMigrate($migrate, &$sourceFields) {
    $field_name = $this->field['field_name'];

    // Default value field.
    $sourceFields[$field_name . '_value'] = t('@field value', array('@field' => $this->field_instance['label']));
    $migrate->addFieldMapping($field_name, $field_name . '_value')->defaultValue(array());

//    // Support for translations.
//    if ($this->field['translatable']) {
//      $sourceFields[$field_name . '_languages'] = t('@field languages', array('@field' => $this->field_instance['label']));
//      $migrate->addFieldMapping($field_name . ':language', $field_name . '_languages')->defaultValue(array());
//    }

    return TRUE;
  }

  /**
   * @param Migration $migrate
   * @param stdClass  $entity
   * @param string    $row
   * @param array     $attributes
   *
   * @return bool
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
      /** @var MigrateAkeneoDestinationCommerceProduct $destination */
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
   * @param string $sku
   *
   * @return int
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
