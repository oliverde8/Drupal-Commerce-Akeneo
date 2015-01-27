<?php

/**
 * @file
 * Migrate class file.
 */

/*/*
 * Implements hook_migrate_api().
 */
function commerce_akeneo_migrate_migrate_api() {
  $migrations = array();

  $families = commerce_akeneo_family_load_all();

  foreach ($families as $family) {
    $name = str_replace('_', ' ', $family->code);
    $name = str_replace(' ', '', ucwords($name));

    $machine_name = 'CommerceAkeneoMigrateProduct' . $name;

    $migrations[$machine_name] = array(
      'machine_name' => $machine_name,
      'class_name'   => 'CommerceAkeneoMigrateProduct',
      'description'  => t('Import products from Akeneo: %type (%label).', array('%type' => $family->code)),
      'type'         => $family->code,
    );
  }

  $api = array(
    'api'    => 2,
    'groups' => array(
      COMMERCE_AKENEO_MIGRATE_GROUP_NAME => array(
        'title' => t('Commerce Akeneo'),
      ),
    ),
  );

  // Common arguments.
  $common_arguments = array(
    'group_name' => COMMERCE_AKENEO_MIGRATE_GROUP_NAME,
  );

  if ($migrations) {
    // Register all the product migrations.
    foreach ($migrations as $code => $migration) {
      $arguments = array_merge_recursive($migration, $common_arguments);

      $api['migrations'][$code] = $arguments;
    }
  }

  return $api;
}
