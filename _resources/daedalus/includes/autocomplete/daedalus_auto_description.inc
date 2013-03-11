<?php

/**
 * @file
 * daedalus_auto_description.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_description
 *
 * Uses current textfield input to determine matches for
 * descriptions while building, and managing program forms.
 */
function daedalus_autocomplete_description($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $descriptions = array(); $valid_descriptions = array();

  $result = db_query("SELECT DISTINCT description FROM {mag_program_requirement} ORDER BY description");
  while ($row = db_fetch_array($result)) {
    $descriptions[$row['description']] = $row['description'];
  }

  $added = 0;
  foreach ($descriptions as $description_text) {

    if (strpos(drupal_strtoupper($description_text), drupal_strtoupper($search) ) !== FALSE) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_descriptions[$description_text] = check_plain($description_text);
      $added++;
    }

    if ($added >= $max_results) {
      break;
    }
  }

  print drupal_to_js($valid_descriptions);

  exit();

}