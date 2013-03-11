<?php

/**
 * @file
 * daedalus_auto_student.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_student
 *
 * Uses current textfield input to determine matches for students
 * currently using Daedalus. These students have logged into the system
 * using their FCS ldap accounts. The LDAP Integration module assigns a specific
 * Daedalus role according to the various FCS user types. Students will have
 * "ldap_drupal_roles";a:1:{i:0;s:7:"Student" appended to the end of the users data.
 */
function daedalus_autocomplete_student($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $student = array(); $valid_students = array();

  $role_id = db_result(db_query("SELECT rid FROM {role} WHERE name='Daedalus Student'"));

  $result = db_query("SELECT DISTINCT users.name FROM {users, users_roles}
    WHERE users_roles.rid=%d AND users.uid=users_roles.uid ORDER BY name", $role_id);

  while ($row = db_fetch_array($result)) {
    $student[$row['name']] = $row['name'];
  }

  $added = 0;
  foreach ($student as $student_text) {

    if (strpos(drupal_strtoupper($student_text), drupal_strtoupper($search) ) !== FALSE) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_students[$student_text] = check_plain($student_text);
      $added++;
    }

    if ($added >= $max_results) {
      break;
    }

  }

  print drupal_to_js($valid_students);

  exit();

}