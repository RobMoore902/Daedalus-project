<?php

/**
 * @file
 * daedalus_auto_support.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_support
 *
 * Uses current textfield input to determine matches for support staff
 * currently using Daedalus. Support staff have been assigned the role
 * by a magellan ar daedalus administrator.
 */
function daedalus_autocomplete_support($search) {

  $max_results = daedalus_get_setting('autocomplete iteration');

  $support = array(); $valid_supports = array();

  $role_id = db_result(db_query("SELECT rid FROM {role} WHERE name='Magellan Support Staff'"));

  $result = db_query("SELECT DISTINCT users.name
                        FROM {users, users_roles}
                       WHERE users_roles.rid=%d
                         AND users.uid=users_roles.uid
                    ORDER BY name", $role_id);

  while ($row = db_fetch_array($result)) {
    $support[$row['name']] = $row['name'];
  }

  $added = 0;
  foreach ($support as $support_text) {

    if (strpos(drupal_strtoupper($support_text), drupal_strtoupper($search) ) !== FALSE) {

      // check_plain encodes special characters in
      // a plain-text string for display as HTML.
      $valid_supports[$support_text] = check_plain($support_text);
      $added++;
    }

    if ($added >= $max_results) {
      break;
    }
  }

  print drupal_to_js($valid_supports);

  exit();

}