<?php

/**
 * @file
 * daedalus_browse_graphviz.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Browse->Graphviz Callback Function
//------------------------------------------------------------------------------
function daedalus_browse_graphviz_page() {
  return drupal_get_form('daedalus_browse_graphviz_form');
}


/**
 * Menu Location: Daedalus -> Browse -> Graphviz
 * URL Location:  daedalus/browse/graphviz
 *
 * Displays visual course, slo and program maps.
 */
function daedalus_browse_graphviz_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='browse visual maps' OR
                             setting='manage courses' OR
                             setting='question mark' OR
                             setting='neato course map setting' OR
                             setting='neato slo map setting' OR
                             setting='graphviz path' OR
                             setting='graphviz iteration' OR
                             setting='manage learning outcomes'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

    // Get daedalus base url
  global $base_url;

  // Get the current page url
  $page_url = $help_url = $page_settings['browse visual maps'];

  // Get the page url for managing a course.
  $course_url = $page_settings['manage courses'];

  $course_url = $base_url . '/' . $course_url;

  // Get the page url for managing a course.
  $slo_url = $page_settings['manage learning outcomes'];

  $slo_url = $base_url . '/' . $slo_url;

  // Store URL Parameters in $param array
  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  // Get the images
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  // Get the settings.
  $cmap_setting = $page_settings['neato course map setting'];
  $smap_setting = $page_settings['neato slo map setting'];

  // Get the working directory and path
  // for the dot and neato programs.
  if ($param[0]) {

    $pwd = exec('pwd');

    $graphviz_path = $page_settings['graphviz path'];

  }

  drupal_set_title(t('Visual Maps !help', array('!help' => $show_help)));

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form[] = daedalus_help_form($help_url, 1, 1);

   ///////////////////////////////////////////////////////////////////////////
  // FULL MAP
  if ($param[0] == 'full_map') {

    // NOTE*********************************
    //   Output must be formatted with
    //   double quotes or the code breaks.

    $output_svg  = "digraph Constellation {\n";
    $output_svg .= "overlap=\"scale\"\n";

    $output_png  = "digraph Constellation {\n";
    $output_png .= "overlap=\"scale\"\n";
    $output_png .= "size=\"10.5\"\n";

    $slo = array();

    // Get all of the SLO information
    $result = db_query("SELECT * FROM {dae_slo} ORDER BY id");
    while ($row = db_fetch_array($result)) {
      $slo[$row['id']]['id'] = $row['id'];
      $slo[$row['id']]['slo_rank'] = $row['slo_rank'];
    }

    // Create the SLO nodes
    if ($slo) {

      foreach ($slo as $s) {

        if ($s['slo_rank'] == 0) {
          $color = 'firebrick4';
        }

        elseif ($s['slo_rank'] == 1) {
          $color = 'firebrick';
        }

        elseif ($s['slo_rank'] >= 2 && $s['slo_rank'] < 4) {
          $color = 'red3';
        }

        elseif ($s['slo_rank'] >= 4 && $s['slo_rank'] < 6) {
          $color = 'orangered';
        }

        elseif ($s['slo_rank'] == 6) {
          $color = 'goldenrod2';
        }

        elseif ($s['slo_rank'] == 7) {
          $color = 'gold';
        }

        elseif ($s['slo_rank'] >= 8) {
          $color = 'yellow';
        }

        $output_svg .= " slo" . $s['id'] . " [shape=circle, style=\"filled\", fillcolor=\"" . $color . "\", label=\"\", URL=\"" . $slo_url . "/" . $s['id'] . "\", height=\"0.17\", width=\"0.17\"];\n";
        $output_png .= " slo" . $s['id'] . " [shape=circle, style=\"filled\", fillcolor=\"" . $color . "\", label=\"\", URL=\"" . $slo_url . "/" . $s['id'] . "\", height=\"1.4\", width=\"1.4\"];\n";

      }

    }

    $course_ids = array(); $course = array();

    // Get all the active courses being used in the graph.
    $result = db_query("SELECT DISTINCT course_id FROM {dae_course_slo}");
    while ($row = db_fetch_array($result)) {
      $course_ids[$row['course_id']] = $row['course_id'];
    }

    $result = db_query("SELECT DISTINCT course_id FROM {dae_prereq_course}");
    while ($row = db_fetch_array($result)) {
      $course_ids[$row['course_id']] = $row['course_id'];
    }

    $result = db_query("SELECT DISTINCT prereq_id FROM {dae_prereq_course}");
    while ($row = db_fetch_array($result)) {
      $course_ids[$row['prereq_id']] = $row['prereq_id'];
    }

    // If there is little data in the database
    // only select from the course table.
    if ($course_ids) {

      $course_placeholders = implode(' OR ', array_fill(0, count($course_ids), 'id=%d'));

      // Query all of the course information
      $result = db_query("SELECT * FROM {dae_course} WHERE " . $course_placeholders . " ORDER BY course", $course_ids);

    }
    else {
      $result = db_query("SELECT * FROM {dae_course} ORDER BY course");
    }

    while ($row = db_fetch_array($result)) {
      $course[$row['id']]['id'] = $row['id'];
      $course[$row['id']]['course'] = $row['course'];
      $course[$row['id']]['course_code'] = $row['course_code'];
      $course[$row['id']]['course_number'] = $row['course_number'];
      $course[$row['id']]['course_name'] = $row['course_name'];
    }

    // Create the course nodes
    if ($course) {

      foreach ($course as $c) {

        // Remove any course that has the code "PREU"
        if ($c['course_code'] != "PREU") {

          $output_svg .= " course" . $c['id'] . " [shape=box, style=\"rounded,filled\", fillcolor=\"aliceblue\", label=\"" . $c['course'] . "\", URL=\"" . $course_url . "/" . $c['course_code'] . "/" . $c['course_number'] . "\", width=\"0.25\", height=\"0.375\", fontsize=\"9\"];\n";
          $output_png .= " course" . $c['id'] . " [shape=box, style=\"rounded,filled\", fillcolor=\"aliceblue\", label=\"" . $c['course'] . "\", URL=\"" . $course_url . "/" . $c['course_code'] . "/" . $c['course_number'] . "\", width=\"3.0\", height=\"2.0\", fontsize=\"72\"];\n";

          // Add the assumed SLOs for each course.
          $assumed_slos = array();
          daedalus_assumed_slos($c['id'], $assumed_slos);

          if ($assumed_slos) {

            foreach ($assumed_slos as $aslo) {
              $output_svg .= " slo" . $aslo . " -> course" . $c['id'] . ";\n";
              $output_png .= " slo" . $aslo . " -> course" . $c['id'] . ";\n";
            }

          }

        }

      }

    }

    $prereq_course = array();

    // Get all of the prerequisite course information
    $result = db_query("SELECT * FROM {dae_prereq_course} ORDER BY prereq_id, course_id");
    while ($row = db_fetch_array($result)) {
      $prereq_course[$row['id']]['course_id'] = $row['course_id'];
      $prereq_course[$row['id']]['prereq_id'] = $row['prereq_id'];
    }

    if ($prereq_course) {

      foreach ($prereq_course as $prec) {
        $output_svg .= " course" . $prec['prereq_id'] . " -> course" . $prec['course_id'] . " [style=\"setlinewidth(3)\"];\n";
        $output_png .= " course" . $prec['prereq_id'] . " -> course" . $prec['course_id'] . " [style=\"setlinewidth(3)\"];\n";
      }

    }

    $course_slo = array(); $i = 0;

    // Get all of courses student learning outcomes.
    $result = db_query("SELECT * FROM {dae_course_slo}");
    while ($row = db_fetch_array($result)) {
      $course_slo[$i]['id'] = $row['slo_id'];
      $course_slo[$i++]['course_id'] = $row['course_id'];
    }

    if ($course_slo) {

      foreach ($course_slo as $cslo) {

        // Remove links from courses with the code "PREU" to SLOs.
        if (db_result(db_query("SELECT course_code FROM {dae_course} WHERE id=%d", $cslo['course_id'])) != "PREU") {
          $output_svg .= " course" . $cslo['course_id'] . " -> slo" . $cslo['id'] . ";\n";
          $output_png .= " course" . $cslo['course_id'] . " -> slo" . $cslo['id'] . ";\n";
        }

      }

    }

    $output_svg .= '}';
    $output_png .= '}';

    // Get the iteration to append to the file.
    // This prevents the browser from displaying
    // the cached file because the location changes.
    $iteration = $page_settings['graphviz iteration'];

    // Update the iteration value
    db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $iteration+1, "graphviz iteration");

    $constellation = 'constellation' . $iteration;

    // Remove old constellation map filesl
    system('rm ' . $pwd . '/graphviz/constellation*');

    // Write the .svg output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output_svg);
    fclose($file_handle);

    // Execute Graphviz to create the svg output.
    system($graphviz_path . 'neato ' . $pwd . '/graphviz/' . $constellation . '.dot -Tsvg -o ' . $pwd . '/graphviz/' . $constellation . '.svg');

    // Parse the .svg output to add the html titles.
    daedalus_parse_graphviz($pwd . '/graphviz/' . $constellation . '.svg');

    // Write the .png output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output_png);
    fclose($file_handle);

    // Execute Graphviz to create the png output.
    system($graphviz_path . 'neato ' . $pwd . '/graphviz/' . $constellation . '.dot -Tpng -o ' . $pwd . '/graphviz/' . $constellation . '.png');

    drupal_goto($page_url . '/view/' . $constellation);

  }

   ///////////////////////////////////////////////////////////////////////////
  // COURSE MAP
  elseif ($param[0] == 'course_map') {

    if ($cmap_setting) {

      $graph_type = "neato";

      $output_svg = "digraph Constellation {\n";
      $output_svg .= "overlap=\"scale\"\n";
      $output_svg .= "size=\"16,19\"\n";

      $output_png  = "digraph Constellation {\n";
      $output_png .= "overlap=\"scale\"\n";
      $output_png .= "size=\"10,13\"\n";

    }

    else {

      $graph_type = "dot";

      $output_svg = "digraph Constellation {\n";

      $output_png  = "digraph Constellation {\n";
      $output_png .= "size=\"10,13\"\n";

    }

    $with_prereqs = array(); $pre_course = array();

    // Select all courses with prerequisites.
    $result = db_query("SELECT DISTINCT course_id FROM {dae_prereq_course} ORDER BY course_id");
    while ($row = db_fetch_array($result)) {
      $with_prereqs[] = $row['course_id'];
    }

    $query1_placeholders = implode(' OR ', array_fill(0, count($with_prereqs), 'id=%d'));

    // Get the the courses with prerequisites
    $result = db_query("SELECT id, course, course_code, course_number FROM {dae_course} WHERE " . $query1_placeholders . " ORDER BY course", $with_prereqs);
    while ($row = db_fetch_array($result)) {
      $pre_course[$row['id']]['id'] = $row['id'];
      $pre_course[$row['id']]['course'] = $row['course'];
      $pre_course[$row['id']]['course_code'] = $row['course_code'];
      $pre_course[$row['id']]['course_number'] = $row['course_number'];
    }

    foreach ($pre_course as $p) {

      // Remove the PREU courses
      if ($p['course_code'] != "PREU") {
        $output_svg .= " course" . $p['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $p['course'] . "\", URL=\"" . $course_url . "/" . $p['course_code'] . "/" . $p['course_number'] . "\" fontsize=9];\n";
        $output_png .= " course" . $p['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $p['course'] . "\", URL=\"" . $course_url . "/" . $p['course_code'] . "/" . $p['course_number'] . "\" fontsize=9];\n";
      }

    }

    $query2_placeholders = implode(' OR ', array_fill(0, count($with_prereqs), 'id <> %d'));

    // Get the rest of the course information
    $result = db_query("SELECT id, course, course_code, course_number FROM {dae_course} WHERE " . $query2_placeholders . " ORDER BY course", $with_prereqs);
    while ($row = db_fetch_array($result)) {
      $course[$row['id']]['id'] = $row['id'];
      $course[$row['id']]['course'] = $row['course'];
      $course[$row['id']]['course_code'] = $row['course_code'];
      $course[$row['id']]['course_number'] = $row['course_number'];
    }

    foreach ($course as $c) {

      // Check to see if the course has a prerequisite, if it does
      // not this course is a singleton and is not displayed.
      $is_prereq = db_result(db_query("SELECT COUNT(*) FROM {dae_prereq_course} WHERE prereq_id=%d", $c['id']));

      if (!in_array($c['id'], $with_prereqs) && $is_prereq) {

        // Remove the PREU courses
        if ($c['course_code'] != "PREU") {
          $output_svg .= " course" . $c['id'] . " [shape=box, style=\"rounded\", label=\"" . $c['course'] . "\", URL=\"" . $course_url . "/" . $c['course_code'] . "/" . $c['course_number'] . "\" fontsize=9];\n";
          $output_png .= " course" . $c['id'] . " [shape=box, style=\"rounded\", label=\"" . $c['course'] . "\", URL=\"" . $course_url . "/" . $c['course_code'] . "/" . $c['course_number'] . "\" fontsize=9];\n";
        }

      }

    }

    // Get all of the prerequisite course information
    $result = db_query("SELECT id, course_id, prereq_id, set_id FROM {dae_prereq_course} ORDER BY prereq_id, course_id");
    while ($row = db_fetch_array($result)) {
      $prereq_link[$row['id']]['course_id'] = $row['course_id'];
      $prereq_link[$row['id']]['prereq_id'] = $row['prereq_id'];
      $prereq_link[$row['id']]['set_id']  = $row['set_id'];
    }

    foreach ($prereq_link as $link) {

      // If the prereqs are a
      // set, dash the line.
      if ($link['set_id'] > 1) {
        $output_svg .= " course" . $link['prereq_id'] . " -> course" . $link['course_id'] . " [style=\"setlinewidth(2),dashed\"];\n";
        $output_png .= " course" . $link['prereq_id'] . " -> course" . $link['course_id'] . " [style=\"setlinewidth(2),dashed\"];\n";
      }

      else {
        $output_svg .= " course" . $link['prereq_id'] . " -> course" . $link['course_id'] . " [style=\"setlinewidth(3)\"];\n";
        $output_png .= " course" . $link['prereq_id'] . " -> course" . $link['course_id'] . " [style=\"setlinewidth(3)\"];\n";
      }

    }

    $output_svg .= '}';
    $output_png .= '}';

    // Get the iteration to append to the file.
    // This prevents the browser from displaying
    // the cached file because the location changes.
    $iteration = $page_settings['graphviz iteration'];

    // Update the iteration value
    db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $iteration+1, "graphviz iteration");

    $constellation = 'constellation' . $iteration;

    // Remove old constellation map filesl
    system('rm ' . $pwd . '/graphviz/constellation*');

    // Write the .svg output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output_svg);
    fclose($file_handle);

    // Execute Graphviz to create the svg output.
    system($graphviz_path . '' . $graph_type . ' ' . $pwd . '/graphviz/' . $constellation . '.dot -Tsvg -o ' . $pwd . '/graphviz/' . $constellation . '.svg');

    // Parse the .svg output to add the html titles.
    daedalus_parse_graphviz($pwd . '/graphviz/' . $constellation . '.svg');

    // Write the .png output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output_png);
    fclose($file_handle);

    // Execute Graphviz to create the png output.
    system($graphviz_path . '' . $graph_type . ' ' . $pwd . '/graphviz/' . $constellation . '.dot -Tpng -o ' . $pwd . '/graphviz/' . $constellation . '.png');

    //drupal_goto($base_url."/graphviz/".$constellation.".svg");
    drupal_goto($page_url . '/view/' . $constellation);

  }

   ///////////////////////////////////////////////////////////////////////////
  // SLO MAP
  elseif ($param[0] == 'slo_map') {

    if ($smap_setting) {
      $graph_type = "neato";

      $output_svg = "digraph Constellation {\n";
      $output_svg .= "overlap=\"scalexy\"\n";
      $output_svg .= "size=\"16,19\"\n";

      $output_png = "digraph Constellation {\n";
      $output_png .= "overlap=\"scalexy\"\n";
      $output_png .= "size=\"10,13\"\n";
    }

    else {
      $graph_type = "dot";

      $output_svg = "digraph Constellation {\n";

      $output_png = "digraph Constellation {\n";
      $output_png .= "size=\"10,13\"\n";
    }

    // Get all of the SLO information
    $result = db_query("SELECT * FROM {dae_slo} ORDER BY id");
    while ($row = db_fetch_array($result)) {
        $slo[$row['id']]['id'] = $row['id'];
        $slo[$row['id']]['slo_rank'] = $row['slo_rank'];
    }

    if ($slo) {

      // Create the SLO nodes
      foreach ($slo as $s) {

        // Remove singleton SLOs from the display.
        if (db_result(db_query("SELECT COUNT(*) FROM {dae_prereq_slo} WHERE target=%d", $s['id'])) || db_result(db_query("SELECT COUNT(*) FROM {dae_prereq_slo} WHERE pre_slo=%d", $s['id']))) {

          if ($s['slo_rank'] == 0) {
            $color = 'firebrick4';
          }
          elseif ($s['slo_rank'] == 1) {
            $color = 'firebrick';
          }
          elseif ($s['slo_rank'] >= 2 && $s['slo_rank'] < 4) {
            $color = 'red3';
          }
          elseif ($s['slo_rank'] >= 4 && $s['slo_rank'] < 6) {
            $color = 'orangered';
          }
          elseif ($s['slo_rank'] == 6) {
            $color = 'goldenrod2';
          }
          elseif ($s['slo_rank'] == 7) {
            $color = 'gold';
          }
          elseif ($s['slo_rank'] >= 8) {
            $color = 'yellow';
          }

          $output_svg .= " slo" . $s['id'] . " [shape=circle, style=\"filled\", fillcolor=\"" . $color . "\", label=\"\", URL=\"" . $slo_url . "/" . $s['id'] . "\", height=0.17, width=0.17];\n";
          $output_png .= " slo" . $s['id'] . " [shape=circle, style=\"filled\", fillcolor=\"" . $color . "\", label=\"\", URL=\"" . $slo_url . "/" . $s['id'] . "\", height=0.17, width=0.17];\n";

        }

      }

    }

    $pre_slo = array();

    $result = db_query("SELECT target, pre_slo FROM {dae_prereq_slo}");
    while ($row = db_fetch_array($result)) {
      $pre_slo[$i]['target'] = $row['target'];
      $pre_slo[$i++]['pre_slo'] = $row['pre_slo'];
    }

    if ($pre_slo) {

      foreach ($pre_slo as $p) {
        $output_svg .= " slo" . $p['pre_slo'] . " -> slo" . $p['target'] . " [style=\"setlinewidth(2)\"];\n";
        $output_png .= " slo" . $p['pre_slo'] . " -> slo" . $p['target'] . " [style=\"setlinewidth(2)\"];\n";
      }

    }

    $output_svg .= '}';
    $output_png .= '}';

    // Get the iteration to append to the file.
    // This prevents the browser from displaying
    // the cached file because the location changes.
    $iteration = $page_settings['graphviz iteration'];

    // Update the iteration value
    db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $iteration+1, "graphviz iteration");

    $constellation = 'constellation' . $iteration;

    // Remove old constellation map filesl
    system('rm ' . $pwd . '/graphviz/constellation*');

    // Write the .svg output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output_svg);
    fclose($file_handle);

    // Execute Graphviz to create the svg output.
    system($graphviz_path . '' . $graph_type . ' ' . $pwd . '/graphviz/' . $constellation . '.dot -Tsvg -o ' . $pwd . '/graphviz/' . $constellation . '.svg');

    // Parse the .svg output to add the html titles.
    daedalus_parse_graphviz($pwd . '/graphviz/' . $constellation . '.svg');

    // Write the .png output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output_png);
    fclose($file_handle);

    // Execute Graphviz to create the png output.
    system($graphviz_path . '' . $graph_type . ' ' . $pwd . '/graphviz/' . $constellation . '.dot -Tpng -o ' . $pwd . '/graphviz/' . $constellation . '.png');

    //drupal_goto($base_url."/graphviz/".$constellation.".svg");
    drupal_goto($page_url . '/view/' . $constellation);

  }

   ///////////////////////////////////////////////////////////////////////////
  // SELECTED COURSE
  elseif ($param[0] == 'selected_course') {

    $output  = "digraph Constellation {\n";
    $output .= "size=\"10,13\"\n";

    // Get all of the course information
    $result = db_query("SELECT * FROM {dae_course} WHERE course='%s'", $param[1]);
    while ($row = db_fetch_array($result)) {
      $selected_id = $row['id'];
      $course = $row['course'];
      $course_code = $row['course_code'];
      $course_number = $row['course_number'];
      $course_name = $row['course_name'];
    }

    // And the selected course to the map
    $output .= " course" . $selected_id . " [shape=box, style=\"rounded,filled\", label=\"" . $course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=9];\n";

    $preq_courses = array();

    // Get the prerequisite course information for the selected course
    $result = db_query("SELECT prereq_id, set_id FROM {dae_prereq_course} WHERE course_id=%d ORDER BY prereq_id, course_id", $selected_id);
    while ($row = db_fetch_array($result)) {
      $preq_courses[$row['prereq_id']] = $row['set_id'];
    }

    if ($preq_courses) {

      // Create the selected course prerequisite courses
      foreach ($preq_courses as $pid => $set_id) {

        $result = db_query("SELECT * FROM {dae_course} WHERE id='%s'", $pid);
        while ($row = db_fetch_array($result)) {

          $course = $row['course'];
          $course_code = $row['course_code'];
          $course_number = $row['course_number'];
          $course_name = $row['course_name'];

          // Add the selected course to the map.
          $output .= " course" . $pid . " [shape=box, style=\"rounded\", label=\"" . $course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=7, height=0.4, width=0.6];\n";

          if ($set_id > 1) {
            $edges .= " course" . $pid . " -> course" . $selected_id . " [style=\"setlinewidth(2), dashed\"];\n";
          }

          else {
            $edges .= " course" . $pid . " -> course" . $selected_id . " [style=\"setlinewidth(2)\"];\n";
          }

        }

        // Select the prerequiste courses to the prerequisite course
        $prepreq_courses = array();

        $result = db_query("SELECT prereq_id, set_id FROM {dae_prereq_course} WHERE course_id=%d ORDER BY prereq_id, course_id", $pid);
        while ($row = db_fetch_array($result)) {
          $prepreq_courses[$row['prereq_id']] = $row['set_id'];
        }

        // Create the selected course's pre prerequisite courses
        if ($prepreq_courses) {

          foreach ($prepreq_courses as $preid => $set_id) {

            $result = db_query("SELECT * FROM {dae_course} WHERE id='%s'", $preid);
            while ($row = db_fetch_array($result)) {
              $course = $row['course'];
              $course_code = $row['course_code'];
              $course_number = $row['course_number'];
              $course_name = $row['course_name'];

              // And the selected course to the map
              $output .= " course" . $preid . " [shape=box, style=\"rounded\", label=\"" . $course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=5, height=0.35, width=0.5];\n";

              if ($set_id > 1) {
                $edges  .= " course" . $preid . " -> course" . $pid . " [style=\"setlinewidth(2), dashed\"];\n";
              }

              else {
                $edges  .= " course" . $preid . " -> course" . $pid . " [style=\"setlinewidth(1)\"];\n";
              }

            }

          }

        }

      }

    }

    $post_courses = array();

    // Get all of the post-requisite course information
    $result = db_query("SELECT course_id, set_id FROM {dae_prereq_course} WHERE prereq_id=%d ORDER BY prereq_id, course_id", $selected_id);
    while ($row = db_fetch_array($result)) {
      $post_courses[$row['course_id']] = $row['set_id'];
    }

    if ($post_courses) {

      foreach ($post_courses as $postid => $set_id) {

        $result = db_query("SELECT * FROM {dae_course} WHERE id='%s'", $postid);
        while ($row = db_fetch_array($result)) {
          $course = $row['course'];
          $course_code = $row['course_code'];
          $course_number = $row['course_number'];
          $course_name = $row['course_name'];

          // And the selected course to the map.
          $output .= " course" . $postid . " [shape=box, style=\"rounded\", label=\"" . $course . "\" URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\" fontsize=7, height=0.4, width=0.6];\n";

          if ($set_id > 1) {
            $edges .= " course" . $selected_id . " -> course" . $postid . " [style=\"setlinewidth(2), dashed\"];\n";
          }

          else {
            $edges .= " course" . $selected_id . " -> course" . $postid . " [style=\"setlinewidth(2)\"];\n";
          }

        }

        // Select the prerequiste courses to the prerequisite course
        $postpost_courses = array();

        $result = db_query("SELECT course_id, set_id FROM {dae_prereq_course} WHERE prereq_id=%d ORDER BY prereq_id, course_id", $postid);
        while ($row = db_fetch_array($result)) {
          $postpost_courses[$row['course_id']] = $row['set_id'];
        }

        if ($postpost_courses) {

          foreach ($postpost_courses as $post_postid => $set_id) {

            $result = db_query("SELECT * FROM {dae_course} WHERE id='%s'", $post_postid);
            while ($row = db_fetch_array($result)) {
              $course = $row['course'];
              $course_code = $row['course_code'];
              $course_number = $row['course_number'];
              $course_name = $row['course_name'];

              // And the selected course to the map
              $output .= " course" . $post_postid . " [shape=box, style=\"rounded\", label=\"" . $course . "\" URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\" fontsize=5, height=0.35, width=0.5];\n";

              if ($set_id > 1) {
                $edges .= " course" . $postid . " -> course" . $post_postid . " [style=\"setlinewidth(2), dashed\"];\n";
              }

              else {
                $edges .= " course" . $postid . " -> course" . $post_postid . " [style=\"setlinewidth(1)\"];\n";
              }

            }

          }

        }

      }

    }

    $output .= $edges;
    $output .= '}';

    // Get the iteration to append to the file.
    // This prevents the browser from displaying
    // the cached file because the location changes.
    $iteration = $page_settings['graphviz iteration'];

    // Update the iteration value
    db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $iteration+1, "graphviz iteration");

    $constellation = 'constellation' . $iteration;

    // Remove old constellation map filesl
    system('rm ' . $pwd . '/graphviz/constellation*');

    // Write the .svg and .png output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output);
    fclose($file_handle);

    // Execute Graphviz to create the svg output.
    system($graphviz_path . 'dot ' . $pwd . '/graphviz/' . $constellation . '.dot -Tsvg -o ' . $pwd . '/graphviz/' . $constellation . '.svg');

    // Execute Graphviz to create the png output.
    system($graphviz_path . 'dot ' . $pwd . '/graphviz/' . $constellation . '.dot -Tpng -o ' . $pwd . '/graphviz/' . $constellation . '.png');

    // Parse the .svg output to add the html titles.
    daedalus_parse_graphviz($pwd . '/graphviz/' . $constellation . '.svg');

    //drupal_goto($base_url."/graphviz/".$constellation.".svg");
    drupal_goto($page_url . '/view/' . $constellation);

  }

   ///////////////////////////////////////////////////////////////////////////
  // SELECTED PROGRAM
  elseif ($param[0] == 'selected_program') {

    $build_access = user_access('daedalus build');

    $output  = "digraph Constellation {\n";
    $output .= "size=\"10,13\"\n";

    $core_courses = $completed_courses = array();

    $program = $param[1];

    // Select the latest program id.
    $program_id = db_result(db_query("SELECT id FROM {mag_program_identification} WHERE program='%s' ORDER BY year DESC", $program));

    $result = db_query("SELECT DISTINCT requirement_id FROM {mag_program_content} WHERE program_id=%d", $program_id);
    while ($row = db_fetch_array($result)) {
      $requirement_ids[] = $row['requirement_id'];
    }

    // Create the list of placeholders to accommodate a variable number of arguments.
    $placeholders = implode(' OR ', array_fill(0, count($requirement_ids), 'id=%d'));

    $result = db_query("SELECT course_id FROM {mag_program_requirement} WHERE course_id <> 0 AND (" . $placeholders . ") ORDER BY course_id", $requirement_ids);
    while ($row = db_fetch_array($result)) {
      $core_courses[$row['course_id']] = $row['course_id'];
    }

    // If the user has build access there is not
    // use to find the list of completed courses.
    if (!$build_access) {

      // Determine the user name
      global $user;
      $user_id = $user->uid;
      $user_name = db_result(db_query("SELECT name FROM {users} WHERE uid=%d", $user_id));

      // Get the user's student information.
      $student_selection = db_result(db_query("SELECT current_selection FROM {mag_student_identification} WHERE user_name='%s'", $user_name));

      // A user may still select a program without logging in.
      // Prevent the completed courses from being compiled by
      // an anonamous user.
      if ($student_selection) {

        $encrypted_na = aes_encrypt('N/A');

        $completed_courses = array();

        // Determine which courses have been completed by the user
        $result = db_query("SELECT mark, requirement_id
                              FROM {mag_student_program_form}
                             WHERE mark <> '%s'
                               AND selection_id=%d", $encrypted_na, $student_selection);

        while ($row = db_fetch_array($result)) {

          switch (aes_decrypt($row['mark'])) {
            case "A+":
            case "A":
            case "A-":
            case "B+":
            case "B":
            case "B-":
            case "C+":
            case "C":
            case "C-":

            // Select the course id from the requirement.
            $completed_id = db_result(db_query("SELECT course_id FROM {mag_program_requirement} WHERE id=%d", $row['requirement_id']));

            // Only add requirements that are
            // associated with a daedalus course.
            if ($completed_id) {
              $completed_courses[$completed_id] = $completed_id;
            }

            break;

          }

        }

      }

    }

    // Create the list of placeholders to accommodate a variable number of arguments.
    $placeholders = implode(' OR ', array_fill(0, count($core_courses), 'id=%d'));

    $result = db_query("SELECT * FROM {dae_course} WHERE $placeholders", $core_courses);
    while ($row = db_fetch_array($result)) {

      if ($build_access) {

        $current_url = $course_url . '/' . $row['course_code'] . '/' . $row['course_number'];

        $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";

      }
      elseif ($row['mapped']) {

        $current_url = $course_url . '/' . $row['course_code'] . '/' . $row['course_number'];

        if (in_array($row['id'], $completed_courses)) {
          $output .= " course" . $row['id'] . " [shape=box, fillcolor=\"goldenrod\", style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
        }
        else {
          $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
        }

      }
      else {

        $current_url = "http://www.registrar.dal.ca/calendar/class.php?subj=" . $row['course_code'] . "&num=" . $row['course_number'];

        if (in_array($row['id'], $completed_courses)) {
          $output .= " course" . $row['id'] . " [shape=box, fillcolor=\"goldenrod\", style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
        }
        else {
          $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
        }

      }

    }

    $completed_courses = array_diff($completed_courses, $core_courses);

    if ($completed_courses) {

      // Create the list of placeholders to accommodate a variable number of arguments.
      $placeholders = implode(' OR ', array_fill(0, count($completed_courses), 'id=%d'));

      $result = db_query("SELECT * FROM {dae_course} WHERE $placeholders", $completed_courses);
      while ($row = db_fetch_array($result)) {

        if ($build_access) {

          $current_url = $course_url . '/' . $row['course_code'] . '/' . $row['course_number'];

          $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";

        }

        elseif ($row['mapped']) {

          $current_url = $course_url . '/' . $row['course_code'] . '/' . $row['course_number'];

          if (in_array($row['id'], $completed_courses)) {
            $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
          }
          else {
            $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
          }

        }

        else {

          $current_url = "http://www.registrar.dal.ca/calendar/class.php?subj=" . $row['course_code'] . "&num=" . $row['course_number'];

          if (in_array($row['id'], $completed_courses)) {
            $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
          }
          else {
            $output .= " course" . $row['id'] . " [shape=box, style=\"rounded,filled\", label=\"" . $row['course'] . "\", URL=\"" . $current_url . "\", fontsize=9];\n";
          }

        }

      }

    }

    // Show the prerequiste links for each core course.
    foreach ($core_courses as $cid) {

      $result = db_query("SELECT prereq_id FROM {dae_prereq_course} WHERE course_id=%d", $cid);
      while ($row = db_fetch_array($result)) {

        if (in_array($row['prereq_id'], $core_courses)) {
          $output .= " course" . $row['prereq_id'] . " -> course" . $cid . " [style=\"setlinewidth(2)\"];\n";
        }
        elseif (in_array($row['prereq_id'], $completed_courses)) {
          $output .= " course" . $row['prereq_id'] . " -> course" . $cid . " [style=\"setlinewidth(2)\"];\n";
        }

      }

    }

    $output .= '}';

    // Get the iteration to append to the file.
    // This prevents the browser from displaying
    // the cached file because the location changes.
    $iteration = $page_settings['graphviz iteration'];

    // Update the iteration value
    db_query("UPDATE {dae_settings} SET VALUE=%d WHERE setting='%s'", $iteration+1, "graphviz iteration");

    $constellation = 'constellation' . $iteration;

    // Remove old constellation map filesl
    system('rm ' . $pwd . '/graphviz/constellation*');

    // Write the .svg and .png output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open file");
    fwrite($file_handle, $output);
    fclose($file_handle);

    // Execute Graphviz to create the svg output.
    system($graphviz_path . 'dot ' . $pwd . '/graphviz/' . $constellation . '.dot -Tsvg -o ' . $pwd . '/graphviz/' . $constellation . '.svg');

    // Execute Graphviz to create the png output.
    system($graphviz_path . 'dot ' . $pwd . '/graphviz/' . $constellation . '.dot -Tpng -o ' . $pwd . '/graphviz/' . $constellation . '.png');

    // Parse the .svg output to add the html titles.
    daedalus_parse_graphviz($pwd . '/graphviz/' . $constellation . '.svg');

    //drupal_goto($base_url."/graphviz/".$constellation.".svg");
    drupal_goto($page_url . '/view/' . $constellation);

  }

   ///////////////////////////////////////////////////////////////////////////
  // VIEW
  elseif ($param[0] == "view") {

    $constellation = $param[1];
    $png_url = $base_url . '/graphviz/' . $constellation . '.png';

    $form['map']['view'] = array(
      '#type' => 'submit',
      '#value' => t('View interactive map'),
    );

    $form['map']['constellation'] = array(
      '#type' => 'value',
      '#value' => $constellation,
    );

    $form[] = array(
      '#type' => 'item',
      '#prefix' => '<div><img src="' . $png_url . '" alt="' . t('Image failed to load') . '"></img></div>',
    );

    return $form;

  }

   ///////////////////////////////////////////////////////////////////////////
  // DEFAULT > User selects the map type
  else {

    // Determine if the current browse is Microsoft's Internet
    // Explorer. If it is inform the user to use another browser.
    if (preg_match('/MSIE/', $_SERVER['HTTP_USER_AGENT'])) {
      drupal_set_message(t('The interactive visual maps are not compatible with Windows Internet Explorer 8.0 or less.'));
      drupal_set_message(t('For the best results please use Mozilla Firefox.'));
    }

    // Determin if the graphviz directory is properly installed.
    $installed = daedalus_graphviz_installed($pwd, $graphviz_path);

    // Determine how to to set up the graphviz selection according to the information
    // available and if the graphviz directory has been installed on the systems server.
    if (db_result(db_query("SELECT COUNT(*) FROM {dae_prereq_course}")) || !$installed) {

      $full_disable = $course_disable = FALSE;
      $full_message = t('Displays the curriculum. map composed of all courses with prerequisite courses and student learing outcomes.');

      if ($cmap_setting) {
        $course_title   = t('Change course map to dot');
        $course_message = t('Displays the complete course map with links between prerequiste courses in the "neato" graph format.');
      }

      else {
        $course_title   = t('Change course map to neato');
        $course_message = t('Displays the complete course map with links between prerequiste courses in the "dot" graph format.');
      }

    }

    else {

      $full_disable = $course_disable = TRUE;
      $full_message = t('Currently there is not enough information to display the full map.');
      $course_message = t('Currently there is not enough information to display the course map.');

      if ($cmap_setting) {
        $course_title = t('Change course map to dot');
      }

      else {
        $course_title = t('Change course map to neato');
      }

    }

    if (db_result(db_query("SELECT COUNT(*) FROM {dae_slo}")) || !$installed) {

      $slo_disable = FALSE;

      if ($smap_setting) {
        $slo_title   = t('Change slo map to dot');
        $slo_message = t('Displays the complete student learning outcome map with links between prerequiste outcomes in the "neato" graph format.');
      }

      else {
        $slo_title   = t('Change slo map to neato');
        $slo_message = t('Displays the complete student learning outcome map with links between prerequiste outcomes in the "dot" graph format.');
      }

    }

    else {

      $slo_disable = TRUE;
      $slo_message = t('Currently there is not enough information to display the slo map.');

      if ($smap_setting) {
        $slo_title = t('Change slo map to dot');
      }

      else {
        $slo_title = t('Change slo map to neato');
      }

    }

    if ($installed) {
      $disabled = FALSE;
    }

    else {
      $disabled = $full_disable = $course_disable = $slo_disable = TRUE;
      form_set_error('', t('The graphviz directory must first be installed and configured. Please contact the systems administrator.'));
    }

    $form[] = array(
      '#type' => 'item',
      '#value' => t('Visual maps displays the curriculum using a variety of informational graphs.'),
      '#suffix' => '<br />',
    );

    $form['full-map'] = array(
      '#type' => 'submit',
      '#value' => t('View full map'),
      '#disabled' => $full_disable,
      '#suffix' => '<br /><small>' . $full_message . '</small><br /><br />',
    );

    $form['course-map'] = array(
      '#type' => 'submit',
      '#disabled' => $course_disable,
      '#value' => t('View course map'),
    );

    $form['course-map-type'] = array(
      '#type' => 'submit',
      '#value' => $course_title,
      '#disabled' => $course_disable,
      '#suffix' => '<br /><small>' . $course_message . '</small><br /><br />',
    );

    $form['slo-map'] = array(
      '#type' => 'submit',
      '#disabled' => $slo_disable,
      '#value' => t('View slo map'),
    );

    $form['slo-map-type'] = array(
      '#type' => 'submit',
      '#default_value' => $slo_title,
      '#disabled' => $slo_disable,
      '#suffix' => '<br /><small>' . $slo_message . '</small><br /><br />',
    );

    $form['course-select'] = array(
      '#type' => 'textfield',
      '#title' => t('View graph by selecting a course'),
      '#autocomplete_path' => 'autocomp/course',
      '#disabled' => $disabled,
      '#description' => t('View a single course map with prerequisite and post-requiste courses at a depth of two.'),
      '#prefix' => '<table><tr><th width=800px;>',
      '#suffix' => '</th>',
    );

    $form['course-submit'] = array(
      '#type' => 'submit',
      '#disabled' => $disabled,
      '#value' => t('View by course'),
      '#prefix' => '<th>',
      '#suffix' => '</th></tr></table>',
    );

    $form['program-select'] = array(
      '#type' => 'textfield',
      '#title' => t('View graph by selecting a program'),
      '#autocomplete_path' => 'autocomp/program',
      '#disabled' => $disabled,
      '#description' => t('View a complete program map with links between prerequiste courses.'),
      '#prefix' => '<table><tr><th width=800px;>',
      '#suffix' => '</th>',
    );

    $form['program-submit'] = array(
      '#type' => 'submit',
      '#disabled' => $disabled,
      '#value' => t('View by program'),
      '#prefix' => '<th>',
      '#suffix' => '</th></tr></table>',
    );


    return $form;

  }

}

/**
 * Implementation of hook_validate().
 */
function daedalus_browse_graphviz_form_validate($form, &$form_state) {

  switch ($form_state['values']['op']) {

    case $form_state['values']['course-submit']:

      if (!$form_state['values']['course-select']) {
        form_set_error('course-select', t('You must enter a course'));
      }

      if (!db_result(db_query("SELECT COUNT(*) FROM {dae_course} WHERE course ='%s'", $form_state['values']['course-select']))) {
        form_set_error('course-select', t('The course entered does not exist'));
      }

      break;

    case $form_state['values']['program-submit']:

      if (!$form_state['values']['program-select']) {
        form_set_error('program-select', t('You must enter a program'));
      }

      if (!db_result(db_query("SELECT COUNT(*) FROM {mag_program_identification} WHERE program='%s'", $form_state['values']['program-select']))) {
        form_set_error('program-select', t('The program entered does not exist'));
      }

      break;

  }

}

/**
 * Implementation of hook_submit().
 */
function daedalus_browse_graphviz_form_submit($form, &$form_state) {

  // Get daedalus base url
  global $base_url;

  // Set's the pages title
  $page_url = daedalus_get_setting('browse visual maps');
  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  switch ($form_state['values']['op']) {

    case $form_state['values']['view']:

      drupal_goto($base_url . '/graphviz/' . $form_state['values']['constellation'] . '.svg');

      break;

    case $form_state['values']['download']:

      drupal_goto($base_url . '/' . $page_url . '/download/' . $form_state['values']['constellation'] . '.svg');

      break;

    case $form_state['values']['full-map']:

      drupal_goto($base_url . '/' . $page_url . '/full_map');

      break;

    case $form_state['values']['course-map']:

      drupal_goto($base_url . '/' . $page_url . '/course_map');

      break;

    case $form_state['values']['course-map-type']:

      if (db_result(db_query("SELECT value FROM {dae_settings} WHERE setting='neato course map setting'")) == 1) {
        db_query("UPDATE {dae_settings} SET value='0' WHERE setting='neato course map setting'");
      }
      else {
        db_query("UPDATE {dae_settings} SET value='1' WHERE setting='neato course map setting'");
      }

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['slo-map']:

      drupal_goto($base_url . '/' . $page_url . '/slo_map');

      break;

    case $form_state['values']['slo-map-type']:

      if (db_result(db_query("SELECT value FROM {dae_settings} WHERE setting='neato slo map setting'")) == 1) {
        db_query("UPDATE {dae_settings} SET value='0' WHERE setting='neato slo map setting'");
      }
      else {
        db_query("UPDATE {dae_settings} SET value='1' WHERE setting='neato slo map setting'");
      }

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['course-submit']:

      drupal_goto($base_url . '/' . $page_url . '/selected_course/' . $form_state['values']['course-select']);

      break;

    case $form_state['values']['program-submit']:

      drupal_goto($base_url . '/' . $page_url . '/selected_program/' . $form_state['values']['program-select']);

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

  }

}