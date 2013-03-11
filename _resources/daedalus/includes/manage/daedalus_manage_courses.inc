ose<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage->Courses Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_courses_page() {
    return drupal_get_form('daedalus_manage_courses_form');
}


/**
 * Menu Location: Daedalus -> Manage -> Courses
 * URL Location: daedalus/manage/courses
 * Redirection: Browse->Courses
 *
 * Displays information regarding a selected course:
 *       - A course map
 *       - Current prerequisite courses
 *       - Current prerequisite learning outcomes
 *       - Current learning outcomes
 *       - Related term reviews
 *       - Term reviews for pre-requisite courses
 *       - Term reviews for courses requiring selected course
 */
function daedalus_manage_courses_form($form) {

  // When loading the page after a failed submit, a user should
  // not have to reload all the textfields they filled in. This
  // checks the post data values and adds a boolean at index to
  // tell the autocomplete to hide the value or not.

  $show_pcourse = array(); $show_tslo = array();
  foreach ($form['post'] as $key => $value) {

    if (strpos($key, 'pcourse') !== FALSE) {

      if (trim($value) != '') {
        $show_pcourse[] = TRUE;
      }
      else {
        $show_pcourse[] = FALSE;
      }

    }
    elseif (strpos($key, 'tslo') !== FALSE) {

      if (trim($value) != '') {
        $show_tslo[] = TRUE;
      }
      else {
        $show_tslo[] = FALSE;
      }

    }

  }

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='manage courses' OR
                             setting='title each autocomplete textfield' OR
                             setting='autocomplete iteration' OR
                             setting='manage learning outcomes' OR
                             setting='checkmark' OR
                             setting='question mark' OR
                             setting='question mark2' OR
                             setting='exclamation mark' OR
                             setting='goal identification' OR
                             setting='browse courses' OR
                             setting='course map size' OR
                             setting='graphviz iteration' OR
                             setting='build term reviews' OR
                             setting='manage term reviews' OR
                             setting='show uncovered learning outcome warning'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  // Get the course information once, so there
  // is no need for multiple database accesses
  // throughout the script.
  $page_courses = array();
  $result = db_query("SELECT * FROM {dae_course}");
  while ($row = db_fetch_array($result)) {
    $page_courses[$row['id']]['course_code'] = $row['course_code'];
    $page_courses[$row['id']]['course_number'] = $row['course_number'];
    $page_courses[$row['id']]['course_name'] = $row['course_name'];
    $page_courses[$row['id']]['course'] = $row['course'];
    $page_courses[$row['id']]['mapped'] = $row['mapped'];
    $page_courses[$row['id']]['viewable'] = $row['viewable'];
  }

  global $base_url;

  $page_url = $help_url = $page_settings['manage courses'];
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  // Determine if the magellan module is installed.
  $magellan_check = module_exists('magellan');

  // Image file locations
  $check_src = $base_url . '/' . $page_settings['checkmark'];
  $question2_src = $base_url . '/' . $page_settings['question mark2'];
  $exclamation_src = $base_url . '/' . $page_settings['exclamation mark'];
  $goal_img = '<img src="' . $base_url . '/' . $page_settings['goal identification'] . '" title="This learning outcome is part of your goal" />';
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  // Get the paramaters
  $param = array();
  $param[0] = arg(0+$page_url_length); // code
  $param[1] = arg(1+$page_url_length); // number
  $param[2] = arg(2+$page_url_length); // delete argument
  $param[3] = arg(3+$page_url_length); // what to delete
  $param[4] = arg(4+$page_url_length); // id of what to delete

  // Assign use for paramaters
  $course_code = drupal_strtoupper($param[0]);
  $course_number = $param[1];
  $course_id = daedalus_course_id($course_code, $course_number);
  $course_name = $page_courses[$course_id]['course_name'];
  $course_visibility = $page_courses[$course_id]['viewable'];

  // Redirect to Browse > Courses if there is no course code or number present
  // in the URL. This occurs when ever Manage > Courses is selected, a course code
  // and number are only provided when a scourse is selected from Browse > Courses.
  if (!$course_code && !$course_number) {

    drupal_set_message(t('Redirected to the browse courses page.') );

    drupal_goto($base_url . '/' . $page_settings['browse courses']);

  }
  elseif (daedalus_valid_alias("$course_code $course_number")) {

    // Redirect to the alias' parent course if the course
    // code and number belong to an alias course.
    $parent_id = db_result(db_query("SELECT parent_id FROM {dae_course_alias} WHERE alias_course='%s'", "$course_code $course_number"));
    $parent_code = $page_courses[$parent_id]['course_code'];
    $parent_number = $page_courses[$parent_id]['course_number'];

    drupal_set_message(t('You have been redirected from the crosslisted course "@course-code @course-number"',
            array('@course-code' => $course_code, '@course-number' => $course_number)));

    drupal_goto($page_url . '/' . $parent_code . '/' . $parent_number);

  }
  elseif ($course_code && $course_number && !daedalus_valid_course($course_code, $course_number)) {

    drupal_set_message(t('Redirected to the browse courses page from an invalid course.') );

    drupal_goto($base_url . '/' . $page_settings['browse courses']);

  }

  // Get access information
  $delete_course_access = user_access('daedalus delete course');
  $build_course_access = user_access('daedalus build courses');
  $build_reviews_access = user_access('daedalus build term reviews');
  $browse_reviews_access = user_access('daedalus browse term reviews');
  $build_access = user_access('daedalus build');
  $manage_access = user_access('daedalus manage');

  // Do not translate title, it is all
  // data and should not be modified.
  drupal_set_title($course_code . ' ' . $course_number . ' - ' . $course_name . $show_help);

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($help_url, 1, 1);

  // Only execute code if magellan
  // database tables are installed.
  if ($magellan_check) {

    // Determine the user name
    global $user;
    $user_id = $user->uid;
    $user_name = db_result(db_query("SELECT name FROM {users} WHERE uid=%d", $user_id));

    // Get the students identification number
    $student_id = db_result(db_query("SELECT id FROM {mag_student_identification} WHERE user_name='%s'", $user_name));

    if ($student_id) {

      $goal_slos = array();
      $result = db_query("SELECT slo_id FROM {mag_goal_student_slo} WHERE student_id=%d ORDER BY slo_id", $student_id);
      while ($row = db_fetch_array($result)) {
        $goal_slos[$row['slo_id']] = $row['slo_id'];
      }

      $goal_courses = array();
      $result = db_query("SELECT course_id FROM {mag_goal_student_course} WHERE student_id=%d", $student_id);
      while ($row = db_fetch_array($result)) {
        $goal_courses[$row['course_id']] = $row['course_id'];
      }

    }

    // If the current user is a Magellan Advisor determine
    // if there is a current advising session open.
    if (magellan_advisor($user_id)) {

      $result = db_query("SELECT id, add_time FROM {mag_advisor_session} WHERE advisor_id=%d", $user_id);
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
        $session_time = db_result(db_query("SELECT session_time FROM {mag_session_log} WHERE advisor_id=%d AND session_id='%s'", $user_id, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse_courses', 'advisor');

      }

    }

    // If the current user is a Magellan Support determine
    // if there is a current support session open.
    if (magellan_support($user_id)) {

      $result = db_query("SELECT id, add_time FROM {mag_support_session} WHERE support_id=%d", $user_id);
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
          WHERE support_id=%d AND session_id='%s'", $user_id, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse_courses', 'support');

      }

    }

  }

  if ($param[2] == 'delete') {

    // This code is implemented here rather then in a submit hook because it allows some of the work
    // to be done in the background with javascript. The javascript removes the item from the list,
    // and in the background (using an http request to this page) deletes the data from database.
    // If param[4] is not numeric, be sure to include the switch id so that it may be accessed.
    if ($delete_course_access && (is_numeric($param[4]) || $param[3] == 'this' || $param[3] == 'prereq_set')) {

      switch ($param[3]) {

        //delete the pcourse
        case 'pcourse':

          db_query("DELETE FROM {dae_prereq_course} WHERE id=%d", $param[4]);
          break;

        //delete the other prereq
        case 'oprereq':

          db_query("DELETE FROM {dae_course_other_prereq} WHERE id=%d", $param[4]);
          break;

        //delete the set of prerequisites
        case 'prereq_set':

          $delete_or_ids = explode('_', $param[4]);
          $or_placeholders = implode(' OR ', array_fill(0, count($delete_or_ids), 'id=%d'));

          db_query("DELETE FROM {dae_prereq_course} WHERE " . $or_placeholders, $delete_or_ids);
          break;

        //delete learning outcome
        case 'slo':

          db_query("DELETE FROM {dae_course_slo} WHERE course_id=%d AND slo_id=%d", $course_id, $param[4]);
          break;

        //delete course exclusion
        case 'exclusion':

          db_query("DELETE FROM {dae_course_exclusions} WHERE id=%d", $param[4]);
          break;

        //delete course exclusion
        case 'coreq':

          db_query("DELETE FROM {dae_course_coreqs} WHERE id=%d", $param[4]);
          break;

        //delete alias course
        case 'alias':

          db_query("DELETE FROM {dae_course_alias} WHERE id=%d", $param[4]);
          break;

        //delete course confirmation page
        case 'this':

          drupal_set_title(t('Delete Confirmation !help', array('!help' => $show_help)));

          $delete_message  = '<br />';
          $delete_message .= t('Are you sure you want to delete the course "%course-code %course-number" - "%course-name"? This can not be undone.',
            array('%course-code' => $course_code, '%course-number' => $course_number, '%course-name' => $course_name));
          $delete_message .= '<br /><br />';
          $delete_message .= '<ul>';
          $delete_message .= '<li>' . t('This will delete all references to alias courses.') . '</li>';
          $delete_message .= '<li>' . t('This will delete all references to prerequisite courses.') . '</li>';
          $delete_message .= '<li>' . t('This will delete all references to learning outcomes.') . '</li>';
          $delete_message .= '<li>' . t('This will delete all references to assumed learning outcomes.') . '</li>';
          $delete_message .= '</ul><br />';
          $delete_message .= t('Are you sure you want to continue?');

          $form[] = array(
            '#type' => 'item',
            '#value' => $delete_message,
          );

          $form['delete-forward'] = array(
            '#type' => 'submit',
            '#value' => t('I understand the consequences. Delete this course forever.'),
          );

          $form['delete-reverse'] = array(
            '#type' => 'submit',
            '#value' => t('On second thought, I better not. Take me back.'),
          );

          // Submit hidden information to pass
          // to the validate and submit hooks.
          $form['pass-manage-courses'] = array( '#type' => 'value', '#value' => $page_settings['manage courses'], );
          $form['pass-browse-courses'] = array( '#type' => 'value', '#value' => $page_settings['browse courses'], );
          $form['pass-build-term-reviews'] = array( '#type' => 'value', '#value' => $page_settings['build term reviews'], );
          $form['pass-autocomplete-iteration'] = array( '#type' => 'value', '#value' => $page_settings['autocomplete iteration'], );
          $form['pass-course-id'] = array( '#type' => 'value', '#value' => $course_id, );

          return $form;

        break;

      }

    }

    // @todo Test if this needs to be here.
    drupal_goto($page_url . '/' . $course_code . '/' . $course_number);

  }

  // Set weights for all the sections
  $weight = array();
  $weight['core']     = 0;
  $weight['cmap']     = 20;
  $weight['prereq']   = 30;
  $weight['pslo']     = 40;
  $weight['slo']      = 50;
  $weight['review']   = 60;
  $weight['revpre']   = 70;
  $weight['witreq']   = 80;
  $weight['comments'] = 90;
  $weight['warning']  = 100;
  $weight['submit']   = 110;

  // Boolean variable to determine if the course is mapped
  $mapped = $page_courses[$course_id]['mapped'];

  if ($build_course_access) {

    if (!$mapped) {

      // Include the pages JavaScript file if the course
      // is not mapped to enable the JavaScript function.
      drupal_add_js(drupal_get_path('module', 'daedalus') . '/javascript/daedalus_manage_courses.js');

      // Add control buttons
      $form['modify-course'] = array(
        '#type' => 'submit',
        '#value' => t('Submit changes'),
        '#weight' => $weight['submit'] - 1,
      );

      // Button settings to swap between a
      // mapped and unmapped course list.
      $form['mapped'] = array(
        '#type' => 'submit',
        '#value' => t('Mark as mapped'),
        '#weight' => $weight['submit'],
      );

      if ($delete_course_access) {

        $form['delete-course'] = array(
          '#type' => 'submit',
          '#value' => t('Delete course'),
          '#weight' => $weight['submit'] + 1,
        );

      }

    }
    else {

      // Button settings to swap between a
      // mapped and unmapped course list.
      $form['unmapped'] = array(
        '#type' => 'submit',
        '#value' => t('Mark as unmapped'),
        '#weight' => $weight['submit'],
      );

    }

  }

  // Add the option to build a term review,
  // The instructor may not have build access.
  if ($build_reviews_access) {

    $form['submit-review'] = array(
      '#type' => 'submit',
      '#value' => t('Build term review'),
      '#weight' => $weight['submit']+2,
    );

  }

       /////////////////////////////////////////////////////////////////////////
      ////////                    /////////////////////////////////////////////
     //////// COURSE INFORMATION /////////////////////////////////////////////
    ////////                    /////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
  // GET the COURSE EXCLUSIONS INFO
  $result = db_query("SELECT id, exclusion_course FROM {dae_course_exclusions} WHERE parent_id=%d ORDER BY exclusion_course", $course_id);
  while ($row = db_fetch_array($result)) {

    $exclusion_list .= '<li>' . $row['exclusion_course'];

    if ($delete_course_access && !$mapped) {
      // Do not wrap this message with the t()
      // this is displayed through JavaScript.
      $message = 'Are you sure you want to remove the course exclusion ' . $row['exclusion_course'];
      $url = $page_url . '/' . $course_code . '/' . $course_number . '/delete/exclusion/' . $row['id'];
      $exclusion_list .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>' . t('delete') . '</strong></a>';
    }

    $exclusion_list .= '</li>';

  }

  if (!$exclusion_list) {

    // Notify if there are no matches found
    $exclusion_list = '<li><i>' . t('No matches found') . '</i></li>';

    // For each section to be displayed here on out,
    // a flag is going to determine if a normal 'user'
    // is going to see any information at all.
    $exclusion_flag = 1;

  }

    //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
  // GET the COURSE CO-REQUISITES INFO

  $result = db_query("SELECT id, coreq_course FROM {dae_course_coreqs} WHERE parent_id=%d ORDER BY coreq_course", $course_id);
  while ($row = db_fetch_array($result)) {

    $coreq_list .= '<li>' . $row['coreq_course'];

    if ($delete_course_access && !$mapped) {
      // Do not wrap this message with the t()
      // this is displayed through JavaScript.
      $message = 'Are you sure you want to remove the course exclusion ' . $row['coreq_course'];
      $url = $page_url . '/' . $course_code . '/' . $course_number . '/delete/coreq/' . $row['id'];
      $coreq_list .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>' . t('delete') . '</strong></a>';
    }

    $coreq_list .= '</li>';

  }

  if (!$coreq_list) {

    // Notify if there are no matches found
    $coreq_list = '<li><i>' . t('No matches found') . '</i></li>';

    // For each section to be displayed here on out,
    // a flag is going to determine if a normal 'user'
    // is going to see any information at all.
    $coreq_flag = 1;

  }

    //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
  // GET the CURRENT CROSSLISTINGS INFO

  $result = db_query("SELECT id, alias_course FROM {dae_course_alias} WHERE parent_id=%d ORDER BY alias_code", $course_id);
  while ($row = db_fetch_array($result)) {

    $crosslist .= '<li>' . $row['alias_course'];

    // Option to remove alias
    if ($delete_course_access && !$mapped) {
      // Do not wrap this message with the t()
      // this is displayed through JavaScript.
      $message = 'Are you sure you want to remove the alias course ' . $row['alias_course'];
      $url = $page_url . '/' . $course_code . '/' . $course_number . '/delete/alias/' . $row['id'];
      $crosslist .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>delete</strong></a>';
    }

    $crosslist .= '</li>';

  }

  if (!$crosslist) {

    // Notify if there are no matches found
    $crosslist = '<li><i>' . t('No matches found') . '</i></li>';

    // For each section to be displayed here on out,
    // a flag is going to determine if a normal 'user'
    // is going to see any information at all.
    $crosslist_flag = 1;

  }

    //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
  // Display the COURSE INFORMATION HEADINGS

  // Display the dropdown only if there is information to display
  // - OR - If the manager must see the info and it is not mapped.
  if (!($exclusion_flag && $coreq_flag && $crosslist_flag) || ($manage_access && !$mapped)) {

    // Create the course info dropdown. The weight ['core'] sets the
    // position of each element on the page. Although the items in this
    // fieldset are all over the place, they are all weighted properly.
    $form['core'] = array(
      '#title' => t('Course Information'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => $weight['core'],
    );

  }

  if (!$exclusion_flag || ($manage_access && !$mapped)) {

    // Create the course info dropdown
    $form['core']['exclusion_field'] = array(
      '#title' => t('Course Exclusions'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => $weight['core']+5,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Display course exclusion information
    $form['core']['exclusion_field'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $exclusion_list . '</ul>',
      '#weight' => $weight['core']+6,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

  }

  if (!$coreq_flag || ($manage_access && !$mapped)) {

    // Create the course info dropdown
    $form['core']['coreq_field'] = array(
      '#title' => t('Course Co-Requisites'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => $weight['core']+8,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Display course exclusion information
    $form['core']['coreq_field'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $coreq_list . '</ul>',
      '#weight' => $weight['core']+9,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

  }

  if (!$crosslist_flag || ($manage_access && !$mapped)) {

    // Create the course info dropdown
    $form['core']['alias_field'] = array(
      '#title' => t('Current Crosslistings'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => $weight['core']+11,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Display crosslisting information
    $form['core']['alias_field'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $crosslist . '</ul>',
      '#weight' => $weight['core']+12,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

  }

    //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
  // Display the COURSE INFORMATION INPUT
  if ($build_course_access && !$mapped) {

    // Only show textfields if the course is not
    // mapped and the user has sufficient access.

    $form['core']['course_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Course Code'),
      '#default_value' => $course_code,
      '#weight' => $weight['core']+1,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['core']['course_number'] = array(
      '#type' => 'textfield',
      '#title' => t('Course Number'),
      '#default_value' => $course_number,
      '#weight' => $weight['core']+2,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['core']['course_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Course Name'),
      '#default_value' => $course_name,
      '#weight' => $weight['core']+3,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['core']['viewable'] = array(
      '#type' => 'checkbox',
      '#title' => '&nbsp;<b>' . t('Map Visibility') . '</b>',
      '#default_value' => $course_visibility,
      '#weight' => $weight['core']+4,
      '#prefix' => '<br /><blockquote>',
      '#suffix' => '</blockquote><br />',
    );

    $form['core']['exclusion_field']['exclusion_course'] = array(
      '#type' => 'textfield',
      '#title' => t('Add Exclusion'),
      '#weight' => $weight['core']+7,
      '#autocomplete_path' => 'autocomp/course',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['core']['coreq_field']['coreq_course'] = array(
      '#type' => 'textfield',
      '#title' => t('Add Co-Requisite'),
      '#weight' => $weight['core']+10,
      '#autocomplete_path' => 'autocomp/course',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['core']['alias_field']['alias_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Crosslisted Course Code'),
      '#weight' => $weight['core']+13,
      '#autocomplete_path' => 'autocomp/coursecode',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['core']['alias_field']['alias_number'] = array(
      '#type' => 'textfield',
      '#title' => t('Crosslisted Course Number'),
      '#weight' => $weight['core']+14,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    if (!$mapped) {

      // Add control buttons
      $form['core']['modify-course-2'] = array(
        '#type' => 'submit',
        '#value' => t('Submit changes'),
        '#weight' => $weight['core']+15,
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

  }

    //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
  // Display the COURSE MAP

  // The course map is designed to have a depth of 2.
  // Prereq -> Prereq -> COURSE <- Postreq <- Postreq

  $pwd = ''; $graphviz_path = '';

  // Determine if the graphviz directory is properly installed
  // and get the working directory and path for the dot program.
  $installed = daedalus_graphviz_installed($pwd, $graphviz_path);

  if ($installed) {

    // *NOTE* do not wrap the output for the course map in the t();

    // The size value controls the size of the displayed map. 9.5" is the
    // largest value I could achieve so that the map does not over-run the
    // display width of the current drupal theme page width.
    $output  = "digraph Constellation {\n";
    $output .= "size=\"" . $page_settings['course map size'] . "\"\n";

    // Boolean value to determine if the
    // map contains a single course.
    $singleton = TRUE;

    $map_course = $course_code . ' ' . $course_number;

    $course_url = $page_url;

      //\                                         /\\
     // \\  Add the selected course to the map.  // \\
    // ^ \\                                     // ^ \\
    if ($goal_courses && !$build_access && in_array($course_id, $goal_courses)) {
      $output .= " course" . $course_id . " [shape=box, fillcolor=\"GoldenRod\", style=\"rounded,filled\", label=\"" . $map_course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=9];\n";
    }
    else {
      $output  .= " course" . $course_id . " [shape=box, style=\"rounded,filled\", label=\"" . $map_course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=9];\n";
    }

    $preq_courses = array(); $final_prereqs = array(); $discovered_edges = array();

    // Get the prerequisite course information for the selected course
    $result = db_query("SELECT prereq_id, set_id FROM {dae_prereq_course} WHERE course_id=%d ORDER BY prereq_id, course_id", $course_id);
    while ($row = db_fetch_array($result)) {
      $preq_courses[$row['prereq_id']] = $row['set_id'];
    }

    if ($preq_courses) {

        //\                                              /\\
       // \\  Add the prerequisite courses to the map.  // \\
      // ^ \\                                          // ^ \\
      foreach ($preq_courses as $pid => $set_id) {

        $preq_course = $page_courses[$pid]['course'];
        $preq_course_code = $page_courses[$pid]['course_code'];
        $preq_course_number = $page_courses[$pid]['course_number'];
        $preq_course_mapped = $page_courses[$pid]['mapped'];
        $preq_course_viewable = $page_courses[$pid]['viewable'];

        // If the course is viewable add it and
        // it's prerequisites to the course map.
        // If the current course is not visible,
        // add any prerequiste regardless of visibility.
        if ($preq_course_viewable || !$course_visibility) {

          $singleton = FALSE;

          // Determine the URL, if the user doesn't
          // have build access and the course is not
          // mapped redirect to the Dal calendar entry.
          if ($preq_course_mapped || $build_access) {
            $preq_url = $course_url . '/' . $preq_course_code . '/' . $preq_course_number;
          }
          else {
            $preq_url = 'http://www.registrar.dal.ca/calendar/class.php?subj=' . $preq_course_code . '&num=' . $preq_course_number;
          }

          // Add the selected course to the map
          if ($goal_courses && !$build_access && in_array($pid, $goal_courses)) {
            $output .= " course" . $pid . " [shape=box, fillcolor=\"GoldenRod\", style=\"rounded,filled\", label=\"" . $preq_course . "\", URL=\"" . $preq_url . "\", fontsize=7, height=0.4, width=0.6];\n";
          }
          else {
            $output .= " course" . $pid . " [shape=box, style=\"rounded\", label=\"" . $preq_course . "\", URL=\"" . $preq_url . "\", fontsize=7, height=0.4, width=0.6];\n";
          }

          // Display a dashed line if the
          // prereq is part of a set.
          if ($set_id > 1) {
            $edges .= " course" . $pid . " -> course" . $course_id . " [style=\"setlinewidth(2), dashed\"];\n";
          }
          else {
            $edges .= " course" . $pid . " -> course" . $course_id . " [style=\"setlinewidth(2)\"];\n";
          }

          // Add the edge to the discovered list.
          $discovered_edges[] = "$pid > $course_id";

          $prepreq_courses = array();

          // Select the prerequiste courses to the prerequisite course.
          $result = db_query("SELECT prereq_id, set_id FROM {dae_prereq_course} WHERE course_id=%d ORDER BY prereq_id, course_id", $pid);
          while ($row = db_fetch_array($result)) {
            $prepreq_courses[$row['prereq_id']] = $row['set_id'];
          }

            //\                                                  /\\
           // \\  Add the pre prerequisite courses to the map.  // \\
          // ^ \\                                              // ^ \\
          if ($prepreq_courses) {

            foreach ($prepreq_courses as $preid => $set_id) {

              $prepreq_course = $page_courses[$preid]['course'];
              $prepreq_course_code = $page_courses[$preid]['course_code'];
              $prepreq_course_number = $page_courses[$preid]['course_number'];
              $prepreq_course_mapped = $page_courses[$preid]['mapped'];
              $prepreq_course_viewable = $page_courses[$preid]['viewable'];

              // If the course is viewable
              // add it to the course map.
              if ($prepreq_course_viewable || !$course_visibility) {

                // Make the pre prereq course list only if the course
                // is not PREU and is a 4000 level or less.
                $final_prereqs[$preid] = $preid;

                // Determine the URL, if the user doesn't
                // have build access and the course is not
                // mapped redirect to the Dal calendar entry.
                if ($prepreq_course_mapped || $build_access) {
                  $prepreq_url = $course_url . '/' . $prepreq_course_code . '/' . $prepreq_course_number;
                }
                else {
                  $prepreq_url = 'http://www.registrar.dal.ca/calendar/class.php?subj=' . $prepreq_course_code . '&num=' . $prepreq_course_number;
                }

                // Add the selected course to the map
                if ($goal_courses && !$build_access && in_array($preid, $goal_courses)) {
                  $output .= " course" . $preid . " [shape=box, fillcolor=\"GoldenRod\", style=\"rounded,filled\", label=\"" . $prepreq_course . "\", URL=\"" . $prepreq_url . "\", fontsize=6, height=0.35, width=0.5];\n";
                }
                else {
                  $output .= " course" . $preid . " [shape=box, style=\"rounded\", label=\"" . $prepreq_course . "\", URL=\"" . $prepreq_url . "\", fontsize=6, height=0.35, width=0.5];\n";
                }

                // Display a dashed line if the
                // prereq is part of a set.
                if ($set_id > 1) {
                  $edges  .= " course" . $preid . " -> course" . $pid . " [style=\"setlinewidth(2), dashed\"];\n";
                }
                else {
                  $edges  .= " course" . $preid . " -> course" . $pid . " [style=\"setlinewidth(1)\"];\n";
                }

                // Add the edge to the discovered list.
                $discovered_edges[] = "$preid > $pid";

              }

            }

          }

        }

      }

      // Make the final edge connections
      // between the pre prereq courses.
      if ($final_prereqs) {

        foreach ($final_prereqs as $final_preid) {

          $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $final_preid);
          while ($row = db_fetch_array($result)) {

            // Make sure the course is in the map.
            if (in_array($row['prereq_id'], $final_prereqs)) {

              // Make sure the link has not already been made.
              if (!in_array($row['prereq_id'] . " > $final_preid", $discovered_edges)) {

                $edges .= " course" . $row['prereq_id'] . " -> course" . $final_preid . " [style=\"setlinewidth(1)\"];\n";

                // Add the edge to the discovered list.
                $discovered_edges[] = $row['prereq_id'] . " > $final_preid";

              }

            }

          }

        }

      }

    }

    $post_courses = array(); $final_postreqs = array();

    // Get all of the post-requisite course information
    $result = db_query("SELECT course_id, set_id FROM {dae_prereq_course} WHERE prereq_id=%d ORDER BY prereq_id, course_id", $course_id);
    while ($row = db_fetch_array($result)) {
      $post_courses[$row['course_id']] = $row['set_id'];
    }

    if ($post_courses) {

        //\                                                /\\
       // \\  Add the post-requisite courses to the map.  // \\
      // ^ \\                                            // ^ \\
      foreach ($post_courses as $postid => $set_id) {

        // Get the post course information
        $post_course = $page_courses[$postid]['course'];
        $post_course_code = $page_courses[$postid]['course_code'];
        $post_course_number = $page_courses[$postid]['course_number'];
        $post_course_mapped = $page_courses[$postid]['mapped'];
        $post_course_viewable = $page_courses[$postid]['viewable'];

        // If the course is viewable add it and
        // it's postrequisites to the course map.
        // If the current course is not visible,
        // add any prerequiste regardless of visibility.
        if ($post_course_viewable || !$course_visibility) {

          $singleton = FALSE;

          // Determine the URL, if the user doesn't
          // have build access and the course is not
          // mapped redirect to the Dal calendar entry.
          if ($post_course_mapped || $build_access) {
            $post_url = $course_url . '/' . $post_course_code . '/' . $post_course_number;
          }
          else {
            $post_url = 'http://www.registrar.dal.ca/calendar/class.php?subj=' . $post_course_code . '&num=' . $post_course_number;
          }

          // Add the selected course to the map
          if ($goal_courses && !$build_access && in_array($postid, $goal_courses)) {
            $output .= " course" . $postid . " [shape=box, fillcolor=\"GoldenRod\", style=\"rounded,filled\", label=\"" . $post_course . "\", URL=\"" . $post_url . "\", fontsize=7, height=0.4, width=0.6];\n";
          }
          else {
            $output .= " course" . $postid . " [shape=box, style=\"rounded\", label=\"" . $post_course . "\", URL=\"" . $post_url . "\", fontsize=7, height=0.4, width=0.6];\n";
          }

          if ($set_id > 1) {
            $edges .= " course" . $course_id . " -> course" . $postid . " [style=\"setlinewidth(2), dashed\"];\n";
          }
          else{
            $edges .= " course" . $course_id . " -> course" . $postid . " [style=\"setlinewidth(2)\"];\n";
          }

          $postpost_courses = array();

          // Select the prerequiste courses to the prerequisite course
          $result = db_query("SELECT course_id, set_id FROM {dae_prereq_course} WHERE prereq_id=%d ORDER BY prereq_id, course_id", $postid);
          while ($row = db_fetch_array($result)) {
            $postpost_courses[$row['course_id']] = $row['set_id'];
          }

          if ($postpost_courses) {

              //\                                                     /\\
             // \\  Add the post post-requisite courses to the map.  // \\
            // ^ \\                                                 // ^ \\
            foreach ($postpost_courses as $post_postid => $set_id) {

              // Get the post post course information
              $postpost_course = $page_courses[$post_postid]['course'];
              $postpost_course_code = $page_courses[$post_postid]['course_code'];
              $postpost_course_number = $page_courses[$post_postid]['course_number'];
              $postpost_course_mapped = $page_courses[$post_postid]['mapped'];
              $postpost_course_viewable = $page_courses[$post_postid]['viewable'];

              // If the course is viewable
              // add it to the course map.
              if ($postpost_course_viewable || !$course_visibility) {

                // Make the post post course list only if the course
                // is not PREU and is a 4000 level or less.
                $final_postreqs[$post_postid] = $post_postid;

                // Determine the URL, if the user doesn't
                // have build access and the course is not
                // mapped redirect to the Dal calendar entry.
                if ($postpost_course_mapped || $build_access) {
                  $postpost_url = $course_url . '/' . $postpost_course_code . '/' . $postpost_course_number;
                }
                else {
                  $postpost_url = 'http://www.registrar.dal.ca/calendar/class.php?subj=' . $postpost_course_code . '&num=' . $postpost_course_number;
                }

                // Add the selected course to the map
                if ($goal_courses && !$build_access && in_array($post_postid, $goal_courses)) {
                  $output .= " course" . $post_postid . " [shape=box, fillcolor=\"GoldenRod\", style=\"rounded,filled\", label=\"" . $postpost_course . "\", URL=\"" . $postpost_url . "\", fontsize=6, height=0.35, width=0.5];\n";
                }
                else {
                  $output .= " course" . $post_postid . " [shape=box, style=\"rounded\", label=\"" . $postpost_course . "\", URL=\"" . $postpost_url . "\", fontsize=6, height=0.35, width=0.5];\n";
                }

                if ($set_id > 1) {
                  $edges .= " course" . $postid . " -> course" . $post_postid . " [style=\"setlinewidth(2), dashed\"];\n";
                }
                else{
                  $edges .= " course" . $postid . " -> course" . $post_postid . " [style=\"setlinewidth(1)\"];\n";
                }

              }

            }

          }

        }

      }

      // Make the final edge connections
      // between the post post courses.
      if ($final_postreqs) {

        foreach ($final_postreqs as $final_postid) {

          $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $final_postid);
          while ($row = db_fetch_array($result)) {

            if (in_array($row['prereq_id'], $final_postreqs)) {
              $edges .= " course" . $row['prereq_id'] . " -> course" . $final_postid . " [style=\"setlinewidth(1)\"];\n";
            }

          }

        }

      }

    }

    $output .= $edges;
    $output .= '}';

    // If the map is a single
    // course, do not display
    if (!$singleton) {

      $output .= $edges;
      $output .= '}';

      // Remove old user maps
      system('rm ' . $pwd . '/graphviz/' . $course_code . '_' . $course_number . '*');

      // Get the iteration to append to the file.
      // This prevents the browser from displaying
      // the cached file because the location changes.
      $iteration = $page_settings['graphviz iteration'];

      // Update the iteration value
      db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $iteration+1, 'graphviz iteration');

      $course_map = $course_code . '_' . $course_number . '_map' . $iteration;

      // Write the .svg and .png output to file
      $my_file = $pwd . '/graphviz/' . $course_map . '.dot';
      $file_handle = fopen($my_file, 'w+') or die("can't open file");
      fwrite($file_handle, $output);
      fclose($file_handle);

      // Execute dot to create the map output.
      system($graphviz_path . 'dot ' . $pwd . '/graphviz/' . $course_map . '.dot -Tcmapx -o ' . $pwd . '/graphviz/' . $course_map . '.map');

      // Execute dot to create the gif output.
      system($graphviz_path . 'dot ' . $pwd . '/graphviz/' . $course_map . '.dot -Tgif -o ' . $pwd . '/graphviz/' . $course_map . '.gif');

      daedalus_parse_coursemap($pwd . '/graphviz/' . $course_map . '.map' );

      // Store the newly creatd map into a variable
      // This is appended to the HTML to make the
      // gif image file linkable.
      $my_file = $pwd . '/graphviz/' . $course_map . '.map';

      $file = fopen($my_file, 'rb') or exit('Unable to open file!');

      while (!feof($file)) {
        $map_file .= fgets($file);
      }

      fclose($file);

      $form['goal_map'] = array(
        '#title' => t('Course Map'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#weight' => $weight['cmap'],
      );

      $course_map_img = '<img id="Constellation" src="' . $base_url . '/graphviz/' . $course_map . '.gif" usemap="#Constellation" />';

      // Display the legend with the user map. Add the map file to
      // the prefix. This is required to make the image map functional.
      $form['goal_map'][] = array(
        '#type' => 'item',
        '#prefix' => t($map_file . '<br /><blockquote>' . $course_map_img),
        '#suffix' => '</blockquote>',
        '#weight' => $weight['cmap']+1,
      );

    }

  }
  else {
    drupal_set_message(t('There is a problem with the graphviz installation. Please contact an administrator.'), 'warning');
  }

    ///////////////////////////////////
   ///////////////////////////////////
  // Display the PREREQUISITE COURSES

  $prereq_list = array(); $other_prereq_list = array(); $set_ids = array(); $prereq_display = '';

  $result = db_query("SELECT id, prereq FROM {dae_course_other_prereq} WHERE course_id=%d", $course_id);
  while ($row = db_fetch_array($result)) {
    $other_prereq_list[$row['id']] = $row['prereq'];
  }

  if ($other_prereq_list) {

    // As prereq id and course id
    foreach ($other_prereq_list as $oid => $other_prereq) {

      $prereq_display .= '<li>' . $other_prereq;

      if ($delete_course_access && !$mapped) {

        // Do not wrap this message with the t()
        // this is displayed through JavaScript.
        $message = 'Are you sure you want to remove the other prerequisite "' . $other_prereq . '"? \n' .
                   '&nbsp;&nbsp;- You must reload the page to view any changes.';

        // Delete using the prereq course id
        $url = $page_url . '/' . $course_code . '/' . $course_number . '/delete/oprereq/' . $oid;

        $prereq_display .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>' . t('delete') . '</strong></a>';

      }

      $prereq_display .= '</li>';

    }

  }

  $result = db_query("SELECT id, prereq_id, set_id FROM {dae_prereq_course} WHERE course_id=%d", $course_id);
  while ($row = db_fetch_array($result)) {

    if ($row['set_id'] == 1) {
      $prereq_list[$row['id']]['prereq_id'] = $row['prereq_id'];
      $prereq_list[$row['id']]['set_id'] = $row['set_id'];
    }
    else{
      $set_ids[$row['set_id']] = $row['set_id'];
    }

  }

  if ($prereq_list) {

    // As prereq id and course id
    foreach ($prereq_list as $preid => $info) {

      $prereq_display .= '<li>';

      $prereq_code = $page_courses[$info['prereq_id']]['course_code'];
      $prereq_numb = $page_courses[$info['prereq_id']]['course_number'];

      $prereq_course = $prereq_code . ' ' . $prereq_numb;

      // Replace each prereq course with a hyperlink the course manage page
      $url = $base_url . '/' . $page_settings['manage courses'] . '/' . $prereq_code . '/' . $prereq_numb;

      $prereq_display .= '<a href="' . $url . '">' . $prereq_course . '</a>';

      if ($delete_course_access && !$mapped) {

        // Do not wrap this message with the t()
        // this is displayed through JavaScript.
        $message = 'Are you sure you want to remove the prerequisite course "' . $prereq_course .
                   '"? \n&nbsp;&nbsp;- Deletion may remove term review information.\n&nbsp;&nbsp;- ' .
                   'You must reload the page to view any changes.';

        // Delete using the prereq course id
        $url = $page_url . '/' . $course_code . '/' . $course_number . '/delete/pcourse/' . $preid;

        $prereq_display .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>' . t('delete') . '</strong></a>';

      }

      $prereq_display .= '</li>';

    }

  }

  if ($set_ids) {

    foreach ($set_ids as $set_id) {

      $prereq_list = array(); $or_delete = '';

      $result = db_query("SELECT id, prereq_id FROM {dae_prereq_course} WHERE course_id=%d AND set_id=%d ORDER BY id", $course_id, $set_id);
      while ($row = db_fetch_array($result)) {
        $prereq_list[$row['id']] = $row['prereq_id'];
      }

      if ($prereq_list) {

        $prereq_display .= '<li>';

        $list_count = count($prereq_list); $i = 0;

        foreach ($prereq_list as $id => $pre_id) {

          $or_url = $base_url . '/' . $page_settings['manage courses'] . '/' . $page_courses[$pre_id]['course_code'] . '/' . $page_courses[$pre_id]['course_number'];
          $or_prereq = '<a href="' . $or_url . '">' . $page_courses[$pre_id]['course'] . '</a>';

          if ($i < $list_count - 1) {
            $or_delete .= $id . '_';
            $prereq_display .= $or_prereq . ' OR ';
          }
          else{
            $or_delete .= $id;
            $prereq_display .= $or_prereq;
          }

          $i++;

        }

        if ($delete_course_access && !$mapped) {

          // Do not wrap this message with the t()
          // this is displayed through JavaScript.
          $message  = 'Are you sure you want to remove the prerequisite set "' . $course1 . ' OR ' . $course2 . '"?';
          $message .= '\n&nbsp;&nbsp;- Deletion may remove term review information.\n&nbsp;&nbsp;- You must reload the page to view any changes.';

          $url = $page_url . '/' . $course_code . '/' . $course_number . '/delete/prereq_set/' . $or_delete;   // Delete using the prereq course ids

          $prereq_display .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>' . t('delete') . '</strong></a>';

        }

        $prereq_display .= '</li>';

      }

    }

  }

  if (!$prereq_display) {
    $prereq_display = '<li><i>' . t('No matches found') . '</i></li>';
    $prereq_flag = 1;
  }

  if (!$prereq_flag || ($manage_access && !$mapped)) {

    $form['prereq'] = array(
      '#title' => t('Prerequisite Courses'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => $weight['prereq'],
    );

    // Show current prereq courses
    $form['prereq'][] = array(
      '#value' => '<blockquote><ul>' . $prereq_display . '</ul></blockquote>',
      '#type' => 'item',
      '#weight' => $weight['prereq']-1,
    );

  }

    ////////////////////////////////////////
   ////////////////////////////////////////
  // Display the ASSUMED LEARNING OUTCOMES

  $assumed_list = array(); $assumed_slo = array();

  // Get the assumed SLOs which are passed to the
  // second parameter of the function by reference.
  daedalus_assumed_slos($course_id, $assumed_list);

  // Format the assumed SLOs and order by rank
  if ($assumed_list) {

    $assumed_slo_placeholders = implode(' OR ', array_fill(0, count($assumed_list), 'id=%d'));

    $result = db_query("SELECT * FROM {dae_slo} WHERE " . $assumed_slo_placeholders . " ORDER BY slo_rank ASC, slo_text ASC", $assumed_list);
    while ($row = db_fetch_array($result)) {
      $assumed_slo[$row['id']] = $row['slo_text'];
    }

    // Foreach assumed slo id as slo text
    foreach ($assumed_slo as $asid => $slo_text) {

      $pslo_display .= '<li>';
      $course_display = '';

      // Add where each assumed learning outcome is taught.
      $result = db_query("SELECT course_id FROM {dae_course_slo} WHERE slo_id=%d", $asid);
      while ($row = db_fetch_array($result)) {

        $course = $page_courses[$row['course_id']]['course'];

        $url = $page_url . '/' . str_replace(' ', '/', $course);

        $course_display .= '[<a href="' . $url . '">' . $course . '</a>] ';

      }

      $url = $base_url . '/' . $page_settings['manage learning outcomes'] . '/' . $asid;

      if ($build_access) {

        $rank = db_result(db_query("SELECT slo_rank FROM {dae_slo} WHERE id=%d", $asid));

        $pslo_display .= '<a href="' . $url . '">' . $slo_text . '</a> <small><small> ' . $course_display . '(' . $rank . ')</small></small></li>';

      }
      else {

        if ($goal_slos && in_array($asid, $goal_slos)) {
          $pslo_display .= '<a href="' . $url . '">' . $slo_text . '</a> ' . $goal_img . '<small><small> ' . $course_display . '</small></small></li>';
        }
        else {
          $pslo_display .= '<a href="' . $url . '">' . $slo_text . '</a> <small><small> ' . $course_display . '</small></small></li>';
        }

      }

    }

  }

  else {
    $pslo_display = '<li><i>' . t('No matches found') . '</i></li>';
    $pslo_flag = 1;
  }

  if (!$pslo_flag || ($manage_access && !$mapped)) {

    $form['pslo'] = array(
      '#type' => 'fieldset',
      '#title' => t('Assumed Learning Outcomes'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => $weight['pslo'],
      '#description' => '&nbsp;&nbsp;&nbsp;' . t(' What the student enrolling in this course is assumed to be able to do.'),
    );

    $form['pslo'][] = array(
      '#type' => 'item',
      '#value' => '<blockquote><ul>' . $pslo_display . '</ul></blockquote>',
      '#weight' => $weight['pslo']-1,
    );

  }

    ////////////////////////////////////////
   ////////////////////////////////////////
  // Display the STUDENT LEARNING OUTCOMES

  $slo_list = array(); $sloid_list = array();

  $result = db_query("SELECT * FROM {dae_course_slo} WHERE course_id=%d", $course_id);
  while ($row = db_fetch_array($result)) {
    $sloid_list[] = $row['slo_id'];
  }

  if ($sloid_list) {

    $sloid_placeholders = implode(' OR ', array_fill(0, count($sloid_list), 'id=%d'));

    $result = db_query("SELECT * FROM {dae_slo} WHERE " . $sloid_placeholders . " ORDER BY slo_rank ASC, slo_text ASC", $sloid_list);

    while ($row = db_fetch_array($result)) {
      $slo_list[$row['id']] = $row['slo_text'];
    }

    foreach ($slo_list as $sid => $slo_text) {

      $slo_display .= '<li>';

      $slo_url = $base_url . '/' . $page_settings['manage learning outcomes'] . '/' . $sid;

      if ($build_access) {

        $rank = db_result(db_query("SELECT slo_rank FROM {dae_slo} WHERE id=%d", $sid));

        $slo_display .= '<a href="' . $slo_url . '">' . $slo_text . '</a> <small><small> (' . $rank . ')</small></small> ';

      }
      else {

        if ($goal_slos && in_array($sid, $goal_slos)) {
          $slo_display .= '<a href="' . $slo_url . '">' . $slo_text . '</a> ' . $goal_img;
        }
        else {
          $slo_display .= '<a href="' . $slo_url . '">' . $slo_text . '</a>';
        }

      }

      if ($delete_course_access && !$mapped) {

        // Do not wrap this message with the t()
        // this is displayed through JavaScript.
        $message = 'Are you sure you want to remove the learning outcome? Deletion may remove assumed learning outcomes, you must reload the page to view any changes.';

        $url = $page_url . '/' . $course_code . '/' . $course_number . '/delete/slo/' . $sid;

        $slo_display .= '<a href="' . $url . '" class="removable" message="' . $message . '"><strong>' . t('delete') . '</strong></a>';

      }

      $slo_display .= '</li>';

    }

  }
  else {
    $slo_display = '<li><i>' . t('No matches found') . '</i></li>';
    $slo_flag = 1;
  }

  if (!$slo_flag || ($manage_access && !$mapped)) {

    $form['slo'] = array(
      '#type' => 'fieldset',
      '#title' => t('Student Learning Outcomes'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => $weight['slo'],
      '#description' => '&nbsp;&nbsp;&nbsp;' . t(' Student learning outcomes that are covered by this course.'),
    );

    $form['slo'][] = array(
      '#type' => 'item',
      '#value' => '<blockquote><ul>' . $slo_display . '</ul></blockquote>',
      '#weight' => $weight['slo']-1,
    );

  }

  // If there are no prereq courses or SLOs added,
  // create a warning informing the user.
  if ($prereq_flag && $slo_flag && !$manage_access) {

    drupal_set_message(t('This course has not been assigned any prerequisite courses or student learning outcomes.'), 'warning' );

  }

  // If the user has the permissions for editing show the
  // textfields to add additional information to the coures.
  if (!$mapped && $build_course_access) {

    for ($i = 0; $i < $page_settings['autocomplete iteration']; $i++) {

      // The settings for the first will be
      // different for all the remaining.
      if ($i == 0) {

        // Not hidden
        $ishid_pcourse = $ishid_tslo = 'no';

        // Singular opening titles
        $pcourse_title = t('Prerequisite Course');
        $tslo_title = t('Student Learning Outcome');

        $pcourse_description = '[<a class="show-pcourse-help">' . t('info') . '</a>]';
        $tslo_description = '[<a class="show-slo-help">' . t('info') . '</a>]';

      }
      else {

        // See if the textfield is hidden
        if ($show_pcourse[$i] == TRUE) {
          $ishid_pcourse = 'no';
        }
        else {
          $ishid_pcourse = 'yes';
        }

        if ($show_tslo[$i] == TRUE) {
          $ishid_tslo = 'no';
        }
        else {
          $ishid_tslo = 'yes';
        }

        // Hidden field titles with iteration number.
        // If each text field is to be be titled or not.
        if ($page_settings['title each autocomplete textfield']) {
          $pcourse_title = '<small>' . t('Prerequisite Course') . ' (' . ($i+1) . ')</small>';
          $tslo_title = '<small>' . t('Student Learning Outcome') . ' (' . ($i+1) . ')</small>';
        }
        else {
          $pcourse_title = '';
          $tslo_title = '';
        }

        $pcourse_description = ''; $tslo_description = '';

      }

      // Make the textfield for a prerequisite course
      $form['prereq']["pcourse_$i"] = array(
        '#type' => 'textfield',
        '#title' => $pcourse_title,
        '#maxlength' => 100,
        '#description' => $pcourse_description,
        '#weight' => $weight['prereq'] + ($i/1000),
        '#autocomplete_path' => 'autocomp/precourse',
        '#prefix' => '<div class="hide-prereq-course" is_hidden="' . $ishid_pcourse . '"><blockquote>',
        '#suffix' => '</blockquote></div>',
      );

      // Make the textfield for a student learning outcome
      $form['slo']["tslo_$i"] = array(
        '#type' => 'textfield',
        '#title' => $tslo_title,
        '#size' => 135,
        '#maxlength' => 255,
        '#size' => 115,
        '#description' => $tslo_description,
        '#weight' => $weight['slo'] + ($i/1000),
        '#autocomplete_path' => 'autocomp/slo',
        '#prefix' => '<div class="hide-slo" is_hidden="' . $ishid_tslo . '"><blockquote>',
        '#suffix' => '</blockquote></div>',
      );

      // Buttons that will unhide 1 hidden
      // textfield (as per javascript).
      if ($i == ($page_settings['autocomplete iteration'] - 1)) {

        $form['prereq']['add-pcourse'] = array(
          '#type' => 'button',
          '#value' => t('Add another'),
          '#attributes' => array('class' => 'add-prereq-course'),
          '#weight' => $weight['prereq']+1,
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote><br />',
        );

        $form['slo']['add-outcome'] = array(
          '#type' => 'button',
          '#value' => t('Add another'),
          '#attributes' => array('class' => 'add-slo'),
          '#weight' => $weight['slo']+1,
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote><br />',
        );

      }

    }

  }

  // Table heading for the Term Review dropdowns.
  $table_header = '<table>' .
      '<tr>' .
        '<th><b>' . t('Ranking') . '</b></th>' .
        '<th><b>' . t('Course') . '</b></th>' .
        '<th><b>' . t('Instructor') . '</b></th>' .
        '<th><b>' . t('Term') . '</b></th>' .
        '<th><b>' . t('Academic Year') . '</b></th>' .
      '</tr>';

    ///////////////////////////////////
   ///////////////////////////////////
  // Display the RELATED TERM REVIEWS
  if ($browse_reviews_access) {

    $review_info = array();

    $result = db_query("SELECT id FROM {dae_term_review_form} WHERE course=%d ORDER BY year", $course_id);
    while ($row = db_fetch_array($result)) {
      $review_info[$row['id']] = db_result(db_query("SELECT term FROM {dae_term_review_form} WHERE id=%d", $row['id']));
    }

    if ($review_info) {

      $review_display = $table_header;

      $form['review'] = array(
        '#title' => t('Term Reviews'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#weight' => $weight['review'],
      );

      // Foreach Review ID and Review Term
      foreach ($review_info as $rid => $term) {

        $i = 0; $total = 0;

        // Calculate the Average Coverage Rating for each Term Review
        $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $rid);
        while ($row = db_fetch_array($result)) {
          $total += $row['rating'];
          $i++;
        }

        if ($i == 0) {
          $rating = '<img src="' . $question2_src . '" alt="?" />';
        }
        elseif (round($total / $i) >= 1) {
          $rating = '<img src="' . $check_src . '" alt="?" />';
        }
        else {
          $rating = '<img src="' . $exclamation_src . '" alt="!" />';
        }

        // Display Extra Review Information
        $instructor_info = db_fetch_array(db_query("SELECT instructor, year FROM {dae_term_review_form} WHERE id=%d", $rid));
        $instructor = $instructor_info['instructor'];
        $year = $instructor_info['year'];

        // Course display information
        $rcourse = $page_courses[$course_id]['course'] . ' - ' . $page_courses[$course_id]['course_name'];

        // The URL to the review
        $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $rid;

        $review_course = '<a href="' . $url . '">' . ucwords($rcourse) . '</a>';

        $review_display .= '<tr>' .
                     '<th>' . $rating . '</th>' .
                     '<th>' . $review_course . '</th>' .
                     '<th>' . $instructor . '</th>' .
                     '<th>' . $term . '</th>' .
                     '<th>' . $year . '/' . ($year+1) . '</th>' .
                   '</tr>';
      }

      $form['review'][] = array(
        '#value' => $review_display . '</table>',
        '#type' => 'item',
        '#weight' => $weight['review'],
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

      ///////////////////////////////////////////////
     ///////////////////////////////////////////////
    // Display the REVIEWS FOR PREREQUISITE COURSES
    $review_info = array(); $prereq_courses = array();
    $prerev_check = FALSE;

    $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $course_id);
    while ($row = db_fetch_array($result)) {
      $prereq_courses[] = $row['prereq_id'];
    }

    if ($prereq_courses) {

      foreach ($prereq_courses as $preid) {

        $result = db_query("SELECT id FROM {dae_term_review_form} WHERE course=%d", $preid);
        while ($row = db_fetch_array($result)) {
          $review_info[$row['id']] = $preid;
        }

      }

      if ($review_info) {

        $prereq_review_display = $table_header;

        // Foreach Review ID and Course ID
        foreach ($review_info as $rid => $cid) {

          if ($rid) {

            // Boolean to determine if the dropdown should be displayed
            // according to the acquisition of a review id.
            $prerev_check = TRUE;

            // Calculate the Average Coverage Rating for each Term Review
            $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $rid);
            $i = 0;
            $total = 0;

            while ($row = db_fetch_array($result)) {
              $total += $row['rating'];
              $i++;
            }

            if ($i == 0) {
              $prerev_rating = '<img src="' . $question2_src . '" alt="?" />';
            }
            elseif (round($total / $i) >= 1) {
              $prerev_rating = '<img src="' . $check_src . '" alt="?" />';
            }
            else {
              $prerev_rating = '<img src="' . $exclamation_src . '" alt="!" />';
            }

            // Display Extra Review Information
            $prereq_review_info = db_fetch_array(db_query("SELECT course, instructor, year, term FROM {dae_term_review_form} WHERE id=%d", $rid));
            $prerev_course     = $prereq_review_info['course'];
            $prerev_instructor = $prereq_review_info['instructor'];
            $prerev_term       = $prereq_review_info['term'];
            $prerev_year       = $prereq_review_info['year'];

            $deleted_info = '';

            if (is_numeric($prerev_course)) {

              // Display Course Information if the selected course is an id. If the course is
              // not an id this means the course for the review has been deleted and the course
              // name has replaced the id in the database.
              $prereq_review_course = $page_courses[$cid]['course'] . ' - ' . $page_courses[$cid]['course_name'];

            }
            else {
              $deleted_info = ' <small><i>(' . t('course deleted') . ')</i></small>';
            }

            // Added daedalus/manage/ to make sure the review is directed to the right page
            $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $rid;

            $prerev_display_course = '<a href="' . $url . '">' . ucwords($prereq_review_course) . '</a>' . $deleted_info;

            $prereq_review_display .= '<tr>' .
                    '<th>' . $prerev_rating . '</th>' .
                    '<th>' . $prerev_display_course . '</th>' .
                    '<th>' . $prerev_instructor . '</th>' .
                    '<th>' . $prerev_term . '</th>' .
                    '<th>' . $prerev_year . '/' . ($year+1) . '</th>' .
                '</tr>';

          }

        }

      }

      if ($prerev_check) {

        $form['revpre'] = array(
          '#title' => t('Prerequisite Course Term Reviews'),
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#weight' => $weight['revpre'],
        );

        $form['revpre'][] = array(
            '#value' => $prereq_review_display . '</table>',
            '#type' => 'item',
            '#weight' => $weight['revpre'],
            '#prefix' => '<blockquote>',
            '#suffix' => '</blockquote>',
        );

      }

    }

      ////////////////////////////////////////////////
     ////////////////////////////////////////////////
    // Display the REVIEWS FOR POST-REQUISTE COURSES

    $postrev_check = FALSE;
    $review_info = array();

    $result = db_query("SELECT course_id FROM {dae_prereq_course} WHERE prereq_id=%d", $course_id);
    while ($row = db_fetch_array($result)) {
      $review_info[$row['course_id']] = db_result(db_query("SELECT id FROM {dae_term_review_form} WHERE course=%d", $row['course_id']));
    }

    if ($review_info) {

      $postreq_review_display = $table_header;

      // Foreach Postreq Course ID and Review ID
      foreach ($review_info as $pcid => $rid) {

        if ($rid) {

          $postrev_check = TRUE;

          // Calculate the Average Coverage Rating for each Term Review
          $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $rid);
          $i = 0;
          $total = 0;

          while ($row = db_fetch_array($result)) {
            $total += $row['rating'];
            $i++;
          }

          if ($i == 0) {
            $postrev_rating = '<img src="' . $question2_src . '" alt="?" />';
          }
          elseif (round($total / $i) >= 1) {
            $postrev_rating = '<img src="' . $check_src . '" alt="?" />';
          }
          else {
            $postrev_rating = '<img src="' . $exclamation_src . '" alt="!" />';
          }

          // Display Extra Review Information
          $postreq_review_info = db_fetch_array(db_query("SELECT course, instructor, year, term FROM {dae_term_review_form} WHERE id=%d", $rid));
          $postrev_course     = $postreq_review_info['course'];
          $postrev_instructor = $postreq_review_info['instructor'];
          $postrev_term       = $postreq_review_info['term'];
          $postrev_year       = $postreq_review_info['year'];

          $deleted_info = '';

          if (is_numeric($postrev_course)) {

            // Display Course Information if the selected course is an id. If the course is
            // not an id this means the course for the review has been deleted and the course
            // name has replaced the id in the database.
            $postreq_review_course = $page_courses[$pcid]['course'] . ' - ' . $page_courses[$pcid]['course_name'];

          }
          else {
            $deleted_info .= ' <small><i>(' . t('course deleted') . ')</i></small>';
          }

          // Added daedalus/manage/ to make sure the review is directed to the right page
          $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $rid;

          $postrev_display_course = '<a href="' . $url . '">' . ucwords($postreq_review_course) . '</a>' . $deleted_info;

          $postreq_review_display .= '<tr>' .
                '<th>' . $postrev_rating . '</th>' .
                '<th>' . $postrev_display_course . '</th>' .
                '<th>' . $postrev_instructor . '</th>' .
                '<th>' . $postrev_term . '</th>' .
                '<th>' . $postrev_year . '/' . ($year+1) . '</th>' .
            '</tr>';

        }

      }

    }

    if ($postrev_check) {

      $form['witreq'] = array(
        '#title' => t('Post-requisite Course Term Reviews'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#weight' => $weight['witreq'],
        '#description' => t('&nbsp;&nbsp;&nbsp; Term reviews of later courses in the learning path.'),
      );

      //show current prereq courses
      $form['witreq'][] = array(
        '#value' => $postreq_review_display . '</table>',
        '#type' => 'item',
        '#weight' => $weight['witreq'],
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

      ///////////////////////////////////////////
     ///////////////////////////////////////////
    // Display the RELATED TERM REVIEW COMMENTS

    $review_comm = '<ul>'; $comment_check = FALSE;
    $review_info = array();

    $result = db_query("SELECT id FROM {dae_term_review_form} WHERE course=%d", $course_id);
    while ($row = db_fetch_array($result)) {
      $review_info[$row['id']] = db_result(db_query("SELECT info FROM {dae_term_review_form} WHERE id=%d", $row['id']));
    }

    // Foreach Review ID and Review Comment(info)
    foreach ($review_info as $rid => $info) {

      if ($info) {

        $comment_check = TRUE;

        // Display Extra Review Information
        $review_form_info = db_fetch_array(db_query("SELECT course, term FROM {dae_term_review_form} WHERE id=%d", $rid));
        $course = $review_form_info['course'];
        $term   = $review_form_info['term'];

        // Display Course Information
        $course = $page_courses[$course]['course'];

        // Added daedalus/manage/ to make sure the review is directed to the right page
        $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $rid;

        $disp_comment = '<a href="' . $url . '">' . ucwords($info) . '</a>';

        $review_comm .= '<li>' . $disp_comment . ' &nbsp;&nbsp;&nbsp;&nbsp;(' . $year . '/' . ($year+1) . ' ' . $term . ' ' . $course . ')';

      }

    }

    if ($comment_check) {

      $form['comments'] = array(
          '#title' => t('Term Review Comments'),
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#weight' => $weight['comments'],
      );

      $form['comments'][] = array(
        '#value' => $review_comm,
        '#type' => 'item',
        '#weight' => $weight['comments'],
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

  }


        /////////////////////////////////////////////////////\\______________
       /////////////////////////////////////////////////////  \\          //\\____________
      //                                                 //    \\        //  \\        //\\__________
     //  DISPLAY THE WARNING FOR THE UNCOVERED COURSES  //      \\      //    \\      //  \\      //\\________
    //                                                 //        \\    //      \\    //    \\    //  \\    //\\______
   /////////////////////////////////////////////////////          \\  //        \\  //      \\  //    \\  //  \\  //\\____
  /////////////////////////////////////////////////////____________\\//__________\\//________\\//______\\//____\\//__\\//\\_
  if ($page_settings['show uncovered learning outcome warning'] && $build_course_access) {

    $amount_uncovered = 0;
    $uncovered = array();

    // The function implements recursion to cycle through each prerequiste course
    // and determines if a given course and any of it's prerequisite courses has
    // covered the given courses learning outcomes. The $amount_uncovered and
    // $uncovered are passed to the function by reference.
    daedalus_uncovered_prerequiste_slos($course_id, $amount_uncovered, $uncovered);

    if ($amount_uncovered && $uncovered) {

      if ($amount_uncovered != 1) {
        $s = 's';
        $isare = 'are';
      }
      else {
        $s = '';
        $isare = 'is';
      }

      foreach ($uncovered as $key => $value) {
        $uncovered[$key] = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $value));
      }

      // Sort array using case insensitive
      // "natural order" algorithm
      natcasesort($uncovered);

      foreach ($uncovered as $slo) {
        $uncovered_list .= '<li>' . $slo . '</li>';
      }

      $warning_display  = '<blockquote><strong>' . t('Warning') . ':</strong><br/> ';
      $warning_display .= t('This course expects students to know ');
      $warning_display .= t('%amt-uncovered learning outcome%s which %isare',
              array('%amt-uncovered' => $amount_uncoverd, '%s' => $s, '%isare' => $isare));
      $warning_display .= t(' not covered by any prerequisite course.');
      $warning_display .= '<ul>' . $uncovered_list . '</ul></blockquote>';

      $form[] = array(
        '#type' => 'item',
        '#value' => $warning_display,
        '#weight' => $weight['warning'],
        '#prefix' => '<div class="messages warning">',
        '#suffix' => '</div><br />',
      );

    }

  }

  // If the user has access to build and the course
  // is not mapped, display a ton of line breaks so
  // that autocomplete information is not cut off..
  if ($build_access && !$mapped) {

    $form[] = array(
      '#type' => 'item',
      '#value' => '<br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                   <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />',
      '#weight' => max($weight) + 100,
    );

  }

  // Submit hidden information to pass
  // to the validate and submit hooks.
  $form['pass-manage-courses'] = array( '#type' => 'value', '#value' => $page_settings['manage courses'], );
  $form['pass-browse-courses'] = array( '#type' => 'value', '#value' => $page_settings['browse courses'], );
  $form['pass-build-term-reviews'] = array( '#type' => 'value', '#value' => $page_settings['build term reviews'], );
  $form['pass-autocomplete-iteration'] = array( '#type' => 'value', '#value' => $page_settings['autocomplete iteration'], );
  $form['pass-course-id'] = array( '#type' => 'value', '#value' => $course_id, );

  return $form;

}


/*
 * Implementation of hook_validate().
 */
function daedalus_manage_courses_form_validate($form, &$form_state) {

  global $base_url;

  $page_url = $form_state['values']['pass-manage-courses'];
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  //get the paramaters
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  switch ($form_state['values']['op']) {

    case $form_state['values']['modify-course']:
    case $form_state['values']['modify-course-2']:

      $alias_code   = $form_state['values']['alias_code'];
      $alias_number = $form_state['values']['alias_number'];

      // You can't have one without the other
      if (!$alias_code && $alias_number) {
        form_set_error('alias_code', t('You must enter an alias course code'));
      }

      if (!$alias_number && $alias_code) {
        form_set_error('alias_number', t('You must enter an alias course number'));
      }

      if ($alias_code && $alias_number) {

        // Make sure the aliase code is in the list of codes
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_valid_course_codes} WHERE course_code LIKE '%s'", $alias_code)) == 0) {
          form_set_error('alias_code', t('The alias course code does not exist'));
        }

        // Do not allow a duplicate alias to be entered
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_course_alias} WHERE alias_course='%s'", $alias_code . ' ' . $alias_number))) {
          form_set_error('alias_code', ' ');
          form_set_error('alias_number', t('Course alias already exists'));
        }

        // Do not enter alias where it already exists as a course
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_course} WHERE course='%s'", $alias_code . ' ' . $alias_number))) {
          form_set_error('alias_number', t('The alias course already exists'));
        }

      }

      $exclusion_course = $form_state['values']['exclusion_course'];

      if ($exclusion_course) {
        // Do not add duplicate course exclusions.
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_course_exclusions} WHERE exclusion_course='%s'", $exclusion_course))) {
          form_set_error('exclusion_course', t('The excluded course already exists'));
        }

        // The excluded course must exist in the database.
        if (!daedalus_valid_course($exclusion_course)) {
          form_set_error('exclusion_course', t('The excluded course is not valid'));
        }

      }

      $coreq_course = $form_state['values']['coreq_course'];

      if ($coreq_course) {

        // Do not add duplicate course co-requisites.
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_course_coreqs} WHERE coreq_course='%s'", $coreq_course))) {
          form_set_error('coreq_course', t('The co-requisite course already exists'));
        }

        // The coreq course must exist in the database.
        if (!daedalus_valid_course($coreq_course)) {
          form_set_error('coreq_course', t('The co-requisite course is not valid'));
        }

      }

      // The current course code and number
      $course_code = drupal_strtoupper($param[0]);
      $course_number = $param[1];

      // The new course information
      $new_course_code   = drupal_strtoupper($form_state['values']['course_code']);
      $new_course_number = $form_state['values']['course_number'];

      $course_id = $form_state['values']['pass-course-id'];

         ////////////////////////////////
        // VALIDATE COURSE CODE & NUMBER

      // Only check if the course information has changed
      if (($course_code != $new_course_code) || ($course_number != $new_course_number)) {

        // Make sure the new code is valid
        if (!daedalus_valid_course_code($new_course_code)) {
          form_set_error('course_code', t('The course code is not valid.'));
        }

        // Make sure the course does not already exist
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_course} WHERE course_code='%s' and course_number=%d ", $new_course_code, $new_course_number))) {
          form_set_error('course_number', t('The course has already been created.'));
        }

      }

      $existing_slos = array(); $existing_prereqs = array(); $existing_other_prereqs = array(); $duplicate_prereq = array();

      // Compile the list of SLOs associated to the course below there
      // is a check to make sure that the SLO is not added twice.
      $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d", $course_id);
      while ($row = db_fetch_array($result)) {
        $existing_slos[$row['slo_id']] = $row['slo_id'];
      }

      // Get the list of prerequisite courses for the same purpose.
      $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $course_id);
      while ($row = db_fetch_array($result)) {
        $existing_prereqs[$row['prereq_id']] = $row['prereq_id'];
      }

//@todo Why is this here?
      // Get the list of other prerequisites for the same purpose.
      $result = db_query("SELECT prereq FROM {dae_course_other_prereq} WHERE course_id=%d", $course_id);
      while ($row = db_fetch_array($result)) {
        $existing_other_prereqs[$row['prereq']] = $row['prereq'];
      }

      for ($i = 0; $i < $form_state['values']['pass-autocomplete-iteration']; $i++) {

           /////////////////////////////////////////////////////////////
          ///////////// VALIDATE STUDENT LEARNING OUTCOMES ////////////

        // Key to return for form_error
        $key = "tslo_$i";
        $outcome = trim($form_state['values'][$key]);

        if ($outcome) {

          // Validate a tag indicated by the hash tag.
          if (drupal_substr($outcome, 0, 1) == '#') {

            if (!daedalus_valid_tag(drupal_substr($outcome, 1))) {
              form_set_error($key, t('The tag "@outcome" can not be added because it does not exist.',
                      array('@outcome' => $outcome)));
            }

          }
          else {

            if (!daedalus_valid_slo($outcome)) {
              form_set_error($key, t('The student learning outcome "@outcome" does not exist.',
                      array('@outcome' => $outcome)));
            }
            else {

              $slo_id = daedalus_slo_id($outcome);

              if (in_array($slo_id, $existing_slos)) {
                form_set_error($key, t('The student learning outcome "@outcome" is already listed as an learning outcome.',
                      array('@outcome' => $outcome)));
              }

            }

          }

        }

           /////////////////////////////////////////////////////////////
          ////////////// VALIDATE PREREQUISITE COURSES ////////////////

        // Key to return if there is a form_error
        $key = "pcourse_$i";
        $pcourse = drupal_strtoupper(trim($form_state['values'][$key]));

        if ($pcourse) {

          // If the user entered a duplicate prerequisite course
          // with the "OR" identifier check each course to ensure
          // that they both are valid.
          if (strpos($pcourse, 'OR') == TRUE) {

            $courses = explode('OR', $pcourse);

            foreach ($courses as $pc) {

              $pc = trim($pc);

              $pc_id = daedalus_course_id($pc);

              if (!daedalus_valid_course($pc)) {
                form_set_error($key, t('The course "@pc" in "@pcourse" is not valid. Only valid courses may be entered into prerequiste sets.',
                        array('@pc' => $pc, '@pcourse' => $pcourse)));
              }
              elseif (in_array($pc_id, $existing_prereqs)) {
                form_set_error($key, t('The course "@pc" in "@pcourse" already exists as a prerequisite course.',
                        array('@pc' => $pc, '@pcourse' => $pcourse)));
              }
              elseif (in_array($pc, $duplicate_prereq)) {
                form_set_error($key, t('The course "@pc" in "@pcourse" is a duplicate entry.',
                        array('@pc' => $pc, '@pcourse' => $pcourse)));
              }

              $duplicate_prereq[$pc] = $pc;

            }

          }
          elseif (daedalus_valid_course($pcourse)) {

            $pc_id = daedalus_course_id($pcourse);

            if (in_array($pc_id, $existing_prereqs)) {
              form_set_error($key, t('The course "@pcourse" already exists as a prerequisite course.',
                      array('@pcourse' => $pcourse)));
            }
            elseif (in_array($pcourse, $duplicate_prereq)) {
              form_set_error($key, t('The course "@pcourse" is a duplicate entry.',
                      array('@pcourse' => $pcourse)));
            }

            $duplicate_prereq[$pcourse] = $pcourse;

          }

        }

      } // End For

    break;

  }

}


/**
 * Implementation of hook_submit().
 */
function daedalus_manage_courses_form_submit($form, &$form_state) {

  global $base_url;

  $page_url = $form_state['values']['pass-manage-courses'];
  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length); // Course Code
  $param[1] = arg(1+$page_url_length); // Course #

  $course_id = $form_state['values']['pass-course-id'];

  switch ($form_state['values']['op']) {

    // Redirect for term reviews
    case $form_state['values']['submit-review']:

      drupal_goto($base_url . '/' . $form_state['values']['pass-build-term-reviews'] . '/' . $param[0] . '/' . $param[1]);

      break;

    // Swith the course mapped value to mapped
    case $form_state['values']['mapped']:

      drupal_set_message(t('The course is now mapped') );

      db_query("UPDATE {dae_course} SET mapped=1 WHERE id=%d", $course_id);

      break;

    // Swith the course mapped value to unmapped
    case $form_state['values']['unmapped']:

      drupal_set_message(t('The course is now unmapped') );

      db_query("UPDATE {dae_course} SET mapped=0 WHERE id=%d", $course_id);

      break;

    // Delete the course confimation
    case $form_state['values']['delete-course']:

      drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1] . '/delete/this');

      break;

    // Delete the course and all
    // of it's associated data here.
    case $form_state['values']['delete-forward']:

      db_query("DELETE FROM {dae_course_alias} WHERE parent_id=%d", $course_id);

      db_query("DELETE FROM {dae_course_slo} WHERE course_id=%d", $course_id);

      db_query("DELETE FROM {dae_prereq_course} WHERE course_id=%d OR prereq_id=%d", $course_id, $course_id);

      $course_info = db_fetch_array(db_query("SELECT course, course_name FROM {dae_course} WHERE id=%d", $course_id));

      $course_name = $course_info['course'] . ' ' . $course_info['course_name'];

      db_query("UPDATE {dae_term_review_form} SET course='%s' WHERE course='%d'", $course_name, $course_id);

      db_query("DELETE FROM {dae_course} WHERE id=%d", $course_id);

      drupal_set_message(t('The course "@p0 @p1" has been deleted.'),
          array('@p0' => $param[0], '@p1' => $param[1]));

      drupal_goto($base_url . '/' . $form_state['values']['pass-browse-courses']);

      break;

    // Cancel deletion of the course
    case $form_state['values']['delete-reverse']:

      drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1]);

      break;

    // Submit buttons to modify the course
    case $form_state['values']['modify-course']:
    case $form_state['values']['modify-course-2']:

      $alias_code   = drupal_strtoupper($form_state['values']['alias_code']);
      $alias_number = $form_state['values']['alias_number'];
      $alias_course = $alias_code . ' ' . $alias_number;

       //////////////////////////
      // Submit the ALIAS COURSE
      if ($alias_code && $alias_number) {
        db_query("INSERT INTO {dae_course_alias} (parent_id, alias_code, alias_number, alias_course)
                  VALUES (%d,'%s','%s','%s')", $course_id, $alias_code, $alias_number, $alias_course);
      }

      $exclusion_course = drupal_strtoupper($form_state['values']['exclusion_course']);

       //////////////////////////////
      // Submit the COURSE EXCLUSION
      if ($exclusion_course) {
        db_query("INSERT INTO {dae_course_exclusions} (parent_id, exclusion_id, exclusion_course)
                  VALUES (%d,%d,'%s')", $course_id, daedalus_course_id($exclusion_course), $exclusion_course);
      }

      $coreq_course = drupal_strtoupper($form_state['values']['coreq_course']);

       /////////////////////////////////
      // Submit the COURSE CO-REQUISITE
      if ($coreq_course) {
        db_query("INSERT INTO {dae_course_coreqs} (parent_id, coreq_id, coreq_course)
                  VALUES (%d,%d,'%s')", $course_id, daedalus_course_id($coreq_course), $coreq_course);
      }

      $outcome_array = array(); $tag_array = array(); $tagged_slos = array();
      $existing_slos = array(); $prereq_course_args = array(); $other_prereq_args = array();
      $outcome_count = 0; $tagged_slo_count = 0; $prereq_course_count = 0; $other_prereq_count = 0;

      // Get list of Prereq SLOs associated with the outcome
      // and add them to the suggested outcome ids if any.
      $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d", $course_id);
      while ($row = db_fetch_array($result)) {
        $existing_slos[$row['slo_id']] = $row['slo_id'];
      }

      // Seletect the maximum set id, if there
      // are no prerequisite courses the set id
      // will automatically be set to two below.
      $set_id = db_result(db_query("SELECT MAX(set_id) FROM {dae_prereq_course} WHERE course_id=%d", $course_id));

      if (!$set_id) {
        $set_id = 1;
      }

      // Iterate each hidden textfield regardless
      // if there are values or not.
      for ($i = 0; $i < $form_state['values']['pass-autocomplete-iteration']; $i++) {

         //////////////////////////////
        // SUBMIT PREREQUISITE COURSES
        $pcourse = drupal_strtoupper(trim($form_state['values']["pcourse_$i"]));

        if ($pcourse) {

          if (strpos($pcourse, 'OR') == TRUE) {

            // Increase the set id
            $set_id++;

            $courses = explode('OR', $pcourse);

            foreach ($courses as $pc) {

              $pc = trim($pc);

              $pc_id = db_result(db_query("SELECT id FROM {dae_course} WHERE course='%s'", $pc));

              $prereq_course_args[] = $set_id;
              $prereq_course_args[] = $course_id;
              $prereq_course_args[] = $pc_id;
              $prereq_course_count++;

            }

          }
          else {

            // If the course is valid add it
            // to the prereq courses list.
            if (daedalus_valid_course($pcourse)) {

              $prereq_id = db_result(db_query("SELECT id FROM {dae_course} WHERE course='%s'", $pcourse));

              $prereq_course_args[] = 1;
              $prereq_course_args[] = $course_id;
              $prereq_course_args[] = $prereq_id;
              $prereq_course_count++;

            }
            else{

              // If not, add it to the other prereqs list,
              // and keep the original formatting.
              $other_prereq_args[] = $course_id;
              $other_prereq_args[] = trim($form_state['values']["pcourse_$i"]);
              $other_prereq_count++;

            }

          }

        }

         ///////////////////////////////////
        // SUBMIT STUDENT LEARNING OUTCOMES
        $outcome = trim($form_state['values']["tslo_$i"]);

        if ($outcome) {

          if (drupal_substr($outcome, 0, 1) == '#') {

            // Remove the # from the outcome
            $tag_array[] = drupal_substr($outcome, 1);

          }
          else {

            $current_sloid = daedalus_slo_id($outcome);

            // Do not add an already associated SLO.
            if (!in_array($current_sloid, $existing_slos)) {

              $outcome_array[] = $course_id;

              $outcome_array[] = $current_sloid;

              $outcome_count++;

            }

          }

        }

      }

      // Add the prequisite courses
      if ($prereq_course_count) {

        $prereq_course_placeholders = implode(', ', array_fill(0, $prereq_course_count, '(%d,%d,%d)'));

        db_query("INSERT INTO {dae_prereq_course} (set_id, course_id, prereq_id) VALUES " . $prereq_course_placeholders, $prereq_course_args);

      }

      // Add the other prequisites
      if ($other_prereq_count) {

        $other_prereq_placeholders = implode(', ', array_fill(0, $other_prereq_count, "(%d,'%s')"));

        db_query("INSERT INTO {dae_course_other_prereq} (course_id, prereq) VALUES " . $other_prereq_placeholders, $other_prereq_args);

      }

      // Add the outcomes
      if ($outcome_count) {

        $outcome_value_placeholders = implode(', ', array_fill(0, $outcome_count, '(%d,%d)'));

        db_query("INSERT INTO {dae_course_slo} (course_id, slo_id) VALUES " . $outcome_value_placeholders, $outcome_array);

      }

      // Add outcomes associated
      // by a tag value.
      if ($tag_array) {

        foreach ($tag_array as $tag) {

          $tag_id = daedalus_tag_id($tag);

          $result = db_query("SELECT DISTINCT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tag_id);
          while ($row = db_fetch_array($result)) {

            if (!in_array($row['slo_id'], $existing_slos)) {
              $tagged_slos[$row['slo_id']] = $row['slo_id'];
            }

          }

        }

        if ($tagged_slos) {

          $tagged_slo_count = count($tagged_slos);

          $new_tagged_slos = array();

          foreach ($tagged_slos as $new_id) {

            $new_tagged_slos[] = $course_id;

            $new_tagged_slos[] = $new_id;

          }

          $tagged_slo_placeholders = implode(', ', array_fill(0, $tagged_slo_count, '(%d,%d)'));

          db_query("INSERT INTO {dae_course_slo} (course_id, slo_id) VALUES " . $tagged_slo_placeholders, $new_tagged_slos);

        }

      }

       ////////////////////////////////////
      // SUBMIT COURSE CODE, NAME & NUMBER
      $ncode = drupal_strtoupper(trim($form_state['values']['course_code']));
      $nnumb = trim($form_state['values']['course_number']);
      $nname = trim($form_state['values']['course_name']);
      $ncour = $ncode . ' ' . $nnumb;
      $nview = $form_state['values']['viewable'];

      db_query("UPDATE {dae_course}
                   SET course_code='%s', course_number=%d, course_name='%s', course='%s', viewable=%d
                 WHERE id=%d", $ncode, $nnumb, $nname, $ncour, $nview, $course_id);

      drupal_goto($base_url . '/' . $page_url . '/' . $ncode . '/' . $nnumb);

      break;

    case $form_state['values']['dae-help-submit']:

      $help_text = $form_state['values']['dae-help'];

      if ($help_text) {

        if (db_result(db_query("SELECT COUNT(*) FROM {dae_page_help} WHERE page_url='%s'", $page_url))) {
          db_query("UPDATE {dae_page_help} SET help_text='%s' WHERE page_url='%s'", $help_text, $page_url);
        }
        else {
          db_query("INSERT INTO {dae_page_help} (page_url, help_text) VALUES ('%s', '%s')", $page_url, $help_text);
        }

        drupal_set_message(t('Help information saved.') );

        drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1]);

      }

      break;

  }

}