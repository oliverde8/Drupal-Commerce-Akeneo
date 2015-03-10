<?php

/**
 * @file
 * CommerceAkeneoMigrateTaxonomy class file.
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
   * Constructor.
   *
   * @param array $arguments
   *   Arguments.
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
      array(
        'entity_type'         => 'taxonomy_term',
        'bundle'              => $arguments['bundle'],
        'track_last_imported' => TRUE,
      )
    );

    $source_fields = array(
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

    $this->source      = new MigrateSourceQueue($arguments['type'] . ':' . $arguments['code'], $source_fields);
    $this->destination = new MigrateDestinationTermMachineName($arguments['bundle']);
  }

  /**
   * Purge map table from missing destination entities.
   */
  protected function preImport() {
    // Create taxonomy vocabulary destination if not already exists.
    if (!$taxonomy = commerce_akeneo_taxonomy_load($this->arguments['code'], $this->arguments['type'])) {
      throw new CommerceAkeneoException('Missing taxonomy origin.');
    }

    if (!$vocabulary = taxonomy_vocabulary_machine_name_load($taxonomy['vocabulary'])) {
      if (!user_access('administer taxonomy')) {
        throw new CommerceAkeneoException('Not authorized to create vocabulary.');
      }

      $vocabulary = (object) array(
        'machine_name' => $taxonomy['vocabulary'],
        'description'  => t('Vocabulary created via Akeneo PIM.'),
        'i18n_mode'    => I18N_MODE_LOCALIZE,
      );
    }

    if (user_access('administer taxonomy')) {
      // Refresh vocabulary name if changed on Akeneo side.
      $vocabulary->name = $taxonomy['label'];

      // Write the vocabulary to the database.
      taxonomy_vocabulary_save($vocabulary);
    }

    // Break if not allowed to create terms for this vocabulary.
    if (!user_access('edit terms in' . $vocabulary->vid)) {
      throw new CommerceAkeneoException('Not allowed to create terms.');
    }

    // Removes map entries pointing at removed terms.
    $this->map->purgeMissingEntities('taxonomy_term', $this->arguments['bundle']);
  }

  /**
   * Release all item not imported.
   */
  protected function postImport() {
    // Release queue items.
    $this->getSource()->releaseItems();

    // Handle deferred items.
    parent::postImport();
  }

  /**
   * Prepare row before import.
   *
   * @param stdclass $row
   *   Row.
   *
   * @return bool
   *   TRUE to import row.
   */
  public function prepareRow($row) {
    // Skip invalid terms.
    if (empty($row->machine_name) || !($valid = parent::prepareRow($row))) {
      return FALSE;
    }

    // Common values.
    $row->label = filter_xss(commerce_akeneo_get_language($row->labels));

    // Default.
    if (empty($row->parent)) {
      $row->parent = 0;
    }

    return TRUE;
  }

  /**
   * Prepare destination entity based on source row.
   *
   * @param stdclass $entity
   *   Destination entity.
   * @param stdclass $row
   *   Source row.
   */
  public function prepare(&$entity, $row) {
    // Nothing special to do.
  }

  /**
   * Complete destination entity based on source row.
   *
   * @param stdclass $entity
   *   Destination entity.
   * @param stdclass $row
   *   Source row.
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
   * Provide source casted.
   *
   * @return \MigrateSourceQueue
   *   Source.
   */
  public function getSource() {
    return $this->source;
  }

  /**
   * Provide destination casted.
   *
   * @return \MigrateDestinationTermMachineName
   *   Destination.
   */
  public function getDestination() {
    return $this->destination;
  }
}
