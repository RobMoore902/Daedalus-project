<?php

/**
 * @file
 * daedalus_auto_course.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_coursecode
 *
 * Uses current textfield input to determine matches to
 * course_codes from the dae_valid_course_codes table.
 */
function daedalus_autocomplete_coursecode($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $course_codes = array(); $valid_codes = array();

  $result = db_query("SELECT * FROM {dae_valid_course_codes} ORDER BY course_code");
  while ($row = db_fetch_array($result)) {
    $course_codes[$row['course_code']] = $row['department_name'];
  }

  $added = 0;
  foreach ($course_codes as $code => $dept) {

    // Combine the code and department
    // name for the search.
    $code_dept = $code . ' ' . $dept;

    if (strpos(drupal_strtoupper($code_dept), drupal_strtoupper($search) ) !== FALSE) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_codes[$code] = check_plain($code . ' - ' . $dept);
      $added++;
    }

    if ($added >= $max_results) {
      break;
    }
  }

  print drupal_to_js($valid_codes);

  exit();

}