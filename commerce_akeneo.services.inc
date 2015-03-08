<?php

/**
 * @file
 * Module file for Commerce Akeneo module.
 */

/**
 * Implements hook_services_resources().
 */
function commerce_akeneo_services_resources() {
  ctools_include('context');
  ctools_include('plugins');

  $resources = array(
    'akeneo' => array(
      'actions' => array(),
    ),
  );

  $services = ctools_get_plugins('commerce_akeneo', 'services');

  if (is_array($services)) {
    foreach ($services as $code => $service) {
      $relative_path = substr($service['path'], strlen(drupal_get_path('module', $service['module'])) + 1);

      $settings = $service['settings'];

      // Add default settings.
      $settings += array(
        'access callback'         => 'commerce_akeneo_resource_access',
        'access arguments'        => array($code),
        'access arguments append' => TRUE,
        'args'                    => array(
          array(
            'name'        => 'request',
            'type'        => 'struct',
            'description' => t('Request'),
            'source'      => 'data',
            'optional'    => FALSE,
          ),
        ),
        'file'                    => array(
          'type'   => 'inc',
          'module' => $service['module'],
          'name'   => $relative_path . '/' . $code,
        ),
      );

      $resources['akeneo']['actions'][$code] = $settings;
    }
  }

  drupal_alter('commerce_akeneo_services_resources', $resources);

  return $resources;
}

/**
 * Implements hook_default_services_endpoint().
 */
function commerce_akeneo_default_services_endpoint() {
  $endpoints = array();

  // Begin exported service endpoint.
  $endpoint                  = new stdClass();
  $endpoint->disabled        = TRUE;
  $endpoint->api_version     = 3;
  $endpoint->name            = 'akeneo';
  $endpoint->server          = 'rest_server';
  $endpoint->path            = 'json';
  $endpoint->authentication  = array(
    'services' => 'services',
  );
  $endpoint->server_settings = array(
    'formatters' => array(
      'json'    => TRUE,
      'bencode' => FALSE,
      'jsonp'   => FALSE,
      'php'     => FALSE,
      'xml'     => FALSE,
      'yaml'    => FALSE,
    ),
    'parsers'    => array(
      'application/json'                  => TRUE,
      'application/vnd.php.serialized'    => FALSE,
      'application/x-www-form-urlencoded' => FALSE,
      'application/x-yaml'                => FALSE,
      'application/xml'                   => FALSE,
      'multipart/form-data'               => FALSE,
      'text/xml'                          => FALSE,
    ),
  );

  $endpoint->resources = array(
    'akeneo' => array(
      'actions' => array(
        'product'     => array(
          'enabled' => '1',
        ),
        'group'       => array(
          'enabled' => '1',
        ),
        'family'      => array(
          'enabled' => '1',
        ),
        'category'    => array(
          'enabled' => '1',
        ),
        'option'      => array(
          'enabled' => '1',
        ),
      ),
    ),
    'user'   => array(
      'actions' => array(
        'login'  => array(
          'enabled' => '1',
        ),
        'logout' => array(
          'enabled' => '1',
        ),
        'token'  => array(
          'enabled' => '1',
        ),
      ),
    ),
  );

  $endpoint->debug = 0;

  $endpoints['akeneo'] = $endpoint;

  return $endpoints;
}

/**
 * Access callback for the note resource.
 *
 * @param string $op
 *   The operation that's going to be performed.
 * @param array  $args
 *   The arguments that will be passed to the callback.
 *
 * @return bool
 *   Whether access is given or not.
 */
function commerce_akeneo_resource_access($op, $args) {
  switch ($op) {
    case 'family':
      return user_access('resource family commerce akeneo');

    case 'category':
      return user_access('resource category commerce akeneo');

    case 'option':
      return user_access('resource option commerce akeneo');

    case 'product':
      return user_access('resource product commerce akeneo');

    case 'group':
      return user_access('resource group commerce akeneo');
  }

  return FALSE;
}
