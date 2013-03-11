<?php

/**
 * @file
 * daedalus_auto_multicode.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_multiplecodes
 *
 * Uses current textfield input to determine matches to
 * course_codes from the dae_valid_course_codes table.
 * This function allows the search of multiple codes by
 * separating the search string with a comma delimiter.
 */
function daedalus_autocomplete_multiplecodes($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $course_codes = array(); $valid_codes = array(); $search_array = array();

  $result = db_query("SELECT * FROM {dae_valid_course_codes} ORDER BY course_code");
  while ($row = db_fetch_array($result)) {
    $course_codes[$row['course_code']] = $row['department_name'];
  }

  $seperate = daedalus_get_setting('tag seperate character');

  // The string broken down into an array delimited by ,.
  $search_array = daedalus_explode_trim($seperate, $search);

  // Add each course code to the string that has already been completed. Do not
  // add the very last result because this is the code being searched to complete.
  for ($i = 0; $i < sizeof($search_array)-1; $i++) {
    $previous_results .= $search_array[$i] . ', ';
  }

  $last_entry = $search_array[sizeof($search_array)-1];

  // If there is not last entry but a trailing ',' in the string and
  // the browser is refreshed the script attempts to match an empty
  // last entry and throws a ton of error messages.
  if ($last_entry) {

    $added = 0;

    foreach ($course_codes as $code => $dept) {

      // Combine the code and department
      // name for the search.
      $code_dept = $code . " " . $dept;

      if (strpos(drupal_strtoupper($code_dept), drupal_strtoupper($last_entry) ) !== FALSE) {

        // check_plain encodes special characters in a plain-text string for
        // display as HTML. The previous results are added to the array, and
        // the resulting codes that passed the check are added.
        $valid_codes[$previous_results . $code] = check_plain($code . " - " . $dept);
        $added++;
      }

      if ($added >= $max_results) {
        break;
      }
    }
  }

  print drupal_to_js($valid_codes);

  exit();

}