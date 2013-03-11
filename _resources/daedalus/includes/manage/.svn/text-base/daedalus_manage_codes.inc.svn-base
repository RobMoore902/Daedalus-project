<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage->Course Codes Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_codes_page() {
  return drupal_get_form('daedalus_manage_codes_form');
}


/**
 * Menu Location: Daedalus -> Manage -> Course Codes
 * URL Location:  daedalus/manage/codes
 *
 * Displays: Page for bulk management of course codes (departments).
 */
function daedalus_manage_codes_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='manage course codes' OR
                             setting='collapse department page on page load' OR
                             setting='build course codes' OR
                             setting='question mark'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  global $base_url;

  // Get the images
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  drupal_set_title(t('Manage Course Codes !help', array('!help' => $show_help)));

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($page_settings['manage course codes'], 1, 1);

  // Valid course code should appear in the table
  $result = db_query("SELECT * FROM {dae_valid_course_codes} ORDER BY course_code");
  while ($row = db_fetch_array($result)) {

    $key = $row['id'];

    // Depending if everything should be collapsed or not, define the fieldset differently.
    if ($page_settings['collapse department page on page load']) {

      $form[$key] = array(
        '#title' => check_plain($row['course_code'] . ' ' . $row['department_name']),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

    }

    else {

      $form[$key] = array(
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
      );

    }

    // Course Code
    $form[$key]['code_' . $row['id']] = array(
      '#type' => 'textfield',
      '#title' => t('Course Code'),
      '#required' => TRUE,
      '#default_value' => $row['course_code'],
      '#size' => drupal_strlen($row['department_name'])+5,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Department Name
    $form[$key]['department_' . $row['id']] = array(
      '#type' => 'textfield',
      '#title' => t('Department name'),
      '#required' => TRUE,
      '#default_value' => $row['department_name'],
      '#size' => drupal_strlen($row['department_name'])+5,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Submit Changes
    $form[$key]['submit_' . $row['id']] = array(
      '#type' => 'submit',
      '#value' => t('Apply Changes to ') . $row['course_code'],
      '#attributes' => array('depnum' => $row['id']),
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

  }

  if (!$key) {

    // If there are no course codes, inform the user a course code must be created first.
    $url = $base_url . '/' . $page_settings['build course codes'];

    drupal_set_message(t('There are no course codes created. <a href="@url">Create a Course Code?</a>',
            array('@url' => url($url))));

  }


  // Submit hidden information to pass
  // to the validate and submit hooks.
  $form['pass-manage-course-codes'] = array( '#type' => 'value', '#value' => $page_settings['manage course codes'], );

  return $form;

}


/*
 * Implementation of hook_validate().
 */
function daedalus_manage_codes_form_validate($form, &$form_state) {

  // Get the button pressed, the id of the
  // course code in valid course code table.
  $id = $form_state['clicked_button']['#attributes']['depnum'];

  // Get the new code and name
  // (what was entered into the textfields)
  $new_code = $form_state['values']["code_$id"];
  $new_name = $form_state['values']["department_$id"];

  if (is_numeric($form_state['values']["course_code_$id"])) {

    // Make sure the course code isn't a number.
    form_set_error("course_code_$id", t('Course codes can not be integers.'));

  }
  elseif (substr_count($new_code, ' ')) {

    // Make sure there are not any spaces.
    form_set_error("course_code_$id", t('Course codes can not contain spaces.'));

  }

  // Now go through the table, and see if there's
  // anything that has the same code or department
  // name as what's being changed. If there is,
  // then don't allow the change.
  $duplicate = db_result(db_query("SELECT COUNT(*)
                                     FROM {dae_valid_course_codes}
                                    WHERE id != %d
                                      AND (course_code like '%s'
                                      AND department_name like '%s')",
                                          $id, $new_code, $new_name));

  if ($duplicate) {
    form_set_error("code_$id", t('The course code and department name are already in use.'));
  }

}


/**
 * Implementation of hook_submit().
 */
function daedalus_manage_codes_form_submit($form, &$form_state) {

  // The id of the course code in valid course code table
  $id = $form_state['clicked_button']['#attributes']['depnum'];

  // Get the course code from the database.
  $code_info = db_fetch_array(db_query("SELECT course_code, department_name
                                          FROM {dae_valid_course_codes}
                                         WHERE id=%d", $id));

  $old_code = $code_info['course_code'];
  $old_name = $code_info['department_name'];
  $new_code = $form_state['values']["code_$id"];
  $new_name = $form_state['values']["department_$id"];

  if ($new_code || $new_name) {

    // Update all existing courses
    // with the new course code.
    if ($old_code != $new_code) {
      db_query("UPDATE {dae_course}
                   SET course_code='%s'
                 WHERE course_code='%s'", $new_code, $old_code);
    }

    db_query("UPDATE {dae_valid_course_codes} SET course_code='%s', department_name='%s' WHERE id=%d", $new_code, $new_name, $id);

    drupal_set_message(t('The course code "@old-code @old-name" has been changed to "@new-code @new-name"',
            array('@old-code' => $old_code, '@old-name' => $old_name, '@new-code' => $new_code, '@new-name' => $new_name)));
  }

  if ($form_state['values']['op'] == $form_state['values']['dae-help-submit']) {

    global $base_url;

    $help_text = $form_state['values']['dae-help'];

    $page_url = $form_state['values']['pass-manage-course-codes'];

    if ($help_text) {

      if (db_result(db_query("SELECT COUNT(*) FROM {dae_page_help} WHERE page_url='%s'", $page_url))) {
        db_query("UPDATE {dae_page_help} SET help_text='%s' WHERE page_url='%s'", $help_text, $page_url);
      }
      else {
        db_query("INSERT INTO {dae_page_help} (page_url, help_text) VALUES ('%s', '%s')", $page_url, $help_text);
      }

      drupal_set_message(t('Help information saved.') );

      drupal_goto($base_url . '/' . $page_url);

    }

  }

}