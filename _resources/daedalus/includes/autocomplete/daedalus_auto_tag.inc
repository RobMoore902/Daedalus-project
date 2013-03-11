<?php

/**
 * @file
 * daedalus_auto_tag.inc
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * Function: daedalus_autocomplete_tag
 *
 * Uses current textfield input to determine matches for tags.
 * The output can be modified by using the asterix character.
 *
 * The '*' will return all tags.
 *
 * The '*' followed by a single character will return all tags
 * beggining with that character.
 */
function daedalus_autocomplete_tag($search) {

  $matches = array();

  $seperate = daedalus_get_setting('tag seperate character');

  $search_array = daedalus_explode_trim($seperate, $search);

  for ($i = 0; $i < sizeof($search_array)-1; $i++) {
    $before .= $search_array[$i] . ', ';
  }

  $last_element =  $search_array[sizeof($search_array)-1];

  if ($last_element == '*') {

    $result = db_query("SELECT tag_label FROM {dae_tag} ORDER BY tag_label");
    while ($row = db_fetch_array($result)) {
      $matches[$before . '' . $row['tag_label']] = $row['tag_label'];
    }

    print drupal_to_js($matches);

    exit();

  }

  if (preg_match('/^\*[a-zA-Z0-9]$/', $last_element)) {

    $character = str_replace('*', '', $last_element);

    $result = db_query("SELECT tag_label FROM {dae_tag} WHERE upper(substring(`tag_label`,1,1))='%s' ORDER BY tag_label", $character);
    while ($row = db_fetch_array($result)) {
      $matches[$before . '' . $row['tag_label']] = $row['tag_label'];
    }

    print drupal_to_js($matches);

    exit();

  }

  else {

    $max_amount = daedalus_get_setting('autocomplete iteration');

    $result = db_query_range("SELECT * FROM {dae_tag} WHERE tag_label LIKE '%s'", "%" . $last_element . '%', 0, $max_amount);
    while ($row = db_fetch_array($result)) {
      $matches[$before . '' . $row['tag_label']] = $row['tag_label'];
    }

    print drupal_to_js($matches);

    exit();

  }

}