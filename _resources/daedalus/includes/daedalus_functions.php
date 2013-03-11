<?php

/**
 * @file
 * daedalus-functions.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */

/**
 * Function: daedalus_get_setting
 *
 * Returns the value of the setting passed. Returns
 * the value "{DNE}" if it doesn't exist.
 *
 * @param type $setting
 *    The Daedalus setting value
 * @return type
 *    The Daedalus setting value
 *    or an error if not found
 */
function daedalus_get_setting($setting) {

  $result = db_query("SELECT * FROM {dae_settings} WHERE setting='%s'", $setting);
  while ($row = db_fetch_array($result)) {
    return $row['value'];
  }

  return '{DNE-' . $setting . '}';

}


/**
 * Function: daedalus_student().
 *
 * Matches the users id from in the database and returns
 * true if the user has the Daedalus Student role and
 * false if the user does not have the role.
 *
 * @param type $uid
 *    The drupal user id.
 * @return boolean
 */
function daedalus_student($uid) {

  // Determine if the current user is a Magellan Advisor
  $daedalus_student = db_result(db_query("SELECT COUNT(*) FROM {role, users_roles}
    WHERE role.name='Daedalus Student' AND users_roles.uid=%d
    AND users_roles.rid = role.rid", $uid));

  if ($daedalus_student) {
    return TRUE;
  }
  else {
    return FALSE;
  }

}


/**
 * Function: daedalus_get_permissions
 *
 * Returns the array of allowed upload
 * file types for Magellan > Advise and
 * Magellan > Support.
 *
 * @return <type>
 *  return the list of permissions
 */
function daedalus_get_permissions() {

  $permissions = array();

  $value = db_result(db_query("SELECT value FROM {dae_settings} WHERE setting='file permissions'"));

  // Explode removing the white space if any.
  $permissions = array_map('trim', explode(',', $value));

  return $permissions;

}


/**
 * Function: daedalus_modify_menu
 *
 * Modify the daedalus menu. Various menu items
 * are not available until a session is created.
 * This resets the menu router and menu linking
 * tables to make the menu items appear when a
 * session is created.
 *
 * @param type $page_url
 *    The URL or Path for the menu item.
 *
 * @param type $hidden_status
 *    Hide if status=1 show if status=0.
 */
function daedalus_modify_menu($page_url, $hidden_status) {

  $path = daedalus_get_setting($page_url);

  $new_link = array();

  $mlid = db_result(db_query("SELECT mlid FROM {menu_links} WHERE link_path='%s'", $path));

  if ($mlid) {

    $menu_link = menu_link_load($mlid);

    if ($menu_link) {

      foreach ($menu_link as $k => $v) {

        if ($k == 'hidden') {
          $new_link[$k] = $hidden_status;
        }
        else {
          $new_link[$k] = $v;
        }

      }

    }

  }

  menu_link_delete($mlid);
  menu_link_save($new_link);

}


/**
 * Function: daedalus_session_timeout_warning
 *
 * Takes the session start time and determines the amount of time that the
 * session has been active. The session time is compared to the session timeout
 * setting plus the additional time to determine the amount of milliseconds
 * that remain until the 10 minute session end warning. This milliseconds value
 * is added to a table which is hidden using jQuery, the table is then revealed
 * after the millisecs variable is reached.
 *
 * @global type $base_url
 * @param type $session_time
 * @param type $page
 * @param type $type
 * @return string
 */
function daedalus_session_timeout_warning($session_time, $add_time, $page, $type) {

  global $base_url;

  // Get the minutes the current session has been open.
  $duration = explode('-', daedalus_session_duration($session_time));
  $hours = $duration[0];
  $minutes = $duration[1];
  $seconds = $duration[2];

  // Get the session timeout setting which is in minutes. Add
  // time to lengthen the session duration then convert the
  // minutes to seconds to make a more accurate time keeping.
  $timeout = (daedalus_get_setting('advising timeout') + $add_time) * 60;

  $session_minutes = ($hours * 60) + $minutes;
  $session_seconds = ($session_minutes * 60) + $seconds;

  // If the totoal session seconds surpasses
  // the timeout seconds, log the user out.
  if ($session_seconds >= $timeout) {

    // If advisor, redirect to the support creation
    // page where the timeout is activated.
    if ($type == 'advisor') {
      drupal_goto($base_url . '/' . daedalus_get_setting('advise student'));
    }

    // Same for the support user.
    elseif ($type == 'support') {
      drupal_goto($base_url . '/' . daedalus_get_setting('support student'));
    }

  }

  // First convert the timeout back to minutes
  $minutes_remaining = ($timeout / 60) - $session_minutes;
  $seconds_remaining = $timeout - $session_seconds;

  if ($type == 'advisor') {
    $add_time_url = $base_url . '/' . daedalus_get_setting('advise student') . '/add_time/' . $page;
  }
  elseif ($type == 'support') {
    $add_time_url = $base_url . '/' . daedalus_get_setting('support student') . '/add_time/' . $page;
  }

  if ($minutes_remaining == 1) {

    // If there is one minute remaining display the minute,
    // There is no seconds countdown, if JS is enabled the
    // session will automatically timeout, if not it won't.

    if (Drupal.jsEnabled) {
      $timeout_message = t('The session will time out in 1 minute. <a href="@url">Add time?</a>',
              array('@url' => url($add_time_url)));
    }
    else {
      drupal_set_message(t('The session will time out in 1 minute. <a href="@url">Add time?</a>',
              array('@url' => url($add_time_url))), 'warning');
    }

  }
  elseif ($minutes_remaining <= 10) {

    // If there are less than 10 minutes remaining display the calculated
    // minutes. If JS is enable the first minute remaining will countdown
    // to the exact remainder of seconds, then by every other 60 seconds.
    // If not enabled, there will be no change in the displayed minutes.

    if (Drupal.jsEnabled) {
      $timeout_message = t('The session will time out in <a class="minutes">@remaining</a> minutes. <a href="@url">Add time?</a>',
              array('@remaining' => $minutes_remaining, '@url' => url($add_time_url)));
    }
    else {
      drupal_set_message(t('The session will time out in @remaining minutes. <a href="@url">Add time?</a>',
              array('@remaining' => $minutes_remaining, '@url' => url($add_time_url))), 'warning');
    }

  }
  else {

    // Exactly 10 minutes remain. The JS will countdown every 60 seconds until
    // the session timeout. If not enabled the 10 minutes will not change.

    if (Drupal.jsEnabled) {
      $timeout_message = t('The session will time out in <a class="minutes">10</a> minutes. <a href="@url">Add time?</a>',
              array('@url' => url($add_time_url)));
    }
    else {
      drupal_set_message(t('The session will time out in 10 minutes. <a href="@url">Add time?</a>',
              array('@url' => url($add_time_url))), 'warning');
    }

  }

  if (Drupal.jsEnabled) {

    // Include the pages JavaScript file.
    drupal_add_js(drupal_get_path('module', 'daedalus') . '/javascript/daedalus_session_timeout.js');

    $form = array();

    $millisecs_remaining = ($seconds_remaining - (10 * 60)) * 1000;

    $form[] = array(
      '#type' => 'item',
      '#value' => '<div class="timeout" millisecs="' . $millisecs_remaining . '">
                     <div class="session-warning">' . $timeout_message . '</div>
                   </div>',
    );

    // Return form if
    // js is enabled.
    return $form;

  }

}


/**
 * Function: daedalus_absolute_path
 *
 * Get the true path to the root of the Drupal site.
 * Better than using DOCUMENT_ROOT and base_path().
 *
 * Currently this function is not in use.
 *
 * This function was posted here:
 * http://drupal.org/node/67961
 *
 * The comment url is here:
 * http://drupal.org/node/67961#comment-3800534
 *
 * @staticvar string $daedalus_absolute_path
 * @return <type>
 */
function daedalus_absolute_path() {

  static $daedalus_absolute_path = NULL;

  if ($daedalus_absolute_path === NULL) {

    // Get the absolute path to this file:
    $dir = rtrim(str_replace('\\', '/', dirname(__FILE__)), '/');
    $parts = explode('/', $dir);

    // Iterate up the directory hierarchy
    // until we find the website root:
    $done = FALSE;

    do {

      // Check a couple of obvious things:
      $done = is_dir("$dir/sites") && is_dir("$dir/includes") && is_file("$dir/index.php");

      if (!$done) {

        // If there's no more path to examine,
        // we didn't find the site root:
        if (empty($parts)) {

          $daedalus_absolute_path = FALSE;

          break;

        }

        // Go up one level
        // and look again:
        array_pop($parts);

        $dir = implode('/', $parts);

      }

    } while (!$done);

    $daedalus_absolute_path = $dir;

  }

  return $daedalus_absolute_path;

}


/**
 * Function: daedalus_session_duration
 *
 * Calculates the difference in minutes between a session start time and the current time.
 * Date difference calculation thanks to http://stackoverflow.com/questions/676824/how-to-calculate-the-difference-between-two-dates-using-php;
 *
 * @param type $session_time
 * @return type
 */
function daedalus_session_duration($session_time) {

  $current_time = date('Y-m-d H:i:s', time());

  $date1 = $current_time;
  $date2 = $session_time;

  $diff = abs(strtotime($date2) - strtotime($date1));

  $years = floor($diff / (365*60*60*24));
  $months = floor(($diff - $years * 365*60*60*24) / (30*60*60*24));
  $days = floor(($diff - $years * 365*60*60*24 - $months*30*60*60*24)/ (60*60*24));

  $hours = floor(($diff - $years * 365*60*60*24 - $months*30*60*60*24 - $days*60*60*24)/ (60*60));
  $minutes = floor(($diff - $years * 365*60*60*24 - $months*30*60*60*24 - $days*60*60*24 - $hours*60*60)/ 60);
  $seconds = floor(($diff - $years * 365*60*60*24 - $months*30*60*60*24 - $days*60*60*24 - $hours*60*60 - $minutes*60));

  return $hours . '-' . $minutes . '-' . $seconds;

}


/**
 * Function: daedalus_remove_downloads
 *
 * Removes the download directory used to
 * temorarily decrypt downloaded files.
 */
function daedalus_remove_downloads() {

  // Include the pages JavaScript file.
  drupal_add_js(drupal_get_path('module', 'daedalus') . '/javascript/daedalus_remove_downloads.js');

  $download_directory = file_directory_path() . '/downloads';

  if (file_exists($download_directory)) {

    exec('rm -R ' . $download_directory);

    return TRUE;

  }

  return FALSE;

}


/**
 * Function: daedalus_parse_imagemap
 *
 * This will open the .map file and add alter the
 * html titles for each area html tag in the file.
 *
 * @param <type> $location
 */
function daedalus_parse_imagemap($location) {

  $handle = @fopen($location, 'r');

  if ($handle) {

    while (($buffer = fgets($handle, 4096)) !== FALSE) {

      // Matching a course
      if (preg_match('/^<area/', $buffer)) {

        $temp = explode(' ', $buffer);
        $course_code = $temp[5];
        $course_number = $temp[6];

        // The code will look like 'title="CSCI'
        $course_code = explode('"', $course_code);
        $course_code = $course_code[1];

        // The course number will look like '1101"'
        $course_number = explode('"', $course_number);
        $course_number = $course_number[0];

        $course_name = db_result(db_query("SELECT course_name FROM {dae_course} WHERE course_code='%s' AND course_number=%d", $course_code, $course_number));
        $course_name = str_replace('&', 'and', $course_name);

        $replace_string = '"' . $course_code . ' ' . $course_number . '"';

        // Replace the original title with the course name.
        $output .= str_replace($replace_string, '"' . $course_name . '"', $buffer);
      }

      else{
        $output .= $buffer;
      }

    }

    if (!feof($handle)) {
      drupal_set_message(t('Error: unexpected fgets() fail'), 'error');
    }

    fclose($handle);

  }

  $file = fopen($location, 'w+');

  fwrite($file, $output);

  fclose($file);

}


/**
 * Function: daedalus_parse_coursemap
 *
 * This will open the .map file and add alter the
 * html titles for each area html tag in the file.
 *
 * There is a small alteration between the course
 * maps and the other image maps. The target is
 * not included in the course maps.
 *
 * @param <type> $location
 */
function daedalus_parse_coursemap($location) {

  $handle = @fopen($location, 'r');

  if ($handle) {

    while (($buffer = fgets($handle, 4096)) !== FALSE) {

      // Matching a course
      if (preg_match('/^<area/', $buffer)) {

        $temp = explode(' ', $buffer);
        $course_code = $temp[4];
        $course_number = $temp[5];

        // The code will look like 'title="CSCI'
        $course_code = explode('"', $course_code);
        $course_code = $course_code[1];

        // The course number will look like '1101"'
        $course_number = explode('"', $course_number);
        $course_number = $course_number[0];

        $course_name = db_result(db_query("SELECT course_name FROM {dae_course} WHERE course_code='%s' AND course_number=%d", $course_code, $course_number));
        $course_name = str_replace('&', 'and', $course_name);

        $replace_string = '"' . $course_code . ' ' . $course_number . '"';

        // Replace the original title with the course name.
        $output .= str_replace($replace_string, '"' . $course_name . '"', $buffer);
      }

      else{
        $output .= $buffer;
      }

    }

    if (!feof($handle)) {
      drupal_set_message(t('Error: unexpected fgets() fail'), 'error');
    }

    fclose($handle);

  }

  $file = fopen($location, 'w+');

  fwrite($file, $output);

  fclose($file);

}


/**
 * Function: daedalus_parse_graphviz
 *
 * This will open the .svg file and add html titles
 * to each course and SLO node.
 *
 * @param <type> $location
 */
function daedalus_parse_graphviz($location) {

  $handle = @fopen($location, 'r');

  if ($handle) {

    while (($buffer = fgets($handle, 4096)) !== FALSE) {

      // Matching a course
      if (preg_match('/^<text/', $buffer)) {

        $temp = explode('>', $buffer);

        $temp = $temp[1] . '\n';

        $temp = explode('<', $temp);

        $course = $temp[0];

        $course_name = db_result(db_query("SELECT course_name FROM {dae_course} WHERE course='%s'", $course));

        $course_name = str_replace('&', 'and', $course_name);

        $title = 'title="' . $course_name . '"';

        $output .= str_replace('">', '" ' . $title . '>', $buffer);

      }

      // Matching a learning outcome
      elseif (preg_match('/^<a xlink:href/', $buffer) && preg_match('/manage\/slo/', $buffer)) {

        $temp = explode('manage', $buffer);

        $slo_id = ereg_replace('[^0-9]', '', $temp[1]) . '\n';

        $slo_text = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $slo_id));

        $slo_text = str_replace('&', 'and', $slo_text);

        $slo_text = str_replace('"', '', $slo_text);

        $title = 'title="' . $slo_text . '"';

        $output .= str_replace('">', '" ' . $title . '>', $buffer);

      }

      else{
        $output .= $buffer;
      }

    }

    if (!feof($handle)) {
      drupal_set_message(t('Error: unexpected fgets() fail'), 'error');
    }

    fclose($handle);

  }

  $file = fopen($location, 'w+');

  fwrite($file, $output);

  fclose($file);

}

/**
 * Function: daedalus_calculate_slo_rank
 *
 * Dr. Blouin's Sudo Code
 *
 * 1. Reset all ranks to -1 (or NULL)
 *    x <- SELECT ALL SLOs of rank 0
 *    r <- 0
 *    while x != 0
 *      x <- All SLOs which rank == NULL ^ has 1+ prerequisites of rank r
 *      Assign rank r+1 to all SLOs E(subset) x
 *      r <- r+1
 */
function daedalus_calculate_slo_rank() {

  $preslo_ids = array();

  // Select all SLO's with a prerequisite SLO
  $result = db_query("SELECT DISTINCT target FROM {dae_prereq_slo} ORDER BY target");
  while ($row = db_fetch_array($result)) {
    $preslo_ids[] = $row['target'];
  }

  if ($preslo_ids) {

    // Set all SLO ranks to -1
    db_query("UPDATE {dae_slo} SET slo_rank=-1");

    // Select all SLO's
    $result = db_query("SELECT DISTINCT id FROM {dae_slo} ORDER BY id");
    while ($row = db_fetch_array($result)) {
      $slo_ids[] = $row['id'];
    }

    // Create an array of SLO's that have no prerequisite SLO's
    $rank_zero = array_diff($slo_ids, $preslo_ids);

    $placeholders = implode(' OR ', array_fill(0, count($rank_zero), 'id=%d'));

    // Set each SLO without a prerequiste to a rank of zero.
    db_query("UPDATE {dae_slo} SET slo_rank=0 WHERE " . $placeholders, $rank_zero);

    $rank_iteration = daedalus_get_setting('learning outcome rank iteration');

    // A slight variation to Christians sudo code that does the same thing. The list of all SLO's
    // that have a prerequisite SLO are iterated checking to see if the have a prerequisite SLO
    // at the current iteration depth. If a prereq SLO is found the current SLO is given a rank
    // of the iteration plus 1.
    for ($i = 0; $i <= $rank_iteration; $i++) {

      foreach ($preslo_ids as $id) {

        $result = db_query("SELECT * FROM {dae_prereq_slo} WHERE target=%d", $id);
        while ($row = db_fetch_array($result)) {

          if (db_result(db_query("SELECT COUNT(*) FROM {dae_slo} WHERE slo_rank=%d AND id=%d", $i, $row['pre_slo']))) {

            db_query("UPDATE {dae_slo} SET slo_rank=%d WHERE id=%d", ($i+1), $id);

            break;

          }

        }

      }

    }

  }

  else{
    // Because there are no prerequisite
    // SLOs set all SLO ranks to 0.
    db_query("UPDATE {dae_slo} SET slo_rank=0");
  }

}


/**
 * Function: daedalus_help_form
 *
 * This function returns the help form found in each page callback.
 * This selects the help text from the database if there is text saved,
 * and displays the results in a dropdown text area if the user has
 * permission to edit. If there is no editing permission the user can
 * only view the plain text.
 *
 * @param <type> $page_url
 * @param <type> $show_border
 * @param <type> $show_break
 * @return <type>
 */
function daedalus_help_form($page_url, $show_border, $show_break) {

  if ($show_border) {
    $border = 'style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"';
  }

  if ($show_break) {
    $break = '<br />';
  }

  $help_text = db_result(db_query("SELECT help_text FROM {dae_page_help} WHERE page_url = '%s'", $page_url));

  if (!$help_text) {
    $help_text = t('Currently there is no help available for this page.');
  }

  $form = array();

  if (user_access('daedalus help edit')) {

    // Editable Hidden Help Section
    $form['dae-help'] = array(
      '#type' => 'textarea',
      '#title' => t('Daedalus Help'),
      '#default_value' => $help_text,
      '#weight' => -60,
      '#description' => t('Saving help text will cause all other form data to be lost.'),
      '#prefix' => '<div class="hide-help" is_hidden="yes">',
      '#suffix' => '</div>',
    );

    $form['dae-help-submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#weight' => -50,
      '#prefix' => '<div class="hide-help" is_hidden="yes">',
      '#suffix' => '</div>',
    );

    $form['dae-help-sep'] = array(
      '#type' => 'item',
      '#weight' => -35,
      '#prefix' => '<div class="hide-help" is_hidden="yes" ' . $border . '>',
      '#suffix' => '</div>' . $break,
    );

  }

  else{

    // Non-Editable Hidden Help Section
    $form['dae-help'] = array(
      '#type' => 'item',
      '#title' => t('Daedalus Help'),
      '#value' => $help_text,
      '#weight' => -60,
      '#prefix' => '<div class="hide-help" is_hidden="yes">',
      '#suffix' => '</div>',
    );

    $form['dae-help-sep'] = array(
      '#type' => 'item',
      '#weight' => -40,
      '#prefix' => '<div class="hide-help" is_hidden="yes" ' . $border . '>',
      '#suffix' => '</div>' . $break,
    );

  }

  return $form;

}


/**
 * Function: daedalus_graphviz_installed
 *
 * Determines if the graphviz directory is installed,
 * also returns the working directory and graphviz path.
 *
 * @param <type> $pwd
 * @param <type> $graphviz_path
 * @return boolean
 */
function daedalus_graphviz_installed(&$pwd, &$graphviz_path) {

  $installed = FALSE;

  // This searches for the directory name graphviz which
  // must be installed including full permissions to be
  // able to interact with the maps.
  if (exec('find -name graphviz')) {

    $pwd = exec('pwd');
    $perm = drupal_substr(sprintf('%o', fileperms($pwd . '/graphviz')), -4);

    if ($perm == '0777') {
      $installed = TRUE;
    }

  }

  $graphviz_path = daedalus_get_setting('graphviz path');

  if ($graphviz_path == '{DNE-graphviz path}' || !$graphviz_path) {

    $installed = FALSE;

    $graphviz_path = '';

  }

  return $installed;

}


/**
 * Function: daedalus_uncovered_prerequiste_slos
 *
 * Determines if a given course and any of it's prerequisite
 * courses has covered the courses learning outcomes.
 *
 * @param <type> $course_id
 * @param <type> $amount
 * @param <type> $uncovered
 * @return <reference>
 */
function daedalus_uncovered_prerequiste_slos($course_id, &$amount, &$uncovered=array()) {

  $all_taught = array(); $slo_list = array(); $asslo_list = array(); $preu_slo_list = array();
  $preu_ids = array(); $discovered_courses = array(); $warning_courses = array();

  // Add the currently selected course
  // to the list of discovered courses.
  $discovered_courses[$course_id] = $course_id;

  // Select all of the learning outcomes from
  // each prerequisite course, add them to the
  // list of all taught learning outcomes.
  $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $course_id);
  while ($row = db_fetch_array($result)) {

    $current_prereq_id = $row['prereq_id'];

    daedalus_slos_recursively($current_prereq_id, $all_taught, $discovered_courses, $warning_courses);

  }

  if ($all_taught) {  // Are there values?

    $all_taught = array_unique($all_taught);

    // Select all the student learning outcomes associated with the given
    // course. For each learning outcome directly associated to the courses
    // learning outcomes add their prerequisite slos to an array, while
    // creating the list of slos for the course.
    $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d", $course_id);
    while ($row = db_fetch_array($result)) {

      $slo_list[] = $row['slo_id'];
      daedalus_prereq_slos($row['slo_id'] , $asslo_list);

    }

    if ($asslo_list) {  // Are there values?

      // Make sure the values are unique
      $asslo_list = array_unique($asslo_list);

      // Make sure that there are no taught slos in the
      // list of assumed slos for the given course.
      $asslo_list = array_diff($asslo_list, $slo_list);

      // Make sure there are no outcomes
      // from a PREU located in the list.
      $result = db_query("SELECT id FROM {dae_course} WHERE course_code='PREU'");
      while ($row = db_fetch_array($result)) {
        $preu_ids[] = $row['id'];
      }

      if ($preu_ids) {  // Are there values?

        foreach ($preu_ids as $id) {

          $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d", $id);
          while ($row = db_fetch_array($result)) {
            $preu_slo_list[] = $row['slo_id'];
          }

        }

      }

      if ($preu_slo_list) {

        // Remove the PREU slo items.
        $asslo_list = array_diff($asslo_list, $preu_slo_list);

        if ($asslo_list) {

          // Iterate each list to look for matches, if there
          // is no match for a given entry of the assumed learning
          // outcomes add the id for it to the list to be displayed.
          foreach ($asslo_list as $assloid) {

            $match = FALSE;

            foreach ($all_taught as $sloid) {

              if ($assloid == $sloid) {
                  $match = TRUE;
              }

            }

            // If no match add the SLO
            // id to the uncovered list
            if (!$match) {

              $uncovered[] = $assloid;
              $amount++;

            }

          }

        }

      }

    }

  }

  if ($warning_courses) {

    $cycle_courses = array();

    foreach ($warning_courses as $warn_cid) {

      // Make sure a cycle is not displayed twice.
      if (!in_array($warn_cid, $cycle_courses)) {

        $warning_prereqs = array();

        $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $warn_cid);
        while ($row = db_fetch_array($result)) {
          $warning_prereqs[$row['prereq_id']] = $row['prereq_id'];
        }

        if ($warning_prereqs) {

          foreach ($warning_prereqs as $warn_preid) {

            if (db_result(db_query("SELECT COUNT(*) FROM {dae_prereq_course} WHERE prereq_id=%d AND course_id=%d", $warn_cid, $warn_preid))) {

              if (in_array($warn_preid, $warning_courses)) {
                $cycle_courses[$warn_preid] = $warn_preid;
              }

              $warning_course = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $warn_cid));
              $warning_prereq = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $warn_preid));

              drupal_set_message(t('WARNING: A prerequisite cycle has been found in the curriculum between @wcourse and @wprereq',
                      array('@wcourse' => $warning_course, '@wprereq' => $warning_prereq)), 'warning');

            }

          }

        }

      }

    }

  }

  if ($amount > 0) {
    return TRUE;
  }

  return FALSE;

}


/**
 * Function: daedalus_slos_recursively
 *
 * Helping function to:
 *       get uncovered prerequiste slos
 *
 * @param <type> $preid
 * @param <type> $array
 */
function daedalus_slos_recursively($preid, &$array, &$discovered, &$warning) {

  // Append all the SLO ids from the
  // prerequisite course to the array.
  $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d", $preid);
  while ($row = db_fetch_array($result)) {
    $array[] = $row['slo_id'];
  }

  // Call the function for every prerequisite course to prerequisite
  // that is passed to the function... and so on... and so on...
  $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $preid);
  while ($row = db_fetch_array($result)) {

    $prereq_id = $row['prereq_id'];

    // Keep track of which courses have had their SLOs added
    // to the SLO array. If another course is discovered do
    // not recurse any further down the path. This indicates
    // that the curriculum has a cycle and will cause the
    // server to hang and fail to display the page. Infinitely
    if (!in_array($prereq_id, $discovered)) {

      // Add the newly discovered prereqisite course.
      $discovered[$prereq_id] = $prereq_id;

      daedalus_slos_recursively($prereq_id, $array, $discovered, $warning);

    }
    else {
      // Add course to the warning array.
      $warning[$prereq_id] = $prereq_id;
    }

  }

}


/**
 * Function: daedalus_assumed_slos
 *
 * Returns the list of assumed slos
 *
 * @param <type> $course_id
 * @param <type> $array
 */
function daedalus_assumed_slos($course_id, &$array) {

  $slo_list = array();

  // Select all the student learning outcomes associated with the given
  // course. For each learning outcome add their prerequisite slos to an
  // array, while creating the list of slos for the course.
  $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d", $course_id);
  while ($row = db_fetch_array($result)) {

    $slo_list[] = $row['slo_id'];

    daedalus_prereq_slos($row['slo_id'] , $array);

  }

  // Make sure the values are unique
  $array = array_unique($array);

  // Make sure that there are no taught
  // slos in the list of assumed slos
  // for the given course.
  $array = array_diff($array, $slo_list);

}


/**
 * Function: daedalus_prereq_slos
 *
 * Returns by reference the list of prerequisite
 * slos associated with an learning outcome.
 *
 * @param <type> $id
 * @param <type> $array
 */
function daedalus_prereq_slos($id , &$array) {

  // Append all the SLO ids to the array
  $result = db_query("SELECT pre_slo FROM {dae_prereq_slo} WHERE target=%d", $id);
  while ($row = db_fetch_array($result)) {
    $array[] = $row['pre_slo'];
  }

}


/**
 * Function: daedalus_prereq_slos_recursively
 *
 * Helping function to:
 *       get the list of prerequisite SLO's to a given SLO.
 *
 * @param <type> $id
 * @param <type> $array
 */
function daedalus_prereq_slos_recursively($id , &$array) {

  // Append all the SLO ids to the array
  daedalus_prereq_slos($id, $array);

  $result = db_query("SELECT pre_slo FROM {dae_prereq_slo} WHERE target=%d", $id);
  while ($row = db_fetch_array($result)) {
    daedalus_prereq_slos_recursively($row['pre_slo'] , $array);
  }

}


/**
 * Function: daedalus_valid_course
 *
 * Returns true if a course with the given code and number exists
 *
 * @param <type> $course_code
 * @param <type> $course_number
 * @return <type> boolean
 */
function daedalus_valid_course($course_code, $course_number='') {

  $course_code = drupal_strtoupper($course_code);

  if (!$course_number) {

    if (db_result(db_query("SELECT COUNT(*) FROM {dae_course} WHERE course='%s'", $course_code))) {
      return TRUE;
    }

  }
  else{

    if (db_result(db_query("SELECT COUNT(*) FROM {dae_course} WHERE course_code='%s' AND course_number='%d'", $course_code, $course_number))) {
      return TRUE;
    }

  }

  return FALSE;

}


/**
 * Function: daedalus_valid_slo
 *
 * returns true if the slo text exists
 *
 * @param <type> $slo_text
 * @return <type>
 */
function daedalus_valid_slo($slo_text) {

  if (db_result(db_query("SELECT COUNT(*) FROM {dae_slo} WHERE slo_text='%s'", $slo_text))) {
    return TRUE;
  }

  return FALSE;

}


/**
 * Function: daedalus_valid_course_code
 *
 * Checks the course code table to see if it is valid.
 *
 * @param <type> $course_code
 * @return <type>
 */
function daedalus_valid_course_code($course_code) {

  if (db_result(db_query("SELECT COUNT(*) FROM {dae_valid_course_codes} WHERE course_code='%s'", $course_code))) {
    return TRUE;
  }

  return FALSE;

}


/**
 * Function: daedalus_valid_tag
 *
 * Checks the tag table to see if it is valid
 *
 * @param <type> $tag
 * @param <type> $id
 * @return <type>
 */
function daedalus_valid_tag($tag) {

  if (db_result(db_query("SELECT COUNT(*) FROM {dae_tag} WHERE tag_label='%s'", $tag))) {
    return TRUE;
  }

  return FALSE;

}


/**
 * Function: daedalus_valid_alias
 *
 * @param <type> $course_code
 * @param <type> $course_number
 * @return <type>
 */
function daedalus_valid_alias($course_code, $course_number="") {

  if (!$course_number) {

    if (db_result(db_query("SELECT COUNT(*) FROM {dae_course_alias} WHERE alias_course='%s'", $course_code))) {
      return TRUE;
    }

  }

  else{

    if (db_result(db_query("SELECT COUNT(*) FROM {dae_course_alias} WHERE alias_code='%s' AND alias_number=%d", $course_code, $course_number))) {
      return TRUE;
    }

  }

  return FALSE;

}


/**
 * Function: daedalus_course_id
 *
 * Accepts a course code and number, and
 * returns the course id that is related
 * to the code and number.
 *
 * @param <type> $course_code
 * @param <type> $course_number
 * @return <type> id()
 */
function daedalus_course_id($course_code, $course_number='') {

  // If only one parameter is present return
  // the id by comparing the db course value.
  // Else compare by both parameters
  if (!$course_number) {
    $id = db_result(db_query("SELECT id FROM {dae_course} WHERE course='%s'", drupal_strtoupper($course_code)));
  }

  else{
    $id = db_result(db_query("SELECT id FROM {dae_course} WHERE course_code='%s' AND course_number=%d", drupal_strtoupper($course_code), $course_number));
  }

  return $id;

}


/**
 * Function: daedalus_slo_id
 *
 * @param <type> $slo_text
 * @return <type>
 */
function daedalus_slo_id($slo_text) {

  $id = db_result(db_query("SELECT id FROM {dae_slo} WHERE slo_text='%s'", $slo_text));

  return $id;

}


/**
 * Function: daedalus_tag_id
 *
 * @param <type> $label
 * @return <int>
 */
function daedalus_tag_id($label) {

  $id = db_result(db_query("SELECT id FROM {dae_tag} WHERE tag_label='%s'", $label));

  return $id;

}


/**
 * Function: daedalus_explode_trim
 *
 * Same as the explode (string, string) PHP function,
 * but trims all of the results too.
 *
 * @param <type> $needle
 * @param <type> $haystack
 * @return <type>
 */
function daedalus_explode_trim($needle, $haystack) {

  $trimmed = explode($needle, $haystack);

  foreach ($trimmed as $key => $value) {
    $trimmed[$key] = trim($value);
  }

  return $trimmed;

}