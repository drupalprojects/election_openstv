<?php
/**
 * @file
 * Drush commands for the Election OpenSTV Integration module.
 */

/**
 * Implements hook_drush_command().
 */
function election_openstv_drush_command() {
  $items = array();
  $items['election-openstv'] = array(
    'description' => 'Counts the results of an election post using OpenSTV. The report is printed to STDOUT.',
    'arguments' => array(
      'post_id' => 'The ID of the election post (required).',
      'method' => 'The count method (default: ERS97STV).',
      'output_format' => 'The output format (default: TextReport).',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'required-arguments' => 1,
    'aliases' => array('eostv'),
  );
  return $items;
}

/**
 * Count the results for an election post.
 *
 * @param int $post_id
 *   The ID of the election post.
 * @param string $method
 *   The count method to use.
 */
function drush_election_openstv($post_id, $method = 'ERS97STV', $output_format = 'TextReport') {
  // Load the post.
  $post = election_post_load($post_id);
  if (!$post) {
    return drush_set_error(dt('There are no election posts with the ID @id.', array('@id' => $post_id)));
  }
  $election = $post->election;
  // Check that it is sane to count results for this post.
  if (!election_openstv_check_support($election)) {
    return drush_set_error(dt('This election type ("@type") does not support OpenSTV.', array('@type' => $election->type_info['name'])));
  }
  // Check the method type.
  $allowed_methods = _election_openstv_get_methods();
  if (!in_array($method, $allowed_methods)) {
    return drush_set_error(dt('The count method "@method" was not found.', array('@method' => $method)));;
  }
  // Check the output format.
  $allowed_formats = _election_openstv_get_output_formats();
  if (!in_array($output_format, $allowed_formats)) {
    return drush_set_error(dt('The output format "@format" was not found.', array('@format' => $output_format)));;
  }
  // Check that OpenSTV is installed.
  $command = _election_openstv_get_command();
  if (!_election_openstv_executable($command)) {
    return drush_set_error(dt('The configured OpenSTV command (@command) cannot be executed. Is OpenSTV installed?', array('@command' => $command)));
  }
  // Run the count.
  echo election_openstv_count($post, $method, $output_format);
}
