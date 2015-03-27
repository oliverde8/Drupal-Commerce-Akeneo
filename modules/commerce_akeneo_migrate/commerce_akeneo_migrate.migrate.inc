<?php

/**
 * @file
 * Migrate script registration.
 */

/**
 * Implements hook_migrate_api().
 */
function commerce_akeneo_migrate_migrate_api() {
  $api = array(
    'api'    => 2,
    'groups' => array(),
  );

  commerce_akeneo_migrate_register_category($api);
  commerce_akeneo_migrate_register_option($api);
  commerce_akeneo_migrate_register_product($api);
  commerce_akeneo_migrate_register_product_display($api);

  return $api;
}

/**
 * Subscribe product migrate scripts.
 *
 * @param array $api
 *   Api.
 */
function commerce_akeneo_migrate_register_category(&$api) {
  $migrations = array();
  $categories = commerce_akeneo_taxonomy_load_all('category');

  foreach ($categories as $category) {
    $name = str_replace('_', ' ', $category['code']);
    $name = str_replace(' ', '', ucwords($name));

    $machine_name = 'CommerceAkeneoMigrateTaxonomyCategory' . $name;

    $migrations[$machine_name] = array(
      'machine_name' => $machine_name,
      'class_name'   => 'CommerceAkeneoMigrateTaxonomy',
      'description'  => t('Import category from Akeneo: %code.', array('%code' => $category['code'])),
      'type'         => 'category',
      'code'         => $category['code'],
      'bundle'       => $category['vocabulary'],
    );
  }

  if ($migrations) {
    $api['groups'][COMMERCE_AKENEO_MIGRATE_GROUP_NAME_CATEGORY]['title'] = t('Commerce Akeneo Category');

    // Common arguments.
    $common_arguments = array(
      'group_name' => COMMERCE_AKENEO_MIGRATE_GROUP_NAME_CATEGORY,
    );

    // Register all the product migrations.
    foreach ($migrations as $code => $migration) {
      $arguments = array_merge_recursive($migration, $common_arguments);

      $api['migrations'][$code] = $arguments;
    }
  }
}

/**
 * Subscribe product migrate scripts.
 *
 * @param array $api
 *   Api.
 */
function commerce_akeneo_migrate_register_option(&$api) {
  $migrations = array();
  $options    = commerce_akeneo_taxonomy_load_all('option');

  foreach ($options as $option) {
    $name = str_replace('_', ' ', $option['code']);
    $name = str_replace(' ', '', ucwords($name));

    $machine_name = 'CommerceAkeneoMigrateTaxonomyOption' . $name;

    $migrations[$machine_name] = array(
      'machine_name' => $machine_name,
      'class_name'   => 'CommerceAkeneoMigrateTaxonomy',
      'description'  => t('Import option from Akeneo: %code.', array('%code' => $option['code'])),
      'type'         => 'option',
      'code'         => $option['code'],
      'bundle'       => $option['vocabulary'],
    );
  }

  if ($migrations) {
    $api['groups'][COMMERCE_AKENEO_MIGRATE_GROUP_NAME_OPTION]['title'] = t('Commerce Akeneo Option');

    // Common arguments.
    $common_arguments = array(
      'group_name' => COMMERCE_AKENEO_MIGRATE_GROUP_NAME_OPTION,
    );

    // Register all the product migrations.
    foreach ($migrations as $code => $migration) {
      $arguments = array_merge_recursive($migration, $common_arguments);
      $api['migrations'][$code] = $arguments;
    }
  }
}

/**
 * Subscribe product migrate scripts.
 *
 * @param array $api
 *   Api.
 */
function commerce_akeneo_migrate_register_product(&$api) {
  $migrations = array();
  $families   = commerce_akeneo_family_load_all();

  foreach ($families as $family) {
    $name = str_replace('_', ' ', $family->code);
    $name = str_replace(' ', '', ucwords($name));

    $machine_name = 'CommerceAkeneoMigrateProduct' . $name;

    $migrations[$machine_name] = array(
      'machine_name' => $machine_name,
      'class_name'   => 'CommerceAkeneoMigrateProduct',
      'description'  => t('Import products from Akeneo: %code.', array('%code' => $family->code)),
      'code'         => $family->code,
    );
  }

  if ($migrations) {
    $api['groups'][COMMERCE_AKENEO_MIGRATE_GROUP_NAME_PRODUCT]['title'] = t('Commerce Akeneo Product');

    // Common arguments.
    $common_arguments = array(
      'group_name' => COMMERCE_AKENEO_MIGRATE_GROUP_NAME_PRODUCT,
    );

    // Register all the product migrations.
    foreach ($migrations as $code => $migration) {
      $arguments = array_merge_recursive($migration, $common_arguments);

      $api['migrations'][$code] = $arguments;
    }
  }
}

/**
 * Subscribe product migrate scripts.
 *
 * @param array $api
 *   Api.
 */
function commerce_akeneo_migrate_register_product_display(&$api) {
  $migrations = array();
  $families   = commerce_akeneo_family_load_all();

  foreach ($families as $family) {
    $name = str_replace('_', ' ', $family->code);
    $name = str_replace(' ', '', ucwords($name));

    $machine_name = 'CommerceAkeneoMigrateProductDisplay' . $name;

    $migrations[$machine_name] = array(
      'machine_name' => $machine_name,
      'class_name'   => 'CommerceAkeneoMigrateProductDisplay',
      'description'  => t('Import products display from Akeneo: %code.', array('%code' => $family->code)),
      'code'         => $family->code,
    );
  }

  if ($migrations) {
    $api['groups'][COMMERCE_AKENEO_MIGRATE_GROUP_NAME_PRODUCT_DISPLAY]['title'] = t('Commerce Akeneo Product Display');

    // Common arguments.
    $common_arguments = array(
      'group_name' => COMMERCE_AKENEO_MIGRATE_GROUP_NAME_PRODUCT_DISPLAY,
    );

    // Register all the product migrations.
    foreach ($migrations as $code => $migration) {
      $arguments = array_merge_recursive($migration, $common_arguments);

      $api['migrations'][$code] = $arguments;
    }
  }
}
