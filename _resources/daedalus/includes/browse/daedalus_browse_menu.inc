<?php

/**
 * @file
 * daedalus_browse_menu.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Browse Callback Functions
//------------------------------------------------------------------------------
function daedalus_browse_page() {
  return drupal_get_form('daedalus_browse_form');
}


/**
 * Menu Location: Daedalus -> Browse
 * URL Location:  daedalus/browse
 *
 * Displays the Browse menu.
 * Links to:
 * 	* Browse Courses
 *	* Browse Learning Outcomes
 *	* Browse Term Reviews
 *      * Browse Visual Maps
 */
function daedalus_browse_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='daedalus browse image' OR
                             setting='browse'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  global $base_url;

  // Set current page url.
  $page_url = $page_settings['browse'];

  // Get the images
  $browse_img_src = $base_url . '/' . $page_settings['daedalus browse image'];

  // Set's a blank title page.
  drupal_set_title(t('Daedalus Browse'));

  $form = array();

  // Only execute code if magellan database tables are installed.
  // This determines if an advisor or support session is open and
  // displays the hidden timeout warning if returned true.
  if (module_exists('magellan')) {

    global $user;

    // If the current user is a Magellan Advisor, determine
    // if there is a current advising session open.
    if (magellan_advisor($user->uid)) {

      $result = db_query("SELECT id, add_time FROM {mag_advisor_session}
        WHERE advisor_id=%d", $user->uid);

      while ($row = db_fetch_array($result)) {
        $session_id  = $row['id'];
        $add_time = $row['add_time'];
      }

      // Get the session name.
      $current_session = $_COOKIE[session_name()];

      // If in session, set the user name as
      // to the selected students username.
      if ($session_id == $current_session) {

        // Get the current session time
        $session_time = db_result(db_query("SELECT session_time FROM {mag_session_log}
          WHERE advisor_id=%d AND session_id='%s'", $user->uid, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse', 'advisor');

      }

    }

    // If the current user is a Magellan Support determine
    // if there is a current support session open.
    if (magellan_support($user->uid)) {

      $result = db_query("SELECT id, add_time FROM {mag_support_session} WHERE support_id=%d", $user->uid);
      while ($row = db_fetch_array($result)) {
        $session_id  = $row['id'];
        $add_time = $row['add_time'];
      }

      // Get the session name.
      $current_session = $_COOKIE[session_name()];

      // If in session, set the user name as
      // to the selected students username.
      if ($session_id == $current_session) {

        // Get the current session time
        $session_time = db_result(db_query("SELECT session_time FROM {mag_session_log}
          WHERE support_id=%d AND session_id='%s'", $user->uid, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse', 'support');

      }

    }

  }

  // Displays the brose menu item. The URL items are
  // stored in the correct order in the database.
  $result = db_query("SELECT value, setting FROM {dae_settings} WHERE type = 'url' ORDER BY id");
  while ($row = db_fetch_array($result)) {

    $setting = $row['value'];

    $title = ucwords($row['setting']);

    // Make sure the setting is not
    // the same as the page url.
    $pos = strpos($setting, $page_url);

    if ($pos !== FALSE && $setting != $page_url) {

      // Hide Term Reviews from users without access
      if (stripos($setting, 'term-review') && user_access('daedalus browse term reviews')) {
        $browse_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
                t('!title', array('!title' => $title)) . '</a></p>';
      }
      if (stripos($setting, 'graphviz') && user_access('daedalus browse visual maps')) {
        $browse_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
                t('!title', array('!title' => $title)) . '</a></p>';
      }
      if (!stripos($setting, 'term-review') && !stripos($setting, 'graphviz')) {
        $browse_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
                t('!title', array('!title' => $title)) . '</a></p>';
      }

    }

  }

  $browse_output  = '<img src="' . $browse_img_src . '" alt="Browse Image" style="float:right; width:375px; margin-right:50px;" />';
  $browse_output .= '<br />' . $browse_list;

  $form[] = array(
    '#type' => 'item',
    '#value' => $browse_output,
  );

  return $form;

}




