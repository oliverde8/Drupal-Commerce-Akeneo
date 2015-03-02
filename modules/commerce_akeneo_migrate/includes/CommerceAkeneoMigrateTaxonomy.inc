<?php

/**
 * @file
 */

/**
 * Class CommerceAkeneoMigrateTaxonomy
 */
class CommerceAkeneoMigrateTaxonomy extends MigrationTaxonomyTermMachineName {
  /**
   * @var array
   */
  protected $attributesByFields;

  /**
   * @param array $arguments
   */
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('Import taxonomy %mode from Akeneo services.', array('%mode' => $arguments['type']));

    // Create a map object for tracking the relationships between source rows.
    $this->map = new MigrateSQLEntityAPIQueueMap(
      $this->machineName,
      array(
        'machine_name' => array(
          'type'        => 'varchar',
          'length'      => 255,
          'not null'    => TRUE,
          'description' => 'Machine name',
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('taxonomy_term', $arguments['bundle']),
      'default',
      array('entity_type' => 'taxonomy_term', 'bundle' => $arguments['bundle'], 'track_last_imported' => TRUE)
    );

    $sourceFields = array(
      'machine_name' => t('Machine name'),
      'label'        => t('Label'),
      'parent'       => t('Parent'),
    );

    // Common field or properties.
    $this->addFieldMapping('machine_name', 'machine_name');
    $this->addFieldMapping('name', 'label');
    $this->addFieldMapping('parent_machine_name', 'parent')->defaultValue(0);
    $this->addFieldMapping('pathauto')->defaultValue(TRUE);

    $this->addUnmigratedDestinations(
      array(
        'parent',
        'parent_name',
        'description',
        'format',
        'weight',
        'path',
      )
    );

    // Skip Metatag destinations.
    $this->addUnmigratedDestinations(
      array(
        'metatag_title',
        'metatag_description',
        'metatag_abstract',
        'metatag_keywords',
        'metatag_robots',
        'metatag_news_keywords',
        'metatag_standout',
        'metatag_generator',
        'metatag_rights',
        'metatag_image_src',
        'metatag_canonical',
        'metatag_shortlink',
        'metatag_publisher',
        'metatag_author',
        'metatag_original-source',
        'metatag_revisit-after',
        'metatag_content-language',
      )
    );

    $this->source      = new MigrateSourceQueue($arguments['type'] . ':' . $arguments['code'], $sourceFields);
    $this->destination = new MigrateDestinationTaxonomyTermMachineName($arguments['bundle']);
  }

  /**
   *
   */
  protected function preImport() {
    // Removes map entries pointing at removed entities.
    $this->map->purgeMissingEntities('taxonomy_term', $this->arguments['bundle']);
  }

  /**
   *
   */
  protected function postImport() {
    // Release queue items.
    $this->getSource()->releaseItems();

    // Handle deferred items.
    parent::postImport();
  }

  /**
   * @param stdClass $row
   *
   * @return bool
   */
  public function prepareRow($row) {
    // Skip invalid terms.
    if (empty($row->machine_name) || !($valid = parent::prepareRow($row))) {
      return FALSE;
    }

    // Common values.
    $row->label = commerce_akeneo_get_language($row->labels);

    // Default.
    if (empty($row->parent)) {
      $row->parent = 0;
    }

    return TRUE;
  }

  /**
   * @param stdClass $entity
   * @param stdClass $row
   */
  public function prepare(&$entity, $row) {
    // Nothing special to do.
  }

  /**
   * @param stdClass $entity
   * @param stdClass $row
   */
  public function complete($entity, $row) {
    // Build context for i18n_string translation.
    $context = 'taxonomy:term:' . (int) $entity->tid . ':name';

    // Store all translations.
    foreach ($row->labels as $locale => $value) {
      $language = commerce_akeneo_locale_to_language($locale);
      i18n_string_translation_update($context, $value, $language, $row->label);
    }

    // Remove processed items from queue.
    $this->getSource()->deleteItem($row->migrate_source_item_id);
  }

  /**
   * @return \MigrateSourceQueue
   */
  public function getSource() {
    return $this->source;
  }

  /**
   * @return \MigrateDestinationTaxonomyTermMachineName
   */
  public function getDestination() {
    return $this->destination;
  }
}
