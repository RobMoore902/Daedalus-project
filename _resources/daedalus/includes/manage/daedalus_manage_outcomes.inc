<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage->Learning Outcomes Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_outcomes_page() {
  return drupal_get_form('daedalus_manage_outcomes_form');
}


/**
 * Menu Location: Daedalus -> Manage -> Learning Outcomes
 * URL Location:  daedalus/manage/slo
 *
 * Displays: The tag cloud and list of SLOs.
 */
function daedalus_manage_outcomes_form($form) {

  // When loading the page after a failed submit, a user should
  // not have to reload all the textfields they filled in. This
  // checks the post data values and adds a boolean at index to
  // tell the autocomplete to hide the value or not.

  $show_prereq = array(); $show_covering = array();
  foreach ($form['post'] as $key => $value) {

    if (strpos($key, 'prereq') !== FALSE) {

      if (trim($value) != '') {
        $show_prereq[] = TRUE;
      }
      else {
        $show_prereq[] = FALSE;
      }

    }
    elseif (strpos($key, 'covering') !== FALSE) {

      if (trim($value) != '') {
        $show_covering[] = TRUE;
      }
      else {
        $show_covering[] = FALSE;
      }

    }

  }

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='checkmark' OR
                             setting='question mark' OR
                             setting='question mark2' OR
                             setting='exclamation mark' OR
                             setting='title each autocomplete textfield' OR
                             setting='autocomplete iteration' OR
                             setting='manage courses' OR
                             setting='manage learning outcomes' OR
                             setting='learning outcome rank iteration' OR
                             setting='tag cloud max font size' OR
                             setting='tag cloud height percent' OR
                             setting='show tag cloud settings' OR
                             setting='tag seperate character' OR
                             setting='manage term reviews'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  // Get the outcome information once, so there
  // is no need for multiple database accesses
  // throughout the script.
  $page_outcomes = array();
  $result = db_query("SELECT * FROM {dae_slo}");
  while ($row = db_fetch_array($result)) {
    $page_outcomes[$row['id']]['slo_text'] = $row['slo_text'];
    $page_outcomes[$row['id']]['slo_rank'] = $row['slo_rank'];
  }

  global $base_url;

  // Get the images
  $check_src = $base_url . '/' . $page_settings['checkmark'];
  $question2_src = $base_url . '/' . $page_settings['question mark2'];
  $exclamation_src = $base_url . '/' . $page_settings['exclamation mark'];
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  // URL Information
  $page_url = $help_url = $page_settings['manage learning outcomes'];
  $course_url = $base_url . '/' . $page_settings['manage courses'];

  // URL Parameters
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);
  $param[2] = arg(2+$page_url_length);
  $param[3] = arg(3+$page_url_length);

  // Name some of the parameters
  // to help with code clarity..
  $selected_slo_id = $param[0];
  $tags = $param[1];
  $sort_type = $param[2];

  drupal_set_title(t('Manage Learning Outcomes !help', array('!help' => $show_help)));

  // Get access information
  $build_access = user_access('daedalus build');

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($help_url, 1, 1);

  // Only execute code if magellan database tables are installed.
  // This will popup a session timeout warning if the user is a
  // Magellan advisor and they are in session
  if (module_exists('magellan')) {

    // Determine the user id
    global $user;
    $user_id = $user->uid;

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
        $session_time = db_result(db_query("SELECT session_time
                                              FROM {mag_session_log}
                                             WHERE advisor_id=%d
                                               AND session_id='%s'", $user_id, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse_learning_outcomes', 'advisor');

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
        $session_time = db_result(db_query("SELECT session_time FROM {mag_session_log} WHERE support_id=%d AND session_id='%s'", $user_id, $session_id ));

        // Add the session timeout warning.
        $form[] = daedalus_session_timeout_warning($session_time, $add_time, 'browse_learning_outcomes', 'support');

      }

    }

  }

  // If the url says to delete something, see what to delete,
  // inform user, and delete appropriate relationship
  if ($param[1] == 'delete') {

    if (is_numeric($param[3]) || $param[2] == 'this') {

      switch ($param[2]) {

        case 'covering':

          db_query("DELETE FROM {dae_course_slo} WHERE course_id=%d and slo_id=%d", $param[3], $selected_slo_id);

          break;

        case 'prereq':

          db_query("DELETE FROM {dae_prereq_slo} WHERE target=%d and pre_slo=%d", $selected_slo_id, $param[3]);

          // Calculate new slo rank after removal
          daedalus_calculate_slo_rank();

          break;

        case 'tag':

          db_query("DELETE FROM {dae_slo_tag} WHERE slo_id=%d and tag_id=%d", $selected_slo_id, $param[3]);

          break;

        case 'this':

          // Must give a confirmation before deleting a learning outcome
          drupal_set_title(t('Delete Confirmation !help', array('!help' => $show_help)));

          $this_message  = '<br />' . t('Are you sure you want to delete the learning outcome');
          $this_message .= ' <strong><u>' . $page_outcomes[$selected_slo_id]['slo_text'] . '</u></strong>? ' . t('This can not be undone.');
          $this_message .= '<br /><br /><ul>';
          $this_message .= '<li>' . t('This will delete all references to prerequisite learning outcomes.') . '</li>';
          $this_message .= '<li>' . t('This will delete all references to any associated courses..') . '</li>';
          $this_message .= '</ul><br />' . t('Are you sure you want to continue?');

          $form[] = array(
            '#value' => $this_message,
            '#type' => 'item',
          );

          $form['delete-forward'] = array(
            '#type' => 'submit',
            '#value' => t('I understand the consequences. Delete this learning outcome forever.'),
          );

          $form['delete-reverse'] = array(
            '#type' => 'submit',
            '#value' => t('On second thought, I better not. Take me back'),
          );

          break;

      }

    }
    else {
      drupal_set_message(t('An error has occurred. Please contact a system administrator'), 'error');
    }

  }

  elseif ($param[0] == 'rank') {

    /**
     * Dr. Blouin's Sudo Code
     *
     * Sorting Slo (cron job)
     *
     * 1. Reset all ranks to -1 (or NULL)
     *    x <- SELECT ALL SLOs of rank 0
     *    r <- 0
     *    while x != 0
     *      x <- All SLOs which rank == NULL ^ has 1+ prerequisites of rank r
     *      Assign rank r+1 to all SLOs E(subset) x
     *      r <- r+1
     *
     * Quick & Dirty rank assignment of SLOs (real time)
     * Caution: A SLO x is updated/created with 1+ prerequisite
     *    r <- min(union of ranks of all prerequiste of x)
     *    Assign x the rank of 1
     *
     * [optimally recursivly apply Q+D reranking to all post-requisite of x and get rid of the cron job.]
     *
     * Q. Can php make system calls or spawn new processes?
     *
     */

    drupal_set_title(t('Rank Calculation !help', array('!help' => $show_help)));

    $rank_depth = $page_settings['learning outcome rank iteration'];

    $form[] = array(
      '#type' => 'item',
      '#title' => t('If marked as valid the learning outcome (preslo) has an assigned prerequisite student learing outcome'),
      '#description' => t('Valid learning outcomes are given the rank of the iteration + 1'),
    );

    $form['new-depth'] = array(
      '#title' => t('Set new rank iteration depth'),
      '#type' => 'select',
      '#options' => array(
          '5' => t('5'),
          '6' => t('6'),
          '7' => t('7'),
          '8' => t('8'),
          '9' => t('9'),
          '10' => t('10'),
          '11' => t('11'),
          '12' => t('12'),
          '13' => t('13'),
          '14' => t('14'),
          '15' => t('15'),
          '16' => t('16'),
          '17' => t('17'),
          '18' => t('18'),
          '19' => t('19'),
          '20' => t('20'),
          '21' => t('21'),
          '22' => t('22'),
          '23' => t('23'),
          '24' => t('24'),
          '25' => t('25'),
      ),
      '#default_value' => $rank_depth,
    );

    $form['submit-depth'] = array(
      '#type' => 'submit',
      '#value' => t('Submit rank depth'),
    );

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
      // and make the query to set each slo to a rank of zero.
      $rank_zero = array_diff($slo_ids, $preslo_ids);

      // Create the placeholders for each id that will be updated.
      $query_placeholders = implode(' OR ', array_fill(0, count($rank_zero), 'id=%d'));

      db_query("UPDATE {dae_slo} SET slo_rank=0 WHERE " . $query_placeholders, $rank_zero);

      // I think this is a slight variation to Christians sudo code. Here the list of all SLO's
      // that have a prerequisite SLO are iterated checking to see if the have a prerequisite SLO
      // at the current iteration depth. If a prereq SLO is found the current SLO is given a rank
      // of the iteration plus 1.
      for ($i=0; $i <= $rank_depth; $i++) {

        $title = 'Iteration - ' . $i;

        $values = '';

        foreach ($preslo_ids as $id) {

          $values .= '<li>ID: ' . $id . ' Preslos: ';

          $result = db_query("SELECT * FROM {dae_prereq_slo} WHERE target=%d", $id);
          while ($row = db_fetch_array($result)) {

            $values .= $row['pre_slo'] . ', ';

            if (db_result(db_query("SELECT COUNT(*) FROM {dae_slo} WHERE slo_rank=%d AND id=%d", $i, $row['pre_slo']))) {

              $values .= ' <-- VALID</li>';

              $valid = TRUE;

              db_query("UPDATE {dae_slo} SET slo_rank=%d WHERE id=%d", ($i+1), $id);

              break;

            }

          }

          if (!$valid) {
            $values .= '</li>';
          }
          else {
            $valid = FALSE;
          }

        }

        $form[] = array(
            '#title' => check_plain($title),
            '#type' => 'item',
            '#value' => '<ul>' . $values . '</ul>',
        );

      }

    }
    else {

      // Set all SLO ranks to 0
      db_query("UPDATE {dae_slo} SET slo_rank=0");

    }

  }

  elseif ($param[0] == 'tag' || $param[0] == 'split' || $param[0] == 'merge' || $param[0] == 'delete' || $param[0] == 'remove') {

    switch ($param[0]) {

      // Tag the outcomes selected
      // from the tag cloud.
      case 'tag':

        // Include the pages JavaScript file.
        drupal_add_js(drupal_get_path('module', 'daedalus') . '/javascript/daedalus_manage_outcome_tags.js');

        drupal_set_title(t('Tag Learning Outcomes !help', array('!help' => $show_help)));

        $slos_to_tag =  explode('_', $param[1]);

        natcasesort($slos_to_tag);

        foreach ($slos_to_tag as $v) {
          $selected .= '<li>' . $page_outcomes[$v]['slo_text'] . '</li>';
        }

        $form[] = array(
          '#type' => 'item',
          '#title' => t('Selected Learning Outcomes'),
          '#value' => '<ul>' . $selected . '</ul>',
        );

        $form['tags-to-add'] = array(
          '#type' => 'textfield',
          '#title' => t('Add Tags'),
          '#autocomplete_path' => 'autocomp/tag',
          '#description' => '[<a class="show-tag-help">' . t('info') . '</a>]',
        );

        $form['tags-add-forward'] = array(
          '#type' => 'submit',
          '#value' => t('Tag'),
        );

        break;

      // Split the outcomes selected
      // from the tag cloud.
      case 'split':

        drupal_set_title(t('Splitting learning outcome !help', array('!help' => $show_help)));

        // Both of these are required. But the required field isn't being used,
        // because it's not nescessary for them to have values if the user is
        // switching the operation they're doing.
        $form['split-a'] = array(
          '#type' => 'textfield',
          '#title' => t('New Learning Outcome #1'),
          '#required' => TRUE,
        );
        $form['split-b'] = array(
          '#type' => 'textfield',
          '#title' => t('New Learning Outcome #2'),
          '#required' => TRUE,
          '#description' => t('You may create two new learning outcomes or select the original learning outcome and one created outcome.'),
        );
        $form['button-split-forward'] = array(
          '#type' => 'submit',
          '#value' => t('Split'),
        );

        break;

      // Merge the outcomes selected
      // from the tag cloud.
      case 'merge':

        drupal_set_title(t('Merging 2 learning outcomes !help', array('!help' => $show_help)));

        $merging_tags = explode('_', $param[1]);

        if (count($merging_tags) == 2) {

          $merge_message = t('Merging the following learning outcomes:') . '<ul>';
          $merge_message .= '<li>' . $page_outcomes[$merging_tags[0]]['slo_text'] . '</li>';
          $merge_message .= '<li>' . $page_outcomes[$merging_tags[1]]['slo_text'] . '</li></ul>';

          $form[] = array(
            '#type' => 'item',
            '#value' => $merge_message,
          );

          $form['new-merged'] = array(
            '#title' => t('Enter new learning outcome'),
            '#type' => 'textfield',
            '#autocomplete_path' => 'autocomp/slo',
            '#required' => TRUE,
            '#description' => t('You may enter an entirly new learning outcome or select one of the two being merged.'),
          );

          $form['button-merge-forward'] = array(
            '#type' => 'submit',
            '#value' => t('Merge'),
          );

        }
        else{
          drupal_set_message(t('An error occured, please contact a system administrator.'), 'error');
        }

        break;

      // Delete selected outcomes
      // from the tag cloud.
      case 'delete':

        drupal_set_title(t('Delete Learning Outcomes !help', array('!help' => $show_help)));

        $deleting = array();

        $slo_ids = explode('_', $param[1]);

        foreach ($slo_ids as $id) {
          $deleting[] = $page_outcomes[$id]['slo_text'];
        }

        if ($deleting) {

          natcasesort($deleting);

          $delete_message = t('Are you sure you want to delete the following learning outcomes?') . '<ul>';

          foreach ($deleting as $slo_text) {
            $delete_message .= '<li>' . $slo_text . '</li>';
          }

          $delete_message .= '</ul><strong><b><u>' . t('This can not be undone.') . '</u></b></strong>';

          $form[] = array(
            '#type' => 'item',
            '#value' => $delete_message,
          );

          $form['button-delete-forward'] = array(
            '#type' => 'submit',
            '#value' => t('Delete'),
          );

        }

        break;

      // Remove multiple tags from SLOs
      // selected from the tag cloud.
      case 'remove':

        drupal_set_title(t('Remove Multiple Tags !help', array('!help' => $show_help)));

        $slo_ids = explode('_', $param[1]);
        $tag_ids = explode('_', $param[2]);

        if (count($tag_ids) > 1) {
          $tag_plural = "'s";
        }

        if (count($slo_ids) > 1) {
          $slo_plural = "'s";
        }

        foreach ($tag_ids as $id) {
          $tag_list .= '"' . db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id=%d", $id)) . '" ';
        }

        $remove_message = t('Are you sure you want to remove the selected tag!plural ',
                array('!plural' => $tag_plural));

        $remove_message .= '<b>' . $tag_list . '</b>' . t(' from the following learning outcome!plural?.',
                array('!plural' => $slo_plural)) . '<ul>';

        foreach ($slo_ids as $sid) {
          $remove_message .= '<li>' . $page_outcomes[$sid]['slo_text'] . '</li>';
        }

        $remove_message .= '</ul><strong><b><u>' . t('This can not be undone.') . '</u></b></strong>';

        $form[] = array(
          '#type' => 'item',
          '#value' => $remove_message,
        );

        $form['button-remove-multiple-forward'] = array(
          '#type' => 'submit',
          '#value' => t('Remove Multiple Tags'),
        );

        break;

    }

    // Add cancel button using the
    // URL parameter as the lablel.
    if (!is_numeric($param[0]) && $param[0] != 'selected') {

      $form[] = array(
        '#type' => 'item',
        '#value' => '<small><a href="' . $page_url . '">' . t('cancel !p0', array('!p0' => $param[0])) . '</a></small><br/><br/><hr><br/>',
      );

    }

  }

  elseif ($param[0] == 'courses') {

    // Display autocomplete course box to submit for slo display
    if (!$param[1]) {

      // Based on the settings make a textfield
      $form['course-slos'] = array(
        '#type' => 'textfield',
        '#title' => t('View learning outcomes by selecting a course'),
        '#autocomplete_path' => 'autocomp/course',
      );

      $form['view-by-course'] = array(
        '#type'  => 'submit',
        '#value' => t('Submit course'),
      );

      $form['view-by-tags'] = array(
        '#type'  => 'submit',
        '#value' => t('Cancel'),
      );

      $form[] = array(
        '#type' => 'item',
        '#value' => '<br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />',
      );

    }
    else {

        ///////////////////////////////////////////////////////////////////////////////////
       ///////////////////////////////////////////////////////////////////////////////////
      // View By Course /////////////////////////////////////////////////////////////////
      $selected_course_info = db_fetch_array(db_query("SELECT id, course_name FROM {dae_course} WHERE course='%s'", $param[1]));
      $selected_course_name = $selected_course_info['course_name'];
      $selected_course_id = $selected_course_info['id'];

      $form['view-by-tags'] = array(
        '#type'  => 'submit',
        '#value' => t('View by tags'),
      );

      $form['course-display'] = array(
        '#type'  => 'item',
        '#title' => t('Selected course'),
        '#value' => $param[1] . ' - ' . $selected_course_name,
      );

      $selected_slos = array();

      $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d", $selected_course_id);
      while ($row = db_fetch_array($result)) {
        $selected_slos[$row['slo_id']] = $page_outcomes[$row['slo_id']]['slo_text'];
      }

      if ($selected_slos) {

        asort($selected_slos);

        $form['checkboxes'] = array(
          '#type' => 'checkboxes',
          '#options' => $selected_slos,
        );

        $form['button-merge-top'] = array(
          '#type' => 'submit',
          '#value' => t('Merge'),
        );
        $form['button-delete-top'] = array(
          '#type' => 'submit',
          '#value' => t('Delete'),
        );
        $form['button-split-top'] = array(
          '#type' => 'submit',
          '#value' => t('Split'),
        );
        $form['button-tag-top'] = array(
          '#type' => 'submit',
          '#value' => t('Tag'),
        );

      }
      else {

        $form['view-by-course'] = array(
          '#type'  => 'submit',
          '#value' => t('View by course'),
          '#weight' => -10,
        );

        $form['tags'] = array(
            '#value' => '<ul><li><i>' . t('No matches found') . '</i></li></ul>',
            '#type'  => 'item',
        );

      }

    }

  }

    ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
  //////                             /////////////////////////////////////////
 //////    TAG CLOUD                /////////////////////////////////////////
//////                             /////////////////////////////////////////
 ///// If the passed learning outcome isn't an id, then change it.
  //// Here the tag cloud is created and the selected tag string.
  elseif (!is_numeric($param[0]) && $build_access) {

    $i = 0; $cloud_string = '';

    // If a tag is not selected display the entire tag cloud
    if ($param[0] != 'selected') {

      $tag_array = array();
      $min = -1; $max = -1;

      // Create the tag array
      $result = db_query("SELECT * FROM {dae_tag} ORDER BY tag_label");
      while ($row = db_fetch_array($result)) {

        $tag_count = db_result(db_query("SELECT COUNT(*) FROM {dae_slo_tag} WHERE tag_id=%d", $row['id']));

        $tag_array[$i]['tag_id'] = $row['id'];
        $tag_array[$i]['label']  = $row['tag_label'];
        $tag_array[$i]['count']  = $tag_count;
        $i++;

        // Store the min and max tag count to
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

          if ($value['count'] == 0) {
            $cloud_string .= '<a style="color: #8B0000"><b>#' . $value['label'] . '</b></a>' . $par_count . ' &nbsp;';
          }
          else {

            if ($param[0] == 'alpha') {

              $cloud_url = $page_url . '/selected/' . $value['tag_id'] . '/' . $param[0];

              $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . '' . $par_count . ' &nbsp;';

            }
            else {

              $cloud_url = $page_url . '/selected/' . $value['tag_id'];

              $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . '' . $par_count . ' &nbsp;';

            }

          }

        }

      }

    }
    else {

      // Else a tag has been selected so narrow the tags in the
      // cloud tag accoring to the select tag. tag tag tag.

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

        // Deselect all tags
        $selected_tags = '<a href="' . $page_url . '/' . $sort_type . '">#' . $label_index[$tags] . '</a>';

        // Get all the slo id's.
        $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d ORDER BY slo_id", $tags);
        while ($row = db_fetch_array($result)) {
          $slo_array[] = $row['slo_id'];
        }

        $result_tag = array();

        // Create the query string placeholder to select the slos.
        $slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'slo_id=%d'));

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

          foreach ($ordered_tags as $current_tag) {

            $slo_array1 = array(); $slo_array2 = array();
//@todo Figure out some fancy sql to merge these queries to get the tag count
            // Calculate the total number of
            // SLO's associated the first tag.
            $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags);
            while ($row = db_fetch_array($result)) {
              $slo_array1[] = $row['slo_id'];
            }

            // Calculate the total number of SLO's
            // associated with the current tag
            $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $current_tag);
            while ($row = db_fetch_array($result)) {
              $slo_array2[] = $row['slo_id'];
            }

//$bill_count = 0;
//$result = db_query("select distinct slo_id from {dae_slo_tag} where tag_id=%d and slo_id in (SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d)", $tags, $current_tag);
//while ($row = db_fetch_array($result)) {
//  $bill_count++;
//}
            // Combine each lists make sure there are no duplicate
            // entries and the number of matching items is the count.
            $temp_slos = array_intersect($slo_array1, $slo_array2);
            $temp_slos = array_unique($temp_slos);
            $tag_count = count($temp_slos);
//echo "Current_Tag = $current_tag Tags = $tags TagCOUNT $tag_count BillCOUNT $bill_count<br />";
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

              $par_count = '<small><small>(' . $value['count'] . ')</small></small>';

              if ($value['count'] == 0) {
                $cloud_string .= '<a style="color: #8B0000"><b>#' . $value['label'] . '</b></a>' . $par_count . ' &nbsp;';
              }
              else {

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

          // Remove 2nd tag.
          $url2 = $page_url . '/selected/' . $tags[0] . '/' . $sort_type;
          $selected_tags .= '&nbsp;&nbsp;<a href="' . $url2 . '">#' . $label_index[$tags[1]] . '</a>';

          // Deselect all tags
          $selected_tags .= '<br /><b><a href="' . $page_url . '/' . $sort_type . '">' . t('deselect all') . '</a></b>';

          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d OR tag_id=%d ORDER BY slo_id", $tags[0], $tags[1]);
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

              // Calculate the total number of SLO's
              // associated with the current tag
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

                // Show the number of times the SLO has been tagged.
                $par_count = '<small><small>(' . $value['count'] . ')</small></small>';

                $cloud_url = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $value['tag_id'] . '/' . $sort_type;

                $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . '' . $par_count . ' &nbsp;';

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

          // Deselect all tags
          $selected_tags .= '<br /><b><a href="' . $page_url . '/' . $sort_type . '">' . t('deselect all') . '</a></b>';

          $result = db_query("SELECT slo_id
                                FROM {dae_slo_tag}
                               WHERE tag_id=%d OR tag_id=%d OR tag_id=%d
                            ORDER BY slo_id", $tags[0], $tags[1], $tags[2]);

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

              // Calculate the total number of SLO's
              // associated with the current tag
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

                $par_count = '<small><small>(' . $value['count'] . ')</small></small>';

                $cloud_url = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[2] . '_' . $value['tag_id'] . '/' . $sort_type;

                $cloud_string .= $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '</b></a>' . $big_end . '' . $par_count . ' &nbsp;';

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

          // Deselect all tags
          $selected_tags .= '<br /><b><a href="' . $page_url . '/' . $sort_type . '">' . t('deselect all') . '</a></b>';

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
              // associated with Tag 1. the current tag
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

            // Create the tag cloud
            if ($tag_array) {

              foreach ($tag_array as $value) {

                // If there are any tag cloud values at this point just make
                // the tag size 15 and call it a !&^#%@ #&&)(#* day!
                $font_size = 15 + ($page_settings['tag cloud max font size'] * 2);
                $big_open = '<div class="cloud-item" style="font-size: ' . $font_size . 'px;">';
                $big_end = '</div>';

                $cloud_url = $page_url . '/selected/' . $tags[0] . '_' . $tags[1] . '_' . $tags[2] . '_' . $tags[3] . '_' . $value['tag_id'] . '/' . $sort_type;

                $cloud_string .= '&nbsp;' . $big_open . '<a href="' . $cloud_url . '" ><b>#' . $value['label'] . '<b/></a>' . $big_end . ' <small>(' . $value['count'] . ')</small>&nbsp;';

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

          // Deselect all tags
          $selected_tags .= '<br /><b><a href="' . $page_url . '/' . $sort_type . '">deselect all</a></b>';

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
      $hide_info = TRUE;
    }

    // Hide buttons if no SLOs
    if (!$hide_info) {

      // Display buttons according to the sort order
      if ($build_access) {

        if ($param[0] == 'alpha' || $param[2] == 'alpha') {
          $form['order-rank'] = array(
            '#type' => 'submit',
            '#value' => t('Order by rank')
          );
        }
        else {
          $form['order-alpha'] = array(
            '#type' => 'submit',
            '#value' => t('Order alphabetically')
          );
        }

      }

      $form['view-by-course'] = array(
        '#type'  => 'submit',
        '#value' => t('View by course'),
      );

      $form['calculate-rank'] = array(
        '#type'  => 'submit',
        '#value' => t('Calculate rank'),
      );

      // Some debugging information for adjusting the appearance of
      // the tag cloud form of buttons. No setting, just change it
      // to true when you want to change the database values based
      // on visual cues instead of guessing the numbers.
      if ($page_settings['show tag cloud settings']) {

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

    }

    $form['tags'] = array(
        '#title' => t('Tags to narrow search ') . '<small>(<i>' . t('red tags are not linked to a student learning outcome') . '</i>)</small>',
        '#value' => $cloud_display,
        '#type'  => 'item',
    );

    if ($param[0] == 'selected') {
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

          // Get the slo ids using the 3rd tag
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

          // Get the slo ids using the 3rd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[2]);
          while ($row = db_fetch_array($result)) {
            $slo_array3[] = $row['slo_id'];
          }

          // Get the slo ids using the 4th tag
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

          // Get the slo ids using the 3rd tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[2]);
          while ($row = db_fetch_array($result)) {
            $slo_array3[] = $row['slo_id'];
          }

          // Get the slo ids using the 4th tag
          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d", $tags[3]);
          while ($row = db_fetch_array($result)) {
            $slo_array4[] = $row['slo_id'];
          }

          // Get the slo ids using the 5th tag
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

          foreach ($slo_array as $sid) {
            $selected_slos[$sid] = $page_outcomes[$sid]['slo_text'];
          }

          asort($selected_slos);

        }
        else {

          $slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'id=%d'));

          $result = db_query("SELECT * FROM {dae_slo} WHERE " . $slo_placeholders . " ORDER BY slo_rank ASC, slo_text ASC", $slo_array);
          while ($row = db_fetch_array($result)) {
            $selected_slos[$row['id']] = $row['slo_text'] . ' <small>(' . $row['slo_rank'] . ')</small>';
          }

        }

      }
      else {

        // Use the $slo array created earlier
        if ($sort_type == 'alpha') {

          foreach ($slo_array as $sid) {
            $selected_slos[$sid] = $page_outcomes[$sid]['slo_text'];
          }

          asort($selected_slos);

        }
        else {

          $slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'id=%d'));

          $result = db_query("SELECT * FROM {dae_slo} WHERE " . $slo_placeholders . " ORDER BY slo_rank ASC, slo_text ASC", $slo_array);
          while ($row = db_fetch_array($result)) {
            $selected_slos[$row['id']] = $row['slo_text'] . ' <small>(' . $row['slo_rank'] . ')</small>';
          }

        }

      }

    }
    else {

      // Make a list of every slo as a checkbox
      if ($param[0] == 'alpha') {

        $result = db_query("SELECT id, slo_text FROM {dae_slo} ORDER BY slo_text");
        while ($row = db_fetch_array($result)) {
          $selected_slos[$row['id']] = $row['slo_text'];
        }

      }
      else {

        $result = db_query("SELECT * FROM {dae_slo} ORDER BY slo_rank ASC, slo_text ASC");
        while ($row = db_fetch_array($result)) {
          $selected_slos[$row['id']] = $row['slo_text'] . ' <small>(' . $row['slo_rank'] . ')</small>';
        }

      }

    }

    // When all the SLOs are selected it may be a long list. Here we can
    // display the function buttons at the top as well as the bottom.
    // Only show the top select buttons if there are no tags selected or
    // there are more than 10 learning outcomes displayed.
    if (!$hide_info) {

      if (!$tags || (count($selected_slos) >= 14)) {

        $form['button-merge-bot'] = array(
          '#type' => 'submit',
          '#value' => t('Merge'),
        );

        $form['button-delete-bot'] = array(
          '#type' => 'submit',
          '#value' => t('Delete'),
        );

        $form['button-split-bot'] = array(
          '#type' => 'submit',
          '#value' => t('Split'),
        );

        $form['button-tag-bot'] = array(
          '#type' => 'submit',
          '#value' => t('Tag'),
        );

        // If a tag is selected the user may
        // now remove muliple tags from the
        // checked learning outcomes.
        if ($param[0] == 'selected') {

          $form['remove-multiple-bot'] = array(
            '#type' => 'submit',
            '#value' => t('Remove Multiple Tags'),
          );

        }

      }

    }

    $form['checkboxes'] = array(
      '#type' => 'checkboxes',
      '#options' => $selected_slos,
    );

    if ($selected_slos) {

      $form['button-merge-top'] = array(
        '#type' => 'submit',
        '#value' => t('Merge'),
      );

      $form['button-delete-top'] = array(
        '#type' => 'submit',
        '#value' => t('Delete'),
      );

      $form['button-split-top'] = array(
        '#type' => 'submit',
        '#value' => t('Split'),
      );

      $form['button-tag-top'] = array(
        '#type' => 'submit',
        '#value' => t('Tag'),
      );

      if ($param[0] == 'selected') {

        $form['remove-multiple-top'] = array(
          '#type' => 'submit',
          '#value' => t('Remove Muliple Tags'),
        );

        // Pass hidden tags to the submit function. Create
        // a list of tag ids delimited by an underscore.
        if (is_array($tags)) {
          $form['tag_list'] = array( '#type' => 'value', '#value' => implode('_', $tags) );
        }
        else {
          $form['tag_list'] = array( '#type' => 'value', '#value' => $tags );
        }

      }

    }

  }

      //////////////////////////////////////////////////////////////////////////
     //////////////////////////////////////////////////////////////////////////
    ////                                      ////////////////////////////////
   ////  MANAGE STUDENT LEARNING OUTCOMES    ////////////////////////////////
  ////                                      ////////////////////////////////
 ////
 ///  Linked from Browse Learning Outcomes, when $slo_id is numeric then
 //  everything above is bypassed and we come to this last section of code.
  elseif (is_numeric($param[0])) {

    // Include the pages JavaScript file.
    drupal_add_js(drupal_get_path('module', 'daedalus') . '/javascript/daedalus_manage_outcomes.js');

    $selected_slo_text = $page_outcomes[$selected_slo_id]['slo_text'];

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

    // Get the access arguments
    $build_outcomes_access = user_access('daedalus build learning outcomes');
    $delete_slo_access = user_access('daedalus delete slo');
    $manage_access = user_access('daedalus manage');

    // Set the title
    drupal_set_title(t('@slo !help', array('@slo' => html_entity_decode($selected_slo_text), '!help' => $show_help)));

    // Set form weights
    $weight['core']      = -10;
    $weight['suggest']   = 0;
    $weight['prereq']    = 10;
    $weight['postreq']   = 20;
    $weight['covering']  = 30;
    $weight['tags']      = 40;
    $weight['taughtrev'] = 50;
    $weight['prereqrev'] = 60;
    $weight['covercomm'] = 70;

    ////////////////////////////////////////////////////////////////////////////
    // Core Settings
    ////////////////////////////////////////////////////////////////////////////

    if ($build_outcomes_access) {

      $form['tags']['new_tags'] = array(
        '#type' => 'textfield',
        '#title' => t('Enter new tags'),
        '#autocomplete_path' => 'autocomp/tag',
        '#description' => '[<a class="show-tag-help">' . t('info') . '</a>]',
      );

      $form['core'] = array(
        '#type' => 'fieldset',
        '#title' => t('Core Settings'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#required' => TRUE,
        '#weight' => $weight['core'],
      );

      $form['core']['slo_text'] = array(
        '#title' => t('Learning Outcome Text'),
        '#type' => 'textarea',
        '#required' => TRUE,
        '#default_value' => html_entity_decode($selected_slo_text),
        '#rows' => 2,
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

    ////////////////////////////////////////////////////////////////////////////
    // Prerequisite Learning Outcomes
    ////////////////////////////////////////////////////////////////////////////

    $prereq_slos = $preslo_ids = array();

    $result = db_query("SELECT pre_slo FROM {dae_prereq_slo} WHERE target = %d", $selected_slo_id);
    while ($row = db_fetch_array($result)) {

      $prereq_slos[$row['pre_slo']] = $page_outcomes[$row['pre_slo']]['slo_text'];

      // An array to compare against the suggested slos.
      $preslo_ids[] = $row['pre_slo'];

    }

    natsort($prereq_slos);
    $prereqs_display = '';

    foreach ($prereq_slos as $id => $text) {

      $prereqs_display .= '<li>';
      $course_display = ''; // Reset the course display

      // Add each course where the assumed learning outcome is taught.
      $result = db_query("SELECT course_id FROM {dae_course_slo} WHERE slo_id=%d", $id);
      while ($row = db_fetch_array($result)) {

        // Get the course and split this value to from
        // the url parameter for viewing the course.
        $url = $course_url . '/' . str_replace(' ', '/', $page_courses[$row['course_id']]['course']);

        $course_display .= '[<a href="' . $url . '">' . $page_courses[$row['course_id']]['course'] . '</a>] '; // Leave the space at the end.

      }

      // Display the rank for users that have
      // build access, admin and builder type.
      if ($build_access) {
        $prereqs_display .= '<a href="' . $page_url . '/' . $id .'">' . $text . '</a> <small><small> ' . $course_display . '(' . $page_outcomes[$id]['slo_rank'] . ')</small></small> ';
      }
      else {
        $prereqs_display .= '<a href="' . $page_url . '/' . $id . '">' . $text . '</a> <small><small> ' . $course_display . '</small></small> ';
      }

      // Link to delete relationship
      if ($delete_slo_access) {

        // Do not wrap this message with the t()
        // this is displayed through JavaScript.
        $message = 'Are you sure you want to remove the prerequisite learning outcome \'' . $text . '\'?';

        $slo_url = $page_url . '/' . $param[0] . '/delete/prereq/' . $id;

        $prereqs_display .= '<a href="' . $slo_url . '" class="removable" message="' . $message . '"><strong>delete</strong></a>';

      }

      $prereqs_display .= '</li>';

    }

    // Notify if no matches were found.
    if (!$prereqs_display) {
      $prereqs_display = '<li><i>' . t('No matches found') . '</i></li>';
      $prereqs_flag = TRUE;
    }

    if (!$prereqs_flag || $manage_access) {

      $form['prereq'] = array(
        '#type' => 'fieldset',
        '#title' => t('Prerequiste Learning Outcomes'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => '&nbsp;&nbsp;&nbsp; ' . t('What the student is assumed to know before learning this outcome.'),
        '#weight' => $weight['prereq'],
      );

      $form['prereq'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $prereqs_display . '</ul>',
        '#weight' => $weight['prereq']-3,
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

    ////////////////////////////////////////////////////////////////////////////
    // Postrequisite Learning Outcomes (Why do I need to know that?)
    ////////////////////////////////////////////////////////////////////////////

    $postreq_slos = array();

    $result = db_query("SELECT target FROM {dae_prereq_slo} WHERE pre_slo = %d", $selected_slo_id);
    while ($row = db_fetch_array($result)) {
      $postreq_slos[$row['target']] = $page_outcomes[$row['target']]['slo_text'];
    }

    natsort($postreq_slos);
    $postreqs_display = '';

    foreach ($postreq_slos as $id => $text) {

      $postreqs_display .= '<li>';
      $course_display = ''; // Reset the course display

      // Add each course where the assumed learning outcome is taught.
      $result = db_query("SELECT course_id FROM {dae_course_slo} WHERE slo_id=%d", $id);
      while ($row = db_fetch_array($result)) {

        // Get the course and split this value to from
        // the url parameter for viewing the course.
        $url = $course_url . '/' . str_replace(' ', '/', $page_courses[$row['course_id']]['course']);

        $course_display .= '[<a href="' . $url . '">' . $page_courses[$row['course_id']]['course'] . '</a>] ';

      }

      if ($build_access) {
        $postreqs_display .= '<a href="' . $page_url . '/' . $id . '">' . $text . '</a> <small><small> ' . $course_display . '(' . $page_outcomes[$id]['slo_rank'] . ')</small></small></li>';
      }
      else {
        $postreqs_display .= '<a href="' . $page_url . '/' . $id . '">' . $text . '</a> <small><small> ' . $course_display . '</small></small></li>';
      }

    }

    // Notify if no matches were found.
    if (!$postreqs_display) {
      $postreqs_display = '<li><i>' . t('No matches found') . '</i></li>';
      $postreqs_flag = TRUE;
    }

    // Only display the postrequisite SLOs
    // if there are some to display.
    if (!$postreqs_flag) {

      $form['postreq'] = array(
        '#type' => 'fieldset',
        '#title' => t('Why do I need to know that?'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => '&nbsp;&nbsp;&nbsp; ' . t('These are the student
          learning outcome(s) that directly require this student learning outcome.'),
        '#weight' => $weight['postreq'],
      );

      $form['postreq'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $postreqs_display . '</ul>',
        '#weight' => $weight['postreq']-3,
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

    ////////////////////////////////////////////////////////////////////////////
    // Courses Covering This Learning Outcome
    ////////////////////////////////////////////////////////////////////////////

    $courses_covering = array(); $prereq_courses = array();

    $result = db_query("SELECT course_id FROM {dae_course_slo} WHERE slo_id = %d", $selected_slo_id);
    while ($row = db_fetch_array($result)) {
      $courses_covering[$row['course_id']] = $page_courses[$row['course_id']]['course'];
    }

    natsort($courses_covering);
    $covering_display = '';

    // List of courses covering
    foreach ($courses_covering as $id => $text) {

      $covering_display .= '<li>';

      // Seperate the string to get the course
      // code[0] and number[1] for the URL.
      $course_info = explode(' ', $text);

      // Make the url for editing the course
      $course_edit_url = $base_url . '/' . $page_settings['manage courses'] . '/' . $course_info[0] . '/' . $course_info[1];

      $covering_display .= '<a href="' . $course_edit_url . '">' . $text . '</a>';

      // If user has permission to delete links, the link to delete the relationship should be visible
      if ($delete_slo_access) {

        // Do not wrap this message with the t()
        // this is displayed through JavaScript.
        $message = 'Are you sure you no longer want \'' . $text . '\' to cover this learning outcome?';

        $url = $page_url . '/' . $param[0] . '/delete/covering/' . $id;

        $covering_display .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>delete</strong></a>';

      }

      $covering_display .= '</li>';

      // Retrieve the the prereq course information to determine
      // the suggested learning outcome later in the script.
      $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $id);
      while ($row = db_fetch_array($result)) {
        $prereq_courses[$row['prereq_id']] = $row['prereq_id'];
      }

    }

    // Notify that no matches were found.
    if (!$covering_display) {
      $covering_display = '<li><i>' . t('No matches found') . '</i></li>';
      $covering_flag = 1;
    }

    if (!$covering_flag || $manage_access) {

      $form['covering'] = array(
        '#type' => 'fieldset',
        '#title' => t('Courses Covering This Learning Outcome'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#weight' => $weight['covering'],
      );

      $form['covering'][] = array(
        '#type' => 'item',
        '#value' => '<ul>' . $covering_display . '</ul>',
        '#weight' => $weight['covering']-1,
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

    }

    ////////////////////////////////////////////////////////////////////////////
    // Courses Covering && Prerequisite Learning Outcome Textfields
    ////////////////////////////////////////////////////////////////////////////

    if ($build_outcomes_access) {

      for ($i = 0; $i < $page_settings['autocomplete iteration']; $i++) {

        // The settings for the first will be
        // different for all the remaining.
        if ($i == 0) {

          // Not hidden
          $ishid_prereq = $ishid_covering = 'no';

          // Singular opening titles
          $prereq_title   = t('Prerequisite learning outcome');
          $covering_title = t('Course covering learning outcome');

          $prereq_description = '[<a class="show-preslo-help">' . t('info') . '</a>]';

        }
        else {

          // See if the textfield is hidden
          if ($show_prereq[$i] == TRUE) {
            $ishid_prereq = 'no';
          }
          else {
            $ishid_prereq = 'yes';
          }

          if ($show_covering[$i] == TRUE) {
            $ishid_covering = 'no';
          }
          else {
            $ishid_covering = 'yes';
          }

          // Hidden field titles with iteration number.
          // If each text field is to be be titled or not.
          if ($page_settings['title each autocomplete textfield']) {
            $prereq_title   = '<small>' . t('Prerequisite learning outcome') . ' (' . ($i+1) . ')</small>';
            $covering_title = '<small>' . t('Course covering learning outcome') . ' (' . ($i+1) . ')</small>';
          }
          else {
            $prereq_title = $covering_title = '';
          }

          $prereq_description = '';

        }

        $form['prereq']["prereq_$i"] = array(
          '#type' => 'textfield',
          '#title' => $prereq_title,
          '#maxlength' => 255,
          '#size' => 115,
          '#description' => $prereq_description,
          '#prefix' => '<div class="hide-prereq-slo" is_hidden="' . $ishid_prereq . '"><blockquote>',
          '#suffix' => '</blockquote></div>',
          '#autocomplete_path' => 'autocomp/slo',
          '#weight' => $weight['prereq']+($i/1000),
        );

        $form['covering']["covering_$i"] = array(
          '#type' => 'textfield',
          '#title' => $covering_title,
          '#autocomplete_path' => 'autocomp/course',
          '#prefix' => '<div class="hide-covering-course" is_hidden="' . $ishid_covering . '"><blockquote>',
          '#suffix' => '</blockquote></div>',
          '#weight' => $weight['covering'] + ($i/1000),
        );

        // Make the buttons that will unhide 1 hidden textfield
        // at the end of the textfields. (as per javascript)
        if ($i == ($page_settings['autocomplete iteration'] - 1)) {

          $form['prereq'][] = array(
            '#type' => 'item',
            '#value' => '<input type="submit" class="add-prereq-slo add-another" value="Add another">',
            '#weight' => $weight['prereq'] + 1,
            '#prefix' => '<blockquote>',
            '#suffix' => '</blockquote>',
          );

          $form['covering'][] = array(
            '#type' => 'item',
            '#value' => '<input type="submit" class="add-covering-course add-another" value="Add another">',
            '#weight' => $weight['covering']+1,
            '#prefix' => '<blockquote>',
            '#suffix' => '</blockquote>',
          );

        }

      }

    }

    ////////////////////////////////////////////////////////////////////////////
    // Tags
    ////////////////////////////////////////////////////////////////////////////

    $current_tags = array();

    //generate a list of current tags
    $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE slo_id = %d", $selected_slo_id);
    while ($row = db_fetch_array($result)) {
      $current_tags[$row['tag_id']] = db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id = %d", $row['tag_id']));
    }

    natsort($current_tags);
    $tags_display = '';

    foreach ($current_tags as $tid => $label) {

      // Add the tag capitalized.
      $tags_display .= '<li>' . ucwords($label);

      // Give link to delete relationship
      if ($delete_slo_access) {

        // Do not wrap this message with the t()
        // this is displayed through JavaScript.
        $message = 'Are you sure you want to remove the tag \'' . $label . '\' from this learning outcome?';

        $url = $page_url . '/' . $param[0] . '/delete/tag/' . $tid;

        $tags_display .= ' - <a href="' . $url . '" class="removable" message="' . $message . '"><strong>' . t('delete') . '</strong></a>';

      }

      $tags_display .= '</li>';

    }

    // Notify if no matches were found
    if (!$tags_display) {
      $tags_display = '<li><i>' . t('No matches found') . '</i></li>';
      $currtag_flag = TRUE;
    }

    if ($build_outcomes_access) {

      if (!$currtag_flag || $manage_access) {
        $form['tags'] = array(
          '#type' => 'fieldset',
          '#title' => t('Tags'),
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          '#weight' => $weight['tags'],
        );

        $form['tags'][] = array(
          '#type' => 'item',
          '#value' => '<ul>' . $tags_display . '</ul>',
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote>',
        );
      }

      $form['tags']['new_tags'] = array(
        '#type' => 'textfield',
        '#title' => t('Tags'),
        '#description' => '[<a class="show-tag-help">' . t('info') . '</a>]',
        '#weight' => $weight['tags'],
        '#cols' => 2,
        '#autocomplete_path' => 'autocomp/tag',
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote><br />',
      );
    }

    // At this point if all the flags are TRUE then a user will see nothing
    // when selecting a learning outcome. Create a message informing the
    // user that there are no prerequiste courses or SLOS
    if ($covering_flag && $prereqs_flag && $postreqs_flag && $currtag_flag && !$manage_access) {

      $form[] = array(
        '#type' => 'item',
        '#value' => '<ul><li><i>' . t('This learning outcome has not been assigned any values.') . '</i></li></ul>',
      );

    }

    ////////////////////////////////////////////////////////////////////////////
    // Suggested Prerequisite Learning Outcomes
    ////////////////////////////////////////////////////////////////////////////

    // Set the user access to
    // view suggested outcomes.
    if ($manage_access) {

      $suggested_slos = array(); $suggested_outcomes = array(); $suggest_array = array();
//COMBINE INTO MYSQL JOIN....
      if ($prereq_courses) {

        foreach ($prereq_courses as $id) {

          $result = db_query("SELECT slo_id FROM {dae_course_slo} WHERE course_id=%d AND slo_id <> %d", $id, $selected_slo_id);
          while ($row = db_fetch_array($result)) {
            $suggested_slos[$row['slo_id']] = $row['slo_id'];
          }

        }

      }

      if ($current_tags) {

        foreach ($current_tags as $tid => $label) {

          $result = db_query("SELECT slo_id FROM {dae_slo_tag} WHERE tag_id=%d AND slo_id <> %d", $tid, $selected_slo_id);
          while ($row = db_fetch_array($result)) {
            $suggested_slos[$row['slo_id']] = $row['slo_id'];
          }

        }

      }

      if (count($suggested_slos)) {

        // Remove any duplicates that are already prereq SLOs.
        $suggested_slos = array_diff($suggested_slos, $preslo_ids);

        // Count the SLOs again as the all may have
        // been removed with the array_diff execution.
        if (count($suggested_slos)) {

          sort($suggested_slos);

          $suggested_slo_placeholders = implode(' OR ', array_fill(0, count($suggested_slos), 'id=%d'));

          // The formatting of this query is important, make
          // sure there is a space between the WHERE and ORDER.
          // If the space is removed it will run into the placeholder.
          $result = db_query("SELECT id, slo_text
                                FROM {dae_slo}
                               WHERE " . $suggested_slo_placeholders .
                          " ORDER BY slo_rank ASC, slo_text ASC", $suggested_slos);
          while ($row = db_fetch_array($result)) {
            $suggested_outcomes[$row['id']] = $row['slo_text'];
          }

          foreach ($suggested_outcomes as $id => $text) {

            // reset the course display
            $course_display = '';

            // Add each course where the assumed learning outcome is taught.
            $result = db_query("SELECT course_id FROM {dae_course_slo} WHERE slo_id=%d", $id);
            while ($row = db_fetch_array($result)) {

              // Get the course and split this value to from
              // the url parameter for viewing the course.
              $url = $course_url . '/' . str_replace(' ', '/', $page_courses[$row['course_id']]['course']);

              $course_display .= '[<a href="' . $url . '">' . $page_courses[$row['course_id']]['course'] . '</a>] ';

            }

            if ($build_access) {
              $suggest_array[$id] = '<a href="' . $page_url . '/' . $id . '">' . $text . '</a> <small><small> ' . $course_display . '(' . $page_outcomes[$id]['slo_rank'] . ')</small></small>';
            }
            else {
              $suggest_array[$id] = '<a href="' . $page_url . '/' . $id . '">' . $text . '</a> <small><small> ' . $course_display . '</small></small>';
            }

          }

          $form['suggest'] = array(
            '#type' => 'fieldset',
            '#title' => t('Suggested Prerequiste Learning Outcomes'),
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#weight' => $weight['suggest'],
          );

          $form['suggest']['suggestboxes'] = array(
            '#type' => 'checkboxes',
            '#options' => $suggest_array,
            '#weight' => $weight['suggest']-3,
            '#prefix' => '<blockquote>',
            '#suffix' => '</blockquote>',
          );

          $form['suggest']['submit-slo-suggestions'] = array(
            '#type' => 'submit',
            '#value' => t('Save suggestions'),
            '#weight' => $weight['suggest']-1,
            '#prefix' => '<br /><blockquote>',
            '#suffix' => '</blockquote><br /><br />',
          );

        }

      }

    }

    ////////////////////////////////////////////////////////////////////////////
    // Term Reviews Rating this Learning Outcome as Taught
    ////////////////////////////////////////////////////////////////////////////
    if (user_access('daedalus browse term reviews')) {

      // Table heading for the Term Review dropdowns.
      $table_header = '<table>
          <tr>
            <th><b>' . t('Ranking')    . '</b></th>
            <th><b>' . t('Course')     . '</b></th>
            <th><b>' . t('Instructor') . '</b></th>
            <th><b>' . t('Term')         . '</b></th>
            <th><b>' . t('Academic Year')  . '</b></th>
          </tr>';

      $review_info = array();
      $result = db_query("SELECT term_review_form_id FROM {dae_term_review_ratings} WHERE slo=%d AND type='taught'", $selected_slo_id);

      while ($row = db_fetch_array($result)) {
        $review_info[$row['term_review_form_id']] = db_result(db_query("SELECT year FROM {dae_term_review_form} WHERE id=%d", $row['term_review_form_id']));
      }

      // This function sorts an array such that array indices maintain their correlation
      // with the array elements they are associated with. This is used mainly when
      // sorting associative arrays where the actual element order is significant.
      asort($review_info);

      if ($review_info) {

        $review_list = $table_header;

        $form['taughtrev'] = array(
          '#type' => 'fieldset',
          '#title' => '&nbsp;&nbsp;&nbsp; ' . t('Term Reviews Rating this Learning Outcome as Taught'),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#weight' => $weight['taughtrev'],
        );

        // Foreach term Review ID
        foreach ($review_info as $rid => $year) {

          // Calculate the Average Coverage Rating for each Term Review.
          $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $rid);
          $i = 0;
          $total = 0;

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
          $extra_info = db_fetch_array(db_query("SELECT course, instructor, term FROM {dae_term_review_form} WHERE id=%d", $rid));
          $course_id  = $extra_info['course'];
          $instructor = $extra_info['instructor'];
          $term       = $extra_info['term'];

          $course = $page_courses[$course_id]['course'] . ' - ' . $page_courses[$course_id]['course_name'];

          $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $rid;

          $disp_course = '<a href="' . $url . '">' . ucwords($course) . '</a>';

          $review_list .= "<tr>
              <th>" . $rating . "</th>
              <th>" . $disp_course . "</th>
              <th>" . $instructor . "</th>
              <th>" . $term . "</th>
              <th>" . $year . "/" . ($year+1) . "</th>
            </tr>";
        }

        $form['taughtrev'][] = array(
          '#type' => 'item',
          '#value' => $review_list . '</table>',
          '#weight' => $weight['taughtrev'],
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote>',
        );

      }

      ////////////////////////////////////////////////////////////////////////////
      // Term Reviews Rating this Learning Outcome as Prerequisite
      ////////////////////////////////////////////////////////////////////////////
      $review_info = array();
      $result = db_query("SELECT term_review_form_id FROM {dae_term_review_ratings} WHERE slo=%d AND type='prereq'", $selected_slo_id);

      while ($row = db_fetch_array($result)) {
        $review_info[$row['term_review_form_id']] = db_result(db_query("SELECT year FROM {dae_term_review_form} WHERE id=%d", $row['term_review_form_id']));
      }

      asort($review_info );

      if ($review_info) {

        $review_list = $table_header;

        $form['prereqrev'] = array(
          '#type' => 'fieldset',
          '#title' => '&nbsp;&nbsp;&nbsp; ' . t('Term Reviews Rating this Learning Outcome as a Prerequisite'),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#weight' => $weight['prereqrev'],
        );

        // Foreach term Review ID and Year
        foreach ($review_info as $rid => $year) {

          $i = $total = 0;

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
          $extra_info = db_fetch_array(db_query("SELECT course, instructor, term FROM {dae_term_review_form} WHERE id=%d", $rid));
          $course_id  = $extra_info['course'];
          $instructor = $extra_info['instructor'];
          $term       = $extra_info['term'];

          $course = $page_courses[$course_id]['course'] . ' - ' . $page_courses[$course_id]['course_name'];

          $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $rid;

          $disp_course = '<a href="' . $url . '">' . ucwords($course) . '</a>';

          $review_list .= "<tr>
              <th>" . $rating . "</th>
              <th>" . $disp_course . "</th>
              <th>" . $instructor . "</th>
              <th>" . $term . "</th>
              <th>" . $year . "/" . ($year+1) . "</th>
            </tr>";
        }

        $form['prereqrev'][] = array(
          '#type' => 'item',
          '#value' => $review_list . '</table>',
          '#weight' => $weight['prereqrev'],
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote>',
        );

      }

      ////////////////////////////////////////////////////////////////////////////
      // Average Coverage Rating and Comments
      ////////////////////////////////////////////////////////////////////////////

      $act_total = $max_total = 0;

      // Calculate the Average Coverage Rating for the Term Reviews
      $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE slo=%d", $selected_slo_id);
      while ($row = db_fetch_array($result)) {
        $act_total += $row['rating'];
        $max_total += 2;
      }

      if ($max_total > 0) {

        $avg_cover = round( ( ($act_total / $max_total) * 100 ), 2 ) . '%';

        $form['covercomm'] = array(
          '#type' => 'fieldset',
          '#title' => '&nbsp;&nbsp;&nbsp; ' . t('Term Coverage Rating and Comments'),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#weight' => $weight['covercomm'],
        );

        $form['covercomm'][] = array(
          '#title' => t('Average Coverage Rating'),
          '#value' => t($avg_cover),
          '#type' => 'item',
          '#weight' => $weight['covercomm'],
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote>',
        );

        $review_info = array();
        $result = db_query("SELECT term_review_form_id FROM {dae_term_review_ratings} WHERE slo=%d", $selected_slo_id);
        while ($row = db_fetch_array($result)) {
          $review_info[$row['term_review_form_id']] = db_result(db_query("SELECT year FROM {dae_term_review_form} WHERE id=%d", $row['term_review_form_id']));
        }

        asort($review_info);

        $review_comm = '<ul>';

        // Foreach term Review ID and Year
        foreach ($review_info as $rid => $year) {

          // Get SLO comments
          $comment = db_result(db_query("SELECT comment
                                           FROM {dae_term_review_ratings}
                                          WHERE term_review_form_id=%d
                                            AND slo=%d", $rid, $selected_slo_id));

          if ($comment) {

            // Display Extra Review Information
            $review_row = db_fetch_array(db_query("SELECT course, term FROM {dae_term_review_form} WHERE id=%d", $rid));
            $review_course_id = $reveiw_row['course'];
            $term = $review_row['term'];

            $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $rid;

            $disp_comment = '<a href="' . $url . '">' . ucwords($comment) . '</a>';

            $review_comm .= '<li>' . $disp_comment . ' &nbsp;&nbsp;&nbsp;&nbsp;(' . $year . '/' . ($year+1) . ' ' . $term . ' ' . $page_courses[$review_course_id]['course'] . ')';

          }

        }

        if ($review_comm == '<ul>') {
          $review_comm .= '<li>' . t('No matches found') . '</li></ul>';
        }
        else {
          $review_comm .= '</ul>';
        }

        $form['covercomm'][] = array(
          '#type' => 'item',
          '#title' => t('Term Review Learning Outcome Comments'),
          '#value' => $review_comm,
          '#weight' => $weight['covercomm'],
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote>',
        );

      }

    }

    if ($build_outcomes_access) {

      $form['submit-slo-changes'] = array(
        '#type' => 'submit',
        '#value' => t('Update Learning Outcome'),
        '#weight' => max($weight)+50,
      );

    }

    if ($delete_slo_access) {

      $form['delete-slo'] = array(
        '#type' => 'submit',
        '#value' => t('Delete Learning Outcome'),
        '#weight' => max($weight)+51,
      );

    }

    // Lengthen the form when building to prevent the
    // tag autocomplete results from being cut off.
    if ($build_access) {

      $form[] = array(
        '#type' => 'item',
        '#value' => '<br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
                     <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />',
        '#weight' => max($weight) + 100,
      );

    }

  }

  // Submit hidden information to pass
  // to the validate and submit hooks.
  $form['pass-manage-learning-outcomes'] = array( '#type' => 'value', '#value' => $page_settings['manage learning outcomes'], );
  $form['pass-autocomplete-iteration'] = array( '#type' => 'value', '#value' => $page_settings['autocomplete iteration'], );
  $form['pass-tag-seperate-character'] = array( '#type' => 'value', '#value' => $page_settings['tag seperate character'], );
  $form['pass-tag-cloud-height-percent'] = array( '#type' => 'value', '#value' => $page_settings['tag cloud height percent'], );
  $form['pass-tag-cloud-max-font-size'] = array( '#type' => 'value', '#value' => $page_settings['tag cloud max font size'], );

  return $form;

}


/*
 * Implementation of hook_validate().
 */
function daedalus_manage_outcomes_form_validate($form, &$form_state) {

  global $base_url;

  $page_url = $form_state['values']['pass-manage-learning-outcomes'];
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  // Buttons for bulk manage
  if (!is_numeric($param[0])) {

    // Get the list of checked learning outcomes
    // which will be used in each case here.
    $selected_slo_ids = array();

    if ($form_state['values']['checkboxes']) {

      foreach ($form_state['values']['checkboxes'] as $key => $value) {

        if ($value != '0') {
          $selected_slo_ids[] = $key;
        }

      }

    }

    switch ($form_state['values']['op']) {

      // The Merge, Delete, Split and Tag buttons are all duplicate to account
      // for there being two button on the for to perform the same task.
      case $form_state['values']['button-merge-top']:
      case $form_state['values']['button-merge-bot']:

        if (sizeof($selected_slo_ids) != 2) {
          form_set_error('', t('You must select 2 learning outcomes to merge.'));
        }

        break;

      // Validate the new slo to be merged
      case $form_state['values']['button-merge-forward']:

        $tags = explode('_', $param[1]);
        $tag1 = $tags[0];
        $tag2 = $tags[1];
        $merge_text = '%' . trim($form_state['values']['new-merged']) . '%';

        // Make sure there are no SLO's in the database the same as
        // merge text, excluding ones that are selected for merging.
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_slo} WHERE slo_text LIKE '%s' AND id <> %d AND id <> %d", $merge_text, $tag1, $tag2))) {
          form_set_error('new-merged', t('That learning outcome already exists. Can not merge.'));
        }

        break;

      case $form_state['values']['button-delete-top']:
      case $form_state['values']['button-delete-bot']:

        if (sizeof($selected_slo_ids) <= 0) {
          form_set_error('', t('You must select at least 1 learning outcome to delete.'));
        }

        break;

      case $form_state['values']['button-tag-top']:
      case $form_state['values']['button-tag-bot']:

        if (sizeof($selected_slo_ids) <= 0) {
          form_set_error('', t('You must select at least 1 learning outcome to tag.'));
        }

        else {

          $endit = '';

          foreach ($selected_slo_ids as $id) {

            if ($endit != '') {
              $endit .= '_';
            }

            $endit .= $id;

          }

          drupal_goto($page_url . '/tag/' . $endit);
        }

        break;

      case $form_state['values']['button-split-top']:
      case $form_state['values']['button-split-bot']:

        if (sizeof($selected_slo_ids) != 1) {
          form_set_error('', t('You must select 1 learning outcome to split.'));
        }

        break;

      case $form_state['values']['button-split-forward']:

        $split_textA = $form_state['values']['split-a'];
        $split_textB = $form_state['values']['split-b'];

        $query = "SELECT COUNT(*) FROM {dae_slo} WHERE id <> %d and slo_text like '%s' ";

        if (db_result(db_query($query, $param[1], $form_state['values']['split-a'])) > 0) {
          form_set_error('split-a', t('The learning outcome "@splitA" already exists.', array('@splitA' => $split_textA)));
        }

        if (db_result(db_query($query, $param[1], $form_state['values']['split-b'])) > 0) {
          form_set_error('split-b', t('The learning outcome "@splitB" already exists.', array('@splitB' => $split_textB)));
        }

        if ($split_textA == $split_textB) {
          form_set_error('split-b', t('The learning outcome are identical.'));
        }

        break;

      case $form_state['values']['remove-multiple-top']:
      case $form_state['values']['remove-multiple-bot']:

        if (sizeof($selected_slo_ids) < 1) {
          form_set_error('', t('You must select at least 1 learning outcome to remove the selected tags.'));
        }

        break;
    }

  }
  else {

    // Get the suggested prerequisite SLO ids, this
    // must remain outside of the switch statement.
    if ($form_state['values']['op'] == $form_state['values']['submit-slo-suggestions']) {

      $suggested_sloids = array();

      foreach ($form_state['values']['suggestboxes'] as $key => $value) {

        if ($value != '0') {
          $suggested_sloids[] = $key;
        }

      }

    }

    switch ($form_state['values']['op']) {

      case $form_state['values']['submit-slo-changes']:
      case $form_state['values']['submit-slo-suggestions']:

        // Validate the suggested prerequiste outcomes only
        // if the submit suggested button is selected.
        if ($form_state['values']['op'] == $form_state['values']['submit-slo-suggestions']) {

          if (sizeof($suggested_sloids) == 0) {
            form_set_error('suggestboxes', t('You must select at least one suggested prerequiste learning outcome.'));
          }

        }

        // Get the information of the SLO that
        // is currently being validated.
        $slo_id = $param[0];

        //@todo pass the original slo text as a hidden value to remove this query.
        $slo_text = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $slo_id));

        $new_slo_text = trim($form_state['values']['slo_text']);

        // If the new slo text has changed.
        if (drupal_strtolower($slo_text) != drupal_strtolower($new_slo_text)) {

          // Check to see if the SLO
          // exists in the database.
          if (daedalus_valid_slo($new_slo_text)) {
            form_set_error('slo_text', t('This learning outcome exists and can not be duplicated.'));
          }

        }

        $existing_slos = array();

        // Compile the list of Prequisite SLOs associated to the SLO, below
        // there is a check to make sure that the SLO is not added twice.
        $result = db_query("SELECT pre_slo FROM {dae_prereq_slo} WHERE target=%d", $slo_id);
        while ($row = db_fetch_array($result)) {
          $existing_slos[$row['pre_slo']] = $row['pre_slo'];
        }

        // Iterate through the hidden textfields.
        for ($i = 0; $i < $form_state['values']['pass-autocomplete-iteration']; $i++) {

           ////////////////////////////////
          // Validate the Courses Covering
          $key = "covering_$i";
          $cover_val = trim($form_state['values'][$key]);

          // First make sure that the course exists
          if ($cover_val) {

            // If the course doesn't exist
            if (!daedalus_valid_course($cover_val)) {
              form_set_error($key, t('The course "@cover-val" does not exist.', array('@cover-val' => $cover_val)));
            }
            elseif (db_result(db_query("SELECT COUNT(*) FROM {dae_course_slo} WHERE course_id=%d AND slo_id=%d", daedalus_course_id($cover_val), $slo_id))) {
              form_set_error($key, t('The course "@cover-val" is already covering this learning outcome.', array('@cover-val' => $cover_val)));
            }

          }

           /////////////////////////////////////////////
          // Validate the Prerequiste Learning Outcomes
          $key = "prereq_$i";
          $prereq_val = trim($form_state['values'][$key]);

          if ($prereq_val) {

            // Validate a tag indicated by the hash tag.
            if (drupal_substr($prereq_val, 0, 1) == '#') {

              if (!daedalus_valid_tag(drupal_substr($prereq_val, 1))) {
                form_set_error($key, t('The tag "@prereq-val" can not be added because it does not exist.', array('@prereq-val' => $prereq_val)));
              }

            }
            else {

              $prereq_id = daedalus_slo_id($prereq_val);

              if ($slo_id == $prereq_id) {
                form_set_error($key, t('The prerequiste learning outcome "@prereq-val" can not be added to the same outcome.', array('@prereq-val' => $prereq_val)));
              }

              elseif (!daedalus_valid_slo($prereq_val)) {
                form_set_error($key, t('The prerequiste learning outcome "@prereq-val" can not be added because it does not exist.', array('@prereq-val' => $prereq_val)));
              }

              elseif (in_array($prereq_id, $existing_slos)) {
                form_set_error($key, t('The prerequiste learning outcome "@prereq-val" is already listed as a prerequisite learning outcome.', array('@prereq-val' => $prereq_val)));
              }

              elseif (db_result(db_query('SELECT COUNT(*) FROM {dae_prereq_slo} WHERE target=%d AND pre_slo=%d', $prereq_id, $slo_id))) {
                form_set_error($key, t('The prerequiste learning outcome "@prereq-val" could not be added because it lists this outcome as a prerequiste learning outcome.',
                        array('@prereq-val' => $prereq_val)));
              }

            }

          }

        }

         ////////////////////
        // Validate the Tags
        $tags = trim($form_state['values']['new_tags']);
        if ($tags != '') {

          $tag_seperate = $form_state['values']['pass-tag-seperate-character'];

          $tags = daedalus_explode_trim($tag_seperate, $tags);

          foreach ($tags as $key => $tag) {

            $tag_id = daedalus_tag_id($tag);

            if ($tag_id) {

              if (db_result(db_query("SELECT * FROM {dae_slo_tag} WHERE slo_id=%d and tag_id=%d", $slo_id, $tag_id))) {
                form_set_error('tags', t('The tag "@tag" is already listed as a tag for this outcome.', array('@tag' => $tag)));
              }

            }

          }

        }

        break;

    }

  }

}


/**
 * Implementation of hook_submit().
 */
function daedalus_manage_outcomes_form_submit($form, &$form_state) {

  global $base_url;

  $page_url = $form_state['values']['pass-manage-learning-outcomes'];
  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length);  // selected
  $param[1] = arg(1+$page_url_length);  // SLO Id's
  $param[2] = arg(2+$page_url_length);  // sorting (alpha)

  $slo_id = $param[0];
  $slo_text = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id = %d", $slo_id));

  // Save the help information
  if ($form_state['values']['op'] == $form_state['values']['dae-help-submit']) {

    $help_text = $form_state['values']['dae-help'];

    if ($help_text) {

      if (db_result(db_query("SELECT COUNT(*) FROM {dae_page_help} WHERE page_url='%s'", $page_url))) {
        db_query("UPDATE {dae_page_help} SET help_text='%s' WHERE page_url='%s'", $help_text, $page_url);
      }
      else {
        db_query("INSERT INTO {dae_page_help} (page_url, help_text) VALUES ('%s', '%s')", $page_url, $help_text);
      }

      drupal_set_message(t('Help information saved.') );

      if ($param[0] && $param[1] && $param[2]) {
        drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1] . '/' . $param[2]);
      }
      elseif ($param[0] && $param[1]) {
        drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1]);
      }
      elseif ($param[0]) {
        drupal_goto($base_url . '/' . $page_url . '/' . $param[0]);
      }
      else {
        drupal_goto($base_url . '/' . $page_url);
      }

    }

  }

  if (!is_numeric($param[0])) {

    // Get the list of checked learning outcomes.
    if ($form_state['values']['checkboxes']) {

      $slo_list = ''; $slo_array = array();

      foreach ($form_state['values']['checkboxes'] as $key => $value) {

        if ($value != '0') {

          // Store slos in an array
          $slo_array[] = $key;

          // and a list delimited with underscores
          if ($slo_list != '') {
            $slo_list .= '_';
          }

          $slo_list .= $key;

        }

      }

    }

    switch ($form_state['values']['op']) {

      case $form_state['values']['order-alpha']:

        if ($param[1]) {
          drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1] . '/alpha');
        }
        else {
          drupal_goto($base_url . '/' . $page_url . '/alpha');
        }

        break;

      case $form_state['values']['order-rank']:

        if ($param[2] == 'alpha') {
          drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1]);
        }
        else {
          drupal_goto($base_url . '/' . $page_url);
        }

        break;

      case $form_state['values']['calculate-rank']:

        drupal_goto($base_url . '/' . $page_url . '/rank');

        break;

      case $form_state['values']['view-by-tags']:

        drupal_goto($base_url . '/' . $page_url);

        break;

      case $form_state['values']['view-by-course']:

        if ($form_state['values']['course-slos']) {
          drupal_goto($base_url . '/' . $page_url . '/courses/' . $form_state['values']['course-slos']);
        }
        else {
          drupal_goto($base_url . '/' . $page_url . '/courses');
        }

        break;

      // Buttons for changing the settings. They just change some db values.
      // "increase-percent", "decrease-percent", "increase-max", "decrease-max"
      case $form_state['values']['increase-percent']:

        $tag_height = $form_state['values']['pass-tag-cloud-height-percent'];

        $value = $tag_height + 20;

        db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud height percent'", $value);

        break;

      case $form_state['values']['decrease-percent']:

        $tag_height = $form_state['values']['pass-tag-cloud-height-percent'];

        if ($tag_height > 40 ) {
          $value = $tag_height - 20;
        }
        else {
          $value = $tag_height - 10;
        }

        db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud height percent'", $value);

        break;

      case $form_state['values']['increase-max']:

        $max_font_size = $form_state['values']['pass-tag-cloud-max-font-size'];

        $value = $max_font_size + 1;

        db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud max font size'", $value);

        break;

      case $form_state['values']['decrease-max']:

        $max_font_size = $form_state['values']['pass-tag-cloud-max-font-size'];

        if ($max_font_size >= 1) {

          $value = $max_font_size - 1;
          db_query("UPDATE {dae_settings} SET value=%d WHERE setting='tag cloud max font size'", $value);

        }
        else {
          drupal_set_message(t('The font size is currently set to its smallest value.') );
        }

        break;

      case $form_state['values']['submit-depth']:

        db_query("UPDATE {dae_settings} SET value=%d WHERE setting='learning outcome rank iteration'", $form_state['values']['new-depth']);

        drupal_set_message(t('Rank depth updated to "@new-depth"', array('@new-depth' => $form_state['values']['new-depth'])));

        drupal_goto($base_url . '/' . $page_url . '/rank');

        break;

      case $form_state['values']['button-delete-forward']:

        $selected_slos = explode('_', $param[1]);

        foreach ($selected_slos as $id) {

          // Get the text to enter into the {dae_term_review_ratings} table so
          // not to remove any of the SLO's that have been review for a specific
          // term. Instead of being deleted from the term review they will
          // remain but marked (deleted) to show the SLO is removed.
          $slo_text = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id = %d", $id));
          $review_slo_text = $slo_text . " <small><b>(deleted)</b></small>";

          db_query("UPDATE {dae_term_review_ratings} SET slo='%s' WHERE slo=%d", $review_slo_text, $id);

          //remove from course slo
          db_query("DELETE FROM {dae_course_slo} WHERE slo_id=%d", $id);
          db_query("DELETE FROM {dae_prereq_slo} WHERE target=%d OR pre_slo=%d", $id, $id);
          db_query("DELETE FROM {dae_slo_tag} WHERE slo_id=%d", $id);
          db_query("DELETE FROM {dae_slo} WHERE id=%d", $id);

        }

        // Recalculate the slo ranking after deletion.
        daedalus_calculate_slo_rank();

        if (count($selected_slos) == 1) {
          drupal_set_message(t('The learning outcome "@slo" has been deleted', array('@slo' => $slo_text)));
        }
        else {
          drupal_set_message(t('Deleted "@slo-count" learning outcomes', array('@slo-count' => count($selected_slos))));
        }

        drupal_goto($base_url . '/' . $page_url);

        break;

      case $form_state['values']['tags-add-forward']:

        $selected = explode('_', $param[1]);

        $tag_array = daedalus_explode_trim($form_state['values']['pass-tag-seperate-character'], $form_state['values']['tags-to-add']);

        $tag_confirmation = FALSE;

        foreach ($selected as $slo_id) {

          $slo_text = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $slo_id));

          foreach ($tag_array as $tag) {

            // Create the tag if it does not exist.
            if (db_result(db_query("SELECT COUNT(*) FROM {dae_tag} WHERE tag_label like '%s'", $tag)) == 0) {

              db_query("INSERT INTO {dae_tag} (tag_label) VALUES('%s')", $tag);

              drupal_set_message(t('The tag "@tag" has been created', array('@tag' => $tag)));

            }

            $tid = db_result(db_query("SELECT id FROM {dae_tag} WHERE tag_label like '%s'", $tag));

            // If the SLO has not been tagged insert the tag, if it has warn the user.
            if (!db_result(db_query("SELECT COUNT(*) FROM {dae_slo_tag} WHERE slo_id=%d AND tag_id=%d", $slo_id, $tid))) {

              db_query("INSERT INTO {dae_slo_tag} (slo_id,tag_id) VALUES(%d,%d)", $slo_id, $tid);

              $tag_confirmation = TRUE;

            }
            else {
              drupal_set_message(t('The learning outcome "@slo" has already been tagged with "@tag".',
                      array('@slo' => $slo_text, '@tag' => $tag)), 'warning');
            }

          }

        }

        if ($tag_confirmation) {
          drupal_set_message(t('Learning Outcomes have been tagged.') );
        }
        else {
          drupal_set_message(t('No learning Outcomes have been tagged.') );
        }

        drupal_goto($base_url . '/' . $page_url);

        break;

      case $form_state['values']['button-merge-forward']:

        $slo_ids = explode('_', $param[1]);

        $slo_id1 = $slo_ids[0];
        $slo_text1 = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $slo_id1));

        $slo_id2 = $slo_ids[1];
        $slo_text2 = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $slo_id2));

        $new_slo = $form_state['values']['new-merged'];

        if ($new_slo == $slo_text2) {

          // If the merged SLO is the same as the second selected SLO, update the dae_slo
          // table and set the first selected SLOs text to the text of the second.
          db_query("UPDATE {dae_slo} SET slo_text='%s' WHERE id=%d", $slo_text2, $slo_id1);

        }
        elseif (($new_slo != $slo_text1) && ($new_slo != $slo_text2)) {

          // If the merge slo is not the same as either selected SLO, update the dae_slo
          // table and set the first selected SLOs text to the text in the merge text field.
          db_query("UPDATE {dae_slo} SET slo_text='%s' WHERE id=%d", $new_slo, $slo_id1);

        }

        // Delete the second learing outcome selected for the merge
        db_query("DELETE FROM {dae_slo} WHERE id=%d", $slo_id2);

        ///////////////////// Manage the {dae_course_slo} table values /////////////////////
        $course_ids = array();

        // Get the course id's associated with the SLOs being merged
        $result = db_query("SELECT DISTINCT course_id FROM {dae_course_slo} WHERE slo_id=%d OR slo_id=%d", $slo_id1, $slo_id2);
        while ($row = db_fetch_array($result)) {
          $course_ids[] = $row['course_id'];
        }

        if ($course_ids) {

          // Delete all the SLOs associated with courses to ensure there are no duplicates
          db_query("DELETE FROM {dae_course_slo} WHERE slo_id=%d OR slo_id=%d", $slo_id1, $slo_id2);

          foreach ($course_ids as $cid) {
            db_query("INSERT INTO {dae_course_slo} (course_id, slo_id) VALUES(%d,%d)", $cid, $slo_id1);
          }

        }

        ///////////////////// Manage the {dae_prereq_slo} table values /////////////////////
        $pre_slos = array();

        // Get the prerequiste SLOs associated with the SLOs being merged
        $result = db_query("SELECT DISTINCT pre_slo FROM {dae_prereq_slo} WHERE target=%d OR target=%d", $slo_id1, $slo_id2);
        while ($row = db_fetch_array($result)) {

          // Make sure not to add a prerequisite slo that will be deleted
          if (($row['pre_slo'] != $slo_id1) && ($row['pre_slo'] != $slo_id2)) {
            $pre_slos[] = $row['pre_slo'];
          }

        }

        if ($pre_slos) {

          // Delete all the SLOs associated with prerequiste SLOs to ensure there are no duplicates
          db_query("DELETE FROM {dae_prereq_slo} WHERE target=%d OR target=%d", $slo_id1, $slo_id2);

          foreach ($pre_slos as $pslo) {
            db_query("INSERT INTO {dae_prereq_slo} (target, pre_slo) VALUES(%d,%d)", $slo_id1, $pslo);
          }

        }

        ///////////////////// Manage the {dae_slo_tag} table values /////////////////////
        $tag_ids = array();

        // Get the tag ids associated with the SLOs being merged
        $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE slo_id=%d OR slo_id=%d", $slo_id1, $slo_id2);
        while ($row = db_fetch_array($result)) {
          $tag_ids[] = $row['tag_id'];
        }

        if ($tag_ids) {

          // Delete all the slos associated with the tags to ensure there are no duplicates
          db_query("DELETE FROM {dae_slo_tag} WHERE slo_id=%d OR slo_id=%d", $slo_id1, $slo_id2);

          foreach ($tag_ids as $tid) {
            db_query("INSERT INTO {dae_slo_tag} (slo_id, tag_id) VALUES(%d,%d)", $slo_id1, $tid);
          }

        }

        ///////////////////// Manage the {dae_term_review_ratings} table values /////////////////////
        //
        // Renames each SLO in the term reviews as merged. Initially the SLO is only listed in the
        // reviews as an id, by renaming the SLOs text replaces the id. The newly created merged SLO
        // is not added to the term review because it may no longer be relevant to the review. This
        // way the original SLOs will still be related to the review.
        //
        // If the new slo is new, mark previous two slos as deleted
        if (($new_slo != $slo_text1) && ($new_slo != $slo_text2)) {

          $review_ids = array();

          // Get the unique review ids from the first SLO id
          $result = db_query("SELECT DISTINCT id FROM {dae_term_review_ratings} WHERE slo=%d", $slo_id1);
          while ($row = db_fetch_array($result)) {
            $review_ids[] = $row['id'];
          }

          $del_text1 = $slo_text1 . ' <small><b>(merged)</b></small>';

          if ($review_ids) {

            $review_placeholders = implode(' OR ', array_fill(0, count($review_ids), 'id=%d'));

            db_query("UPDATE {dae_term_review_ratings} SET slo='%s' WHERE " . $review_placeholders, $del_text1, $review_ids);

          }

          $review_ids = array();

          // Get the unique review ids from the second SLO id
          $result = db_query("SELECT DISTINCT id FROM {dae_term_review_ratings} WHERE slo=%d", $slo_id2);
          while ($row = db_fetch_array($result)) {
            $review_ids[] = $row['id'];
          }

          $del_text2 = $slo_text2 . ' <small><b>(merged)</b></small>';

          if ($review_ids) {

            $review_placeholders = implode(' OR ', array_fill(0, count($review_ids), 'id=%d'));

            db_query("UPDATE {dae_term_review_ratings} SET slo='%s' WHERE " . $review_placeholders, $del_text2, $review_ids);

          }

        }

        elseif (($new_slo == $slo_text1)) {

          $review_ids = array();

          // Get the unique review ids from the second SLO id ONLY
          $result = db_query("SELECT DISTINCT id FROM {dae_term_review_ratings} WHERE slo=%d", $slo_id2);
          while ($row = db_fetch_array($result)) {
            $review_ids[] = $row['id'];
          }

          $del_text2 = $slo_text2 . ' <small><b>(merged)</b></small>';

          if ($review_ids) {
//@todo add placeholders
            $review_query = "UPDATE {dae_term_review_ratings} SET slo='" . $del_text2 . "' WHERE ";

            for ($i = 0; $i < count($review_ids); $i++) {

              if ($i == count($review_ids)-1) {
                $review_query .= 'id=%d';
              }
              else {
                $review_query .= 'id=%d OR ';
              }

            }

            db_query($review_query, $review_ids);
          }

        }

        else {

          $review_ids = array();

          // Get the unique review ids from the first SLO id ONLY
          $result = db_query("SELECT DISTINCT id FROM {dae_term_review_ratings} WHERE slo=%d", $slo_id1);
          while ($row = db_fetch_array($result)) {
            $review_ids[] = $row['id'];
          }

          $del_text1 = $slo_text1 . ' <small><b>(merged)</b></small>';

          if ($review_ids) {

            $review_placeholders = implode(' OR ', array_fill(0, count($review_ids), 'id=%d'));

            db_query("UPDATE {dae_term_review_ratings} SET slo='%s' WHERE " . $review_placeholders, $del_text1, $review_ids);

          }

        }

        // Finally re-calculate the rank
        daedalus_calculate_slo_rank();

        drupal_set_message(t('The learning outcomes have been merged into "@new-slo"', array('@new-slo' => $new_slo)));

        drupal_goto($base_url . '/' . $page_url);

        break;

      case $form_state['values']['button-merge-top']:
      case $form_state['values']['button-merge-bot']:

        drupal_goto($base_url . '/' . $page_url . '/merge/' . $slo_list);

        break;

      case $form_state['values']['button-delete-top']:
      case $form_state['values']['button-delete-bot']:

        drupal_goto($base_url . '/' . $page_url . '/delete/' . $slo_list);

        break;

      case $form_state['values']['button-split-top']:
      case $form_state['values']['button-split-bot']:

        drupal_goto($base_url . '/' . $page_url . '/split/' . $slo_array[0]);

        break;

      case $form_state['values']['remove-multiple-top']:
      case $form_state['values']['remove-multiple-bot']:

        drupal_goto($base_url . '/' . $page_url . '/remove/' . $slo_list . '/' . $form_state['values']['tag_list']);

        break;

      case $form_state['values']['button-remove-multiple-forward']:

        $slo_ids = explode('_', $param[1]);
        $tag_ids = explode('_', $param[2]);

        foreach ($slo_ids as $sid) {

          foreach ($tag_ids as $tid) {

            // Remove each tag from the current SLO.
            db_query("DELETE FROM {dae_slo_tag} WHERE slo_id=%d AND tag_id=%d", $sid, $tid);

          }

        }

        drupal_set_message(t('The tags have been removed.') );

        drupal_goto($base_url . '/' . $page_url);

        break;

      case $form_state['values']['button-split-forward']:

        $split_textA = $form_state['values']['split-a'];
        $split_textB = $form_state['values']['split-b'];

        $original_sid = $param[1];

        $original_txt = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $original_sid));

        // 1st split value is the same as the original
        if ($split_textA == $original_txt) {

          // Enter new SLO value
          db_query("INSERT INTO {dae_slo} (slo_text, slo_rank) VALUES('%s', 0)", $split_textB);

          // Get new SLOs ID
          $new_sid = db_result(db_query("SELECT id FROM {dae_slo} WHERE slo_text='%s'", $split_textB));

        }

        // 2nd split value is the same as the original
        elseif ($split_textB == $original_txt) {

          // Enter new SLO value
          db_query("INSERT INTO {dae_slo} (slo_text, slo_rank) VALUES('%s', 0)", $split_textA);

          // Get new SLOs ID
          $new_sid = db_result(db_query("SELECT id FROM {dae_slo} WHERE slo_text='%s'", $split_textA));

        }

        // Neither split value matches the original SLO. Update the original SLO with
        // the first split value and insert a new SLO with the second split value.
        elseif ($split_textA != $original_txt && $split_textB != $original_txt) {

          // Update the orininal SLO to the new split value
          db_query("UPDATE {dae_slo} SET slo_text='%s' WHERE id=%d", $split_textA, $original_sid);

          // Enter new SLO value
          db_query("INSERT INTO {dae_slo} (slo_text, slo_rank) VALUES('%s', 0)", $split_textB);

          // Get new SLOs ID
          $new_sid = db_result(db_query("SELECT id FROM {dae_slo} WHERE slo_text='%s'", $split_textB));

        }

        else {

          drupal_set_message(t('Error, invalid input') );

          drupal_goto($base_url . '/' . $page_url);

        }

        ///////////////////// Manage the {dae_course_slo} table values /////////////////////
        $course_ids = array();

        // Get the course id's associated with the SLOs being merged
        $result = db_query("SELECT DISTINCT course_id FROM {dae_course_slo} WHERE slo_id=%d", $original_sid);

        while ($row = db_fetch_array($result)) {
          $course_ids[] = $row['course_id'];
        }

        if ($course_ids) {

          foreach ($course_ids as $cid) {
            db_query("INSERT INTO {dae_course_slo} (course_id, slo_id) VALUES(%d,%d)", $cid, $new_sid);
          }

        }

        ///////////////////// Manage the {dae_prereq_slo} table values /////////////////////
        $pre_slos = array();

        // Get the prerequiste SLOs associated with the SLOs being merged
        $result = db_query("SELECT DISTINCT pre_slo FROM {dae_prereq_slo} WHERE target=%d", $original_sid);

        while ($row = db_fetch_array($result)) {
          $pre_slos[] = $row['pre_slo'];
        }

        if ($pre_slos) {

          foreach ($pre_slos as $pslo) {
            db_query("INSERT INTO {dae_prereq_slo} (target, pre_slo) VALUES(%d,%d)", $new_sid, $pslo);
          }

        }

        ///////////////////// Manage the {dae_slo_tag} table values /////////////////////
        $tag_ids = array();

        // Get the tag ids associated with the SLOs being merged
        $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE slo_id=%d OR slo_id=%d", $original_sid);

        while ($row = db_fetch_array($result)) {
          $tag_ids[] = $row['tag_id'];
        }

        if ($tag_ids) {

          foreach ($tag_ids as $tid) {
            db_query("INSERT INTO {dae_slo_tag} (slo_id, tag_id) VALUES(%d,%d)", $new_sid, $tid);
          }

        }

        ///////////////////// Manage the {dae_term_review_ratings} table values /////////////////////
        //
        // Same as for merge. The SLOs in the term reviews will now reflect that they have been split.
        // The newley created SLOs from the split will not be added to the term reviews
        //
        // Get the unique review ids
        $result = db_query("SELECT DISTINCT id FROM {dae_term_review_ratings} WHERE slo=%d", $original_sid);

        $review_ids = array();
        while ($row = db_fetch_array($result)) {
          $review_ids[] = $row['id'];
        }

        $split_text = $original_txt . ' <small><b>(split)</b></small>';

        if ($review_ids) {

          $review_placeholders = implode(' OR ', array_fill(0, count($review_ids), 'id=%d'));

          db_query("UPDATE {dae_term_review_ratings} SET slo='" . $split_text . "' WHERE " . $review_placeholders, $review_ids);

        }

        // Finally re-calculate the rank
        daedalus_calculate_slo_rank();

        drupal_set_message(t('The learning outcome "@orig" has been split: <br />Outcome A: "@splitA"<br />Outcome B: "@splitB"',
                array('@orig' => $original_txt, '@splitA' => $split_textA, '@splitB' => $split_textB)));

        drupal_goto($base_url . '/' . $page_url);

        break;

    }

  }

  else {

    // Begin the list of prerequisite learning outcomes
    // that are associated with the learing outcome.
    $existing_slos = array();

    // Get the list of checked suggested learning outcomes.
    // Create the list here, for some reason that I am not
    // aware if the list is created in the switch statement
    // other form values are messed up.... ??
    if ($form_state['values']['suggestboxes']) {

      $suggested_slos = array();
      $suggested_count = 0;

      foreach ($form_state['values']['suggestboxes'] as $key => $value) {

        if ($value != '0') {

          // Stagger the array with
          // the target value.
          $suggested_slos[] = $slo_id;

          // Store suggested SLOs
          $suggested_slos[] = $key;

          // Add the suggested SLOs to the
          // list of exisiting prereq SLOs.
          $existing_slos[$key] = $key;

          $suggested_count++;

        }

      }

    }

    switch ($form_state['values']['op']) {

      // Confirmed deletion of a learning outcome.
      case $form_state['values']['delete-forward']:

        // Delete from course teaching
        db_query("DELETE FROM {dae_course_slo} WHERE slo_id=%d", $slo_id);

        // Before deleting, it should replace the slo reference with the slo text
        $slo_text = db_result(db_query("SELECT slo_text FROM {dae_slo} WHERE id=%d", $slo_id));

        $deleted_slo_text = $slo_text . '<small><b>(deleted)</b></small>';

        db_query("UPDATE {dae_term_review_ratings} SET slo='%s' WHERE slo=%d", $deleted_slo_text, $slo_id);

        // Delete from prereq slo
        db_query("DELETE FROM {dae_prereq_slo} WHERE pre_slo=%d or target=%d", $slo_id, $slo_id);

        // Delete tags
        db_query("DELETE FROM {dae_slo_tag} WHERE slo_id=%d", $slo_id);

        // Delete slo
        db_query("DELETE FROM {dae_slo} WHERE id=%d", $slo_id);

        drupal_set_message(t('The student learning outcome "@slo-text" has been deleted.',
                array('@slo-text' => html_entity_decode($slo_text))));

        drupal_goto($base_url . '/' . $page_url);

        break;

      case $form_state['values']['delete-reverse']:

        drupal_goto($base_url . '/' . $page_url . '/' . $slo_id);

        break;

      case $form_state['values']['delete-slo']:

        drupal_goto($base_url . '/' . $page_url . '/' . $slo_id . '/delete/this');

        break;

      case $form_state['values']['submit-slo-changes']:
      case $form_state['values']['submit-slo-suggestions']:

        // Determine if the slo rank should be recalculated
        // if there are changes made to the {dae_slo} table.
        $rank_flag = FALSE;

        // First update the text. Remove any html formatting and check
        // for multiple spacess and anything that should not be there.
        $new_slo_text = drupal_html_to_text($form_state['values']['slo_text']);

        // Replace any special html characters
        $new_slo_text = check_plain($new_slo_text);

        // Remove any line breaks from the text.
        $new_slo_text = str_replace(Array("\r\n","\n","\r"), "", $new_slo_text);

        // Finally remove any double spaces.
        $new_slo_text = str_replace("  ", " ", $new_slo_text);

        $new_slo_text = trim($new_slo_text);

        db_query("UPDATE {dae_slo} SET slo_text = '%s' WHERE id=%d", $new_slo_text, $slo_id);

        // Add the suggested SLOs that
        // have been created outside of
        // the case switch statement.
        if ($suggested_count) {

          $suggested_slo_values = implode(', ', array_fill(0, $suggested_count, '(%d,%d)'));

          db_query("INSERT INTO {dae_prereq_slo} (target, pre_slo) VALUES " . $suggested_slo_values, $suggested_slos);

          $rank_flag = TRUE;

        }

        // Get list of Prereq SLOs associated with the outcome
        // and add them to the suggested outcome ids if any.
        $result = db_query("SELECT pre_slo FROM {dae_prereq_slo} WHERE target=%d", $slo_id);
        while ($row = db_fetch_array($result)) {
          $existing_slos[$row['pre_slo']] = $row['pre_slo'];
        }

        $covering_array = array(); $outcome_array = array(); $tag_array = array(); $tagged_slos = array();
        $covering_count = 0; $outcome_count = 0; $tagged_slo_count = 0;

        // Iterate each hidden textfield.
        for ($i = 0; $i < $form_state['values']['pass-autocomplete-iteration']; $i++) {

           ///////////////////////////////////////////////
          // Create Courses Covering the Learning Outcome
          $course_covering = trim($form_state['values']["covering_$i"]);

          if ($course_covering) {

            // Make a list of covering course ids so that
            // each of these courses may be updated with
            // their newly assumed slos.

            $covering_array[] = daedalus_course_id($course_covering);

            $covering_array[] = $slo_id;

            $covering_count++;

          }

           ///////////////////////////////////////////////////
          // Create the Prerequiste Student Learning Outcomes
          $outcome = trim($form_state['values']["prereq_$i"]);

          if ($outcome) {

            if (drupal_substr($outcome, 0, 1) == '#') {

              $tag_array[] = drupal_substr($outcome, 1);

            }
            else {

              $current_sloid = daedalus_slo_id($outcome);

              if (!in_array($current_sloid, $existing_slos)) {

                $outcome_array[] = $slo_id;

                $outcome_array[] = $current_sloid;

                $outcome_count++;

              }

            }

          }

        }

        // Add the courses covering
        if ($covering_count) {

          $covering_value_placeholders = implode(', ', array_fill(0, $covering_count, '(%d,%d)'));

          db_query("INSERT INTO {dae_course_slo} (course_id,slo_id) VALUES " . $covering_value_placeholders, $covering_array);

        }

        // Add the outcomes
        if ($outcome_count) {

          $outcome_value_placeholders = implode(', ', array_fill(0, $outcome_count, '(%d,%d)'));

          db_query("INSERT INTO {dae_prereq_slo} (target, pre_slo) VALUES " . $outcome_value_placeholders, $outcome_array);

          $rank_flag = TRUE;

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

            $new_tagged_slos = array();

            $tagged_slo_count = count($tagged_slos);

            foreach ($tagged_slos as $new_id) {

              $new_tagged_slos[] = $slo_id;

              $new_tagged_slos[] = $new_id;

            }

            $tagged_slo_placeholders = implode(', ', array_fill(0, $tagged_slo_count, '(%d,%d)'));

            db_query("INSERT INTO {dae_prereq_slo} (target, pre_slo) VALUES " . $tagged_slo_placeholders, $new_tagged_slos);

          }

          $rank_flag = TRUE;

        }

        // After the slos have been submitted,
        // update the slo rank.
        if ($rank_flag) {
          daedalus_calculate_slo_rank();
        }

         //////////////////
        // Submit the Tags
        $tags = trim($form_state['values']['new_tags']);

        if ($tags) {

          $new_tags = array();

          $new_tag_count = 0;

          // Make an array from the string, seperating by
          // the database tag seperate character value.
          $tags = daedalus_explode_trim($form_state['values']['pass-tag-seperate-character'], $tags);

          // Add each tag, stagger the values for the
          // database query of one tag_id and slo_id.
          foreach ($tags as $tag) {

            // If the tag doesn't exist, create it
            if (!daedalus_valid_tag($tag)) {

              db_query("INSERT INTO {dae_tag} (tag_label) VALUES('%s')", $tag);

              $tag_id = daedalus_tag_id($tag);

              $new_tags[] = $tag_id;

              $new_tags[] = $slo_id;

            }
            else {

              $tag_id = daedalus_tag_id($tag);

              $new_tags[] = $tag_id;

              $new_tags[] = $slo_id;

            }

            $new_tag_count++;

          }

          // If there are tags to be entered make a query
          // placeholder and enter all of the values
          if ($new_tags) {

            $new_tag_placeholders = implode(', ', array_fill(0, $new_tag_count, '(%d,%d)'));

            db_query("INSERT INTO {dae_slo_tag} (tag_id,slo_id) VALUES " . $new_tag_placeholders, $new_tags);

          }

        }

        drupal_set_message(t('The learning outcome has been updated.',
                array('%slo-text' => html_entity_decode($slo_text))));

        break;

    }

  }

}