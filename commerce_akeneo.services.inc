<?php

/**
 * @file
 * Module file for Commerce Akeneo module.
 */

/**
 * Implements hook_services_resources().
 */
function commerce_akeneo_services_resources() {
  return array(
    'akeneo' => array(
      'actions' => array(
        'product'          => array(
          'help'                    => t("Save a product based on it's SKU."),
          'file'                    => array(
            'type'   => 'inc',
            'module' => 'commerce_akeneo',
            'name'   => 'resources/product_resource',
          ),
          'callback'                => '_commerce_akeneo_resource_product',
          'access callback'         => '_commerce_akeneo_resource_access',
          'access arguments'        => array('product'),
          'access arguments append' => TRUE,
          'args'                    => array(
            array(
              'name'        => 'product',
              'type'        => 'struct',
              'description' => t('A struct representing the product.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
          ),
        ),
        'family'           => array(
          'help'                    => t('Synchronize a family.'),
          'file'                    => array(
            'type'   => 'inc',
            'module' => 'commerce_akeneo',
            'name'   => 'resources/family_resource',
          ),
          'callback'                => '_commerce_akeneo_resource_family',
          'access callback'         => '_commerce_akeneo_resource_access',
          'access arguments'        => array('family'),
          'access arguments append' => TRUE,
          'args'                    => array(
            array(
              'name'        => 'code',
              'type'        => 'string',
              'description' => t('Code.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
            array(
              'name'        => 'labels',
              'type'        => 'struct',
              'description' => t('Label.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
            array(
              'name'        => 'attributes_sets',
              'type'        => 'struct',
              'description' => t('Attributes sets.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
          ),
        ),
        'category'         => array(
          'help'                    => t('Synchronize categories.'),
          'file'                    => array(
            'type'   => 'inc',
            'module' => 'commerce_akeneo',
            'name'   => 'resources/category_resource',
          ),
          'callback'                => '_commerce_akeneo_resource_category',
          'access callback'         => '_commerce_akeneo_resource_access',
          'access arguments'        => array('category'),
          'access arguments append' => TRUE,
          'args'                    => array(
            array(
              'name'        => 'categories',
              'type'        => 'array',
              'description' => t('An array representing the category entries.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
          ),
        ),
        'attribute'        => array(
          'help'                    => t('Synchronize an attribute.'),
          'file'                    => array(
            'type'   => 'inc',
            'module' => 'commerce_akeneo',
            'name'   => 'resources/attribute_resource',
          ),
          'callback'                => '_commerce_akeneo_resource_attribute',
          'access callback'         => '_commerce_akeneo_resource_access',
          'access arguments'        => array('attribute'),
          'access arguments append' => TRUE,
          'args'                    => array(
            array(
              'name'        => 'attribute',
              'type'        => 'struct',
              'description' => t('A struct representing the attribute.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
          ),
        ),
        'attribute_group'  => array(
          'help'                    => t('Synchronize an attribute group.'),
          'file'                    => array(
            'type'   => 'inc',
            'module' => 'commerce_akeneo',
            'name'   => 'resources/attribute_group_resource',
          ),
          'callback'                => '_commerce_akeneo_resource_attribute_group',
          'access callback'         => '_commerce_akeneo_resource_access',
          'access arguments'        => array('attribute_group'),
          'access arguments append' => TRUE,
          'args'                    => array(
            array(
              'name'        => 'attribute_group',
              'type'        => 'struct',
              'description' => t('A struct representing the attribute group.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
          ),
        ),
        'attribute_option' => array(
          'help'                    => t('Synchronize an attribute option list.'),
          'file'                    => array(
            'type'   => 'inc',
            'module' => 'commerce_akeneo',
            'name'   => 'resources/attribute_option_resource',
          ),
          'callback'                => '_commerce_akeneo_resource_attribute_option',
          'access callback'         => '_commerce_akeneo_resource_access',
          'access arguments'        => array('attribute_option'),
          'access arguments append' => TRUE,
          'args'                    => array(
            array(
              'name'        => 'attribute_option',
              'type'        => 'struct',
              'description' => t('A struct representing the attribute option.'),
              'source'      => 'data',
              'optional'    => FALSE,
            ),
          ),
        ),
      ),
    ),
  );
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
    ),
    'parsers'    => array(
      'application/json'                  => TRUE,
      'application/vnd.php.serialized'    => FALSE,
      'application/x-www-form-urlencoded' => FALSE,
      'application/xml'                   => FALSE,
      'multipart/form-data'               => FALSE,
      'text/xml'                          => FALSE,
    ),
  );

  $endpoint->resources = array(
    'akeneo' => array(
      'actions' => array(
        'product'          => array(
          'enabled' => '1',
        ),
        'family'           => array(
          'enabled' => '1',
        ),
        'category'         => array(
          'enabled' => '1',
        ),
        'attribute_option' => array(
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
function _commerce_akeneo_resource_access($op, $args) {
  $access = FALSE;

  switch ($op) {
    case 'product':
      $access = user_access('resource product commerce akeneo');
      break;

    case 'family':
      $access = user_access('resource family commerce akeneo');
      break;

    case 'category':
      $access = user_access('resource category commerce akeneo');
      break;

    case 'attribute_option':
      $access = user_access('resource attribute option commerce akeneo');
      break;
  }

  // $access;
  return TRUE;
}

/**
 * Class CommerceAkeneoException
 */
class CommerceAkeneoException extends Exception {

}
