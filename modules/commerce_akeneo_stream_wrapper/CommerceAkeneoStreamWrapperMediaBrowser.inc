<?php

/**
 * @file
 * Definition of CommerceAkeneoStreamWrapperMediaBrowser.
 */

// If the Media Browser plugin class does not exist, prevent the rest of this
// file from being loaded.
if (!class_exists('MediaBrowserPlugin')) {
  return;
}

/**
 * Media browser plugin for akeneo files.
 */
class CommerceAkeneoStreamWrapperMediaBrowser extends MediaBrowserPlugin {
  /**
   * Implements MediaBrowserPluginInterface::access().
   */
  public function access($account = NULL) {
    return user_access('administer files', $account) || user_access('add media from akeneo urls', $account);
  }

  /**
   * Implements MediaBrowserPlugin::view().
   */
  public function view() {
    $build['form'] = drupal_get_form('commerce_akeneo_stream_wrapper_file_add_form', $this->params);
    return $build;
  }
}
