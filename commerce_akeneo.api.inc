<?php

/**
 * @file
 * API file for Commerce Akeneo module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Allow to override resources exposed.
 *
 * @param array $resources
 *   Resources to alter.
 */
function hook_commerce_akeneo_services_resources_alter(&$resources) {

}

/**
 * Allow to define your own logic between locale and language translation.
 *
 * @param string $language
 *   Language to alter.
 * @param string $locale
 *   Locale provided as reference.
 */
function hook_commerce_akeneo_locale_to_language_alter(&$language, $locale) {
  switch ($locale) {
    case 'fr_CA':
    case 'fr_FR':
      $language = 'fr-FR';
      break;

    case 'en_GB':
    case 'en_US':
      $language = 'en-US';
      break;
  }
}

/**
 * @} End of "addtogroup hooks".
 */
