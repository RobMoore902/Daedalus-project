<?php

/**
 * @file
 * daedalus_auto_advisor.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_advisor
 *
 * Uses current textfield input to determine matches for advisors
 * currently using Daedalus. Advisors have been assigned the role
 * by a magellan ar daedalus administrator.
 */
function daedalus_autocomplete_advisor($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $role_id = db_result(db_query("SELECT rid FROM {role} WHERE name='Magellan Advisor'"));

  $advisor = array(); $valid_advisors = array();

  $result = db_query("SELECT DISTINCT users.name
                        FROM {users, users_roles}
                       WHERE users_roles.rid=%d
                         AND users.uid=users_roles.uid
                       ORDER BY name", $role_id);

  while ($row = db_fetch_array($result)) {
    $advisor[$row['name']] = $row['name'];
  }

  $added = 0;
  foreach ($advisor as $advisor_text) {

    if (strpos(drupal_strtoupper($advisor_text), drupal_strtoupper($search) ) !== FALSE) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_advisors[$advisor_text] = check_plain($advisor_text);

      $added++;

    }

    if ($added >= $max_results) {
      break;
    }

  }

  print drupal_to_js($valid_advisors);

  exit();

}