<?php

/**
 * @file
 * daedalus_auto_precourse.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_prereq_course
 *
 * Uses current textfield input to determine matches for prerequiste courses.
 */
function daedalus_autocomplete_prereq_course($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $valid_courses = array(); $before = ''; $added = 0;

  // Make sure the characters are all uppercase.
  // If a lower case or is entered it must be converted
  // to uppercase or the search_array can not be created.
  $search = drupal_strtoupper($search);

  if (preg_match('/OR/', $search)) {

    $search_array = daedalus_explode_trim(' OR ', $search);

    // The before string adds the previously searched
    // items to the textfield, if not they will be erased.
    for ($i = 0; $i < sizeof($search_array)-1; $i++) {
      $before .= $search_array[$i] . ' OR ';
    }

    // Set the new search item as the last of the array.
    $search = trim($search_array[sizeof($search_array)-1]);

  }

  $result = db_query("SELECT course, course_name FROM {dae_course} ORDER BY course");
  while ($row = db_fetch_array($result)) {

    $course = drupal_strtoupper($row['course']);
    $course_name = $row['course_name'];

    if (($course && $search) && (strpos($course, $search) !== FALSE)) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_courses[$before . $course] = check_plain($course . ' - ' . $course_name);
      $added++;

    }
    elseif (($course_name && $search) && (strpos(drupal_strtoupper($course_name), drupal_strtoupper($search)) !== FALSE)) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_courses[$before . $course] = check_plain($course . ' - ' . $course_name);
      $added++;

    }

    if ($added >= $max_results) {
      break;
    }

  }

  print drupal_to_js($valid_courses);

  exit();

}