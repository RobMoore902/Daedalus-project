<?php

/**
 * @file
 * daedalus_browse_outcomes.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Browse->Learning Outcomes Callback Function
//------------------------------------------------------------------------------
function daedalus_browse_outcomes_page() {
  return drupal_get_form('daedalus_browse_outcomes_form');
}


/**
 * Menu Location: Daedalus -> Browse -> Learning Outcomes
 * URL Location:  daedalus/browse/slo
 *
 * Displays the tag cloud and the list of SLO's.
 */
function daedalus_browse_outcomes_form($form) {

  // Include the pages JavaScript file.
  drupal_add_js(drupal_get_path('module', 'daedalus') . '/javascript/daedalus-browse-course.js');

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='browse learning outcomes' OR
                             setting='manage learning outcomes' OR
                             setting='question mark' OR
                             setting='goal identification' OR
                             setting='tag cloud max font size' OR
                             setting='build learning outcomes' OR
                             setting='tag cloud height percent' OR
                             setting='show tag cloud settings'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  global $base_url;
  global $user;

  $page_url = $help_url = $page_settings['browse learning outcomes'];

  // Store URL Parameters in $param array
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  // The Manage SLO URL.
  $slo_url = $page_settings['manage learning outcomes'];

  // URL Parameters.
  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);
  $param[2] = arg(2+$page_url_length);

  $slo_id = $param[0];
  $tags = $param[1];
  $sort_type = $param[2];

  // Determine if the magellan module is installed.
  $magellan_check = module_exists('magellan');

  // Get Access Information
  $build_access = user_access('daedalus build');

  // Get the images
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';
  $goal_img = '<img src="' . $base_url . '/' . $page_settings['goal identification'] . '" title="This learning outcome is part of your goal" />';

  drupal_set_title(t('Browse Learning Outcomes !help', array('!help' => $show_help)));

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form[] = daedalus_help_form($help_url, 1, 1);

  // Only execute code if magellan database tables are installed.
  // This will popup a session timeout warning if the user is a
  // Magellan advisor and they are in session
  if ($magellan_check) {

    // If the current user is a Magellan Advisor determine
    // if there is a current advising session open.
    if (db_result(db_query("SELECT COUNT(*)
                               FROM {role, users_roles}
                              WHERE role.name='Magellan Advisor'
                                AND users_roles.uid=%d
                                AND users_roles.rid = role.rid", $user->uid))) {

      $result = db_query("SELECT id, add_time FROM {mag_advisor_session} WHERE advisor_id=%d", $user->uid);
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
        $session_time = db_result(db_query("SELECT session_time FROM {mag_session_log} WHERE advisor_id=%d AND session_id='%s'", $user->uid, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse_learning_outcomes', 'advisor');

      }

    }

    // If the current user is a Magellan Support determine
    // if there is a current support session open.
    if (db_result(db_query("SELECT COUNT(*)
                               FROM {role, users_roles}
                              WHERE role.name='Magellan Support Staff'
                                AND users_roles.uid=%d
                                AND users_roles.rid = role.rid", $user->uid))) {

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
        $session_time = db_result(db_query("SELECT session_time FROM {mag_session_log} WHERE support_id=%d AND session_id='%s'", $user->uid, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse_learning_outcomes', 'support');

      }

    }

  }

  // Make sure there are SLOs present.
  if (!db_result(db_query("SELECT COUNT(*) FROM {dae_slo}"))) {

    if ($build_access) {

      // If there are no learning outcomes, inform
      // the user a SLO must be created first.
      $url = $base_url . '/' . $page_settings['build learning outcomes'];

      drupal_set_message(t('There are no learning outcomes created. <a href="@url"><b>Create a Learning Outcome?</b>.</a>',
              array('@url' => url($url))));

    }

  }
  else {

    // Only execute code if magellan
    // database tables are installed.
    if ($magellan_check) {

      // Get the user name.
      $user_name = db_result(db_query("SELECT name FROM {users} WHERE uid=%d", $user->uid));

      // Get the students identification number
      $student_id = db_result(db_query("SELECT id FROM {mag_student_identification} WHERE user_name='%s'", $user_name));

      $goal_slos = array();
      $result = db_query("SELECT slo_id FROM {mag_goal_student_slo} WHERE student_id=%d ORDER BY slo_id", $student_id);
      while ($row = db_fetch_array($result)) {
        $goal_slos[$row['slo_id']] = $row['slo_id'];
      }

    }

    // Initialize the cloud tag string.
    $cloud_string = '';

    // If a tag is not selected display the entire tag cloud.
    if ($slo_id != 'selected') {

      $tag_array = array();

      $min = -1; $max = -1; $i = 0;

      // Create the entire tag array.
      $result = db_query("SELECT * FROM {dae_tag} ORDER BY tag_label");
      while ($row = db_fetch_array($result)) {

        $tag_count = db_result(db_query("SELECT COUNT(*) FROM {dae_slo_tag} WHERE tag_id=%d", $row['id']));

        if ($tag_count) {

          $tag_array[$i]['tag_id'] = $row['id'];
          $tag_array[$i]['label'] = $row['tag_label'];
          $tag_array[$i]['count'] = $tag_count;
          $i++;

          // Store the min and max tag counts to
          // help calculate the tag cloud text size.
          if ($min == -1) {
            $min = $max = $tag_count;
          }
          elseif ($tag_count < $min && $tag_count != 0) {
            $min = $tag_count;
          }
          elseif ($tag_count > $max) {
            $max = $tag_count;
          }

        }

      }

      if ($tag_array) {

        foreach ($tag_array as $value) {

          // Make sure we don't divide by 0
          if ($max == $min && $max > 1) {
            $min = $max-1;
          }
          elseif ($max == $min && $max == 1) {
            $max++;
          }

          // Log scale formula found online
          $weight = (log($value['count'])-log($min))/(log($max)-log($min));

          switch ($weight) {
            case ($weight == 0):
              $font_size = 8 + ($page_settings['tag cloud max font size'] * 2);
              $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
              $big_end = '</div>';
              break;
            case ($weight > 0 && $weight <= 0.4):
              $font_size = 15 + ($page_settings['tag cloud max font size'] * 2);
              $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
              $big_end = '</div>';
              break;
            case ($weight > 0.4 && $weight <= 0.6):
              $font_size = 23 + ($page_settings['tag cloud max font size'] * 2);
              $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
              $big_end = '</div>';
              break;
            case ($weight > 0.6 && $weight <= 0.8):
              $font_size = 30 + ($page_settings['tag cloud max font size'] * 2);
              $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
              $big_end = '</div>';
              break;
            case ($weight > 0.8 && $weight <= 1.0):
              $font_size = 38 + ($page_settings['tag cloud max font size'] * 2);
              $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
              $big_end = '</div>';
              break;
          }

          // Show the number of times the SLO has been tagged.
          $par_count = '<small><small>(' . $value['count'] . ')</small></small>';

          if ($slo_id == 'alpha') {

            $cloud_url = $page_url . '/selected/' . $value['tag_id'] . '/' . $slo_id;

            $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . $par_count . ' &nbsp;';

          }
          else {

            $cloud_url = $page_url . '/selected/' . $value['tag_id'];

            $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . $par_count . ' &nbsp;';

          }

        }

      }

    }
    else {

      // Else a tag has been selected so narrow the tags in the
      // cloud tag accoring to the select tag. tag tag tag

      $label_index = array();

      // Make a list of ids and their tag labels to reduce
      // the amount of database access required for the script.
      $result = db_query("SELECT * FROM {dae_tag} ORDER BY tag_label");
      while ($row = db_fetch_array($result)) {
        $label_index[$row['id']] = $row['tag_label'];
      }

      // If the tags are not delimited by an underscore
      // select the results with only the singel tag.
      if (!strrpos($tags, '_')) {

        // Deselect all tags by clicking the only selected tag.
        $selected_tags = '<a href="' . $page_url . '/' . $sort_type . '">#' . $label_index[$tags] . '</a>';

        $slo_array = array();

        // Get all the slo id's for the selected tag.
        $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d ORDER BY slo_id", $tags);
        while ($row = db_fetch_array($result)) {
          $slo_array[] = $row['slo_id'];
        }

        // Create the query string placeholder to select the slos.
        $slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'slo_id=%d'));

        $result_tag = array();

        $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE " . $slo_placeholders . " ORDER BY tag_id", $slo_array);
        while ($row = db_fetch_array($result)) {

          if ($row['tag_id'] != $tags) {
            $result_tag[] = $row['tag_id'];
          }

        }

        if ($result_tag) {

          $ordered_tags = array();

          // Create the query string placeholder to select the tags again in alphabetical order.
          $tag_placeholders = implode(' OR ', array_fill(0, count($result_tag), 'id=%d'));

          $result = db_query("SELECT id FROM {dae_tag} WHERE " . $tag_placeholders . " ORDER BY tag_label", $result_tag);
          while ($row = db_fetch_array($result)) {
            $ordered_tags[] = $row['id'];
          }

          // Calculate the total number of SLO's associated with the the
          // current tag being computed for the tag cloud.
          foreach ($ordered_tags as $current_tag) {

            $slo_array1 = array(); $slo_array2 = array();

            // Get the slo ids using the URL's tag
            $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags);
            while ($row = db_fetch_array($result)) {
              $slo_array1[] = $row['slo_id'];
            }

            // Get the slo ids using the current tag
            $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $current_tag);
            while ($row = db_fetch_array($result)) {
              $slo_array2[] = $row['slo_id'];
            }

            // Combine each lists make sure there are no duplicate
            // entries and the number of matching items is the count.
            $temp_slos = array_intersect($slo_array1, $slo_array2);
            $temp_slos = array_unique($temp_slos);
            $tag_count = count($temp_slos);

            $tag_array[$current_tag]['tag_id'] = $current_tag;
            $tag_array[$current_tag]['count'] = $tag_count;
            $tag_array[$current_tag]['label'] = $label_index[$current_tag];

            // Store the min and max tag count to
            // help calculate the tag cloud text size.
            if (!$min) {
              $min = $max = $tag_count;
            }
            elseif ($tag_count < $min) {
              $min = $tag_count;
            }
            elseif ($tag_count > $max) {
              $max = $tag_count;
            }

          }

          if ($tag_array) {

            foreach ($tag_array as $value) {

              // Make sure we don't divide by 0
              if ($max == $min && $max > 1) {
                $min = $max-1;
              }
              elseif ($max == $min && $max == 1) {
                $max++;
              }

              $weight = (log($value['count'])-log($min))/(log($max)-log($min));

              switch ($weight) {
                case ($weight == 0):
                  $font_size = 8 + ($page_settings['tag cloud max font size'] * 2);
                  $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                  $big_end = '</div>';
                  break;
                case ($weight > 0 && $weight <= 0.4):
                  $font_size = 15 + ($page_settings['tag cloud max font size'] * 2);
                  $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                  $big_end = '</div>';
                  break;
                case ($weight > 0.4 && $weight <= 0.6):
                  $font_size = 23 + ($page_settings['tag cloud max font size'] * 2);
                  $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                  $big_end = '</div>';
                  break;
                case ($weight > 0.6 && $weight <= 0.8):
                  $font_size = 30 + ($page_settings['tag cloud max font size'] * 2);
                  $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                  $big_end = '</div>';
                  break;
                case ($weight > 0.8 && $weight <= 1.0):
                  $font_size = 38 + ($page_settings['tag cloud max font size'] * 2);
                  $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                  $big_end = '</div>';
                  break;
              }

              // Only show tags with results
              if ($value['count'] > 0) {

                // Show the number of times the SLO has been tagged.
                $par_count = '<small><small>(' . $value['count'] . ')</small></small>';

                $cloud_url = $page_url . '/selected/' . $tags . '_' . $value['tag_id'] . '/' . $sort_type;

                $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . '' . $par_count . ' &nbsp;';

              }

            }

          }

        }

      }
      else {

        // Now the url parameter has more than one tag
        // selected. Turn the tags into an array of tags.
        $tags = explode('_', $tags);

        // Create the selected tag string with a twist. When two tags or more
        // have been selected, the selected tags string now has the option of
        // deselecting one of the tags or all of them.
        if (count($tags) == 2) {

          // Remove 1st tag.
          $url1 = $page_url . '/selected/' . $tags[1] . '/' . $sort_type;
          $selected_tags = '<a href="' . $url1 . '">#' . $label_index[$tags[0]] . '</a>';

          // Remove 2st tag.
          $url2 = $page_url . '/selected/' . $tags[0] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url2 . '" >#' . $label_index[$tags[1]] . '</a>';

          // Deselect all tags
          $deselect_url = $page_url . '/' . $sort_type;
          $selected_tags .= '<br /><b><a href="' . $deselect_url . '">' . t('deselect all') . '</a></b>';

          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d OR tag_id=%d ORDER BY slo_id", $tags[0], $tags[1]);
          while ($row = db_fetch_array($result)) {
            $slo_array[] = $row['slo_id'];
          }

          $slo_array = array_unique($slo_array);

          $slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'slo_id=%d'));

          $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE " . $slo_placeholders . " ORDER BY tag_id", $slo_array);
          while ($row = db_fetch_array($result)) {

            // Make sure not to add the selected tags.
            if (!in_array($row['tag_id'], $tags)) {
              $result_tag[] = $row['tag_id'];
            }

          }

          if ($result_tag) {

            $tag_placeholders = implode(' OR ', array_fill(0, count($result_tag), 'id=%d'));

            $result = db_query("SELECT id FROM {dae_tag} WHERE " . $tag_placeholders . " ORDER BY tag_label", $result_tag);
            while ($row = db_fetch_array($result)) {
              $ordered_tags[] = $row['id'];
            }

            foreach ($ordered_tags as $current_tag) {

              $slo_array1 = array(); $slo_array2 = array(); $slo_array3 = array();

              // Calculate the total number of
              // SLO's associated with Tag 1.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[0]);
              while ($row = db_fetch_array($result)) {
                $slo_array1[] = $row['slo_id'];
              }

              // Calculate the total number of
              // SLO's associated with Tag 2.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[1]);
              while ($row = db_fetch_array($result)) {
                $slo_array2[] = $row['slo_id'];
              }

              // Calculate the total number of SLOs
              // associated with the current tag.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $current_tag);
              while ($row = db_fetch_array($result)) {
                $slo_array3[] = $row['slo_id'];
              }

              // Combine each lists make sure there are no duplicate
              // entries and the number of matching items is the count.
              $temp_slos = array_intersect($slo_array1, $slo_array2);
              $temp_slos = array_intersect($slo_array3, $temp_slos);
              $temp_slos = array_unique($temp_slos);
              $tag_count = count($temp_slos);

              if ($tag_count > 0) {

                $tag_array[$current_tag]['tag_id'] = $current_tag;
                $tag_array[$current_tag]['count'] = $tag_count;
                $tag_array[$current_tag]['label'] = $label_index[$current_tag];

                // Store the min and max tag count to
                // help calculate the tag cloud text size.
                if (!$min) {
                  $min = $max = $tag_count;
                }
                elseif ($tag_count < $min) {
                  $min = $tag_count;
                }
                elseif ($tag_count > $max) {
                  $max = $tag_count;
                }

              }

            }

            // Create the tag cloud
            if ($tag_array) {

              foreach ($tag_array as $value) {

                // Make sure we don't divide by 0
                if ($max == $min && $max > 1) {
                  $min = $max-1;
                }
                elseif ($max == $min && $max == 1) {
                  $max++;
                }

                $weight = (log($value['count'])-log($min))/(log($max)-log($min));

                switch ($weight) {
                  case ($weight == 0):
                    $font_size = 8 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0 && $weight <= 0.4):
                    $font_size = 15 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0.4 && $weight <= 0.6):
                    $font_size = 23 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0.6 && $weight <= 0.8):
                    $font_size = 30 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0.8 && $weight <= 1.0):
                    $font_size = 38 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                }

                if ($value['count'] > 0) {

                  $par_count = '<small><small>(' . $value['count'] . ')</small></small>';

                  $cloud_url = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $value['tag_id'] . '/' . $sort_type;

                  $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . '' . $par_count . ' &nbsp;';

                }

              }

            }

          }

        }
        elseif (count($tags) == 3) {

          // Remove 1st tag.
          $url1 = $page_url . '/selected/' . $tags[1] . '_' . $tags[2] . '/' . $sort_type;
          $selected_tags = '<a href="' . $url1 . '">#' . $label_index[$tags[0]] . '</a>';

          // Remove 2nd tag.
          $url2 = $page_url . '/selected/' . $tags[0] . '_' . $tags[2] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url2 . '">#' . $label_index[$tags[1]] . '</a>';

          // Remove 3rd tag.
          $url3 = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url3 . '">#' . $label_index[$tags[2]] . '</a>';

          // Deselect all tags.
          $selected_tags .= '<br /><b><a href="' . $page_url . '/' . $sort_type . '">' . t('deselect all') . '</a></b>';

          // Select associated SLOs.
          $result = db_query("SELECT slo_id
                                FROM {dae_slo_tag}
                               WHERE tag_id=%d OR tag_id=%d OR tag_id=%d
                            ORDER BY slo_id", $tags[0], $tags[1], $tags[2]);

          while ($row = db_fetch_array($result)) {
            $slo_array[] = $row['slo_id'];
          }

          // Remove duplicate values.
          $slo_array = array_unique($slo_array);

          // Create the query string placeholder to select the slos.
          $slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'slo_id=%d'));

          $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE " . $slo_placeholders . " ORDER BY tag_id", $slo_array);
          while ($row = db_fetch_array($result)) {

            // Make sure not to add the selected tags.
            if (!in_array($row['tag_id'], $tags)) {
              $result_tag[] = $row['tag_id'];
            }

          }

          if ($result_tag) {

            // Create the query string placeholder to select the tags again in alphabetical order.
            $tag_placeholders = implode(' OR ', array_fill(0, count($result_tag), 'id=%d'));

            $result = db_query("SELECT id FROM {dae_tag} WHERE " . $tag_placeholders . " ORDER BY tag_label", $result_tag);
            while ($row = db_fetch_array($result)) {
              $ordered_tags[] = $row['id'];
            }

            foreach ($ordered_tags as $current_tag) {

              $slo_array1 = array(); $slo_array2 = array(); $slo_array3 = array(); $slo_array4 = array();

              // Calculate the total number of
              // SLO's associated with Tag 1.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[0]);
              while ($row = db_fetch_array($result)) {
                $slo_array1[] = $row['slo_id'];
              }

              // Calculate the total number of
              // SLO's associated with Tag 2.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[1]);
              while ($row = db_fetch_array($result)) {
                $slo_array2[] = $row['slo_id'];
              }

              // Calculate the total number of
              // SLO's associated with Tag 3.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[2]);
              while ($row = db_fetch_array($result)) {
                $slo_array3[] = $row['slo_id'];
              }

              // Calculate the total number of SLOs
              // associated with the current tag.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $current_tag);
              while ($row = db_fetch_array($result)) {
                $slo_array4[] = $row['slo_id'];
              }

              // Combine each lists make sure there are no duplicate
              // entries and the number of matching items is the count.
              $temp_slos = array_intersect($slo_array1, $slo_array2);
              $temp_slos = array_intersect($slo_array3, $temp_slos);
              $temp_slos = array_intersect($slo_array4, $temp_slos);
              $temp_slos = array_unique($temp_slos);
              $tag_count = count($temp_slos);

              if ($tag_count > 0) {
                $tag_array[$current_tag]['tag_id'] = $current_tag;
                $tag_array[$current_tag]['count']  = $tag_count;
                $tag_array[$current_tag]['label']  = $label_index[$current_tag];

                // Store the min and max tag count to
                // help calculate the tag cloud text size.
                if (!$min) {
                  $min = $max = $tag_count;
                }
                elseif ($tag_count < $min) {
                  $min = $tag_count;
                }
                elseif ($tag_count > $max) {
                  $max = $tag_count;
                }

              }

            }

            // Create the tag cloud
            if ($tag_array) {

              foreach ($tag_array as $value) {

                // Make sure we don't divide by 0
                if ($max == $min && $max > 1) {
                  $min = $max-1;
                }
                elseif ($max == $min && $max == 1) {
                  $max++;
                }

                $weight = (log($value['count'])-log($min))/(log($max)-log($min));

                switch ($weight) {
                  case ($weight == 0):
                    $font_size = 8 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0 && $weight <= 0.4):
                    $font_size = 15 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0.4 && $weight <= 0.6):
                    $font_size = 23 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0.6 && $weight <= 0.8):
                    $font_size = 30 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                  case ($weight > 0.8 && $weight <= 1.0):
                    $font_size = 38 + ($page_settings['tag cloud max font size'] * 2);
                    $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                    $big_end = '</div>';
                    break;
                }

                if ($value['count'] > 0) {

                  $par_count = '<small><small>(' . $value['count'] . ')</small></small>';

                  $cloud_url = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[2] . '_' . $value['tag_id'] . '/' . $sort_type;

                  $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . '' . $par_count . ' &nbsp;';

                }

              }

            }

          }

        }
        elseif (count($tags) == 4) {

          // Remove 1st tag.
          $url1 = $page_url . '/selected/' . $tags[1] . '_' . $tags[2] . '_' . $tags[3] . '/' . $sort_type;
          $selected_tags = '<a href="' . $url1 . '">#' . $label_index[$tags[0]] . '</a>';

          // Remove 2nd tag.
          $url2 = $page_url . '/selected/' . $tags[0] . '_' . $tags[2] . '_' . $tags[3] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url2 . '">#' . $label_index[$tags[1]] . '</a>';

          // Remove 3rd tag.
          $url3 = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[3] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url3 . '">#' . $label_index[$tags[2]] . '</a>';

          // Remove 4th tag.
          $url4 = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[2] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url4 . '">#' . $label_index[$tags[3]] . '</a>';

          // Deselect all tags.
          $selected_tags .= '<br /><b><a href="' . $page_url . '/' . $sort_type . '">' . t('deselect all') . '</a></b>';

          // Select associated SLOs.
          $result = db_query("SELECT slo_id
                                FROM {dae_slo_tag}
                               WHERE tag_id=%d OR tag_id=%d OR tag_id=%d OR tag_id=%d
                            ORDER BY slo_id", $tags[0], $tags[1], $tags[2], $tags[3]);

          while ($row = db_fetch_array($result)) {
            $slo_array[] = $row['slo_id'];
          }

          $slo_array = array_unique($slo_array);

          $slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'slo_id=%d'));

          $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE " . $slo_placeholders . " ORDER BY tag_id", $slo_array);
          while ($row = db_fetch_array($result)) {

            // Make sure not to add the selected tags.
            if (!in_array($row['tag_id'], $tags)) {
              $result_tag[] = $row['tag_id'];
            }

          }

          if ($result_tag) {

            $tag_placeholders = implode(' OR ', array_fill(0, count($result_tag), 'id=%d'));

            $result = db_query("SELECT id FROM {dae_tag} WHERE " . $tag_placeholders . " ORDER BY tag_label", $result_tag);
            while ($row = db_fetch_array($result)) {
              $ordered_tags[] = $row['id'];
            }

            foreach ($ordered_tags as $current_tag) {

              $slo_array1 = array(); $slo_array2 = array(); $slo_array3 = array();
              $slo_array4 = array(); $slo_array5 = array();

              // Calculate the total number of
              // SLO's associated with Tag 1.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[0]);
              while ($row = db_fetch_array($result)) {
                $slo_array1[] = $row['slo_id'];
              }

              // Calculate the total number of
              // SLO's associated with Tag 2.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[1]);
              while ($row = db_fetch_array($result)) {
                $slo_array2[] = $row['slo_id'];
              }

              // Calculate the total number of
              // SLO's associated with Tag 3.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[2]);
              while ($row = db_fetch_array($result)) {
                $slo_array3[] = $row['slo_id'];
              }

              // Calculate the total number of
              // SLO's associated with Tag 4.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[3]);
              while ($row = db_fetch_array($result)) {
                $slo_array4[] = $row['slo_id'];
              }

              // Calculate the total number of SLO's
              // associated with the current tag.
              $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $current_tag);
              while ($row = db_fetch_array($result)) {
                $slo_array5[] = $row['slo_id'];
              }

              // Combine each lists make sure there are no duplicate
              // entries and the number of matching items is the count.
              $temp_slos = array_intersect($slo_array1, $slo_array2);
              $temp_slos = array_intersect($slo_array3, $temp_slos);
              $temp_slos = array_intersect($slo_array4, $temp_slos);
              $temp_slos = array_intersect($slo_array5, $temp_slos);
              $temp_slos = array_unique($temp_slos);
              $tag_count = count($temp_slos);

              if ($tag_count > 0) {

                $tag_array[$current_tag]['tag_id'] = $current_tag;
                $tag_array[$current_tag]['count']  = $tag_count;
                $tag_array[$current_tag]['label']  = $label_index[$current_tag];

              }

            }

            if ($tag_array) {

              foreach ($tag_array as $value) {

                // If there are any tag cloud values at this point just make
                // the tag size 15 and call it a !&^#%@ #&&)(#* day!
                $font_size = 15 + ($page_settings['tag cloud max font size'] * 2);
                $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                $big_end = '</div>';

                $cloud_url = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[2] . '_' . $tags[3] . '_' . $value['tag_id'] . '/' . $sort_type;

                $cloud_string .= '&nbsp;' . $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . ' <small>(' . $value['count'] . ')</small>&nbsp;';

              }

            }

          }

        }
        elseif (count($tags) == 5) {

          // Remove 1st tag.
          $url1 = $page_url . '/selected/' . $tags[1] . '_' . $tags[2] . '_' . $tags[3] . '_' . $tags[4] . '/' . $sort_type;
          $selected_tags = '<a href="' . $url1 . '">#' . $label_index[$tags[0]] . '</a>';

          // Remove 2nd tag.
          $url2 = $page_url . '/selected/' . $tags[0] . '_' . $tags[2] . '_' . $tags[3] . '_' . $tags[4] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url2 . '">#' . $label_index[$tags[1]] . '</a>';

          // Remove 3rd tag.
          $url3 = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[3] . '_' . $tags[4] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url3 . '">#' . $label_index[$tags[2]] . '</a>';

          // Remove 4th tag.
          $url4 = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[2] . '_' . $tags[4] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url4 . '">#' . $label_index[$tags[3]] . '</a>';

          // Remove 5th tag.
          $url5 = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[2] . '_' . $tags[3] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url5 . '">#' . $label_index[$tags[4]] . '</a>';

          // Deselect all tags.
          $selected_tags .= '<br /><b><a href="' . $page_url . '/' . $sort_type . '">' . t('deselect all') . '</a></b>';

        }

      }

    }

    if ($cloud_string) {
      // Add the line height daedalus setting here since the value
      // (as far as I know) can not be retrieved within a css file.
      $cloud_display = '<div class="cloud" style="line-height: ' . $page_settings['tag cloud height percent'] . '%;">' . $cloud_string . '</div>';
    }
    else {
      $cloud_display = '<div class="cloud"><div class="cloud-item" style="font-size: 13px;"><b>' . t('#No matches found') . '</b></div></div>';
    }

    // Display buttons according to the sort order,
    // if the user is Admin, Builder or Root.
    if ($build_access) {

      if ($param[0] == 'alpha' || $param[2] == 'alpha') {

        $form['browse-rank'] = array(
          '#type' => 'submit',
          '#value' => t('Order by rank')
        );

      }
      else {

        $form['browse-alpha'] = array(
          '#type' => 'submit',
          '#value' => t('Order alphabetically')
        );

      }

    }

    // Some debugging information for adjusting the appearance of
    // the tag cloud form of buttons. No setting, just change it
    // to true when you want to change the database values based
    // on visual cues instead of guessing the numbers.
    if ($page_settings['show tag cloud settings'] && $build_access) {

      $form['increase-percent'] = array(
        '#type' => 'submit',
        '#value' => t('Increase spacing'),
      );

      $form['decrease-percent'] = array(
        '#type' => 'submit',
        '#value' => t('Decrease spacing'),
      );

      $form['increase-max'] = array(
        '#type' => 'submit',
        '#value' => t('Increase font'),
      );

      $form['decrease-max'] = array(
        '#type' => 'submit',
        '#value' => t('Decrease font'),
      );

    }

    $form['tags'] = array(
      '#type' => 'item',
      '#title' => t('Tags to narrow search ') . '<small>(' . t('tags which would result in an empty result are excluded from display') . ')</small>',
      '#value' => $cloud_display,
      '#suffix' => '<br />',
    );

    if ($slo_id == 'selected') {

      $form['selected'] = array(
        '#title' => t('Selected tags'),
        '#value' => $selected_tags,
        '#type'  => 'item',
      );

    }

    if ($tags) {

      if (is_array($tags)) {

        // Combine the arrays according to the amount of tags selected
        if (count($tags) == 2) {

          // Get the slo ids using the 1st tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[0]);
          while ($row = db_fetch_array($result)) {
            $slo_array1[] = $row['slo_id'];
          }

          // Get the slo ids using the 2nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[1]);
          while ($row = db_fetch_array($result)) {
            $slo_array2[] = $row['slo_id'];
          }

          $slo_array = array_intersect($slo_array1, $slo_array2);

        }
        elseif (count($tags) == 3) {

          // Get the slo ids using the 1st tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[0]);
          while ($row = db_fetch_array($result)) {
            $slo_array1[] = $row['slo_id'];
          }

          // Get the slo ids using the 2nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[1]);
          while ($row = db_fetch_array($result)) {
            $slo_array2[] = $row['slo_id'];
          }

          // Get the slo ids using the 3nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[2]);
          while ($row = db_fetch_array($result)) {
            $slo_array3[] = $row['slo_id'];
          }

          $slo_array = array_intersect($slo_array1, $slo_array2);
          $slo_array = array_intersect($slo_array3, $slo_array);

        }
        elseif (count($tags) == 4) {

          // Get the slo ids using the 1st tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[0]);
          while ($row = db_fetch_array($result)) {
            $slo_array1[] = $row['slo_id'];
          }

          // Get the slo ids using the 2nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[1]);
          while ($row = db_fetch_array($result)) {
            $slo_array2[] = $row['slo_id'];
          }

          // Get the slo ids using the 3nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[2]);
          while ($row = db_fetch_array($result)) {
            $slo_array3[] = $row['slo_id'];
          }

          // Get the slo ids using the 4nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[3]);
          while ($row = db_fetch_array($result)) {
            $slo_array4[] = $row['slo_id'];
          }

          $slo_array = array_intersect($slo_array1, $slo_array2);
          $slo_array = array_intersect($slo_array3, $slo_array);
          $slo_array = array_intersect($slo_array4, $slo_array);

        }
        elseif (count($tags) == 5) {

          // Get the slo ids using the 1st tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[0]);
          while ($row = db_fetch_array($result)) {
            $slo_array1[] = $row['slo_id'];
          }

          // Get the slo ids using the 2nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[1]);
          while ($row = db_fetch_array($result)) {
            $slo_array2[] = $row['slo_id'];
          }

          // Get the slo ids using the 3nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[2]);
          while ($row = db_fetch_array($result)) {
            $slo_array3[] = $row['slo_id'];
          }

          // Get the slo ids using the 4nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[3]);
          while ($row = db_fetch_array($result)) {
            $slo_array4[] = $row['slo_id'];
          }

          // Get the slo ids using the 4nd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[4]);
          while ($row = db_fetch_array($result)) {
            $slo_array5[] = $row['slo_id'];
          }

          $slo_array = array_intersect($slo_array1, $slo_array2);
          $slo_array = array_intersect($slo_array3, $slo_array);
          $slo_array = array_intersect($slo_array4, $slo_array);
          $slo_array = array_intersect($slo_array5, $slo_array);

        }

        // Get the SLO's from the selected array
        if ($sort_type == 'alpha') {

          $placeholders = implode(' OR ', array_fill(0, count($slo_array), 'id=%d'));

          $result = db_query("SELECT * FROM {dae_slo} WHERE $placeholders ORDER BY slo_text ASC", $slo_array);
          while ($row = db_fetch_array($result)) {

            $url = $base_url . '/' . $slo_url . '/' . $row['id'];

            if ($build_access) {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
            }

            else {

              // If a goal SLO show the goal image
              if ($goal_slos && in_array($row['id'], $goal_slos)) {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . ' ' . $goal_img . '</a></li>';
              }

              else {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
              }

            }

          }

        }

        else {

          $placeholders = implode(' OR ', array_fill(0, count($slo_array), 'id=%d'));

          $result = db_query("SELECT * FROM {dae_slo} WHERE " . $placeholders . " ORDER BY slo_rank ASC, slo_text ASC", $slo_array);
          while ($row = db_fetch_array($result)) {

            $url = $base_url . '/' . $slo_url . '/' . $row['id'];

            if ($build_access) {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a> <small>(' . $row['slo_rank'] . ')</small></li>';
            }

            else {

              // If a goal SLO show the goal image
              if ($goal_slos && in_array($row['id'], $goal_slos)) {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . ' ' . $goal_img . '</a></li>';
              }

              else {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
              }

            }

          }

        }

      }

      else {

        // Use the $slo array created earlier
        if ($sort_type == 'alpha') {

          $placeholders = implode(' OR ', array_fill(0, count($slo_array), 'id=%d'));

          $result = db_query("SELECT * FROM {dae_slo} WHERE $placeholders ORDER BY slo_text ASC", $slo_array);
          while ($row = db_fetch_array($result)) {

            $url = $base_url . '/' . $slo_url . '/' . $row['id'];

            if ($build_access) {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
            }

            else {

              // If a goal SLO show the goal image
              if ($goal_slos && in_array($row['id'], $goal_slos)) {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . ' ' . $goal_img . '</a></li>';
              }

              else {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
              }

            }

          }

        }

        else {

          $placeholders = implode(' OR ', array_fill(0, count($slo_array), 'id=%d'));

          $result = db_query("SELECT * FROM {dae_slo} WHERE $placeholders ORDER BY slo_rank ASC, slo_text ASC", $slo_array);
          while ($row = db_fetch_array($result)) {

            $url = $base_url . '/' . $slo_url . '/' . $row['id'];

            if ($build_access) {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a> <small>(' . $row['slo_rank'] . ')</small></li>';
            }

            else {

              // If a goal SLO show the goal image
              if ($goal_slos && in_array($row['id'], $goal_slos)) {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . ' ' . $goal_img . '</a></li>';
              }

              else {
                $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
              }

            }

          }

        }

      }

    }
    else {

      // List each SLO either by
      // rank or alphabetically.
      if ($slo_id == 'alpha') {

        $result = db_query("SELECT * FROM {dae_slo} ORDER BY slo_text");
        while ($row = db_fetch_array($result)) {

          $url = $base_url . '/' . $slo_url . '/' . $row['id'];

          if ($build_access) {
            $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
          }
          else {

            // If a goal SLO show the goal image
            if ($goal_slos && in_array($row['id'], $goal_slos)) {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . ' ' . $goal_img . '</a></li>';
            }
            else {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
            }

          }

        }

      }

      else {

        $result = db_query("SELECT * FROM {dae_slo} ORDER BY slo_rank ASC, slo_text ASC");
        while ($row = db_fetch_array($result)) {

          $url = $base_url . '/' . $slo_url . '/' . $row['id'];

          if ($build_access) {
            $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a> <small>(' . $row['slo_rank'] . ')</small></li>';
          }
          else {

            // If a goal SLO show the goal image
            if ($goal_slos && in_array($row['id'], $goal_slos)) {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . ' ' . $goal_img . '</a></li>';
            }
            else {
              $selected_slos .= '<li><a href="' . $url . '">' . $row['slo_text'] . '</a></li>';
            }

          }

        }

      }

    }

    // Display the SLOs
    $form['matches'] = array(
      '#value' => '<ul>' . $selected_slos . '</ul>',
      '#type'  => 'item',
    );

  }

  // Submit hidden information to pass
  // to the validate and submit hooks.
  $form['pass-browse-learning-outcomes'] = array( '#type' => 'value', '#value' => $page_settings['browse learning outcomes'], );
  $form['pass-tag-cloud-max-font-size']  = array( '#type' => 'value', '#value' => $page_settings['tag cloud max font size'], );
  $form['pass-tag-cloud-height-percent'] = array( '#type' => 'value', '#value' => $page_settings['tag cloud height percent'], );

  return $form;

}


/**
 * Implementation of hook_submit().
 */
function daedalus_browse_outcomes_form_submit($form, &$form_state) {

  global $base_url;

  // Get some of the page information.
  $page_url = $form_state['values']['pass-browse-learning-outcomes'];
  $page_url_length = sizeof(explode('/', $page_url));

  $tag_option1 = arg(0+$page_url_length);
  $tag_option2 = arg(1+$page_url_length);
  $tag_option3 = arg(2+$page_url_length);

  $max_font_size = $form_state['values']['pass-tag-cloud-max-font-size'];
  $tag_height = $form_state['values']['pass-tag-cloud-height-percent'];

  switch ($form_state['values']['op']) {

      case $form_state['values']['browse-alpha']:

        if ($tag_option1) {
          drupal_goto($page_url . '/' . $tag_option1 . '/' . $tag_option2 . '/alpha');
        }
        else {
          drupal_goto($page_url . '/alpha');
        }

        break;

      case $form_state['values']['browse-rank']:

        if ($tag_option3 == 'alpha') {
          drupal_goto($page_url . '/' . $tag_option1 . '/' . $tag_option2);
        }
        else {
          drupal_goto($page_url);
        }

        break;

      // Buttons for changing the settings. They just change some db values.
      // "increase-percent", "decrease-percent", "increase-max", "decrease-max"
      case $form_state['values']['increase-percent']:

        $value = $tag_height + 20;
        db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud height percent'", $value);

        break;

      case $form_state['values']['decrease-percent']:

        if ($tag_height > 40 ) {
            $value = $tag_height - 20;
        }
        else {
            $value = $tag_height - 10;
        }

        db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud height percent'", $value);

        break;

      case $form_state['values']['increase-max']:

        $value = $max_font_size + 1;

        db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud max font size'", $value);

        break;

      case $form_state['values']['decrease-max']:

        if ($max_font_size >= 1) {

          $value = $max_font_size - 1;

          db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud max font size'", $value);

        }
        else {
          drupal_set_message(t('Font size is currently set to its smallest value.'), 'warning');
        }

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

          drupal_set_message(t('Help information saved.'));

          if ($tag_option1 && $tag_option2) {
            drupal_goto($base_url . '/' . $page_url . '/' . $tag_option1 . '/' . $tag_option2);
          }
          elseif ($tag_option1) {
            drupal_goto($base_url . '/' . $page_url . '/' . $tag_option1);
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