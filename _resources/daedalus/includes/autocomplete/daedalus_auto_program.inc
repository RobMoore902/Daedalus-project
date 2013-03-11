<?php

/**
 * @file
 * daedalus_auto_program.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_program
 *
 * Uses current textfield input to determine matches for programs
 */
function daedalus_autocomplete_program($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $programs = array(); $valid_programs = array();

  $result = db_query("SELECT DISTINCT program FROM {mag_program_identification} ORDER BY program");
  while ($row = db_fetch_array($result)) {
    $programs[$row['program']] = $row['program'];
  }

  $added = 0;
  foreach ($programs as $program_text) {

    if (strpos(drupal_strtoupper($program_text), drupal_strtoupper($search) ) !== FALSE) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_programs[$program_text] = check_plain($program_text);
      $added++;
    }

    if ($added >= $max_results) {
      break;
    }

  }

  print drupal_to_js($valid_programs);

  exit();

}