<?php

/**
 * @file
 * Commerce Akeneo Field Group API file.
 */

/**
 * Allow field name to be overridden.
 *
 * @param string $field_name
 *   Field name.
 * @param array  $attribute
 *   Attribute.
 */
function hook_commerce_akeneo_field_get_name_alter(&$field_name, $attribute) {

}

/**
 * Allow field label to be overridden.
 *
 * @param string $field_label
 *   Field label.
 * @param array  $attribute
 *   Attribute.
 */
function hook_commerce_akeneo_field_get_label_alter(&$field_label, $attribute) {

}

/**
 * Declare plugins to support Akeneo attribute type.
 *
 * @return array
 *   Plugins.
 */
function hook_commerce_akeneo_field_plugins_info() {
  return array(
    'akeneo_type' => array(
      'class'     => 'CommerceAkeneoFieldPluginPimCatalogNumber',
      'file'      => 'plugins/pim_catalog_number.inc',
      'file path' => drupal_get_path('module', 'foo'),
    ),
  );
}

/**
 * Interface CommerceAkeneoFieldPluginInterface
 */
interface CommerceAkeneoFieldPluginInterface {
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
   *   Fields details.
   */
  public function getFieldDetails($attribute, $bundle, $field_name, $field_label);
}

/**
 * Class CommerceAkeneoFieldPluginPimCatalogAbstract
 */
abstract class CommerceAkeneoFieldPluginPimCatalogAbstract implements CommerceAkeneoFieldPluginInterface {
  /**
   * Prepare field and instance according to Core API.
   *
   * @param string $name
   *   Field name.
   * @param string $bundle
   *   Bundle.
   * @param string $type
   *   Field type.
   * @param string $instance_type
   *   Instance type.
   * @param string $label
   *   Label.
   * @param bool   $translatable
   *   Translatable.
   *
   * @return array
   *   Field details.
   */
  protected function prepareFieldDetails(
    $name,
    $bundle,
    $type,
    $instance_type = NULL,
    $label = NULL,
    $translatable = FALSE
  ) {
    $field_info = field_info_field_types($type);

    $field = array(
      'field_name'   => $name,
      'type'         => $type,
      'translatable' => $translatable,
      'settings'     => isset($field_info['settings']) ? $field_info['settings'] : array(),
    );

    $field_instance = array(
      'field_name'    => $name,
      'entity_type'   => 'commerce_product',
      'label'         => $label ? $label : $field_info['label'],
      'description'   => isset($field_info['description']) ? $field_info['description'] : '',
      'bundle'        => $bundle,
      'required'      => FALSE,
      'settings'      => isset($field_info['instance_settings']) ? $field_info['instance_settings'] : array(),
      'widget'        => array(
        'type' => $instance_type ? $instance_type : $field_info['default_widget'],
      ),
      'default_value' => array(),
    );

    return array(
      'field'          => $field,
      'field_instance' => $field_instance,
    );
  }
}
