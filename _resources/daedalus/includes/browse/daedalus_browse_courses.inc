<?php

/**
 * @file
 * daedalus_browse_courses.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */

//------------------------------------------------------------------------------
// Daedalus->Browse->Courses Callback Functions
//------------------------------------------------------------------------------
function daedalus_browse_courses_page() {
  return drupal_get_form('daedalus_browse_courses_form');
};


/**
 * Menu Location: Daedalus -> Browse -> Courses
 * URL Location:  daedalus/browse/courses
 *
 * Displays all available courses with links to update each course
 * prerequisite courses, prerequisite SLO and SLO's.
 */
function daedalus_browse_courses_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='browse courses' OR
                             setting='question mark' OR
                             setting='mapped identification' OR
                             setting='goal identification' OR
                             setting='goal space' OR
                             setting='unmapped identification' OR
                             setting='show courses in the browse courses page' OR
                             setting='build course codes' OR
                             setting='build courses' OR
                             setting='column iteration' OR
                             setting='course name preview length' OR
                             setting='manage courses' OR
                             setting='show table settings in the browse courses page' OR
                             setting='course name preview length'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  global $base_url;

  // Get current page url.
  $page_url = $help_url = $page_settings['browse courses'];

  // Store URL Parameters in $param array
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  // Determine if the magellan module is installed.
  $magellan_check = module_exists('magellan');

  // Get the images
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';
  $mapped_img   = '<img src="' . $base_url . '/' . $page_settings['mapped identification'] . '" title="Course is mapped" />';
  $unmapped_img = '<img src="' . $base_url . '/' . $page_settings['unmapped identification'] . '" title="Course is unmapped" />';
  $goal_img     = '<img src="' . $base_url . '/' . $page_settings['goal identification'] . '" title="This course is part of your goal" />';
  $space_img    = '<img src="' . $base_url . '/' . $page_settings['goal space'] . '" />';

  // Get Daedalus Settings
  $show_courses = $page_settings['show courses in the browse courses page'];

  // Get Access Information
  $build_courses_access = user_access('daedalus build courses');

  // Cannot fully sanitize $show_help using
  // check_plain(), filter_xss() or similar.
  drupal_set_title(t('Browse Courses !help', array('!help' => $show_help)));

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($help_url, 0, 1);

  // Only execute code if magellan database tables are installed.
  // This will popup a session timeout warning if the user is a
  // Magellan advisor or support and they are in session.
  if ($magellan_check) {

    // Determine the user id
    global $user;
    $user_id = $user->uid;

    // If the current user is a Magellan Advisor determine
    // if there is a current advising session open.
    if (db_result(db_query("SELECT COUNT(*)
                               FROM {role, users_roles}
                              WHERE role.name='Magellan Advisor'
                                AND users_roles.uid=%d
                                AND users_roles.rid = role.rid", $user_id))) {

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
    if (db_result(db_query("SELECT COUNT(*)
                               FROM {role, users_roles}
                              WHERE role.name='Magellan Support Staff'
                                AND users_roles.uid=%d
                                AND users_roles.rid = role.rid", $user_id))) {

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
        $session_time = db_result(db_query("SELECT session_time FROM {mag_session_log} WHERE support_id=%d AND session_id='%s'", $user_id, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse_courses', 'support');

      }

    }

  }

  if (!db_result(db_query("SELECT COUNT(*) FROM {dae_valid_course_codes}"))) {

    if ($build_courses_access) {

      // If there are no course codes, inform the user a course code must be created first.
      $url = $base_url . '/' . $page_settings['build course codes'];

      drupal_set_message(t('There are no course codes created. <a href="@url"><b>Create a Course Code?</b></a>', array('@url' => url($url))));

    }

  }
  elseif (!db_result(db_query("SELECT COUNT(*) FROM {dae_course}"))) {

    if ($build_courses_access) {

      // If there are no courses redirect
      // the user to build a course.
      $url = $base_url . '/' . $page_settings['build courses'];

      drupal_set_message(t('There are no courses created. <a href="@url"><b>Create a Course?</b></a>', array('@url' => url($url))));

    }

  }
  else {

    // Only execute code if magellan database tables are installed.
    if ($magellan_check) {

      // Determine the user name
      global $user;
      $user_id = $user->uid;
      $user_name = db_result(db_query("SELECT name FROM {users} WHERE uid=%d", $user_id));

      // Get the students identification number
      $student_id = db_result(db_query("SELECT id FROM {mag_student_identification} WHERE user_name='%s'", $user_name));

      $result = db_query("SELECT course_id FROM {mag_goal_student_course} WHERE student_id=%d", $student_id);
      while ($row = db_fetch_array($result)) {
        $goal_courses[$row['course_id']] = $row['course_id'];
      }

    }

    // Border size, Column size, and table string
    $columns = $page_settings['column iteration'];

    // If  mapped (incomplete courses), then the page
    // will only list courses which are not mapped.
    if ($param[0] == 'mapped') {

      $map_value = 'WHERE mapped=1';   // String that will be appended to the query to determine map values
      $department = $param[1];         // If mapped, then the selected department is the second paramater

    }
    elseif ($param[0] == 'unmapped') {

      $map_value = 'WHERE mapped=0';

    }
    else {

      $map_value  = '';
      $department = $param[0];

    }

    $course_codes = array();

    // Select all course codes from dae_course and fill an array with course
    // codes for each department. If a department is selected via the URL
    // parameter at the first position, only select courses from this department.
    if ($department) {

      // Modify the query when a department is selected.
      $map_value = str_replace('WHERE', 'AND', $map_value);

      // Get all courses with the current code.
      $result = db_query("SELECT course_code, course, course_name
                            FROM {dae_course}
                           WHERE course_code='%s' " . $map_value . "
                        ORDER BY course_number", $department);

      while ($row = db_fetch_array($result)) {
        $course_codes[$row['course_code']][] = $row['course'] . ' - ' . $row['course_name'];
      }

      // Get all alias courses with the current code.
      $result = db_query("SELECT parent_id, alias_code, alias_course
                            FROM {dae_course_alias}
                           WHERE alias_code='%s'
                        ORDER BY alias_number", $department);

      while ($row = db_fetch_array($result)) {

        $alias_name = db_result(db_query("SELECT course_name FROM {dae_course} WHERE id=%d", $row['parent_id']));

        $course_codes[$row['alias_code']][] = $row['alias_course'] . ' - ' . $alias_name;

      }

    }
    else {

      $result = db_query("SELECT DISTINCT course_code FROM {dae_course} " . $map_value);
      while ($row = db_fetch_array($result)) {
        $course_codes[$row['course_code']] = array();
      }

      if ($param[0] != 'mapped' && $param[0] != 'unmapped') {

        // Select all the course aliases from dae_course_alias
        // and fill the array with any uncreated course codes.
        $result = db_query("SELECT DISTINCT alias_code FROM {dae_course_alias}");
        while ($row = db_fetch_array($result)) {
          $course_codes[$row['alias_code']] = array();
        }

      }

      // Sort by the code value.
      ksort($course_codes);

      $map_value = str_replace('WHERE', 'AND', $map_value);

      foreach ($course_codes as $code => $array) {

        // Get all courses with the current code.
        $result = db_query("SELECT course_code, course, course_name
                              FROM {dae_course}
                            WHERE course_code='%s' " . $map_value . "
                          ORDER BY course_number", $code);

        while ($row = db_fetch_array($result)) {
          $course_codes[$row['course_code']][] = $row['course'] . ' - ' . $row['course_name'];
        }

      }

      if ($param[0] != 'mapped' && $param[0] != 'unmapped') {

        foreach ($course_codes as $code => $array) {

          // Get all alias courses with the current code.
          $result = db_query("SELECT parent_id, alias_code, alias_course
                                FROM {dae_course_alias}
                              WHERE alias_code='%s'
                            ORDER BY alias_number", $code);

          while ($row = db_fetch_array($result)) {

            $alias_name = db_result(db_query("SELECT course_name FROM {dae_course} WHERE id=%d", $row['parent_id']));

            $course_codes[$row['alias_code']][] = $row['alias_course'] . ' - ' . $alias_name;

          }

        }

      }

    }

    // Order the courses for a single column.
    if ($columns == 1 || $columns > 4) {

      foreach ($course_codes as $code => $array) {

        // Sort the list of courses
        sort($array);

        // For each course code (dept) save the
        // freshly sorted course list.
        $course_codes[$code] = $array;

      }

    }

    // Order the courses for 2TWO2 columns.
    if ($columns == 2) {

      foreach ($course_codes as $code => $array) {

        // Sort the list of courses
        sort($array);
        $course_total = count($array);

        if ($course_total >= 2) {

          $column_division = round(count($array)/$columns);
          $column_remainder = ($column_division * 2) - $course_total;
          $new_order = array();

          $first_array = $second_array = array();

          if ($column_remainder == 0) {
            $division1 = $column_division;
            $division2 = ($column_division * 2);
          }
          elseif ($column_remainder == 1) {
            $division1 = $column_division;
            $division2 = ($column_division * 2) - 1;
          }
          elseif ($column_remainder == -1) {
            $division1 = $column_division + 1;
            $division2 = ($column_division * 2);
          }

          for ($i = 0; $i < $course_total; $i++) {

            if ($i < $division1) {
              $first_array[] = $array[$i];
            }
            else {
              $second_array[] = $array[$i];
            }

          }

          $inc1 = $inc2 = 0;

          for ($i = 0; $i < $course_total; $i++) {

            if (($i % 2) == 0) {
              $new_order[] = $first_array[$inc1++];
            }
            elseif (($i % 2) == 1) {
              $new_order[] = $second_array[$inc2++];
            }

          }

          $course_codes[$code] = $new_order;

        }

        // else if the amount of courses is too small to
        // divide into columns, just copy the ordered array.
        else {
          $course_codes[$code] =  $array;
        }

      }

    }

    // Order the courses for 3THREE3 columns.
    if ($columns == 3) {

      foreach ($course_codes as $code => $array) {

        // Sort the list of courses
        sort($array);
        $course_total = count($array);

        if ($course_total >= 3) {

          $column_division = round(count($array)/$columns);
          $column_remainder = ($column_division * 3) - $course_total;
          $new_order = array();

          $first_array = $second_array = $third_array = array();

          if ($column_remainder == 0) {
            $division1 = $column_division;
            $division2 = ($column_division * 2);
            $division3 = ($column_division * 3);
          }
          elseif ($column_remainder == 1) {
            $division1 = $column_division;
            $division2 = ($column_division * 2);
            $division3 = ($column_division * 3) - 1;
          }
          elseif ($column_remainder == 2) {
            $division1 = $column_division;
            $division2 = ($column_division * 2) - 1;
            $division3 = ($column_division * 3) - 1;
          }
          elseif ($column_remainder == -1) {
            $division1 = $column_division + 1;
            $division2 = ($column_division * 2) + 1;
            $division3 = ($column_division * 3);
          }

          for ($i = 0; $i < $course_total; $i++) {

            if ($i < $division1) {
              $first_array[] = $array[$i];
            }
            elseif ($i >= $division1 && $i < $division2) {
              $second_array[] = $array[$i];
            }
            else {
              $third_array[] = $array[$i];
            }

          }

          $inc1 = $inc2 = $inc3 = 0;

          for ($i = 0; $i < $course_total; $i++) {

            if (($i % 3) == 0) {
              $new_order[] = $first_array[$inc1++];
            }
            elseif (($i % 3) == 1) {
              $new_order[] = $second_array[$inc2++];
            }
            elseif (($i % 3) == 2) {
              $new_order[] = $third_array[$inc3++];
            }

          }

          $course_codes[$code] = $new_order;

        }

        // elseif the amount of courses is too small to
        // divide into columns, just copy the ordered array.
        else {
          $course_codes[$code] =  $array;
        }

      }

    }

    // Order the courses for 4FOUR4 columns.
    if ($columns == 4) {

      foreach ($course_codes as $code => $array) {

        // Sort the list of courses
        sort($array);
        $course_total = count($array);

        if ($course_total >= 4) {

          $column_division = round(count($array)/$columns);
          $column_remainder = ($column_division * 4) - $course_total;
          $new_order = array();

          $first_array = $second_array = $third_array = $fourth_array = array();

          if ($column_remainder == 0) {
            $division1 = $column_division;
            $division2 = ($column_division * 2);
            $division3 = ($column_division * 3);
            $division4 = ($column_division * 4);
          }
          elseif ($column_remainder == 1) {
            $division1 = $column_division;
            $division2 = ($column_division * 2);
            $division3 = ($column_division * 3);
            $division4 = ($column_division * 4) - 1;
          }
          elseif ($column_remainder == 2) {
            $division1 = $column_division;
            $division2 = ($column_division * 2);
            $division3 = ($column_division * 3) - 1;
            $division4 = ($column_division * 4) - 1;
          }
          elseif ($column_remainder == 3) {
            $division1 = $column_division;
            $division2 = ($column_division * 2) - 1;
            $division3 = ($column_division * 3) - 1;
            $division4 = ($column_division * 4) - 1;
          }
          elseif ($column_remainder == -1) {
            $division1 = $column_division + 1;
            $division2 = ($column_division * 2) + 1;
            $division3 = ($column_division * 3) + 1;
            $division4 = ($column_division * 4);
          }

          for ($i = 0; $i < $course_total; $i++) {

            if ($i < $division1) {
              $first_array[] = $array[$i];
            }
            elseif ($i >= $division1 && $i < $division2) {
              $second_array[] = $array[$i];
            }
            elseif ($i >= $division2 && $i < $division3) {
              $third_array[] = $array[$i];
            }
            else {
              $fourth_array[] = $array[$i];
            }

          }

          $inc1 = $inc2 = $inc3 = $inc4 = 0;

          for ($i = 0; $i < $course_total; $i++) {

            if (($i % 4) == 0) {
              $new_order[] = $first_array[$inc1++];
            }
            elseif (($i % 4) == 1) {
              $new_order[] = $second_array[$inc2++];
            }
            elseif (($i % 4) == 2) {
              $new_order[] = $third_array[$inc3++];
            }
            elseif (($i % 4) == 3) {
              $new_order[] = $fourth_array[$inc4++];
            }

          }

          $course_codes[$code] = $new_order;

        }

        // elseif the amount of courses is too small to
        // divide into columns, just copy the ordered array.
        else {
          $course_codes[$code] =  $array;
        }

      }

    }

    $table = '';

    // The string can be set to cut off after a certian amount
    // of digits. This finds the value from the database
    $maxlen = $page_settings['course name preview length'];

    // Display all the course information in the table.
    foreach ($course_codes as $code => $course_list) {

      // Start out by making one row with just the title of the department being viewed,
      // and the code. So this gets the name of the department currently being printed.
      $name = db_result(db_query("SELECT department_name FROM {dae_valid_course_codes} WHERE course_code='%s'", $code));

      // Make the url to address clicking on the header.
      //(show only courses from that dept)
      $code_url = $page_url . '/' . $code;

      // This is what will be displayed, for formatting and such
      $display = $code . ' - ' . $name;

      // Only enter space after first department.
      if ($flag++) {
        $space = '<tr><td>&nbsp;</td></tr>';
      }

      // Put it into the string
      $table .= $space . '<tr style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;">
                            <td colspan=' . $columns . '><b><big><a href="' . $code_url . '">' . $display . '</a></big></b>
                            </td>
                          </tr>';

      $i = 0; // Row counter

      // It's possible the settings are set so only the departments are listed on the immediate page.
      // If so, and there is no department selected, then skip this part of printing the values
      if ($show_courses || $department != '') {

        foreach ($course_list as $id => $course) {

          // The course list contains the course code, number and name.
          // Explode this value to get the number from the 2nd element.
          $temp = explode(' ', $course);

          $number = $temp[1];

          $crosslisted = db_result(db_query("SELECT COUNT(*) FROM {dae_course_alias} WHERE alias_code='%s' AND alias_number=%d", $code, $number));

          if ($crosslisted) {

            // Get the cross listed courses parent id
            $cross_id = db_result(db_query("SELECT parent_id FROM {dae_course_alias} WHERE alias_code='%s' AND alias_number=%d", $code, $number));

            // Find the map value from the parent id.
            $mapped = db_result(db_query("SELECT mapped FROM {dae_course} WHERE id=%d", $cross_id));

          }

          else {

            // Get the mapped value from the current course information.
            $mapped = db_result(db_query("SELECT mapped FROM {dae_course} WHERE course_code='%s' AND course_number=%d", $code, $number));

          }

          // Clear the map value
          $map = '';

          // Goal courses are only visible to students.
          if ($goal_courses && !$build_courses_access) {

            // Add the goal image to courses.
            if (in_array(daedalus_course_id($code, $number), $goal_courses)) {
              $map = $goal_img;
            }
            else {
              $map = $space_img;
            }

          }

          // Link to course information, if the user has access to build course information
          // always link to the Daedalus course entry for editing. If a student or other user
          // selects the link, only show the Daedalus entry if the course has been mapped, if
          // the course has not been mapped goto the Dalhousie calendar entry for the course.
          if ($build_courses_access) {

            if ($mapped) {

              $course_url = $base_url . '/' . $page_settings['manage courses'] . '/' . $code . '/' . $number;
              $map .= $mapped_img;
              $target = '';

            }
            else {

              $course_url = $base_url . '/' . $page_settings['manage courses'] . '/' . $code . '/' . $number;
              $map .= $unmapped_img;
              $target = '';

            }

          }
          else {

            if ($mapped) {

              $course_url = $base_url . '/' . $page_settings['manage courses'] . '/' . $code . '/' . $number;
              $map .= $mapped_img;
              $target = '';

            }
            else {

              $course_url = 'http://www.registrar.dal.ca/calendar/class.php?subj=' . $code . '&num=' . $number;
              $map .= $unmapped_img;
              $target = 'target="_blank"';

            }

          }

          // Find new Row position
          if ($i % $columns == 0) {

            if ($i != 0) {
              $table .= '</tr>';
            }

            $table .= '<tr>';

          }

          $table .= '<td><small>';

          // The course variable may need to be cut off depending on length. To make sure the
          // full string isn't lost make a copy for hovering. Display (...) to replace excess.
          $display_course = $course;

          if ($maxlen > 0) {

            if (drupal_strlen($display_course) > $maxlen) {
              $display_course = drupal_substr($course, 0, $maxlen-3) . '...';
            }

          }

          $id = $course;

          // Append what should be displayed in the table to the string.
          $table .= $map . ' <a id="' . $id . '" href="' . $course_url . '" ' . $target . ' title="' . $course . '" name="' . $course . '">' . $display_course . '</a></small></td>';

          // Increment counter
          $i++;

        }

        // Close final row
        $table .= '</tr>';

      }

    }

    // Decide button text
    if ($param[0] == 'mapped' || $param[0] == 'unmapped') {
      $mapped_txt  = t('Show all courses');
    }
    else {
      $mapped_txt  = t('Show mapped courses');
    }

    // Display buttons at the top of the page
    if ($department) {

      // Button to swap a course from being mapped and mapped
      if ($build_courses_access) {

        $form['mapped'] = array(
          '#type' => 'submit',
          '#value' => $mapped_txt,
        );

      }

      // Button to link back to all
      // courses and all departments
      $form['list-all'] = array(
        '#type' => 'submit',
        '#value' => t('Show all departments'),
      );

    }
    elseif (!$department && user_access('daedalus manage settings')) {

      // Give an option to switch between all the courses being listed on the main page or not.
      // If not just the departments and the courses only listed when you click on a department.
      // But first the text to inform the user what they will be doing needs to be generated
      if ($show_courses) {
        $show_hide = t('Hide courses');
      }
      else {
        $show_hide = t('Show courses');
      }

      $form['show-hide-courses'] = array(
        '#type'  => 'submit',
        '#value' => $show_hide,
      );

      if ($show_courses && $build_courses_access) {

        $form['mapped'] = array(
          '#type'  => 'submit',
          '#value' => $mapped_txt,
        );

      }

      if ($page_settings['show table settings in the browse courses page']) {

        //if they should appear, add buttons to the table which will adjust the table properties
        $form['increase-size'] = array(
          '#type'  => 'submit',
          '#value' => t('Increase text'),
        );

        if ($page_settings['course name preview length'] > 4) {

          $form['decrease-size'] = array(
            '#type'  => 'submit',
            '#value' => t('Decrease text'),
          );

        }

        if ($columns < 4 ) {

          $form['add-column'] = array(
            '#type'  => 'submit',
            '#value' => t('Add column'),
          );

        }

        if ($columns > 1) {

          $form['remove-column'] = array(
            '#type'  => 'submit',
            '#value' => t('Remove column'),
          );

        }

      }

    }
    elseif ($param[0] == 'unmapped') {

      $form['mapped'] = array(
        '#type' => 'submit',
        '#value' => $mapped_txt,
      );

    }

    if (!$build_courses_access && !$table) {
      $table = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
    }

    // Display the table and return
    $form['display-courses'] = array(
      '#type' => 'item',
      '#value' => '<table>' . $table . '</table>',
    );

  }

  // Submit hidden information to pass
  // to the validate and submit hooks.
  $form['pass-browse-courses']   = array( '#type' => 'value', '#value' => $page_settings['browse courses'], );
  $form['pass-column-iteration'] = array( '#type' => 'value', '#value' => $page_settings['column iteration'], );
  $form['pass-preview-length']   = array( '#type' => 'value', '#value' => $page_settings['course name preview length'], );
  $form['pass-show-courses']   = array( '#type' => 'value', '#value' => $page_settings['show courses in the browse courses page'], );

  return $form;

}


/**
 * Implementation of hook_submit().
 */
function daedalus_browse_courses_form_submit($form, &$form_state) {

  // Get daedalus base url
  global $base_url;

  // Get current page url.
  $page_url = $form_state['values']['pass-browse-courses'];

  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  $mod_txt = 5;  // Amount to modify text display length
  $min_dig = 4;  // Minimum amount of digits to appear

  switch ($form_state['values']['op']) {

    //if they want to add a column
    case $form_state['values']['add-column']:

      db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $form_state['values']['pass-column-iteration']+1, 'column iteration');

      break;

    //if they want to remove a column
    case $form_state['values']['remove-column']:

      db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $form_state['values']['pass-column-iteration']-1, 'column iteration');

      break;

    //increase the amount of text that can be displayed at a time
    case $form_state['values']['increase-size']:

      if ($form_state['values']['pass-preview-length'] == $min_dig) {
        $mod_txt -=1;
      }

      db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $form_state['values']['pass-preview-length']+$mod_txt, 'course name preview length');

      break;

    //if they want to decrease it
    case $form_state['values']['decrease-size']:

      db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $form_state['values']['pass-preview-length']-$mod_txt, 'course name preview length');

      if ($form_state['values']['pass-preview-length'] < $min_dig) {
        db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $min_dig, 'course name preview length');
      }

      break;

    //if they want to show/hide the settings (one line yeaaa)
    case $form_state['values']['show-hide-courses']:

      db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", !$form_state['values']['pass-show-courses'], 'show courses in the browse courses page');

      break;

    //if they want to toggle between showing all and only mapped courses
    case $form_state['values']['mapped']:

      if ($param[0] == 'mapped' || $param[0] == 'unmapped') {
        drupal_goto($base_url . '/' . $page_url . '/' . $param[1]);
      }
      else {
        drupal_goto($base_url . '/' . $page_url . '/mapped/' . $param[0]);
      }

      break;

    //if they want to go back and list all the departments
    case $form_state['values']['list-all']:

      if ($param[0] == 'mapped') {
        $map_value = 'mapped';
      }
      elseif ($param[0] == 'unmapped') {
        $map_value = 'unmapped';
      }

      drupal_goto($page_url . '/' . $map_value);

      break;

    //if the help is submitted
    case $form_state['values']['dae-help-submit']:

      $page_url = $form_state['values']['pass-browse-courses'];

      $help_text = $form_state['values']['dae-help'];

      if ($help_text) {

        if (db_result(db_query("SELECT COUNT(*) FROM {dae_page_help} WHERE page_url='%s'", $page_url))) {
          db_query("UPDATE {dae_page_help} SET help_text='%s' WHERE page_url='%s'", $help_text, $page_url);
        }
        else {
          db_query("INSERT INTO {dae_page_help} (page_url, help_text) VALUES ('%s', '%s')", $page_url, $help_text);
        }

        drupal_set_message(t('Help information saved.'));

        if ($param[0] && $param[1]) {
          drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1]);
        }
        elseif ($param[0]) {
          drupal_goto($base_url . '/' . $page_url . '/' . $param[0]);
        }
        else {
          drupal_goto($base_url . '/' . $page_url);
        }

      }

      break;

    default:

      drupal_set_message(t('Unknown operation selected'), 'warning');

  }

}