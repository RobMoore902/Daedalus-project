<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_page() {
  return drupal_get_form('daedalus_manage_form');
}


/**
 * Menu Location: Daedalus -> Manage
 * URL Location:  daedalus/manage
 *
 * Displays the Manage menu.
 * Links to:
 *    -> Courses
 *    -> Course Codes
 *    -> Learning Outcomes
 *    -> Tags
 *    -> Term Reviews
 *    -> Programs
 *    -> Students
 *    -> Session History
 *    -> Goals
 *    -> Imports/Exports
 *    -> Daedalus Settings
 */
function daedalus_manage_form($form) {

  global $base_url;

  drupal_set_title(t('Manage Daedalus'));

  $page_url = daedalus_get_setting('manage');

  $form = array();

  $result = db_query("SELECT * FROM {dae_settings}
    WHERE type = 'url' AND setting LIKE '%Manage%' ORDER BY id");

  while ($row = db_fetch_array($result)) {

    $setting = $row['value'];
    $title = ucwords($row['setting']);
    $pos = strpos($setting, $page_url);

    // Restrict the viewing of certain pages for special users.
    if ($pos !== FALSE && $setting != $page_url) {
      if ($title != 'Manage Student' && $title != 'Manage Session History' &&
              $title != 'Manage Settings' && $title != 'Manage Programs' && $title != 'Manage Goals') {

        $manage_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
                t('@title', array('@title' => $title)) . '</b></a></font></p>';

      }
    }
    elseif ($title == 'Manage Student' && user_access('magellan manage student')) {
      $manage_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
              t('@title', array('@title' => $title)) . '</a></p>';
    }
    elseif ($title == 'Manage Session History' && user_access('magellan manage session history')) {
      $manage_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
              t('@title', array('@title' => $title)) . '</a></p>';
    }
    elseif ($title == 'Manage Programs' && user_access('magellan manage programs')) {
      $manage_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
              t('@title', array('@title' => $title)) . '</a></p>';
    }
    elseif ($title == 'Manage Goals' && user_access('magellan manage goals')) {
      $manage_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
              t('@title', array('@title' => $title)) . '</a></p>';
    }
    elseif ($title == 'Manage Settings' && user_access('daedalus manage settings')) {
      $manage_list .= '<p class="menu"><a href="' . $base_url . '/' . $setting . '">' .
              t('@title', array('@title' => $title)) . '</a></p>';
    }

  }

  $manage_output .= '<br />' . $manage_list;
  $manage_output .= '<br /><br />';

  $form[] = array(
    '#type'  => 'item',
    '#value' => $manage_output,
  );

  return $form;

}