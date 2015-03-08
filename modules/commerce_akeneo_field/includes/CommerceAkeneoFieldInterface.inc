<?php

/**
 * @file
 */

/**
 * Interface CommerceAkeneoFieldInterface
 */
interface CommerceAkeneoFieldInterface {
  /**
   * @param Migration $migrate
   * @param array     $source_fields
   *
   * @return bool
   */
  public function addFieldMappingToMigrate($migrate, &$source_fields);

  /**
   * @param Migration $migrate
   * @param string    $row
   * @param array     $attributes
   *
   * @return bool
   */
  public function prepareRow($migrate, $row, $attributes);

  /**
   * @param Migration $migrate
   * @param stdClass  $entity
   * @param string    $row
   * @param array     $attributes
   *
   * @return bool
   */
  public function prepare($migrate, $entity, $row, $attributes);

  /**
   * @param Migration $migrate
   * @param stdClass  $entity
   * @param string    $row
   * @param array     $attributes
   *
   * @return bool
   */
  public function complete($migrate, $entity, $row, $attributes);
}
