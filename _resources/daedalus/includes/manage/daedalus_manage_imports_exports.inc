<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage->Downloads Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_information_page() {
  return drupal_get_form('daedalus_manage_information_form');
}


/**
 * Menu Location: Daedalus -> Manage -> Information
 * URL Location:  daedalus/manage/information
 *
 * Displays: Uploading and Downloading of Daedalus course and SLO data.
 */
function daedalus_manage_information_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                        WHERE setting='manage imports/exports' OR
                              setting='manage learning outcomes' OR
                              setting='manage courses' OR
                              setting='question mark'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  global $base_url;

  $page_url = $help_url = $page_settings['manage imports/exports'];
  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);  // file name

  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($help_url, 0, 1);

  //------------------------------------------------------------------------//
  //    Import/Export Welcome                                               //
  //------------------------------------------------------------------------//
  if (!$param[0]) {

    // First remove any file with the .tsv extension from the sites/files directory.
    // This will prevent the wrong file from being uploaded in the case the user already
    // uploaded a file and redirected from the page before the changes were commited.
    system('rm ' . file_directory_path() . '/*.tsv');

    // The title is set with an 'a' tag with the class='show-help'. When the image is clicked the help section is shown.
    drupal_set_title(t('Manage Importing and Exporting !help', array('!help' => $show_help)));

    // Upload the student learning outcomes.
    $form['upload-slos'] = array(
      '#type' => 'file',
      '#title' => t('Upload Student Learning Outcomes'),
      '#size' => 50,
      '#description' => t('Upload .tsv files only. The file format can be found <a href="@url">here</a>.',
              array('@url' => url($base_url . '/' . $page_url . '/slo_format'))),
      '#prefix' => '<br /><div class="table"><table><tr><td><blockquote>',
      '#suffix' => '</blockquote></td>',
    );

    $form['import-slos'] = array(
      '#type' => 'submit',
      '#value' => t('Upload Student Learning Outcomes'),
      '#prefix' => '<td>',
      '#suffix' => '</td></tr>',
    );

    // Export the student learing outcomes.
    $form['slo-filename'] = array(
      '#type' => 'textfield',
      '#title' => t('Export Student Learning Outcome Filename'),
      '#size' => 53,
      '#default_value' => t('outcome_export'),
      '#description' => t('Export .tsv files only, the file suffix is added automatically.'),
      '#prefix' => '<tr><td><blockquote>',
      '#suffix' => '</blockquote></td>',
    );

    $form['export-slos'] = array(
      '#type' => 'submit',
      '#value' => t('Export Student Learning Outcomes'),
      '#prefix' => '<td>',
      '#suffix' => '</td></tr></table></div>',
    );

    // Upload the courses.
    $form['upload-courses'] = array(
      '#type' => 'file',
      '#title' => t('Upload Courses'),
      '#size' => 50,
      '#description' => t('Upload .tsv files only. The file format can be found <a href="@url">here</a>.',
              array('@url' => url($base_url . '/' . $page_url . '/course_format'))),
      '#prefix' => '<br /><br /><div class="table"><table><tr><td><blockquote>',
      '#suffix' => '</blockquote></td>',
    );

    $form['import-courses'] = array(
      '#type' => 'submit',
      '#value' => t('Upload Courses'),
      '#prefix' => '<td>',
      '#suffix' => '</td></tr>',
    );

    // Export the courses.
    $form['course-filename'] = array(
      '#type' => 'textfield',
      '#title' => t('Export Course Filename'),
      '#size' => 53,
      '#default_value' => t('course_export'),
      '#description' => t('Export .tsv files only, the file suffix is added automatically.'),
      '#prefix' => '<tr><td><blockquote>',
      '#suffix' => '</blockquote></td>',
    );

    $form['export-courses'] = array(
      '#type' => 'submit',
      '#value' => t('Export Courses'),
      '#prefix' => '<td>',
      '#suffix' => '</td></tr></table></div>',
    );

    // This is required for the '#type' => 'file' form item.
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    return $form;

  }

  //------------------------------------------------------------------------//
  //    SLO FORMAT                                                          //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'slo_format') {

    // Displays the SLO.tsv file format with
    // the option to download a sample file.
    drupal_set_title(t('Student Learning Outcomes Tab Delimited File Format !help', array('!help' => $show_help)));

    $format  = '<br />To upload student learning outcomes you must first create a <b>tab delimited</b> file.';
    $format .= '<br />Example File: <b><a href="' . $base_url . '/' . $page_url . '/example_slo_format">outcome_example.tsv</a></b><br /><br /><br />';
    $format .= '<h2>Update/Modify Daedalus Outcomes</h2>';
    $format .= '<table><tr><th colspan=2>Outcome Id</th><th colspan=2>Learning Outcome Text</th><th colspan=2>Courses Covering the Outcome</th><th colspan=2>Tags</th><th>Prerequisite Learning Outcomes</th></tr>';
    $format .= '<tr><th>1</th><th>(tab)</th><th>Outcome Text 1</th><th>(tab)</th><th>Course 1</th><th>(tab)</th><th>Tag 1</th><th>(tab)</th><th></th></tr>';
    $format .= '<tr><th>2</th><th>(tab)</th><th>Outcome Text 2</th><th>(tab)</th><th>Course 2</th><th>(tab)</th><th>Tag 2, Tag 3</th><th>(tab)</th><th>1</th></tr>';
    $format .= '<tr><th>3</th><th>(tab)</th><th>Outcome Text 3</th><th>(tab)</th><th>Course 3, Course 4</th><th>(tab)</th><th>Tag 4</th><th>(tab)</th><th>1, new2</th></tr></table><br />';
    $format .= '<h2>New Daedalus Outcomes</h2>';
    $format .= '<table><tr><th colspan=2>New Id</th><th colspan=2>Learning Outcome Text</th><th colspan=2>Courses Covering the Outcome</th><th colspan=2>Tags</th><th>Prerequisite Learning Outcomes</th></tr>';
    $format .= '<tr><th>new1</th><th>(tab)</th><th>Outcome Text 1</th><th>(tab)</th><th>Course 1</th><th>(tab)</th><th>Tag 1</th><th>(tab)</th><th></th></tr>';
    $format .= '<tr><th>new2</th><th>(tab)</th><th>Outcome Text 2</th><th>(tab)</th><th>Course 2</th><th>(tab)</th><th>Tag 2, Tag 3</th><th>(tab)</th><th>new1</th></tr>';
    $format .= '<tr><th>new3</th><th>(tab)</th><th>Outcome Text 3</th><th>(tab)</th><th>Course 3, Course 4</th><th>(tab)</th><th>Tag 4</th><th>(tab)</th><th>new2, 3</th></tr></table><br />';
    $format .= '<br />Multiple form elements may contain duplicate values delimited by a comma (<b>,</b>).';
    $format .= '<br />The prerequisites can be a mix of updated, modified and new outcomes.';
    $format .= '<br />The "Id" and "Learning Outcome Text" columns can not contain multiple values.';

    $form[] = array(
      '#type' => 'item',
      '#value' => t('!format', array('!format' => $format)),
    );

    $form['return'] = array(
      '#type' => 'submit',
      '#value' => t('Return'),
    );

    return $form;

  }

  //------------------------------------------------------------------------//
  //    EXAMPLE SLO FORMAT                                                  //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'example_slo_format') {

    // Outputs the .tsv file
    $output  = "<?php\n";
    $output .= "header('Content-Type: text/tab-delimited-values');\n";
    $output .= "header('Content-Disposition: attachment; filename=\"outcome_example.tsv\"');\n\n";

    $output .= "echo\n";
    $output .= "'1\tAlgebra and Pre-calculus Skills\r\n'.\n";
    $output .= "'2\tFor Loops\tCSCI 1100, CSCI 1101\tComputer Science\r\n'.\n";
    $output .= "'3\tObject Oriented Programming\tCSCI 2110\tComuter Science, Mathmatics\t1, new1\r\n'.\n";
    $output .= "'new1\tC in Unix\tCSCI 2132\tC, Unix\t2\r\n'.\n";
    $output .= "'new2\tAlgorithm Design\tCSCI 3110\tComputer Science, Software Development\tnew1\r\n'.\n";
    $output .= "'new3\tPython Development\tGUNS 1001\tHammer Curl, Preacher Curl\t1, 2, 3, new1, new2\r\n';\n";
    $output .= "?>";

    // Write the .txt output to file
    $pwd = exec('pwd');
    $my_file = $pwd . '/graphviz/export.php';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output);
    fclose($file_handle);

    drupal_goto($base_url . '/graphviz/export.php');

  }

  //------------------------------------------------------------------------//
  //    IMPORT SLO CHECK                                                    //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'import_slo_check') {

    drupal_set_title(t('Student Learning Outcome Upload Check !help', array('!help' => $show_help)));

    $my_file = file_directory_path() . '/' . $param[1];

    // Check that the file has been saved.
    if (file_exists($my_file)) {

      $handle = fopen($my_file, 'r');

      if ($handle) {

        $input = array();

        while ($line = fgets($handle)) {

          // The tab is only recognized if the
          // double quotes are used, not single
          $data = explode("\t", $line);

          // There must be data, at least the
          // id and the Learning Outcome text.
          if (count($data) > 1) {

            $id = trim($data[0]);

            $input[$id]['id'] = $id;
            $input[$id]['outcome'] = stripslashes(trim($data[1]));
            $input[$id]['courses'] = trim(str_replace('"', '', $data[2]));
            $input[$id]['tagging'] = trim(str_replace('"', '', $data[3]));
            $input[$id]['preslos'] = trim(str_replace('"', '', $data[4]));

          }

        }

        fclose($handle);

      }

      $valid_input = FALSE;
      $slo_ids = array(); $slo_text = array();

      // Create the list of current SLO ids.
      $result = db_query("SELECT id, slo_text FROM {dae_slo} ORDER BY id");
      while ($row = db_fetch_array($result)) {

        $slo_ids[$row['id']] = $row['id'];

        $slo_text[$row['id']] = $row['slo_text'];

      }

      $dae_courses = array(); $course_info = array();

      // Create the list of current daedalus courses.
      $result = db_query("SELECT DISTINCT id, course FROM {dae_course} ORDER BY id");
      while ($row = db_fetch_array($result)) {

        $dae_courses[$row['course']] = $row['course'];

        $course_info[$row['id']] = $row['course'];

      }

      $dae_tags = array(); $tag_info = array();

      // Create the list of current daedalus tags.
      $result = db_query("SELECT * FROM {dae_tag} ORDER BY id");
      while ($row = db_fetch_array($result)) {

        $dae_tags[$row['tag_label']] = $row['tag_label'];

        $tag_info[$row['id']] = $row['tag_label'];

      }

      foreach ($input as $key => $values) {

        $input[$key]['valid'] = $valid_slo = daedalus_valid_slo($values['outcome']);

        // Check to see if the SLO has an
        // valid Daedalus identification #.
        if (is_numeric($key) && in_array($key, $slo_ids)) {
          $input[$key]['status'] = 'update';
        }

        // New courses have the word 'new' prefixed to a temporary id. Double
        // check to make sure the course does not alread exist in the database.
        elseif (preg_match('/^new/', $key)) {

          // Do not add new courses that
          // have been created previously.
          if ($valid_slo) {
            $input[$key]['status'] = 'created';
          }
          else {
            $input[$key]['status'] = 'new';
          }

        }

        // The input fails
        else {
          $input[$key]['status'] = 'failed';
        }

      }

      // Prepare the SLO URL to link the SLO text to the manage SLO page.
      $slo_url = $base_url . '/' . $page_settings['manage learning outcomes'];

      foreach ($input as $key => $values) {

        $status_flag = FALSE;

        // Display the course to be updated.
        if ($values['status'] == 'update') {

          $slo_display = '<a href="' . $slo_url . '/' . $key . '" style="color: #52514F;" target="_blank">' . $slo_text[$key] . '</a>';

          $confirmation .= '<div id="slo_' . $key . '"><b>' . $slo_display . '</b> - <i>id(' . $key . ')</i>';

          if ($values['outcome'] != $slo_text[$key]) {

            $confirmation .= '<div class="single">Learning Outcome Text:';
            $confirmation .= '<div class="double">' . $values['outcome'];
            $confirmation .= '<font class="accepted"> (accepted)</font></div></div>';
            $status_flag = TRUE; $valid_input = TRUE;

          }

            ////////           ////////
           ////////  COURSES  ////////
          ///  /////////////////  ///
          $present_courses = array(); $found_courses = array(); $course_deletion = array();

          $result = db_query("SELECT DISTINCT course_id FROM {dae_course_slo} WHERE slo_id=%d", $key);
          while ($row = db_fetch_array($result)) {

            $pcourse = $course_info[$row['course_id']];

            $present_courses[$pcourse] = $pcourse;

          }

          if ($values['courses']) {

            $courses = array();

            $courses = explode(',', $values['courses']);

            $confirmation .= '<div class="single"> Courses Covering the Learning Outcome:';

            foreach ($courses as $course) {

              $course = trim($course);

              // Keep a list of the courses that are found in the file.
              // This will be compared to those in the database, removing
              // those from the database that are found but not present
              // in the import file.
              $found_courses[$course] = $course;

              if (in_array($course, $dae_courses) && !in_array($course, $present_courses)) {
                $confirmation .= '<div class="double">' . $course . '<font class="accepted"> (accepted)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }
              elseif (in_array($course, $dae_courses)) {
                $confirmation .= '<div class="double">' . $course . '<font class="duplicate"> (duplicate)</font></div>';
              }
              else {
                $confirmation .= '<div class="double">' . $course . '<font class="failed"> (failed)</font></div>';
              }

            }

            $course_deletion = array_diff($present_courses, $found_courses);

            if ($course_deletion) {

              foreach ($course_deletion as $dcourse) {

                $dcourse = trim($dcourse);

                if ($dcourse) {   // Check for data
                  $confirmation .= '<div class="double">' . $dcourse . '<font class="failed"> (delete covering course)</font></div>';
                  $status_flag = TRUE; $valid_input = TRUE;
                }

              }

            }

            // Close the Courses Covering div.
            $confirmation .= '</div>';

          }
          elseif (!$values['courses'] && $present_courses) {

            $confirmation .= '<div class="single">Courses Covering the Learning Outcome:';

            foreach ($present_courses as $dcourse) {

              $dcourse = trim($dcourse);

              if ($dcourse) {
                $confirmation .= '<div class="double">' .  $dcourse . ' <font class="failed"> (delete covering course)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }

            }

            // Close the Courses Covering div.
            $confirmation .= '</div>';

          }

            ////////        ////////
           ////////  TAGS  ////////
          ///  //////////////  ///
          $present_tags = array(); $found_tags = array(); $tag_deletion = array();

          // Get the tags that are associated to the SLO.
          $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE slo_id=%d", $key);
          while ($row = db_fetch_array($result)) {

            $ptag = $tag_info[$row['tag_id']];

            $present_tags[$ptag] = $ptag;

          }

          if ($values['tagging']) {

            $tagging = array();

            $tagging = explode(',', $values['tagging']);

            $confirmation .= '<div class="single">Tags:';

            foreach ($tagging as $tag) {

              $tag = trim($tag);

              // Keep a list of the courses that are found in the file.
              // This will be compared to those in the database, removing
              // those from the database that are found but not present
              // in the import file.
              $found_tags[$tag] = $tag;

              if (in_array($tag, $dae_tags) && !in_array($tag, $present_tags)) {
                $confirmation .= '<div class="double">' . $tag . '<font class="accepted"> (accepted)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }
              elseif (in_array($tag, $dae_tags)) {
                $confirmation .= '<div class="double">' . $tag . '<font class="duplicate"> (duplicate)</font></div>';
              }
              else {
                $dae_tags[$tag] = $tag;
                $confirmation .= '<div class="double">' . $tag . '<font class="accepted"> (new tag)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }

            }

            // If tags are not found in the file for a given
            // SLO, delete the tags that are no longer present.
            $tag_deletion = array_diff($present_tags, $found_tags);

            if ($tag_deletion) {

              foreach ($tag_deletion as $dtag) {

                $dtag = trim($dtag);

                if ($dtag) {
                  $confirmation .= '<div class="double">' . $dtag . '<font class="failed"> (delete tag)</font></div>';
                  $status_flag = TRUE; $valid_input = TRUE;
                }

              }

            }

            // Close the Tags div.
            $confirmation .= '</div>';

          }
          elseif (!$values['tagging'] && $present_tags) {

            $confirmation .= '<div class="single"> Tags:';

            foreach ($present_tags as $dtag) {

              $dtag = trim($dtag);

              if ($dtag) {
                $confirmation .= '<div class="double">' .  $dtag . '<font class="failed"> (delete tag)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }

            }

            // Close the Tags div.
            $confirmation .= '</div>';

          }

            ////////            ////////
           ////////  OUTCOMES  ////////
          ///  //////////////////  ///
          $present_preslos = array(); $found_preslos = array(); $preslo_deletion = array();

          $result = db_query("SELECT DISTINCT pre_slo FROM {dae_prereq_slo} WHERE target=%d", $key);
          while ($row = db_fetch_array($result)) {
            $present_preslos[$row['pre_slo']] = $row['pre_slo'];
          }

          if ($values['preslos']) {

            $preslos = array();

            $preslos = explode(',', $values['preslos']);

            $confirmation .= '<div class="single">Prerequisite Learning Outcomes:';

            foreach ($preslos as $preslo) {

              $preslo = trim($preslo);

              // Keep a list of the prerequisites that are found in the file.
              // This will be compared to those in the database, removing those
              // from the database that are found but not present in the import file.
              $found_preslos[$preslo] = $preslo;

              if (preg_match('/^new/', $preslo)) {

                if ($input[$preslo]['status'] == 'new') {
                  $confirmation .= '<div class="double">' . $input[$preslo]['outcome'];
                  $confirmation .= '<font class="duplicate"> (new outcome)</font></div>';
                  $status_flag = TRUE; $valid_input = TRUE;
                }
                else {
                  $confirmation .= '<div class="double">' . $input[$preslo]['outcome'];
                  $confirmation .= '<font class="failed"> (failed)</font></div>';
                }

              }
              elseif (in_array($preslo, $slo_ids) && !in_array($preslo, $present_preslos)) {
                $confirmation .= '<div class="double">' . $slo_text[$preslo];
                $confirmation .= ' <font class="accepted">(accepted)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }
              elseif (in_array($preslo, $present_preslos)) {
                $confirmation .= '<div class="double"> ' . $slo_text[$preslo];
                $confirmation .= ' <font class="duplicate">(duplicate)</font></div>';
              }
              elseif ($input[$preslo]['status'] == 'update') {
                $confirmation .= '<div class="double">' . $slo_text[$preslo];
                $confirmation .= ' <font class="accepted">(accepted)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }
              elseif ($input[$preslo]['status'] == 'failed') {
                $confirmation .= '<div class="double">' . $slo_text[$preslo];
                $confirmation .= ' <font class="failed"> (failed)</font></div>';
              }
              else {
                $confirmation .= '<div class="double">Outcome Id ' . $preslo;
                $confirmation .= ' <font class="failed"> (failed)</font></div>';
              }

            }

            $preslo_deletion = array_diff($present_preslos, $found_preslos);

            if ($preslo_deletion) {

              foreach ($preslo_deletion as $dslo) {

                if ($dslo) { // Check for data
                  $confirmation .= '<div class="double">' . $slo_text[$dslo] . '<font class="failed">';
                  $confirmation .= ' (delete prerequisite outcome)</font></div>';
                  $status_flag = TRUE; $valid_input = TRUE;
                }

              }

            }

            // Close the Prerequisite SLOs div.
            $confirmation .= '</div>';

          }
          elseif (!$values['preslos'] && $present_preslos) {

            $confirmation .= '<div class="single">Prerequisite Learning Outcomes:';

            foreach ($present_preslos as $dslo) {

              if ($dslo) {
                $confirmation .= '<div class="double"> ' . $slo_text[$dslo] . '<font class="failed">';
                $confirmation .= ' (delete prerequisite outcome)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }

            }

            // Close the Prerequisite SLOs div.
            $confirmation .= '</div>';

          }

          // Display the status
          if ($status_flag) {
            $confirmation .= '<div class="single">Status: <font color="Green">';
            $confirmation .= 'Update Student Learning Outcome</font></div>';
          }
          else {
            $confirmation .= '<div class="single">Status: <font color="GoldenRod">';
            $confirmation .= 'No Changes Detected</font></div>';
          }

          $confirmation .= '<hr></div>';  // Close the div for the slo.

        }

        // New
        elseif ($values['status'] == 'new') {

          $valid_input = TRUE;

          $confirmation .= '<div id="slo_' . $key . '"><b>' . $values['outcome'] . '</b> - <i>id(' . $key . ')</i>';

            ////////   NEW     ////////
           ////////  COURSES  ////////
          ///  /////////////////  ///
          if ($values['courses']) {

            $courses = explode(',', $values['courses']);

            $confirmation .= '<div class="single">Courses Covering the Learning Outcome:';

            foreach ($courses as $course) {

              $course = trim($course);

              if (in_array($course, $dae_courses)) {
                $confirmation .= '<div class="double">' . $course . '<font class="accepted"> (accepted)</font></div>';
              }
              else {
                $confirmation .= '<div class="double">' . $course . '<font class="failed"> (failed)</font></div>';
              }

            }

            // Close the New Courses div.
            $confirmation .= '</div>';

          }

            ////////  NEW   ////////
           ////////  TAGS  ////////
          ///  //////////////  ///
          if ($values['tagging']) {

            $tagging = explode(',', $values['tagging']);

            $confirmation .= '<div class="single">Tags:';

            foreach ($tagging as $tag) {

              $tag = trim($tag);

              if (in_array($tag, $dae_tags)) {
                $confirmation .= '<div class="double">' . $tag . '<font class="accepted"> (accepted)</font></div>';
              }
              else {
                $dae_tags[$tag] = $tag;
                $confirmation .= '<div class="double">' . $tag . '<font class="accepted"> (new tag)</font></div>';
              }

            }

            // Close the New Tags div.
            $confirmation .= '</div>';

          }

            ////////    NEW     ////////
           ////////  OUTCOMES  ////////
          ///  //////////////////  ///
          if ($values['preslos']) {

            $preslos = explode(',', $values['preslos']);

            $confirmation .= '<div class="single">Prerequisite Learning Outcomes:';

            foreach ($preslos as $preslo) {

              $preslo = trim($preslo);

              if (preg_match('/^new/', $preslo)) {
                $confirmation .= '<div class="double">' . $input[$preslo]['outcome'];
                $confirmation .= '<font class="duplicate"> (new outcome)</font></div>';
              }
              elseif (in_array($preslo, $slo_ids)) {
                $confirmation .= '<div class="double">' . $slo_text[$preslo];
                $confirmation .= ' <font class="accepted">(accepted)</font></div>';
              }
              elseif ($input[$preslo]['status'] == 'update') {
                $confirmation .= '<div class="double">' . $input[$preslo]['outcome'];
                $confirmation .= '<font class="accepted"> (accepted)</font></div>';
              }
              elseif ($input[$preslo]['status'] == 'failed') {
                $confirmation .= '<div class="double">' . $input[$preslo]['outcome'];
                $confirmation .= '<font class="failed"> (failed)</font></div>';
              }
              else {
                $confirmation .= '<div class="double">Outcome Id ' . $preslo . '<font class="failed"> (failed)</font></div>';
              }

            }

            // Close the New Prerequisite SLOs div.
            $confirmation .= '</div>';

          }

          $confirmation .= '<div class="single">Status: <font color="Green">New Learning Outcome</font></div>';
          $confirmation .= '<hr></div>'; // Close the div for the new slo.

        }

        elseif ($values['status'] == 'created') {
          $confirmation .= '<div><b>' . $values['outcome'] . '</b> - <i>id(' . $key . ')</i>';
          $confirmation .= '<div><div class="single">Status: ';
          $confirmation .= '<font color="GoldenRod">Learning Outcome Previously Created</font></div>';
          $confirmation .= '<hr></div>';
        }

        elseif ($values['status'] == 'failed') {
          $confirmation .= '<div><b>' . $values['outcome'] . '</b> - <i>id(' . $key . ')</i>';
          $confirmation .= '<div class="single">Status: ';
          $confirmation .= '<font color="Red">Learning Outcome Failed</font></div>';
          $confirmation .= '<hr></div>';
        }

        else {
          $confirmation .= '<div><b>' . $values['outcome'] . '</b> - <i>id(' . $key . ')</i>';
          $confirmation .= '<div class="single">Status: ';
          $confirmation .= '<font color="GoldenRod">Course Previously Created</font></div>';
          $confirmation .= '<hr></div>';
        }

      }

      $form[] = array(
        '#type' => 'item',
        '#value' => $confirmation,
      );

      $match = '(failed)';
      $failed_input = strpos($confirmation, $match);

      $match = 'Learning Outcome Failed';
      $failed_outcome = strpos($confirmation, $match);

      // Note our use of ===.  Simply == would not work as expected
      // because the position of 'a' was the 0th (first) character.
      if ($failed_input !== FALSE || $failed_outcome !== FALSE) {

        drupal_set_message(t('One or more fields have failed the upload check.
          Please check the learning outcomes for errors before uploading.'), 'error');

      }
      elseif ($valid_input) {

        $form['confirm-slos'] = array(
          '#type' => 'submit',
          '#value' => t('Confirm Learning Outcome Information'),
        );

      }

      $form['return-delete'] = array(
        '#type' => 'submit',
        '#value' => t('Return'),
      );

      // Pass the hidden filename
      $form['filename'] = array( '#type' => 'value', '#value' => $param[1] );

    }
    else {
      drupal_set_message(t('The uploaded outcome file has been deleted.'), 'warning');
    }

    return $form;

  }

  //------------------------------------------------------------------------//
  //    IMPORT SLOS                                                         //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'import_slos') {

    drupal_set_message(t('Only modified items are displayed.'), 'warning');

    drupal_set_title(t('Student Learning Outcome Upload Confirmation !help', array('!help' => $show_help)));

    $my_file = file_directory_path() . '/' . $param[1];

    // Check that the file has been saved.
    if (file_exists($my_file)) {

      $handle = fopen($my_file, 'r');

      if ($handle) {

        $input = array();

        while ($line = fgets($handle)) {

          // The tab is only recognized if the
          // double quotes are used, not single
          $data = explode("\t", $line);

          // There must be data, at least the
          // id and the Learning Outcome text.
          if (count($data) > 1) {

            $id = trim($data[0]);

            $input[$id]['id'] = $id;
            $input[$id]['outcome'] = stripslashes(trim($data[1]));
            $input[$id]['courses'] = trim(str_replace('"', '', $data[2]));
            $input[$id]['tagging'] = trim(str_replace('"', '', $data[3]));
            $input[$id]['preslos'] = trim(str_replace('"', '', $data[4]));

          }

        }

        fclose($handle);

      }

      $slo_ids = array(); $slo_text = array(); $slo_id = array();

      // Create the list of current SLO ids.
      $result = db_query("SELECT id, slo_text FROM {dae_slo} ORDER BY id");
      while ($row = db_fetch_array($result)) {

        $slo_ids[$row['id']] = $row['id'];

        $slo_text[$row['id']] = $row['slo_text'];

        $slo_id[$row['outcome']] = $row['id'];

      }

      $dae_courses = array(); $course_info = array(); $course_id_info = array();

      // Create the list of current daedalus courses.
      $result = db_query("SELECT DISTINCT id, course FROM {dae_course} ORDER BY id");
      while ($row = db_fetch_array($result)) {

        $dae_courses[$row['course']] = $row['course'];

        $course_info[$row['id']] = $row['course'];

        $course_id_info[$row['course']] = $row['id'];

      }

      $dae_tags = array(); $tag_info = array(); $tag_id_info = array();

      // Create the list of current daedalus courses.
      $result = db_query("SELECT * FROM {dae_tag} ORDER BY id");
      while ($row = db_fetch_array($result)) {

        $dae_tags[$row['tag_label']] = $row['tag_label'];

        $tag_info[$row['id']] = $row['tag_label'];

        $tag_id_info[$row['tag_label']] = $row['id'];

      }

      $new_outcome_args = array();

      // Determine the status of each SLO
      foreach ($input as $key => $values) {

        $input[$key]['valid'] = $valid_slo = daedalus_valid_slo($values['outcome']);

        // Check to see if the SLO has an
        // valid Daedalus identification #.
        if (is_numeric($key) && in_array($key, $slo_ids)) {

          $input[$key]['status'] = 'update';

          if ($values['outcome'] != $slo_text[$key]) {
            db_query("UPDATE {dae_slo} SET slo_text='%s' WHERE id=%d", $values['outcome'], $key);
          }

        }

        // New outcomes have the word 'new'
        // prefixed to a temporary id.
        elseif (preg_match('/^new/', $key)) {

          // Do not add new learning outcomes that
          // have been created previously.
          if ($valid_slo) {
            $input[$key]['status'] = 'created';
          }
          else {

            $input[$key]['status'] = 'new';

            // Add the values to an argument array
            // to be inserted in one query below.
            $new_outcome_args[] = $values['outcome'];

          }

        }
        else {
          // The input fails
          $input[$key]['status'] = 'failed';
        }

      }

      // Insert all new outcomes.
      if ($new_outcome_args) {

        $new_placeholders = implode(', ', array_fill(0, count($new_outcome_args), "('%s')"));

        db_query("INSERT INTO {dae_slo} (slo_text) VALUES " . $new_placeholders, $new_outcome_args);

      }

      $manage_slo_url = $base_url . '/' . $page_settings['manage learning outcomes'];

      $new_covering_args = array(); $new_tag_args = array(); $new_preslo_args = array();
      $new_covering_count = 0; $new_tag_count = 0; $new_preslo_count = 0;

      $delete_covering_args = array(); $delete_tag_args = array(); $delete_preslo_args = array();
      $delete_covering_count = 0; $delete_tag_count = 0; $delete_preslo_count = 0;

      foreach ($input as $key => $values) {

        $current_confirmation = '';
        $covering_confirmation = '';
        $tag_confirmation = '';
        $preslo_confirmation = '';

        // The status flag determines...
        $status_flag = FALSE;

        // Display the course to be updated.
        if ($values['status'] == 'update') {

          $current_confirmation .= '<a href="' . $manage_slo_url . '/' . $key . '" target="_blank"><b>';
          $current_confirmation .= $values['outcome'] . '</b></a> - <i>id(' . $key . ')</i>';

          if ($values['outcome'] != $slo_text[$key]) {

            $current_confirmation .= '<div class="single">Learning Outcome Text:';
            $current_confirmation .= '<div class="double">' . $values['outcome'];
            $current_confirmation .= '<font class="accepted"> (outcome text accepted)</font></div></div>';
            $status_flag = TRUE;

          }

            ////////           ////////
           ////////  COURSES  ////////
          ///  /////////////////  ///
          $present_courses = array(); $found_courses = array(); $course_deletion = array();

          $result = db_query("SELECT DISTINCT course_id FROM {dae_course_slo} WHERE slo_id=%d", $key);
          while ($row = db_fetch_array($result)) {

            $pcourse = $course_info[$row['course_id']];

            $present_courses[$pcourse] = $pcourse;

          }

          // Set a flag for the covering
          // courses to display or not.
          $covering_flag = FALSE;

          if ($values['courses']) {

            $courses = array();

            $courses = explode(',', $values['courses']);

            foreach ($courses as $course) {

              $course = trim($course);

              // Keep a list of the courses that are found in the file.
              // This will be compared to those in the database, removing
              // those from the database that are found but not present
              // in the import file.
              $found_courses[$course] = $course;

              if (in_array($course, $dae_courses) && !in_array($course, $present_courses)) {

                $new_covering_args[] = $key;
                $new_covering_args[] = $course_id_info[$course];
                $new_covering_count++;

                $covering_confirmation .= '<div class="double">' . $course;
                $covering_confirmation .= '<font class="accepted"> (covering course accepted)</font></div>';

                $covering_flag = TRUE;

              }

            }

            $course_deletion = array_diff($present_courses, $found_courses);

            if ($course_deletion) {

              foreach ($course_deletion as $dcourse) {

                $dcourse = trim($dcourse);

                $delete_covering_args[] = $key;
                $delete_covering_args[] = $course_id_info[$dcourse];
                $delete_covering_count++;

                $covering_confirmation .= '<div class="double">' . $dcourse;
                $covering_confirmation .= '<font class="failed"> (covering course deleted)</font></div>';

                $covering_flag = TRUE;

              }

            }

          }
          elseif (!$values['courses'] && $present_courses) {

            foreach ($present_courses as $dcourse) {

              $dcourse = trim($dcourse);

              $delete_covering_args[] = $key;
              $delete_covering_args[] = $course_id_info[$dcourse];
              $delete_covering_count++;

              $covering_confirmation .= '<div class="double">' .  $dcourse;
              $covering_confirmation .= '<font class="failed"> (covering course deleted)</font></div>';

              $covering_flag = TRUE;

            }

          }

          if ($covering_flag) {

            $status_flag = TRUE;

            $current_confirmation .= '<div class="single">Courses Covering the Learning Outcome:';
            $current_confirmation .= $covering_confirmation . '</div>';

          }

            ////////        ////////
           ////////  TAGS  ////////
          ///  //////////////  ///
          $present_tags = array(); $found_tags = array(); $tag_deletion = array();

          $result = db_query("SELECT DISTINCT tag_id FROM {dae_slo_tag} WHERE slo_id=%d", $key);
          while ($row = db_fetch_array($result)) {

            $ptag = $tag_info[$row['tag_id']];

            $present_tags[$ptag] = $ptag;

          }

          // Set a flag for the tags.
          $tag_flag = FALSE;

          if ($values['tagging']) {

            $tagging = array();

            $tagging = explode(',', $values['tagging']);

            foreach ($tagging as $tag) {

              $tag = trim($tag);

              // Keep a list of the courses that are found in the file.
              // This will be compared to those in the database, removing
              // those from the database that are found but not present
              // in the import file.
              $found_tags[$tag] = $tag;

              if (in_array($tag, $dae_tags) && !in_array($tag, $present_tags)) {

                $new_tag_args[] = $key;
                $new_tag_args[] = $tag_id_info[$tag];
                $new_tag_count++;

                $tag_confirmation .= '<div class="double">' . $tag;
                $tag_confirmation .= '<font class="accepted"> (tag accepted)</font></div>';

                $tag_flag = TRUE;

              }
              elseif (in_array($tag, $dae_tags)) {
                // Do nothing
              }
              else {

                db_query("INSERT INTO {dae_tag} (tag_label) VALUES ('%s')", $tag);
                $new_tag_id = db_result(db_query("SELECT id FROM {dae_tag} WHERE tag_label = '%s'", $tag));

                // Add the tag info so the id may be
                // referenced by the lable later on.
                $dae_tags[$tag] = $tag;
                $tag_info[$new_tag_id] = $tag;
                $tag_id_info[$tag] = $new_tag_id;

                $new_tag_args[] = $key;
                $new_tag_args[] = $new_tag_id;
                $new_tag_count++;

                $tag_confirmation .= '<div class="double">' . $tag;
                $tag_confirmation .= '<font class="accepted"> (new tag)</font></div>';

                $tag_flag = TRUE;

              }

            }

            $tag_deletion = array_diff($present_tags, $found_tags);

            if ($tag_deletion) {

              foreach ($tag_deletion as $dtag) {

                $dtag = trim($dtag);

                $delete_tag_args[] = $key;
                $delete_tag_args[] = $tag_id_info[$dtag];
                $delete_tag_count++;

                $tag_confirmation .= '<div class="double">' . $dtag;
                $tag_confirmation .= '<font class="failed"> (tag deleted)</font></div>';

                $tag_flag = TRUE;

              }

            }

          }
          elseif (!$values['tagging'] && $present_tags) {

            foreach ($present_tags as $dtag) {

              $dtag = trim($dtag);

              $delete_tag_args[] = $key;
              $delete_tag_args[] = $tag_id_info[$dtag];
              $delete_tag_count++;

              $tag_confirmation .= '<div class="double">' .  $dtag;
              $tag_confirmation .= '<font class="failed"> (tag deleted)</font></div>';

              $tag_flag = TRUE;

            }

          }

          if ($tag_flag) {

            $current_confirmation .= '<div class="single">Tags:';
            $current_confirmation .= $tag_confirmation . '</div>';

            $status_flag = TRUE;

          }

            ////////            ////////
           ////////  OUTCOMES  ////////
          ///  //////////////////  ///
          $present_preslos = array(); $found_preslos = array(); $preslo_deletion = array();

          $result = db_query("SELECT DISTINCT pre_slo FROM {dae_prereq_slo} WHERE target=%d", $key);
          while ($row = db_fetch_array($result)) {
            $present_preslos[$row['pre_slo']] = $row['pre_slo'];
          }

          $preslo_flag = FALSE;

          if ($values['preslos']) {

            $preslos = array();

            $preslos = explode(',', $values['preslos']);

            foreach ($preslos as $preslo) {

              $preslo = trim($preslo);

              // Keep a list of the prerequisites that are found in the file.
              // This will be compared to those in the database, removing those
              // from the database that are found but not present in the import file.
              $found_preslos[$preslo] = $preslo;

              if (preg_match('/^new/', $preslo)) {

                if ($input[$preslo]['status'] == 'new') {

                  $new_sloid = daedalus_slo_id($input[$preslo]['outcome']);

                  $new_preslo_args[] = $key;
                  $new_preslo_args[] = $new_sloid;
                  $new_preslo_count++;

                  $preslo_confirmation .= '<div class="double">' . $input[$preslo]['outcome'];
                  $preslo_confirmation .= '<font class="accepted"> (outcome accepted)</font></div>';

                  $preslo_flag = TRUE;

                }

              }
              elseif (in_array($preslo, $slo_ids) && !in_array($preslo, $present_preslos)) {

                $new_preslo_args[] = $key;
                $new_preslo_args[] = $preslo;
                $new_preslo_count++;

                $preslo_confirmation .= '<div class="double">' . $slo_text[$preslo];
                $preslo_confirmation .= '<font class="accepted"> (outcome accepted)</font></div>';

                $preslo_flag = TRUE;

              }
              elseif (in_array($preslo, $present_preslos)) {
                // Do nothing
              }
              elseif ($input[$preslo]['status'] == 'update') {

                $new_preslo_args[] = $key;
                $new_preslo_args[] = $preslo;
                $new_preslo_count++;

                $preslo_confirmation .= '<div class="double">' . $slo_text[$preslo];
                $preslo_confirmation .= '<font class="accepted"> (outcome accepted)</font></div>';

                $preslo_flag = TRUE;

              }

            }

            $preslo_deletion = array_diff($present_preslos, $found_preslos);

            if ($preslo_deletion) {

              foreach ($preslo_deletion as $dslo) {

                $delete_preslo_args[] = $key;
                $delete_preslo_args[] = $dslo;
                $delete_preslo_count++;

                $preslo_confirmation .= '<div class="double">' . $slo_text[$dslo];
                $preslo_confirmation .= '<font class="failed"> (prerequisite outcome deleted)</font></div>';

                $preslo_flag = TRUE;

              }

            }

          }
          elseif (!$values['preslos'] && $present_preslos) {

            foreach ($present_preslos as $dslo) {

              $delete_preslo_args[] = $key;
              $delete_preslo_args[] = $dslo;
              $delete_preslo_count++;

              $preslo_confirmation .= '<div class="double">' . $slo_text[$dslo];
              $preslo_confirmation .= '<font class="failed"> (prerequisite outcome deleted)</font></div>';

              $preslo_flag = TRUE;

            }

          }

          if ($preslo_flag) {

            $current_confirmation .= '<div class="single">Prerequisite Learning Outcomes:';
            $current_confirmation .= $preslo_confirmation . '</div>';

            $status_flag = TRUE;

          }

          // Display the status
          if ($status_flag) {
            $confirmation .= '<div id="slo_' . $key . '">' . $current_confirmation;
            $confirmation .= '<div class="single">Status: ';
            $confirmation .= '<font color="Green">Student Learning Outcome Updated</font></div>';
            $confirmation .= '<hr></div>';
          }

        }

        // New
        elseif ($values['status'] == 'new') {

          $new_slo_id = daedalus_slo_id($values['outcome']);

          $confirmation .= '<div id="slo_' . $key . '"><a href="' . $manage_slo_url . '/' . $new_slo_id . '" target="_blank">';
          $confirmation .= '<b>' . $values['outcome'] . '</b></a> - <i>id(' . $new_slo_id . ')</i>';

            ////////   NEW     ////////
           ////////  COURSES  ////////
          ///  /////////////////  ///
          if ($values['courses']) {

            $courses = explode(',', $values['courses']);

            $confirmation .= '<div class="single">Courses Covering the Learning Outcome:';

            foreach ($courses as $course) {

              $course = trim($course);

              if (in_array($course, $dae_courses)) {

                $new_covering_args[] = $new_slo_id;
                $new_covering_args[] = $course_id_info[$course];
                $new_covering_count++;

                $confirmation .= '<div class="double">' . $course;
                $confirmation .= '<font class="accepted"> (covering course accepted)</font></div>';

              }

            }

            // Close the New Courses div.
            $confirmation .= '</div>';

          }

            ////////  NEW   ////////
           ////////  TAGS  ////////
          ///  //////////////  ///
          if ($values['tagging']) {

            $tagging = explode(',', $values['tagging']);

            $confirmation .= '<div class="single">Tags:';

            foreach ($tagging as $tag) {

              $tag = trim($tag);

              if (in_array($tag, $dae_tags)) {

                $new_tag_args[] = $new_slo_id;
                $new_tag_args[] = $tag_id_info[$tag];
                $new_tag_count++;

                $confirmation .= '<div class="double">' . $tag;
                $confirmation .= '<font class="accepted"> (tag accepted)</font></div>';

              }
              else {

                db_query("INSERT INTO {dae_tag} (tag_label) VALUES ('%s')", $tag);
                $new_tag_id = db_result(db_query("SELECT id FROM {dae_tag} WHERE tag_label='%s'", $tag));

                // Add the tag info so the id may be
                // referenced by the lable later on.
                $dae_tags[$tag] = $tag;
                $tag_info[$new_tag_id] = $tag;
                $tag_id_info[$tag] = $new_tag_id;

                $new_tag_args[] = $new_slo_id;
                $new_tag_args[] = $new_tag_id;
                $new_tag_count++;

                $confirmation .= '<div class="double">' . $tag;
                $confirmation .= '<font class="accepted"> (new tag)</font></div>';

              }

            }

            // Close the New Tags div.
            $confirmation .= '</div>';

          }

            ////////    NEW     ////////
           ////////  OUTCOMES  ////////
          ///  //////////////////  ///
          if ($values['preslos']) {

            $preslos = explode(',', $values['preslos']);

            $confirmation .= '<div class="single">Prerequisite Learning Outcomes:';

            foreach ($preslos as $preslo) {

              $preslo = trim($preslo);

              if (preg_match('/^new/', $preslo) || $input[$preslo]['status'] == 'update') {

                $new_preslo_id = daedalus_slo_id($input[$preslo]['outcome']);

                $new_preslo_args[] = $new_slo_id;
                $new_preslo_args[] = $new_preslo_id;
                $new_preslo_count++;

                $confirmation .= '<div class="double">' . $input[$preslo]['outcome'];
                $confirmation .= '<font class="accepted"> (outcome accepted)</font></div>';

              }
              elseif (in_array($preslo, $slo_ids)) {

                $new_preslo_args[] = $new_slo_id;
                $new_preslo_args[] = $preslo;
                $new_preslo_count++;

                $confirmation .= '<div class="double">' . $slo_text[$preslo];
                $confirmation .= '<font class="accepted"> (outcome accepted)</font></div>';

              }
              elseif ($input[$preslo]['status'] == 'update') {

                $new_preslo_args[] = $new_slo_id;
                $new_preslo_args[] = $preslo;
                $new_preslo_count++;

                $confirmation .= '<div class="double">' . $slo_text[$preslo];
                $confirmation .= '<font class="accepted"> (outcome accepted)</font></div>';

              }

            }

            // Close the New Prerequisite SLOs div.
            $confirmation .= '</div>';

          }

          $confirmation .= '<div class="single">Status: ';
          $confirmation .= '<font color="Green">New Learning Outcome</font></div>';
          $confirmation .= '<hr></div>';

        }

      }

      if ($new_covering_count) {

        $new_placeholders = implode(', ', array_fill(0, $new_covering_count, "(%d, %d)"));

        db_query("INSERT INTO {dae_course_slo} (slo_id, course_id) VALUES " . $new_placeholders, $new_covering_args);

      }

      if ($new_tag_count) {

        $new_placeholders = implode(', ', array_fill(0, $new_tag_count, "(%d, %d)"));

        db_query("INSERT INTO {dae_slo_tag} (slo_id, tag_id) VALUES " . $new_placeholders, $new_tag_args);

      }

      if ($new_preslo_count) {

        $new_placeholders = implode(', ', array_fill(0, $new_preslo_count, "(%d, %d)"));

        db_query("INSERT INTO {dae_prereq_slo} (target, pre_slo) VALUES " . $new_placeholders, $new_preslo_args);

      }

      if ($delete_covering_count) {

        $delete_placeholders = implode(' OR ', array_fill(0, $delete_covering_count, "(slo_id=%d AND course_id=%d)"));

        db_query("DELETE FROM {dae_course_slo} WHERE " . $delete_placeholders, $delete_covering_args);

      }

      if ($delete_tag_count) {

        $delete_placeholders = implode(' OR ', array_fill(0, $delete_tag_count, "(slo_id=%d AND tag_id=%d)"));

        db_query("DELETE FROM {dae_slo_tag} WHERE " . $delete_placeholders, $delete_tag_args);

      }

      if ($delete_preslo_count) {

        $delete_placeholders = implode(' OR ', array_fill(0, $delete_preslo_count, "(target=%d AND pre_slo=%d)"));

        db_query("DELETE FROM {dae_prereq_slo} WHERE " . $delete_placeholders, $delete_preslo_args);

      }

      // Calculate the new SLO ranks
      // after the SLO modifications.
      daedalus_calculate_slo_rank();

      // Remove the file once the data has been stored.
      system('rm ' . file_directory_path() . '/' . $param[1]);

      $form[] = array(
        '#type' => 'item',
        '#value' => t($confirmation),
      );

      $form[] = array(
        '#type' => 'item',
        '#value' => '<input type="button" class="form-submit" value="Return" />',
        '#prefix' => '<a href="' . $base_url . '/' . $page_url . '" style="text-decoration:none" >',
        '#suffix' => '</a>',
      );

      return $form;

    }
    else {
      drupal_set_message(t('The uploaded outcome file has been deleted.'), 'warning');
    }

  }

  //------------------------------------------------------------------------//
  //    COURSE FORMAT                                                       //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'course_format') {

    drupal_set_title(t('Course Tab Delimited File Format !help', array('!help' => $show_help)));

    $format  = '<br />To upload course information you must first create a <b>tab delimited</b> text file.';
    $format .= '<br />Example File: <b><a href="' . $base_url . '/' . $page_url . '/example_course_format">course_example.tsv</a></b><br /><br /><br />';
    $format .= '<h2>Update/Modify Daedalus Courses</h2>';
    $format .= '<table><tr><th colspan=2>Course Id</th><th colspan=2>Course Code</th><th colspan=2>Course Number</th><th colspan=2>Course Name</th><th>Prerequisite Courses</th></tr>';
    $format .= '<tr><th>1</th><th>(tab)</th><th>Course Code 1</th><th>(tab)</th><th>Course Number 1</th><th>(tab)</th><th>Course Name 1</th><th>(tab)</th><th></th></tr>';
    $format .= '<tr><th>2</th><th>(tab)</th><th>Course Code 2</th><th>(tab)</th><th>Course Number 2</th><th>(tab)</th><th>Course Name 2</th><th>(tab)</th><th>1</th></tr>';
    $format .= '<tr><th>3</th><th>(tab)</th><th>Course Code 3</th><th>(tab)</th><th>Course Number 3</th><th>(tab)</th><th>Course Name 3</th><th>(tab)</th><th>1, new2</th></tr></table><br />';
    $format .= '<h2>New Daedalus Courses</h2>';
    $format .= '<table><tr><th colspan=2>New Id</th><th colspan=2>Course Code</th><th colspan=2>Course Number</th><th colspan=2>Course Name</th><th>Prerequisite Courses</th></tr>';
    $format .= '<tr><th>new1</th><th>(tab)</th><th>Course Code 1</th><th>(tab)</th><th>Course Number 1</th><th>(tab)</th><th>Course Name 1</th><th>(tab)</th><th></th></tr>';
    $format .= '<tr><th>new2</th><th>(tab)</th><th>Course Code 2</th><th>(tab)</th><th>Course Number 2</th><th>(tab)</th><th>Course Name 2</th><th>(tab)</th><th>1</th></tr>';
    $format .= '<tr><th>new3</th><th>(tab)</th><th>Course Code 3</th><th>(tab)</th><th>Course Number 3</th><th>(tab)</th><th>Course Name 3</th><th>(tab)</th><th>new1, new2</th></tr></table><br />';
    $format .= '<br />The Prerequisite Course form element may contain duplicate values delimited by a comma (<b>,</b>).';
    $format .= '<br />The prerequisites can be a mix of updated, modified and new outcomes.';

    $form[] = array(
      '#type' => 'item',
      '#value' => t('!format', array('!format' => $format)),
    );

    $form['return'] = array(
      '#type' => 'submit',
      '#value' => t('Return'),
    );

    return $form;

  }

  //------------------------------------------------------------------------//
  //    EXAMPLE COURSE FORMAT                                               //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'example_course_format') {

    $output  = "<?php\n";
    $output .= "header('Content-Type: text/tab-delimited-values');\n";
    $output .= "header('Content-Disposition: attachment; filename=\"course_example.tsv\"');\n\n";

    $output .= "echo\n";
    $output .= "'1\tCSCI\t1100\tComputer Science I\r\n'.\n";
    $output .= "'2\tCSCI\t1101\tComputer Science II\t1\r\n'.\n";
    $output .= "'3\tCSCI\t2110\tComputer Science III\t1, new1\r\n'.\n";
    $output .= "'new1\tCSCI\t2132\tSoftware Development\r\n'.\n";
    $output .= "'new2\tCSCI\t2121\tComputer Architecture\t2\r\n'.\n";
    $output .= "'new3\tCSCI\t3110\tAlgorith Design\tnew1, 3\r\n';\n";
    $output .= "?>";

    // Write the .txt output to file
    $pwd = exec('pwd');
    $my_file = $pwd . '/graphviz/export.php';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output);
    fclose($file_handle);

    drupal_goto($base_url . '/graphviz/export.php');

  }

  //------------------------------------------------------------------------//
  //    IMPORT COURSES CHECK                                                //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'import_courses_check') {

    drupal_set_title(t('Course Upload Check !help', array('!help' => $show_help)));

    $my_file = file_directory_path() . '/' . $param[1];

    // Check that the file has been saved.
    if (file_exists($my_file)) {

      $handle = fopen($my_file, 'r');

      if ($handle) {

        $input = array(); $new_codes = array();

        // Read the file and save the data.
        while ($line = fgets($handle)) {

          // The tab is only recognized if the
          // double quotes are used, not single
          $data = explode("\t", $line);

          // There must be data. At least the id, course
          // code with course number and the course name.
          if (count($data) > 2) {

            $course_id = trim($data[0]);
            $course_code = trim($data[1]);

            $input[$course_id]['id'] = $course_id;
            $input[$course_id]['cocode'] = $course_code;
            $input[$course_id]['conumb'] = trim($data[2]);
            $input[$course_id]['coname'] = trim($data[3]);
            $input[$course_id]['prereq'] = trim(str_replace('"', '', $data[4]));

            // Add the unique course codes from the file to
            // check against the list of valid course codes.
            if (!in_array($course_code, $new_codes)) {
              $new_codes[$course_code] = $course_code;
            }

          }

        }

        fclose($handle);

      }

      $valid_input = FALSE;
      $course_ids = $course_info = array();

      // Create the list of current course ids.
      $result = db_query("SELECT id, course, course_name, course_code, course_number
        FROM {dae_course} ORDER BY id");

      while ($row = db_fetch_array($result)) {

        $course_ids[$row['id']] = $row['id'];

        // Add a course info array to check
        // input values for any changes.
        $course_info[$row['id']]['course'] = $row['course'];
        $course_info[$row['id']]['cocode'] = $row['course_code'];
        $course_info[$row['id']]['conumb'] = $row['course_number'];
        $course_info[$row['id']]['coname'] = $row['course_name'];

      }

      // Get the valid course codes to check the import information, if a course
      // has an unrecognized code prompt the user to input it's information.
      $valid_course_codes = array();
      $result = db_query("SELECT DISTINCT course_code FROM {dae_valid_course_codes}");
      while ($row = db_fetch_array($result)) {
        $valid_course_codes[$row['course_code']] = $row['course_code'];
      }

      // Gather preliminary data on how the inputted
      // values should be entered into the system.
      foreach ($input as $key => $values) {

        // Put the full course name together
        $input[$key]['course'] = $values['cocode'] . ' ' . $values['conumb'];
        $input[$key]['valid'] = $valid_course = daedalus_valid_course($values['cocode'], $values['conumb']);

        // Check to see if the course input has an
        // valid Daedalus course identification #.
        if (is_numeric($values['id']) && in_array($values['id'], $course_ids)) {
          $input[$key]['status'] = 'update';
        }

        // New courses have the word 'new' prefixed to a temporary id. Double
        // check to make sure the course does not alread exist in the database.
        elseif (preg_match('/^new/', $values['id'])) {

          // Do not add new courses that
          // have been created previously.
          if ($valid_course) {
            $input[$key]['status'] = 'created';
          }
          else {
            $input[$key]['status'] = 'new';
          }

        }

        // The input fails
        else {
          $input[$key]['status'] = 'failed';
        }

      }

      // Prepare the SLO URL to link the SLO text to the manage page.
      $course_url = $base_url . '/' . $page_settings['manage courses'];

      foreach ($input as $key => $values) {

        $status_flag = FALSE;

        // Display the course to be updated.
        if ($values['status'] == 'update') {

          $course_display = '<a href="' . $course_url . '/' . $values['cocode'] . '/' . $values['conumb'] .
                  '" style="color: #52514F;" target="_blank">' . $course_info[$key]['course'] . ' - ' . $course_info[$key]['coname'] . '</a>';

          $confirmation .= '<div id="course_' . $key . '">';
          $confirmation .= '<b>' . $course_display . '</b>';
          $confirmation .= ' - <i>id(' . $key . ')</i>';

          if ($values['cocode'] != $course_info[$key]['cocode']) {
            $confirmation .= '<div class="single">Course Code: ' . $values['cocode'] . '</div>';
            $status_flag = TRUE; $valid_input = TRUE;
          }

          if ($values['conumb'] != $course_info[$key]['conumb']) {
            $confirmation .= '<div class="single">Course Number: ' . $values['conumb'] . '</div>';
            $status_flag = TRUE; $valid_input = TRUE;
          }

          if ($values['coname'] != $course_info[$key]['coname']) {
            $confirmation .= '<div class="single">Course Name: ' . $values['coname'] . '</div>';
            $status_flag = TRUE; $valid_input = TRUE;
          }

          $present_prereqs = array(); $found_prereqs = array(); $prereq_deletion = array();

          $result = db_query("SELECT DISTINCT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $key);
          while ($row = db_fetch_array($result)) {
            $present_prereqs[$row['prereq_id']] = $row['prereq_id'];
          }

          if ($values['prereq']) {

            $prereq_list = array();

            $prereq_list = explode(',', $values['prereq']);

            $confirmation .= '<div class="single">Prerequisite Courses:';

            foreach ($prereq_list as $prereq) {

              $prereq = trim($prereq);

              // Keep a list of the prerequisites that are found in the file.
              // This will be compared to those in the database, removing those
              // from the database that are found but not present in the import file.
              $found_prereqs[$prereq] = $prereq;

              if (preg_match('/^new/', $prereq)) {

                if ($input[$prereq]['valid']) {
                  $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                  $confirmation .= '<font class="accepted"> (accepted)</font></div>';
                  $status_flag = TRUE; $valid_input = TRUE;
                }
                else {

                  // Make sure all of the new course information is
                  // present. If it isn't do not enter the course.
                  if ($input[$prereq]['course'] && $input[$prereq]['cocode'] && $input[$prereq]['conumb']) {

                    $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                    $confirmation .= '<font class="duplicate"> (new course)</font></div>';
                    $status_flag = TRUE; $valid_input = TRUE;

                  }
                  else {

                    $confirmation .= '<div class="double">Entry ' . $prereq;
                    $confirmation .= '<font class="failed"> (failed)</font></div>';

                  }

                }

              }
              elseif (in_array($prereq, $course_ids) && !in_array($prereq, $present_prereqs)) {
                $confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $confirmation .= '<font class="accepted"> (accepted)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }
              elseif (in_array($prereq, $present_prereqs)) {
                $confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $confirmation .= '<font class="duplicate"> (duplicate)</font></div>';
              }
              elseif ($input[$prereq]['status'] == 'update') {
                $confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $confirmation .= '<font class="accepted"> (accepted)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;
              }
              elseif ($input[$prereq]['status'] == 'failed') {
                $confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $confirmation .= '<font class="failed"> (failed)</font></div>';
              }
              else {
                $confirmation .= '<div class="double">Course Id ' . $prereq;
                $confirmation .= '<font class="failed"> (failed)</font></div>';
              }

            }

            $prereq_deletion = array_diff($present_prereqs, $found_prereqs);

            if ($prereq_deletion) {

              foreach ($prereq_deletion as $prereq) {

                $prereq = trim($prereq);

                $confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $confirmation .= '<font class="failed"> (delete prerequisite)</font></div>';
                $status_flag = TRUE; $valid_input = TRUE;

              }

            }

            // Close the Prerequisite Course div.
            $confirmation .= '</div>';

          }
          elseif (!$values['prereq'] && $present_prereqs) {

            $confirmation .= '<div class="single">Prerequisite Courses:';

            foreach ($present_prereqs as $prereq) {

              $prereq = trim($prereq);

              $confirmation .= '<div class="double">' .  $course_info[$prereq]['course'];
              $confirmation .= '<font class="failed"> (delete prerequisite)</font></div>';
              $status_flag = TRUE; $valid_input = TRUE;

            }

            // Close the deleted prerequisite course div
            $confirmation .= "</div>";

          }

          if ($status_flag) {
            $confirmation .= '<div class="single">Status: ';
            $confirmation .= '<font color="Green">Update Course</font></div>';
            $confirmation .= '<hr width=45% align="left"></div>';
          }
          else {
            $confirmation .= '<div class="single">Status: ';
            $confirmation .= '<font color="GoldenRod">No Changes Detected</font></div>';
            $confirmation .= '<hr width=45% align="left"></div>';
          }

        }

        // New course
        elseif ($values['status'] == 'new') {

          $valid_input = TRUE;

          $confirmation .= '<div id="course_' . $key . '">';
          $confirmation .= '<b>' . $values['course'] . ' - ' . $values['coname'] . '</b>';
          $confirmation .= ' - <i>id(' . $key . ')</i>';

          if ($values['prereq']) {

            $prereq_list = array();

            $prereq_list = explode(',', $values['prereq']);

            $confirmation .= '<div class="single">Prerequisite Courses:';

            foreach ($prereq_list as $prereq) {

              $prereq = trim($prereq);

              if (preg_match('/^new/', $prereq)) {

                if ($input[$prereq]['valid']) {

                  $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                  $confirmation .= '<font class="accepted"> (accepted)</font></div>';

                }
                else {

                  // Make sure all of the new course information is
                  // present. If it isn't do not enter the course.
                  if ($input[$prereq]['course'] && $input[$prereq]['cocode'] && $input[$prereq]['conumb']) {

                    $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                    $confirmation .= '<font class="duplicate"> (new course)</font></div>';

                  }
                  else {

                    $confirmation .= '<div class="double">' . $prereq;
                    $confirmation .= '<font class="failed"> (failed)</font></div>';

                  }

                }

              }
              elseif (in_array($prereq, $course_ids)) {
                $confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $confirmation .= '<font class="accepted"> (accepted)</font></div>';
              }
              elseif ($input[$prereq]['status'] == 'update') {
                $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                $confirmation .= '<font class="accepted"> (accepted)</font></div>';
              }
              elseif ($input[$prereq]['status'] == 'failed') {
                $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                $confirmation .= '<font class="failed"> (failed)</font></div>';
              }
              else {
                $confirmation .= '<div class="double">Course Id ' . $prereq;
                $confirmation .= '<font class="failed"> (failed)</font></div>';
              }

            }

            // Close the New Prerequsite Course div
            $confirmation .= '</div>';

          }

          $confirmation .= '<div class="single">Status: <font color="Green">New Course</font></div>';
          $confirmation .= '<hr width=45% align="left"></div>';

        }

        elseif ($values['status'] == 'created') {
          $confirmation .= '<div id="course_' . $key . '"><b>' . $values['course'] . ' - ' . $values['coname'] . '</b>';
          $confirmation .= '<div class="single">Status: <font color="GoldenRod">New Course Previously Created</font></div>';
          $confirmation .= '<hr width=45% align="left"></div>';
        }

        elseif ($values['status'] == 'failed') {
          $confirmation .= '<div id="course_' . $key . '"><b>' . $values['course'] . ' - ' . $values['coname'] . '</b>';
          $confirmation .= '<div class="single">Status: <font color="Red">Course Failed</font></div>';
          $confirmation .= '<hr width=45% align="left"></div>';
        }

        else {
          $confirmation .= '<div id="course_' . $key . '"><b>' . $values['course'] . ' - ' . $values['coname'] . '</b>';
          $confirmation .= '<div class="single">Status: <font color="GoldenRod">Course Previously Created</font></div>';
          $confirmation .= '<hr width=45% align="left"></div>';
        }

      }

      $form[] = array(
        '#type' => 'item',
        '#value' => $confirmation,
      );

      $match = '(failed)';
      $failed_input = strpos($confirmation, $match);

      // Note our use of ===.  Simply == would not work as expected
      // because the position of 'a' was the 0th (first) character.
      if ($failed_input !== false) {
        drupal_set_message(t('One or more fields have failed the upload check.
          Please check the course information for errors before uploading.'), 'error');
      }

      if ($new_codes) {

        $new_code_count = 0; $i = 0;

        $form[] = array(
          '#type' => 'item',
          '#value' => '<br />',
        );

        foreach ($new_codes as $course_code) {

          if (!in_array($course_code, $valid_course_codes)) {

            $valid_input = TRUE;

            $valid_course_codes[$course_code] = $course_code;

            $form["new-code$i"] = array(
              '#type' => 'value',
              '#value' => $course_code,
            );

            $form["new-dept$i"] = array(
              '#type' => 'textfield',
              '#title' => t('Enter department name for @cc', array('@cc' => $course_code)),
            );

            $new_code_count++;
            $i++;

          }

        }

        $form['new-code-count'] = array(
          '#type' => 'value',
          '#value' => $new_code_count,
        );

        $form[] = array(
          '#type' => 'item',
          '#value' => '<br />',
        );

      }

      if ($valid_input) {

        $form['confirm-courses'] = array(
          '#type' => 'submit',
          '#value' => t('Confirm Course Information'),
        );

      }

      $form['return-delete'] = array(
        '#type' => 'submit',
        '#value' => t('Return'),
      );

      // Pass the hidden filename
      $form['filename'] = array( '#type' => 'value', '#value' => $param[1] );

      return $form;

    }
    else {
      drupal_set_message(t('The uploaded course file has been deleted.'), 'warning');
    }

  }

  //------------------------------------------------------------------------//
  //    IMPORT COURSES                                                      //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'import_courses') {

    drupal_set_message(t('Only modified items are displayed.'), 'warning');

    drupal_set_title(t('Course Upload Confirmation !help', array('!help' => $show_help)));

    $my_file = file_directory_path() . '/' . $param[1];

    // Check that the file has been saved.
    if (file_exists($my_file)) {

      $handle = fopen($my_file, 'r');

      if ($handle) {

        $input = array();

        while ($line = fgets($handle)) {

          // The tab is only recognized if the
          // double quotes are used, not single
          $data = explode("\t", $line);

          // There must be data. At least the id, course
          // code with course number and the course name.
          if (count($data) > 2) {

            $course_id = trim($data[0]);

            $input[$course_id]['id'] = $course_id;
            $input[$course_id]['cocode'] = trim($data[1]);
            $input[$course_id]['conumb'] = trim($data[2]);
            $input[$course_id]['coname'] = trim($data[3]);
            $input[$course_id]['prereq'] = trim(str_replace('"', '', $data[4]));

          }

        }

        fclose($handle);

      }

      $course_ids = $course_info = array();

      // Create the list of current course ids.
      $result = db_query("SELECT id, course, course_name, course_code, course_number FROM {dae_course} ORDER BY id");
      while ($row = db_fetch_array($result)) {

        $course_ids[$row['id']] = $row['id'];

        $course_info[$row['id']]['course'] = $row['course'];
        $course_info[$row['id']]['cocode'] = $row['course_code'];
        $course_info[$row['id']]['conumb'] = $row['course_number'];
        $course_info[$row['id']]['coname'] = $row['course_name'];

      }

      $new_course_args = array();
      $new_course_count = 0;

      // Gather preliminary data on how the inputted
      // values should be entered into the system.
      foreach ($input as $key => $values) {

        // Put the full course name together
        $input[$key]['course'] = $course = $values['cocode'] . ' ' . $values['conumb'];
        $input[$key]['valid'] = $valid_course = daedalus_valid_course($values['cocode'], $values['conumb']);

        // Check to see if the course input has an
        // valid Daedalus course identification #.
        if (is_numeric($values['id']) && in_array($values['id'], $course_ids)) {

          $input[$key]['status'] = 'update';

          // Only update if the course code, course number or course name
          // have been changed. If one of the code or number are changed,
          // the course value has too, being made of the code and number.
          if ($course_info[$key]['cocode'] != $values['cocode'] ||
              $course_info[$key]['conumb'] != $values['conumb'] ||
                $course_info[$key]['coname'] != $values['coname']) {

            db_query("UPDATE {dae_course}
                   SET course='%s', course_code='%s', course_number=%d, course_name='%s'
                   WHERE id=%d", $course, $values['cocode'], $values['conumb'], $values['coname'], $values['id']);

          }

        }

        // New courses have the word 'new' prefixed to a temporary id. Double
        // check to make sure the course does not alread exist in the database.
        elseif (preg_match('/^new/', $values['id'])) {

          // Do not add new courses that
          // have been created previously.
          if ($valid_course) {
            $input[$key]['status'] = 'created';
          }
          else {

            $input[$key]['status'] = 'new';

            // Add the values to an argument array
            // to be inserted in one query below.
            $new_course_args[] = $course;
            $new_course_args[] = $values['cocode'];
            $new_course_args[] = $values['conumb'];
            $new_course_args[] = $values['coname'];

            $new_course_count++;

          }

        }

        // The input fails
        else {
          $input[$key]['status'] = 'failed';
        }

      }

      // Insert all new courses.
      if ($new_course_count) {

        $new_placeholders = implode(', ', array_fill(0, $new_course_count, "('%s', '%s', %d, '%s')"));

        db_query("INSERT INTO {dae_course} (course, course_code, course_number, course_name) VALUES " . $new_placeholders, $new_course_args);

      }

      $manage_course_url = $base_url . '/' . $page_settings['manage courses'];
      $new_prereq_args = array();
      $new_prereq_count = 0;

      foreach ($input as $key => $values) {

        $status_flag = FALSE;
        $course_confirmation = '';

        // Display the course to be updated.
        if ($values['status'] == 'update') {

          $course_confirmation .= '<a href="' . $manage_course_url . '/' . $values['cocode'] . '/' . $values['conumb'] . '" target="_blank">';
          $course_confirmation .= '<b>' . $values['course'] . ' - ' . $values['coname'] . '</b>';
          $course_confirmation .= '</a> - <i>id(' . $key . ')</i>';

          if ($values['cocode'] != $course_info[$key]['cocode']) {
            $course_confirmation .= '<div class="single">Course Code: ' . $values['cocode'];
            $course_confirmation .= '<font class="accepted"> (course code accepted)</font></div>';
            $status_flag = TRUE;
          }

          if ($values['conumb'] != $course_info[$key]['conumb']) {
            $course_confirmation .= '<div class="single">Course Number: ' . $values['conumb'];
            $course_confirmation .= '<font class="accepted"> (course number accepted)</font></div>';
            $status_flag = TRUE;
          }

          if ($values['coname'] != $course_info[$key]['coname']) {
            $course_confirmation .= '<div class="single">Course Name: ' . $values['coname'];
            $course_confirmation .= '<font class="accepted"> (course name accepted)</font></div>';
            $status_flag = TRUE;
          }

          $present_prereqs = array(); $found_prereqs = array(); $prereq_deletion = array();

          $result = db_query("SELECT DISTINCT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $key);
          while ($row = db_fetch_array($result)) {
            $present_prereqs[$row['prereq_id']] = $row['prereq_id'];
          }

          if ($values['prereq']) {

            $prereq_list = array();

            $prereq_list = explode(',', $values['prereq']);

            $course_confirmation .= '<div class="single">Prerequisite Courses:';

            foreach ($prereq_list as $prereq) {

              $prereq = trim($prereq);

              // Keep a list of the prerequisites that are found in the file.
              // This will be compared to those in the database, removing those
              // from the database that are found but not present in the import file.
              $found_prereqs[$prereq] = $prereq;

              if (preg_match('/^new/', $prereq)) {

                // If listed as 'new' but already entered
                // into daedalus, add the course anyways.
                if ($input[$prereq]['valid']) {

                  $new_prereq_args[] = $values['id'];
                  $new_prereq_args[] = daedalus_course_id($input[$prereq]['course']);
                  $new_prereq_count++;

                  $course_confirmation .= '<div class="double">' . $input[$prereq]['course'];
                  $course_confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

                  $status_flag = TRUE;

                }
                else {

                  // Make sure all of the new course information is
                  // present. If it isn't do not enter the course.
                  if ($input[$prereq]['course'] && $input[$prereq]['cocode'] && $input[$prereq]['conumb']) {

                    $new_prereq_id = daedalus_course_id($input[$prereq]['cocode'], $input[$prereq]['conumb']);

                    $new_prereq_args[] = $values['id'];
                    $new_prereq_args[] = $new_prereq_id;
                    $new_prereq_count++;

                    $course_confirmation .= '<div class="double">' . $input[$prereq]['course'];
                    $course_confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

                    $status_flag = TRUE;

                  }

                }

              }
              elseif (in_array($prereq, $course_ids) && !in_array($prereq, $present_prereqs)) {

                $new_prereq_args[] = $values['id'];
                $new_prereq_args[] = $prereq;
                $new_prereq_count++;

                $course_confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $course_confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

                $status_flag = TRUE;

              }
              elseif (in_array($prereq, $present_prereqs)) {
                // Do nothing
              }
              elseif ($input[$prereq]['status'] == 'update') {

                $new_prereq_args[] = $values['id'];
                $new_prereq_args[] = $course_info[$prereq]['id'];
                $new_prereq_count++;

                $course_confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $course_confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

                $status_flag = TRUE;

              }

            }

            $prereq_deletion = array_diff($present_prereqs, $found_prereqs);

            if ($prereq_deletion) {

              foreach ($prereq_deletion as $prereq) {

                $prereq = trim($prereq);

                db_query("DELETE FROM {dae_prereq_course} WHERE course_id=%d AND prereq_id=%d", $key, $prereq);

                $course_confirmation .= '<div class="double">' .  $course_info[$prereq]['course'];
                $course_confirmation .= '<font class="accepted"> (prerequisite deleted)</font></div>';

                $status_flag = TRUE;

              }

            }

            // Close the Prerequisite Course div.
            $course_confirmation .= '</div>';

          }
          elseif (!$values['prereq'] && $present_prereqs) {

            $course_confirmation .= '<div class="single">Prerequisite Courses:';

            foreach ($present_prereqs as $prereq) {

              $prereq = trim($prereq);

              db_query("DELETE FROM {dae_prereq_course} WHERE course_id=%d AND prereq_id=%d", $key, $prereq);

              $course_confirmation .= '<div class="double">' .  $course_info[$prereq]['course'];
              $course_confirmation .= '<font class="accepted"> (prerequisite deleted)</font></div>';

              $status_flag = TRUE;

            }

            $course_confirmation .= '</div>';

          }

          if ($status_flag) {

            // Only show courses that have modifications.
            $confirmation .= '<div id="course_' . $key . '">' . $course_confirmation;
            $confirmation .= '<div class="single">Status: <font color="Green">Course Updated</font></div>';
            $confirmation .= '<hr width=45% align="left"></div>';

          }

        }

        // New course insert
        elseif ($values['status'] == 'new') {

          // Get the id for newly created course id.
          $new_course_id = daedalus_course_id($values['course']);

          $confirmation .= '<div id="course_' . $new_course_id . '">';
          $confirmation .= '<a href="' . $manage_course_url . '/' . $values['cocode'] . '/' . $values['conumb'] . '" target="_blank">';
          $confirmation .= '<b>' . $values['course'] . ' - ' . $values['coname'] . '</b>';
          $confirmation .= '</a> - <i>id(' . $new_course_id . ')</i>';

          if ($values['prereq']) {

            $prereq_list = array();

            $prereq_list = explode(',', $values['prereq']);

            $confirmation .= '<div class="single">Prerequisite Courses:';

            foreach ($prereq_list as $prereq) {

              $prereq = trim($prereq);

              if (preg_match('/^new/', $prereq)) {

                // If listed as 'new' but already entered
                // into daedalus, add the course anyways.
                if ($input[$prereq]['valid']) {

                  $new_prereq_args[] = $values['id'];
                  $new_prereq_args[] = daedalus_course_id($input[$prereq]['course']);
                  $new_prereq_count++;

                  $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                  $confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

                }
                else {

                  // Make sure all of the new course information is
                  // present. If it isn't do not enter the course.
                  if ($input[$prereq]['course'] && $input[$prereq]['cocode'] && $input[$prereq]['conumb']) {

                    $new_prereq_id = daedalus_course_id($input[$prereq]['cocode'], $input[$prereq]['conumb']);

                    $new_prereq_args[] = $new_course_id;
                    $new_prereq_args[] = $new_prereq_id;
                    $new_prereq_count++;

                    $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                    $confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

                  }

                }

              }
              elseif (in_array($prereq, $course_ids)) {

                $new_prereq_args[] = $new_course_id;
                $new_prereq_args[] = $prereq;
                $new_prereq_count++;

                $confirmation .= '<div class="double">' . $course_info[$prereq]['course'];
                $confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

              }
              elseif ($input[$prereq]['status'] == 'update') {

                $new_prereq_args[] = $new_course_id;
                $new_prereq_args[] = $input[$prereq]['id'];
                $new_prereq_count++;

                $confirmation .= '<div class="double">' . $input[$prereq]['course'];
                $confirmation .= '<font class="accepted"> (prerequisite accepted)</font></div>';

              }

            }

            // Close the Prerequisite Course div.
            $confirmation .= '</div>';

          }

          $confirmation .= '<div class="single">Status: <font color="Green">New Course Entered</font></div>';
          $confirmation .= '<hr width=45% align="left"></div>';

        }

      }

      // Insert all new prereq courses.
      if ($new_prereq_count) {

        $new_placeholders = implode(', ', array_fill(0, $new_prereq_count, "(%d, %d)"));

        db_query("INSERT INTO {dae_prereq_course} (course_id, prereq_id) VALUES " . $new_placeholders, $new_prereq_args);

      }

      // Remove the file once the data has been stored.
      system('rm ' . file_directory_path() . '/' . $param[1]);

      $form[] = array(
        '#type' => 'item',
        '#value' => t($confirmation),
      );

      $form[] = array(
        '#type' => 'item',
        '#value' => '<input type="button" class="form-submit" value="Return" />',
        '#prefix' => '<br /><a href="' . $base_url . '/' . $page_url . '" style="text-decoration:none" >',
        '#suffix' => '</a>',
      );

      return $form;

    }
    else {
      drupal_set_message(t('The uploaded course file has been deleted.'), 'warning');
    }

  }

  //------------------------------------------------------------------------//
  //    EXPORT COURSES                                                      //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'export-courses') {

    $filename = $param[1];

    $output  = "<?php\n";
    $output .= "header('Content-Type: text/tab-delimited-values');\n";
    $output .= "header('Content-Disposition: attachment; filename=\"" . $filename . ".tsv\"');\n\n";

    $all_courses = array(); $course_info = array();

    $result = db_query("SELECT id, course_code, course_number, course_name FROM {dae_course} ORDER BY course_code ASC, course_number ASC");
    while ($row = db_fetch_array($result)) {

      $course_info[$row['id']]['id'] = $row['id'];
      $course_info[$row['id']]['course_code']   = $row['course_code'];
      $course_info[$row['id']]['course_number'] = $row['course_number'];
      $course_info[$row['id']]['course_name']   = $row['course_name'];

      // Create the list to compare future course
      // id's to make sure they are valid.
      $all_courses[$row['id']] = $row['id'];

    }

    $result = db_query("SELECT course_id, prereq_id FROM {dae_prereq_course} ORDER BY course_id");
    while ($row = db_fetch_array($result)) {

      if (in_array($row['course_id'], $all_courses)) {

        if ($course_info[$row['course_id']]['prereqs']) {
          $course_info[$row['course_id']]['prereqs'] = $course_info[$row['course_id']]['prereqs'] . ', ' . $course_info[$row['prereq_id']]['id'];
        }

        else {
          $course_info[$row['course_id']]['prereqs'] = $course_info[$row['prereq_id']]['id'];
        }

      }

    }

    foreach ($course_info as $id => $course_array) {
      $output .= 'echo "' . $course_array['id'] . '\t' . $course_array['course_code'] . '\t' . $course_array['course_number'] . '\t' . $course_array['course_name'] . '\t' . $course_array['prereqs'] . '\r\n"; ';
    }

    $output .= '?>';

    // Write the output to file
    $pwd = exec('pwd');
    $my_file = $pwd . '/graphviz/export.php';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output);
    fclose($file_handle);

    drupal_goto($base_url . '/graphviz/export.php');

  }

  //------------------------------------------------------------------------//
  //    EXPORT SLOS                                                         //
  //------------------------------------------------------------------------//
  elseif ($param[0] == 'export-slos') {

    $filename = $param[1];

    $output  = "<?php\n";
    $output .= "header('Content-Type: text/tab-delimited-values');\n";
    $output .= "header('Content-Disposition: attachment; filename=\"" . $filename . ".tsv\"');\n\n";

    $all_slos = array(); $slo_info = array();

    // Select the SLOs
    $result = db_query("SELECT id, slo_text FROM {dae_slo} ORDER BY id ASC");
    while ($row = db_fetch_array($result)) {

      $slo_text = $row['slo_text'];
      $slo_text = str_replace("'", "\'", $slo_text);
      $slo_text = str_replace('"', '\"', $slo_text);

      $slo_info[$row['id']]['id'] = $row['id'];
      $slo_info[$row['id']]['slo_text'] = $slo_text;

      // Create the list of SLO ids to compare future
      // SLO id's to make sure they are valid.
      $all_slos[$row['id']] = $row['id'];

    }

    // Select the courses associated with each SLO.
    $result = db_query("SELECT slo_id, course_id FROM {dae_course_slo} ORDER BY slo_id");
    while ($row = db_fetch_array($result)) {

      if (in_array($row['slo_id'], $all_slos)) {

        $course = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $row['course_id']));

        // Create a comma separated list
        // if there is more than one course.
        if ($slo_info[$row['slo_id']]['courses']) {
          $slo_info[$row['slo_id']]['courses'] = $slo_info[$row['slo_id']]['courses'] . ', ' . $course;
        }

        else {
          $slo_info[$row['slo_id']]['courses'] = $course;
        }

      }

    }

    // Select the tags associated with each SLO
    $result = db_query("SELECT slo_id, tag_id FROM {dae_slo_tag} ORDER BY slo_id");
    while ($row = db_fetch_array($result)) {

      // Make sure there are no garbage
      // values in the selected tags.
      if (in_array($row['slo_id'], $all_slos)) {

        $tag = db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id=%d", $row['tag_id']));

        // Create a comma separated list
        // if there is more than one tag.
        if ($slo_info[$row['slo_id']]['tags']) {
          $slo_info[$row['slo_id']]['tags'] = $slo_info[$row['slo_id']]['tags'] . ', ' . $tag;
        }

        else {
          $slo_info[$row['slo_id']]['tags'] = $tag;
        }

      }

    }

    // Select the prerequiste SLOs
    $result = db_query("SELECT target, pre_slo FROM {dae_prereq_slo} ORDER BY target");
    while ($row = db_fetch_array($result)) {

      if (in_array($row['target'], $all_slos)) {

        // Create a comma separated list if the
        // re is more than one prerequiste SLO.
        if ($slo_info[$row['target']]['prereqs']) {
          $slo_info[$row['target']]['prereqs'] = $slo_info[$row['target']]['prereqs'] . ', ' . $slo_info[$row['pre_slo']]['id'];
        }

        else {
          $slo_info[$row['target']]['prereqs'] = $slo_info[$row['pre_slo']]['id'];
        }

      }

    }

    foreach ($slo_info as $id => $slo_array) {
      $output .= 'echo "' . $slo_array['id'] . '\t' . $slo_array['slo_text'] . '\t' . $slo_array['courses'] . '\t' . $slo_array['tags'] . '\t' . $slo_array['prereqs'] . '\r\n"; ';
    }

    $output .= '?>';

    // Write the output to file
    $pwd = exec('pwd');
    $my_file = $pwd . '/graphviz/export.php';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output);
    fclose($file_handle);

    drupal_goto($base_url . '/graphviz/export.php');

  }

}


/**
 * Implementation of hook_validate().
 */
function daedalus_manage_information_form_validate($form, &$form_state) {

  switch ($form_state['values']['op']) {

    // Make sure the upload SLOs
    // file is of the type .tsv
    case $form_state['values']['import-slos']:

      $session_file = $_FILES['files']['name']['upload-slos'];

      if ($session_file) {

        $file_info = explode('.', $session_file);
        $file_ext  = $file_info[1];

        // Validate the file to .tsv files only
        if ($file_ext != 'tsv') {
          form_set_error('upload-slos', t('Upload ".tsv" file formats only for the student learning outcomes.'));
        }

      }
      else {
        form_set_error('upload-slos', t('Upload "Upload Student Learning Outcomes" field is required.'));
      }

      break;

    case $form_state['values']['export-slos']:

      if (!$form_state['values']['slo-filename']) {
        form_set_error('slo-filename', t('The field "Export Student Learning Outcome Filename" is required.'));
      }

      // Match .tsv at the end of the file name.
      elseif (preg_match("/.tsv$/", $form_state['values']['slo-filename'])) {
        form_set_error('slo-filename', t('The file suffix ".tsv" is automatically attached to the file name.'));
      }

      break;

    case $form_state['values']['confirm-courses']:

      $new_code_count = $form_state['values']['new-code-count'];

      if ($new_code_count) {

        for ($i=0; $i < $new_code_count; $i++) {

          $new_course_code = $form_state['values']["new-code$i"];
          $new_course_dept = $form_state['values']["new-dept$i"];

          if (!$new_course_dept) {
            form_set_error("new-dept$i", t('The department name for "@new-course-code" is required', array('@new-course-code' => $new_course_code)));
          }

        }

      }

      break;

    // Make sure the upload course
    // file is of the type .tsv
    case $form_state['values']['import-courses']:

      $session_file = $_FILES['files']['name']['upload-courses'];

      if ($session_file) {

        $file_info = explode('.', $session_file);
        $file_ext  = $file_info[1];

        // Validate the file to .tsv files only
        if ($file_ext != 'tsv') {
          form_set_error('upload-courses', t('Upload ".tsv" file formats only for the course upload.'));
        }

      }
      else {
        form_set_error('upload-courses', t('Upload "Upload Courses" field is required.'));
      }

      break;

    case $form_state['values']['export-courses']:

      $session_file = $_FILES['files']['name']['upload-courses'];

      if ($session_file) {

        $file_info = explode('.', $session_file);
        $file_ext  = $file_info[1];

        // Validate the file to .tsv files only
        if ($file_ext != '.tsv') {
          form_set_error('upload-courses', t('Upload ".tsv" file formats only for the session file.'));
        }

      }

      if (!$form_state['values']['course-filename']) {
        form_set_error('slo-filename', t('The field "Export Course Filename" is required.'));
      }

      // Match .tsv at the end of the file name.
      // preg_match requires the double quotes.
      elseif (preg_match("/.tsv$/", $form_state['values']['course-filename'])) {
        form_set_error('course-filename', t('The file suffix ".tsv" is automatically attached to the file name.'));
      }

      break;
  }

}


/**
 * Implementation of hook_submit().
 */
function daedalus_manage_information_form_submit($form, &$form_state) {

  global $base_url;

  $page_url = daedalus_get_setting('manage imports/exports');

  $my_file = $form_state['values']['filename'];

  switch ($form_state['values']['op']) {

    case $form_state['values']['cancel']:

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['return']:

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['return-delete']:

      // Remove the file once the data has been stored.
      system('rm ' . file_directory_path() . '/' . $my_file);

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['export-slos']:

      drupal_goto($base_url . '/' . $page_url . '/export-slos/' . $form_state['values']['slo-filename']);

      break;

    case $form_state['values']['confirm-slos']:

      drupal_goto($base_url . '/' . $page_url . '/import_slos/' . $my_file);

      break;

    case $form_state['values']['import-slos']:

      $validators = array( 'file_validate_extensions' => array( 'tsv' ) );

      // Save new file uploads.
      $file = file_save_upload('upload-slos', $validators, file_directory_path());

      if ($file) {

        // Determine the file name and location
        // and move it to the graviz folder.
        $filename = $file->filename;

        drupal_set_message(t('File temporarily stored as "@filename" in @path/@filename',
                array('@filename' => $filename, '@path' => file_directory_path())), 'warning');

        file_set_status($file, FILE_STATUS_PERMANENT);

        drupal_goto($base_url . '/' . $page_url . '/import_slo_check/' . $filename);

      }

      break;

    case $form_state['values']['export-courses']:

      drupal_goto($base_url . '/' . $page_url . '/export-courses/' . $form_state['values']['course-filename']);

      break;

    case $form_state['values']['confirm-courses']:

      $new_code_count = $form_state['values']['new-code-count'];

      if ($new_code_count) {

        $code_args = array();

        for ($i=0; $i < $new_code_count; $i++) {

          $new_course_code = $form_state['values']["new-code$i"];
          $new_course_dept = $form_state['values']["new-dept$i"];

          $code_args[] = $new_course_code;
          $code_args[] = $new_course_dept;

          drupal_set_message(t('"@new-course-code-@new_course_dept" added to valid course code list.',
                  array('@new-course-code' => $new_course_code, '@new-course-dept' => $new_course_dept)));

        }

        $code_placeholders = implode(', ', array_fill(0, $new_code_count, "('%s', '%s')"));

        db_query("INSERT INTO {dae_valid_course_codes} (course_code, department_name) VALUES " . $code_placeholders, $code_args);

      }

      drupal_goto($base_url . '/' . $page_url . '/import_courses/' . $my_file);

      break;

    case $form_state['values']['import-courses']:

      $validators = array( 'file_validate_extensions' => array( "tsv" ) );

      // Save new file uploads.
      $file = file_save_upload('upload-courses', $validators, file_directory_path());

      if ($file) {

        // Determine the file name and location
        // and move it to the graviz folder.
        $filename = $file->filename;

        drupal_set_message(t('File temporarily stored as "@filename" in @path/@filename',
                array('@filename' => $filename, '@path' => file_directory_path())), 'warning');

        file_set_status($file, FILE_STATUS_PERMANENT);

        drupal_goto($base_url . '/' . $page_url . '/import_courses_check/' . $filename);

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

        drupal_set_message(t('Help information saved.') );

        drupal_goto($base_url . '/' . $page_url);

      }

      break;

  }

}