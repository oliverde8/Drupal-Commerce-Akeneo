<?php

/**
 * @file
 * CommerceAkeneoAttributeAbstract class file.
 */

/**
 * Class CommerceAkeneoAttributeAbstract
 */
abstract class CommerceAkeneoAttributeAbstract implements CommerceAkeneoAttributeInterface {
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
  protected function prepareFieldSettings(
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
