<?php

/**
 * @file
 * daedalus-menu-analyse.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


/**
 * DAEDALUS-ANALYSE-MENU-FILES.PHP
 *
 * TODO: Analyse course dropdowns such as COURSE STUBS display a ton of information.
 *       Suggestions:  1 - Split this information up by department
 *                     2 - Split this information up in multiple colums
 *                     3 - Split the info up depending on how many taught slos there are
 * TODO: Analyse courses that are crosslisted.
 */


//------------------------------------------------------------------------------
// Daedalus->Analyse Callback Functions
//------------------------------------------------------------------------------
function daedalus_analyse_page() {
  return drupal_get_form('daedalus_analyse_form');
}


/**
 * Menu Location: Daedalus -> Analyse
 * URL Location:  daedalus/daedalus/analyse
 *
 * Displays the Analyse menu.
 *     SECTION: Daedalus->Analyse->Course
 *     SECTION: Daedalus->Analyse->Learning Outcomes
 *     SECTION: Daedalus->Analyse->Term Reviews
 *
 * This single function handles the entire menu using the database
 * settings to select the appropriate information to display.
 */
function daedalus_analyse_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='analyse' OR
                             setting='question mark' OR
                             setting='list size before collapsing' OR
                             setting='manage courses' OR
                             setting='manage learning outcomes' OR
                             setting='manage term reviews' OR
                             setting='analyse term reviews' OR
                             setting='manage student' OR
                             setting='file duration'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  global $base_url;

  // Set current page url.
  $page_url = $help_url = $page_settings['analyse'];

  // Get URL parameters
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  $param = array();
  $param[0] = arg(0+$page_url_length);  // URL info 'term-review'
  $param[1] = arg(1+$page_url_length);  // Year info
  $param[2] = arg(2+$page_url_length);  // Year Selection info (change || current || viewall)

  // Get the permissions to hide and show the various analytical
  // sections depending on the users peremission level.
  $course_permission = user_access('daedalus analyse course');
  $slo_permission = user_access('daedalus analyse learning outcomes');
  $term_review_permission = user_access('daedalus analyse term reviews');
  $student_file_permission = user_access('daedalus analyse student files');
  $build_access = user_access('daedalus build');

  // Get the images
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  if ($param[0]) {

    // Replace the hypen and capitalize the first characters
    drupal_set_title(t('Analyse @selection Statistics !help',
            array('@selection' => ucwords(str_replace('-', ' ', $param[0])), '!help' => $show_help)));

  }
  else {

    // Cannot fully sanitize $show_help using
    // check_plain(), filter_xss() or similar.
    drupal_set_title(t('Analyse Daedalus Statistics !help',
            array('!help' =>$show_help)));

  }

  $year = Date('Y');

  $form = array();

  if (module_exists('magellan')) {

    global $user;

    // If the current user is a Magellan Advisor, determine
    // if there is a current advising session open.
    if (magellan_advisor($user->uid)) {

      $result = db_query("SELECT id, add_time
        FROM {mag_advisor_session} WHERE advisor_id=%d", $user->uid);

      while ($row = db_fetch_array($result)) {
        $session_id = $row['id'];
        $add_time = $row['add_time'];
      }

      // Get the session name.
      $current_session = $_COOKIE[session_name()];

      // If in session, set the user name as
      // to the selected students username.
      if ($session_id == $current_session) {

        // Get the current session time
        $session_time = db_result(db_query("SELECT session_time
          FROM {mag_session_log} WHERE advisor_id=%d
          AND session_id='%s'", $user->uid, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'analyse', 'advisor');

      }

    }

    // If the current user is a Magellan Support Staff,
    // determine if there is a current support session open.
    if (magellan_support($user->uid)) {

      $result = db_query("SELECT id, add_time
        FROM {mag_support_session} WHERE support_id=%d", $user->uid);

      while ($row = db_fetch_array($result)) {
        $session_id = $row['id'];
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
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'analyse', 'support');

      }

    }

  }

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form[] = daedalus_help_form($help_url, 1, 1);

  // If there are no paramaters passed, or the paramater passed is a number, then everything
  // should be put in fieldsets to hold the stats of all their related information
  if ($param[0] == '') {       // If it's just the default, or a year then
                                 // make areas for statistics on various things
    if ($course_permission) {

      //course statistics
      $form['course'] = array(
        '#type' => 'fieldset',
        '#title' => t('Course Statistics'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

    }

    if ($slo_permission) {

      //learning outcome statistics
      $form['slo'] = array(
        '#type' => 'fieldset',
        '#title' => t('Student Learning Outcome Statistics'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

    }

    if ($term_review_permission) {

      //term review statistics
      $form['review'] = array(
        '#type' => 'fieldset',
        '#title' => t('Term Review Statistics'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

    }

    if ($student_file_permission && module_exists('magellan')) {

      //student statistics
      $form['student'] = array(
        '#type' => 'fieldset',
        '#title' => t('Student File Statistics'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

    }

  }
  else {

    // If there was a parater passed we need
    // to check for a year
    if (is_numeric($param[1])) {
      $year = $param[1];
    }

  }

  // Get the number of courses. Right now this will select crosslisted courses
  // such as CSCI 2112 and MATH 2112 and count this is two separate courses.
  $course_total = db_result(db_query("SELECT COUNT(*) FROM {dae_course}"));

  // Decimal places in rounding numbers
  $decimal_places = 2;

    ////// ANALYSE COURSE SECTION //////////////////////////////////////////////
   /////// ANALYSE COURSE SECTION ///////// Calculate and show course stats ///
  //////// ANALYSE COURSE SECTION ////////////////////////////////////////////
  if (($param[0] == 'course' && $course_permission) || (!$param[0] && $course_permission)) {

    $num_mapped = 0;

    // Select course codes
    $result = db_query("SELECT * FROM {dae_course} ORDER BY course");
    while ($row = db_fetch_array($result)) {

      // URL link to view the course
      $url = $base_url . '/' . $page_settings['manage courses'] . '/' . $row['course_code'] . '/' . $row['course_number'];

      $course_link = '<li><a href="' . $url . '">' . $row['course'] . ' - ' . $row['course_name'] . '</a></li>';

      // If not mapped then it might need to go into a list to be provided to the user
      if (!$row['mapped']) {

        $slo_count = db_result(db_query("SELECT COUNT(*) FROM {dae_course_slo} WHERE course_id=%d", $row['id']));

        // Courses with more than 10 SLO's taught
        // are considered "nearly completed"
        if ($slo_count > 10) {
          $complete_course .= $course_link;
        }
        // Otherwise it's a course
        // stub (not quite complete)
        elseif ($slo_count <= 5) {
          $course_stubs .= $course_link;
        }

      }
      else {
        $num_mapped++;
      }

      // Displays courses that have uncovered student learning outcomes.
      if (!db_result(db_query("SELECT COUNT(*) FROM {dae_course_slo} WHERE course_id=%d", $row['id']))) {
        $uncovered_slo .= $course_link;
      }

      // Displays courses with no assigned prerequisite courses.
      if (!db_result(db_query("SELECT COUNT(*) FROM {dae_prereq_course} WHERE course_id=%d", $row['id']))) {
        $uncovered_prereq .= $course_link;
      }

    }

    if ($course_total > 0) {
      $mapped_percent = round(100 * ($num_mapped / $course_total), $decimal_places);
    }
    else {
      $mapped_percent = 100;
    }

    $form['course'][] = array(
      '#title' => t('Total course count'),
      '#type' => 'item',
      '#value' => $course_total,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['course'][] = array(
      '#title' => t('Mapped course percentage'),
      '#type' => 'item',
      '#value' => $mapped_percent . '%',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Nearly mapped courses
    // NEARLY MAPPED COURSES
    $form['course']['nearly'] = array(
      '#type' => 'fieldset',
      '#title' => t('Nearly mapped courses') . ' (' . substr_count($complete_course, '<li>') . ')',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays courses that are almost completely mapped with ten or more student learning outcomes.'),
      '#prefix' => '<br />',
    );

    if (substr_count($complete_course, '<li>') < 1) {
      $complete_course = '<li><i>' . t('No matches found') . '</i></li>';
    }

    $form['course']['nearly'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $complete_course . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Course stubs
    // COURSE STUBS
    $form['course']['stubbs'] = array(
      '#type' => 'fieldset',
      '#title' => t('Course stubs') . ' (' . substr_count($course_stubs, '<li>') . ')',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays unmapped courses with fewer than five student learning outcomes.'),
    );

    if (substr_count($course_stubs, '<li>') < 1) {
      $course_stubs = '<li><i>' . t('No matches found') . '</i></li>';
    }

    $form['course']['stubbs'][] = array(
      '#type'  => 'item',
      '#value' => t('<ul>' . $course_stubs . '</ul>'),
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Courses without student learning outcomes
    // COURSES WITHOUT STUDENT LEARNING OUTCOMES
    $form['course']['untaught'] = array(
      '#title' => t('Courses without student learning outcomes') . ' (' . substr_count($uncovered_slo, '<li>') . ')',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays courses that do not have any student learning outcomes.'),
    );

    if (substr_count($uncovered_slo, '<li>') < 1) {
      $uncovered_slo = '<li><i>' . t('No matches found') . '</i></li>';
    }

    $form['course']['untaught'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $uncovered_slo . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Courses without prerequisite courses
    // COURSES WITHOUT PREREQUISITE COURSES
    $form['course']['no_prereq'] = array(
      '#title' => t('Courses without a prerequisite course') . ' (' . substr_count($uncovered_prereq, '<li>') . ')',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays courses with no assigned prerequisite courses.'),
    );

    if (substr_count($uncovered_prereq, '<li>') < 1) {
      $uncovered_prereq = '<li><i>' . t('No matches found') . '</i></li>';
    }

    $form['course']['no_prereq'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $uncovered_prereq . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

  }

    //// ANALYSE LEARNING OUTCOMES SECTION /////////////////////////////////////
   ///// ANALYSE LEARNING OUTCOMES SECTION ////////////////////////////////////
  ////// ANALYSE LEARNING OUTCOMES SECTION ///////////////////////////////////
  if (($param[0] == 'slo' && $slo_permission) || (!$param[0] && $slo_permission)) {

    // Integer Variables
    $total_rslo = 0; $rslo_course = 0; $rslo_count = 0; $avg_rslo_count = 0;
    $total_slo = 0; $slo_course = 0; $slo_count = 0; $avg_slo_count = 0; $total_slos = 0;

    // String Variables
    $uncovered_slos = ''; $multiple_slo = ''; $untagged_slo = '';
    $wout_slo_list = ''; $terminal_slo_list = '';

    // Array Variables
    $slo_ids = array(); $slo_info = array(); $slos_with_prereqs = array();
    $slos_without_prereqs = array(); $slos_as_prereqs = array(); $not_prereqs = array();

    // Select all student learning outcomes
    $result = db_query("SELECT * FROM {dae_slo} ORDER BY slo_rank ASC, slo_text ASC");
    while ($row = db_fetch_array($result)) {

      // Count the SLOs
      $total_slos++;

      // Create the list of SLO ids.
      $slo_ids[$row['id']] = $row['id'];

      // Create the list of SLO info.
      $slo_info[$row['id']]['slo_text'] = $row['slo_text'];
      $slo_info[$row['id']]['slo_rank'] = $row['slo_rank'];

      $url = $base_url . '/' . $page_settings['manage learning outcomes'] . '/' . $row['id'];

      // Display the rank for users that have
      // build access, admin and builder type.
      if ($build_access) {
        $link_to_slo = '<li><a href="' . $url . '">' . $row['slo_text'] . '</a> <item title="' . t('outcome rank') . '"><small>(' . $row['slo_rank'] . ')</small></item></li>';
      }
      else {
        $link_to_slo = '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
      }

      $slo_times_covered = db_result(db_query("SELECT COUNT(*) FROM {dae_course_slo} WHERE slo_id=%d", $row['id']));

      if ($slo_times_covered == 0) {
        $uncovered_slos .= $link_to_slo;
      }
      elseif ($slo_times_covered > 1) {
        $multiple_slo .= $link_to_slo;
      }

      // Create the list of SLO's that have not been tagged
      $times_tagged = db_result(db_query("SELECT COUNT(*) FROM {dae_slo_tag} WHERE slo_id=%d", $row['id']));

      if ($times_tagged == 0) {
        $untagged_slo .= $link_to_slo;
      }

    }

    // Get the other statistics if there
    // are learning outcomes present.
    if ($slo_ids) {

      // Select all course ids
      $result = db_query("SELECT id FROM {dae_course} ORDER BY course");
      while ($row = db_fetch_array($result)) {

        // Find the number of taught and expected
        $slo_count  = db_result(db_query("SELECT COUNT(*) FROM {dae_course_slo} WHERE course_id=%d", $row['id']));
        $rslo_count = db_result(db_query("SELECT COUNT(*) FROM {dae_prereq_course} WHERE course_id=%d", $row['id']));

        if ($slo_count > 0) {
          $total_slo += $slo_count;
          $slo_course++;
        }

        if ($rslo_count > 0) {
          $total_rslo += $rslo_count;
          $rslo_course++;
        }

      }

      // Average prereq and post-req SLO count per course.
      if ($slo_course > 0) {
        $avg_slo_count = round(($total_slo  / $slo_course), $decimal_places);
      }

      if ($rslo_course > 0) {
        $avg_rslo_count = round(($total_rslo / $rslo_course), $decimal_places);
      }


      $result = db_query("SELECT target, pre_slo FROM {dae_prereq_slo} ORDER BY target");
      while ($row = db_fetch_array($result)) {

        $slos_with_prereqs[$row['target']] = $row['target'];

        $slos_as_prereqs[$row['pre_slo']] = $row['pre_slo'];

      }

      $slos_without_prereqs = array_diff($slo_ids, $slos_with_prereqs);
      $outcome_title = t('outcome rank');

      if ($slos_without_prereqs) {

        foreach ($slos_without_prereqs as $id) {

          $slo_rank = $slo_info[$id]['slo_rank'];
          $slo_text = $slo_info[$id]['slo_text'];

          $url = $base_url . '/' . $page_settings['manage learning outcomes'] . '/' . $id;

          if ($build_access) {
            $wout_slo_list .= '<li><a href="' . $url . '">' . $slo_text . '</a> <item title="' . $outcome_title . '"><small>(' . $slo_rank . ')</small></item></li>';
          }
          else {
            $wout_slo_list .= '<li><a href="' . $url . '">' . $slo_text . '</a></li>';
          }

        }

      }

      $not_prereqs = array_diff($slo_ids, $slos_as_prereqs);

      if ($not_prereqs) {

        foreach ($not_prereqs as $id) {

          $slo_rank = $slo_info[$id]['slo_rank'];
          $slo_text = $slo_info[$id]['slo_text'];

          $url = $base_url . '/' . $page_settings['manage learning outcomes'] . '/' . $id;

          if ($build_access) {
            $terminal_slo_list .= '<li><a href="' . $url . '">' . $slo_text . '</a> <item title="' . $outcome_title . '"><small>(' . $slo_rank . ')</small></item></li>';
          }

          else {
            $terminal_slo_list .= '<li><a href="' . $url . '">' . $slo_text . '</a></li>';
          }

        }

      }

    }

    $form['slo'][] = array(
      '#type' => 'item',
      '#title' => t('Total student learning outcomes'),
      '#value' => $total_slos,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['slo'][] = array(
      '#type' => 'item',
      '#title' => t('Total tags'),
      '#value' => db_result(db_query("SELECT COUNT(*) FROM {dae_tag}")),
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['slo'][] = array(
      '#type' => 'item',
      '#title' => t('Average student learning outcomes'),
      '#value' => $avg_slo_count,
      '#description' => t('Averages student learning outcomes from courses with assigned outcomes'),
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    $form['slo'][] = array(
      '#type' => 'item',
      '#title' => t('Average post-requisite student learning outcomes'),
      '#value' => $avg_rslo_count,
      '#description' => t('Averages post-requisite student learning outcomes from courses with assigned post-requisite outcomes'),
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Setting the substr_count to >= to $collapse_amount since the $collapse_amount is zero. This is to account for
    // items that are zero. Instead of being the only field not collapsible it will and if the collapse amount is set
    // to more than zero these items will collapse.

    // Orphan student learning outcomes
    // ORPHAN STUDENT LEARNING OUTCOMES
    $form['slo']['uslo'] = array(
      '#title' => t('Orphan student learning outcomes') . ' (' . substr_count($uncovered_slos, '<li>') . ')',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays student learning outcomes that are not being covered by any course.'),
      '#prefix' => '<br />',
    );

    if (substr_count($uncovered_slos, '<li>') == 0) {
      $uncovered_slos = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
    }

    $form['slo']['uslo'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $uncovered_slos . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // student learning outcomes without prerequiste outcomes
    // STUDENT LEARNING OUTCOMES WITHOUT PREREQUISTE OUTCOMES
    $form['slo']['woslo'] = array(
      '#title' => t('Student learning outcomes without prerequisite outcomes') . ' (' . substr_count($wout_slo_list, '<li>') . ')',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays student learning outcomes that do not have a single prerequisite student learning outcome.'),
    );

    if (substr_count($wout_slo_list, '<li>') == 0) {
      $wout_slo_list = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
    }

    $form['slo']['woslo'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $wout_slo_list . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // terminal student learning outcomes
    // TERMINAL STUDENT LEARNING OUTCOMES
    $form['slo']['termslo'] = array(
      '#title' => t('Terminal student learning outcomes') . ' (' . substr_count($terminal_slo_list, '<li>') . ')',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays student learning outcomes that have not been assigned as a prerequisite learning outcome to any other outcome.'),
    );

    if (substr_count($terminal_slo_list, '<li>') == 0) {
      $terminal_slo_list = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
    }

    $form['slo']['termslo'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $terminal_slo_list . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // Student learning outcomes covered in more than one course
    // STUDENT LEARNING OUTCOMES COVERED IN MORE THAN ONE COURSE
    $form['slo']['mslo'] = array(
      '#title' => t('Student learning outcomes covered in more than one course') . ' (' . substr_count($multiple_slo, '<li>') . ')',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays student learning outcomes that are covered in more than one course.'),
    );

    if (substr_count($multiple_slo, '<li>') == 0) {
      $multiple_slo = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
    }

    $form['slo']['mslo'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $multiple_slo . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

    // untagged student learning outcomes
    // UNTAGGED STUDENT LEARNING OUTCOMES
    $form['slo']['tagslo'] = array(
      '#title' => t('Untagged student learning outcomes') . ' (' . substr_count($untagged_slo, '<li>') . ')',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays student learning outcomes that have not been tagged.'),
    );

    if (substr_count($untagged_slo, '<li>') == 0) {
      $untagged_slo = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
    }

    $form['slo']['tagslo'][] = array(
      '#type' => 'item',
      '#value' => '<ul>' . $untagged_slo . '</ul>',
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

  }

    ////// ANALYSE TERM REVIEW SECTION /////////////////////////////////////////
   /////// ANALYSE TERM REVIEW SECTION ////////////////////////////////////////
  //////// ANALYSE TERM REVIEW SECTION ///////////////////////////////////////
  if (($param[0] == 'term-review' && $term_review_permission) || (!$param[0] && $term_review_permission)) {

    // If no reviews found
    if (db_result(db_query("SELECT COUNT(*) FROM {dae_term_review_form}")) == 0) {
      $form['review'][] = array(
        '#type' => 'item',
        '#title' => t('Total term reviews'),
        '#value' => t('0'),
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );
    }

    else {

      $form['review'][] = array(
        '#type' => 'item',
        '#title' => t('Total term reviews'),
        '#value' => db_result(db_query("SELECT COUNT(*) FROM {dae_term_review_form}")),
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // Get the review form id's
      if ($param[2] == 'viewall' || (($param[0] && $param[1] && $param[2]) == '' )) {
        $result = db_query("SELECT id, year FROM {dae_term_review_form}");
      }
      else {
        $result = db_query("SELECT id, year FROM {dae_term_review_form} WHERE year = %d", $year);
      }

      $review_id_list = array();

      while ($row = db_fetch_array($result)) {
        $review_id_list[] = $row['id'];
      }

      ////////// 1 - List all taught SLO which have been rated 0 for not covered
      //////////     at least once in the last and current academic year.
      //////////     Hold the id's of all the feedback forms that met
      //////////     this from the previous 2 years.

      $tslo_ncovered_list = array();

      foreach ($review_id_list as $review_id) {

        $tslo_ncovered_count = db_result(db_query("SELECT COUNT(*)
                                                     FROM {dae_term_review_ratings}
                                                    WHERE term_review_form_id=%d
                                                      AND type='taught'
                                                      AND rating = 0", $review_id));

        if ($tslo_ncovered_count > 0) {

          $row = db_fetch_array(db_query("SELECT * FROM {dae_term_review_form} WHERE id=%d", $review_id));

          if (is_numeric($row['course'])) {
            // Here the query above stay intact except for the reassignment
            // of the 'course' row to the new value in the dae_course table.
            $row['course'] = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $row['course']));
          }

          $tslo_ncovered_list[$review_id] = $row['year'] . '/' . ($row['year']+1) . ' ' . $row['term'] . ' ' . $row['course'];

        }

      }

      natsort($tslo_ncovered_list);

      foreach ($tslo_ncovered_list as $id => $title) {
        $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $id;
        $tslo_ncovered_string .= '<li><a href="' . $url . '">' . $title . '</a></li>';
      }

      if (sizeof($tslo_ncovered_list) == 0) {
        $tslo_ncovered_string = '<li><i>' . t('No matches found') . '</i></li>';
      }

      $form['review']['tlt3'] = array(
        '#type' => 'fieldset',
        '#title' => t('Term reviews with taught student learning outcomes that were not covered') . ' (' . sizeof($tslo_ncovered_list) . ')',
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
        '#prefix' => '<br />',
      );

      $form['review']['tlt3'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $tslo_ncovered_string . '</ul>',
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );


      ////////// 2 - List all prereq SLO (to a course) which have been rated
      //////////   unprepared at least once in the last and current academic year.

      // Hold the id's of all the feedback forms
      // that met this from the previous 2 years.
      $pslo_uprep_list = array();
      $pslo_uprep_string = '';

      foreach ($review_id_list as $review_id) {

        $pslo_uprep_count = db_result(
          db_query("SELECT COUNT(*)
                      FROM {dae_term_review_ratings}
                     WHERE term_review_form_id=%d
                       AND type='prereq'
                       AND rating = 0", $review_id)
        );

        if ($pslo_uprep_count > 0) {

          $row = db_fetch_array(db_query("SELECT * FROM {dae_term_review_form} WHERE id=%d", $review_id));

          if (is_numeric($row['course'])) {
            $row['course'] = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $row['course']));
          }

          // Format the year Eg. 2010/2011
          $temp_year = $row['year'] = $row['year'] . '/' . ($row['year']+1);

          $pslo_uprep_list[$review_id] = $temp_year . ' ' . $row['term'] . ' ' . $row['course'];

        }

      }

      natsort($pslo_uprep_list);

      foreach ($pslo_uprep_list as $id => $title) {

        $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $id;

        $pslo_uprep_string .= '<li><a href="' . $url . '">' . $title . '</a></li>';

      }

      if (sizeof($pslo_uprep_list) == 0) {
        $pslo_uprep_string = '<li><i>' . t('No matches found') . '</i></li>';
      }

      $form['review']['pll3'] = array(
        '#type' => 'fieldset',
        '#title' => t('Term reviews with unprepared prerequisite student learning outcomes') . ' (' . sizeof($pslo_uprep_list) . ')',
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
      );

      $form['review']['pll3'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $pslo_uprep_string . '</ul>',
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      ////////// 3 - List all Term review which contain text in the Misc comment section
      //////////   (which should be read by the Curriculum Cmtee by definition).

      $review_comment_id = array();

      $review_comment_string = '';

      foreach ($review_id_list as $review_id) {

        $row = db_fetch_array(db_query("SELECT * FROM {dae_term_review_form} WHERE id=%d", $review_id));

        if (is_numeric($row['course'])) {
          $row['course'] = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $row['course']));
        }

        if (trim($row['info']) != '' && $row['info'] != NULL) {
          $review_comment_id[$review_id] = $row['year'] . '/' . ($row['year']+1) . ' ' . $row['term'] . ' ' . $row['course'];
        }

      }

      natsort($review_comment_id);

      foreach ($review_comment_id as $id => $title) {

        $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $id;

        $review_comment_string .= '<li><a href="' . $url . '">' . $title . '</a></li>';

      }

      if (sizeof($review_comment_id) == 0) {
        $review_comment_string = '<li><i>' . t('No matches found') . '</i></li>';
      }

      $form['review']['wcomments'] = array(
        '#title' => t('Term reviews with comments on the course') . ' (' . sizeof($review_comment_id) . ')',
        '#type' => 'fieldset',
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
      );

      $form['review']['wcomments'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $review_comment_string . '</ul>',
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // Displays the filtering year select box and buttons for selecting the
      // current year to display in the analyse/term-review section. $param[0]
      // returns the first value in the URL so when $param[0] == 'term-review'
      // this information is shown.
      if ($param[0] == $page_settings['analyse term reviews']) {

        $first_year = db_result(db_query("SELECT year FROM {dae_term_review_form} ORDER BY year"));

        $current_year = Date('Y');

        $year_values = array();

        for ($i = $first_year; $i <= $current_year; $i++) {
          $year_values[$i] = t($i . '/' . ($i+1));
        }

        if ($param[2] == 'viewall'  || (($param[0] && $param[1] && $param[2]) == '')) {
          $description = t('You are currently viewing statistics from all academic years');
        }

        else {
          $description = t('You are currently viewing statistics for the academic year: !year/!next-year',
                  array('!year' => $year, '!next-year' => ($year+1)));
        }

        $form['year'] = array(
          '#type' => 'select',
          '#title' => t('Select academic year'),
          '#options' => $year_values,
          '#weight' => -10,
          '#default_value' => $year,
          '#description' => $description,
        );

        // Button to view a different year
        $form['change'] = array(
          '#type' => 'submit',
          '#value' => t('Change Academic Year'),
          '#weight' => $form['year']['#weight']+1,
          '#hidden' => TRUE,
        );

        // Button to return to viewing the current academic year
        $form['current'] = array(
          '#type' => 'submit',
          '#value' => t('Current Year'),
          '#weight' => $form['year']['#weight']+2,
          '#hidden' => TRUE,
        );

        // Button to view all the academic years
        $form['viewall'] = array(
          '#type' => 'submit',
          '#value' => t('View All Academic Years'),
          '#weight' => $form['year']['#weight']+3,
          '#hidden' => TRUE,
        );

      }

    }

  }

    //////// ANALYSE STUDENT SECTION ///////////////////////////////////////////
   ///////// ANALYSE STUDENT SECTION //////////////////////////////////////////
  ////////// ANALYSE STUDENT SECTION /////////////////////////////////////////
  if (module_exists('magellan')) {

    if (($param[0] == 'student-files' && $student_file_permission) || (!$param[0] && $student_file_permission)) {

//      global $user;
//
//      // If the current user is a Magellan Advisor, determine
//      // if there is a current advising session open.
//      if (magellan_advisor($user->uid)) {
//
//        $result = db_query("SELECT id, add_time
//          FROM {mag_advisor_session} WHERE advisor_id=%d", $user->uid);
//
//        while ($row = db_fetch_array($result)) {
//          $session_id = $row['id'];
//          $add_time = $row['add_time'];
//        }
//
//        // Get the session name.
//        $current_session = $_COOKIE[session_name()];
//
//        // If in session, set the user name as
//        // to the selected students username.
//        if ($session_id == $current_session) {
//
//          // Get the current session time
//          $session_time = db_result(db_query("SELECT session_time
//            FROM {mag_session_log} WHERE advisor_id=%d
//            AND session_id='%s'", $user->uid, $session_id ));
//
//          // Add the session timeout warning.
//          $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'my_program', 'advisor');
//
//        }
//
//      }

      $student_total = 0;

      // The amount of time between the
      // last log and the current date.
      $file_duration = $page_settings['file duration'];

      $current_date = date('Y-m-d H:i:s', time());

      $result = db_query("SELECT id, user_name, first_name, last_name, file_closed FROM {mag_student_identification}");
      while ($row = db_fetch_array($result)) {

        $student_total++;

        if ($row['file_closed']) {

          // URL link to view the course
          $url = $base_url . '/' . $page_settings['manage student'] . '/update_files/highlighted/' . $row['id'];

          $closed_files = '<li>' . t('User_Name: ') . '<a href="' . $url . '" title="' . $row['first_name'] . ' ' . $row['last_name'] . '">' . $row['user_name'] . '</a></li>';

        }

        $last_session_time = db_result(db_query("SELECT session_time FROM {mag_session_log}
          WHERE student_id=%d ORDER BY session_time DESC LIMIT 1", $row['id']));

        if ($last_session_time) {

          // Get the time between the last
          // session and the current date.
          $date1 = $current_date;
          $date2 = $last_session_time;

          $diff = abs(strtotime($date2) - strtotime($date1));

          $years = floor($diff / (365*60*60*24));

          if ($years >= $file_duration) {

            // URL link to view the course
            $url = $base_url . '/' . $page_settings['manage student'] . '/update_files/highlighted/' . $row['id'];

            $deletable_files = '<li>' . t('User_Name: ') . '<a href="' . $url . '" title="' . $row['first_name'] . ' ' . $row['last_name'] . '">' . $row['user_name'] . '</a></li>';

          }

        }

      }

      $form['student'][] = array(
        '#title' => t('Student total'),
        '#type' => 'item',
        '#value' => $student_total,
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // closed student files
      // CLOSED STUDENT FILES
      $form['student']['closed'] = array(
        '#title' => t('Closed student files') . ' (' . substr_count($closed_files, '<li>') . ')',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays student files that have been closed.'),
        '#prefix' => '<br />',
      );

      if (substr_count($closed_files, '<li>') == 0) {
        $closed_files = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
      }

      $form['student']['closed'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $closed_files . '</ul>',
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // deletable student files
      // DELETABLE STUDENT FILES
      $form['student']['deletable'] = array(
        '#title' => t('Deletable student files') . ' (' . substr_count($deletable_files, '<li>') . ')',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => '&nbsp;&nbsp;&nbsp; ' . t('Displays student files that have been closed and are now deletable. ' .
                'These files have been inactive for more than !duration years.',
                array('!duration' => $file_duration)),
      );

      if (substr_count($deletable_files, '<li>') == 0) {
        $deletable_files = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
      }

      $form['student']['deletable'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $deletable_files . '</ul>',
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

  }

  return $form;

}


/**
 * Implementation of hook_submit().
 */
function daedalus_analyse_form_submit($form, &$form_state) {

  global $base_url;

  $page_url = daedalus_get_setting('analyse');

  $param = array();
  $page_url_length = sizeof(explode('/', $page_url));
  $param[0] = arg(0+$page_url_length);

  switch ($form_state['values']['op']) {

    case $form_state['values']['dae-help-submit']:

        $help_text = $form_state['values']['dae-help'];

        if ($help_text) {

          if (db_result(db_query("SELECT COUNT(*) FROM {dae_page_help} WHERE page_url='%s'", $page_url))) {
            db_query("UPDATE {dae_page_help} SET help_text='%s' WHERE page_url='%s'", $help_text, $page_url);
          }
          else {
            db_query("INSERT INTO {dae_page_help} (page_url, help_text) VALUES ('%s', '%s')", $page_url, $help_text);
          }

          drupal_set_message(t('Help information saved.'));

          if ($param[0]) {
            drupal_goto($base_url . '/' . $page_url . '/' . $param[0]);
          }
          else {
            drupal_goto($base_url . '/' . $page_url);
          }

        }

      break;

  }

}