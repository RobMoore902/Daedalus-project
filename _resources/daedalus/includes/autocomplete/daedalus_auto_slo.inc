<?php

/**
 * @file
 * daedalus_auto_slo.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_slo
 *
 * Uses current textfield input to determine matches for SLOs.
 * If a hash key is given return values for a tags.
 *
 */
function daedalus_autocomplete_slo($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  // If the hash tag is entered return the
  // list of tags in alphabetical order.
  if ($search == '#') {

    $tags = array(); $valid_tags = array();

    $result = db_query_range("SELECT DISTINCT tag_label
      FROM {dae_tag} ORDER BY tag_label", 0, 15);

    while ($row = db_fetch_array($result)) {
      $tags[$row['tag_label']] = $row['tag_label'];
    }

    foreach ($tags as $tag_text) {

      // Add the # to the values so if a
      // tag is selected it will have it.
      $tag_text = '#' . $tag_text;

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_tags[$tag_text] = check_plain($tag_text);

    }

    print drupal_to_js($valid_tags);

    exit();

  }

  // If the search begins with a hash tag.
  elseif (drupal_substr($search, 0, 1) == '#') {

    $tags = $valid_tags = array();

    // Remove the hash tag
    $search = substr_replace($search, '', 0, 1);

    // Select tags that are like the current search text.
    $result = db_query("SELECT DISTINCT id, tag_label FROM {dae_tag}
      WHERE tag_label LIKE '%s' ORDER BY tag_label", "%$search%");

    while ($row = db_fetch_array($result)) {

      $tags[$row['tag_label']] = $row['tag_label'];

      // Only used if one tag is selected.
      $selected_tag = '#' . $row['tag_label'];

      // The single tag id.
      $selected_tagid = $row['id'];

    }

    // If only one tag is returned
    if (count($tags) == 1) {

      $outcomes = array();

      // Add the selected tag to the top
      // of the learning outcomes list.
      $valid_outcomes = array($selected_tag => check_plain($selected_tag . ' - Select all associated outcomes.'));

      // Select learning outcomes that are tagged
      // with the user's search selection.
      $result = db_query("SELECT slo_text FROM {dae_slo, dae_slo_tag}
        WHERE dae_slo_tag.tag_id = %d AND dae_slo.id = dae_slo_tag.slo_id
        ORDER BY dae_slo.slo_text", $selected_tagid);

      while ($row = db_fetch_array($result)) {
        $outcomes[$row['slo_text']] = $row['slo_text'];
      }

      // Don't limit the amount of returned values.
      foreach ($outcomes as $outcome_text) {
        $valid_outcomes[$outcome_text] = check_plain($outcome_text);
      }

      print drupal_to_js($valid_outcomes);

      exit();

    }

    // Display the returned
    // tags from the query.
    else {

      $added = 0;
      foreach ($tags as $tag_text) {

        if (strpos(drupal_strtoupper($tag_text), drupal_strtoupper($search) ) !== FALSE) {

          // Add the # to the values so if a
          // tag is selected it will have it.
          $tag_text = '#' . $tag_text;

          // check_plain encodes special characters in
          // a plain-text string for display as HTML.
          $valid_tags[$tag_text] = check_plain($tag_text);
          $added++;

        }

        if ($added >= $max_results) {
          break;
        }

      }

      print drupal_to_js($valid_tags);

      exit();

    }

  }

  // If there is no hash tag present,
  // search for learning outcomes.
  else {

    $outcomes = $valid_outcomes = array();

    $result = db_query("SELECT DISTINCT slo_text FROM {dae_slo} WHERE slo_text LIKE '%s' ORDER BY slo_text", "%$search%");
    while ($row = db_fetch_array($result)) {
      $outcomes[$row['slo_text']] = $row['slo_text'];
    }

    $added = 0;
    foreach ($outcomes as $outcome_text) {

      if (strpos(drupal_strtoupper($outcome_text), drupal_strtoupper($search) ) !== FALSE) {

        // check_plain encodes special characters in
        // a plain-text string for display as HTML.
        $valid_outcomes[$outcome_text] = check_plain($outcome_text);
        $added++;
      }

      if ($added >= $max_results) {
        break;
      }

    }

    print drupal_to_js($valid_outcomes);

    exit();

  }

}