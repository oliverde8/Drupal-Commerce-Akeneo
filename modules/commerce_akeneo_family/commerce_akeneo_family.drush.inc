<?php

/**
 * @file
 * Commerce Akeneo Family Drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function commerce_akeneo_family_drush_command() {
  $commands = array();

  $commands['akeneo-family'] = array(
    'description' => 'Synchronize Akeneo families into Drupal product types.',
    'callback'    => 'commerce_akeneo_family_drush_import',
    'arguments'   => array(),
    'options'     => array(
      'remove-leases' => 'Remove leases',
    ),
    'examples'    => array(),
  );

  return $commands;
}

/**
 * Drush command callback.
 */
function commerce_akeneo_family_drush_import() {
  $args    = drush_get_arguments();
  $options = drush_migrate_get_options();

  $queue_name  = 'commerce_akeneo:family';
  $queues_info = module_invoke_all('queue_info');
  $queue_info  = $queues_info[$queue_name];

  /* @var SystemQueue $queue */
  $queue = DrupalQueue::get($queue_name);

  // Remove any leases.
  if (!empty($options['remove-leases'])) {
    $num_updated = db_update('queue')
      ->fields(array('expire' => 0))
      ->condition('name', $queue_name, '=')
      ->execute();

    if ($num_updated) {
      drupal_set_message(
        t('!count lease reset in queue "@name".', array('!count' => $num_updated, '@name' => $queue_info['title']))
      );
    }

    // Don't go further.
    return;
  }

  if (isset($queue_info['batch'])) {
    $batch = $queue_info['batch'];

    $operations = array();
    foreach ($batch['operations'] as $operation) {
      // First element is the batch process callback, second is the argument.
      $operations[] = array(
        $operation[0],
        array_merge(array($queue), $operation[1]),
      );
    }
    $batch['operations'] = $operations;

    // Get the batch process all ready.
    batch_set($batch);
    $batch = &batch_get();

    // Because we are doing this on the back-end, we set progressive to false.
    $batch['progressive'] = FALSE;

    // Process the batch.
    drush_backend_batch_process();
  }
}
