<?php

/**
 * @file
 * Plugin file to match with attribute type from Akeneo.
 */

/**
 * Class CommerceAkeneoFieldPluginPimCatalogMetric
 */
class CommerceAkeneoFieldPluginPimCatalogMetric extends CommerceAkeneoFieldPluginPimCatalogAbstract {
  /**
   * Perform matching for Akeneo attribute type with Drupal fields.
   *
   * @param array  $attribute
   *   Attribute.
   * @param string $bundle
   *   Bundle.
   * @param string $field_name
   *   Field name.
   * @param string $field_label
   *   Field label.
   *
   * @return array|bool
   *   Field details.
   */
  public function getFieldDetails($attribute, $bundle, $field_name, $field_label) {
    $parameters     = isset($attribute['parameters']) ? $attribute['parameters'] : array();
    $allow_decimale = isset($parameters['allow_decimale']) ? $parameters['allow_decimale'] : FALSE;
    $type           = $allow_decimale ? 'mvf_number_decimal_entityreference' : 'mvf_number_integer_entityreference';

    $field_info = field_info_field('field_test');

    // Generic initialization.
    $field_details = $this->prepareFieldDetails($field_name, $bundle, $type, 'mvf_widget_default', $field_label);

    // Custom part based on Akeneo information.
    $metric_family = drupal_strtolower(
      $parameters['metric_family']
    );

    $field_details['field']['settings']['unit']['handler_settings'] = array(
      'target_bundles' => array(
        $metric_family => $metric_family,
      ),
      'sort'           => array(
        'type'      => 'property',
        'property'  => 'umid',
        'direction' => 'ASC',
      ),
      'behaviors'      => array(
        'views-select-list' => array(
          'status' => 0,
        ),
      ),
    );

    $field_details['field']['settings']['meta_info']['value']['field_type'] = $allow_decimale ? 'number_decimal' : 'number_integer';
    $field_details['field']['settings']['meta_info']['value']['formatter']  = $allow_decimale ? 'number_decimal' : 'number_integer';

    $field_details['field']['settings']['meta_info']['unit']['widget'] = 'options_select';

    return $field_details;
  }
}