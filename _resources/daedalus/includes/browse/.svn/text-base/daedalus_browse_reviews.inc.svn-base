<?php

/**
 * @file
 * daedalus_browse_reviews.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Browse->Term Reviews Callback Function
//------------------------------------------------------------------------------
function daedalus_browse_review_page() {
  return drupal_get_form('daedalus_browse_review_form');
}


/**
 * Menu Location: Daedalus -> Browse -> Term Reviews
 * URL Location:  daedalus/browse/term-review
 *
 * Displays Term Reviews for a selected academic term.
 */
function daedalus_browse_review_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                      WHERE setting='browse term reviews' OR
                            setting='checkmark' OR
                            setting='question mark' OR
                            setting='question mark2' OR
                            setting='exclamation mark' OR
                            setting='build courses' OR
                            setting='build course codes' OR
                            setting='manage term reviews'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  // Get daedalus base url
  global $base_url;

  // Get current page url.
  $page_url = $help_url = $page_settings['browse term reviews'];
  $page_url_length = sizeof(explode('/', $page_url));

  // Get the images
  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';
  $check_src = $base_url . '/' . $page_settings['checkmark'];
  $question2_src = $base_url . '/' . $page_settings['question mark2'];
  $exclamation_src = $base_url . '/' . $page_settings['exclamation mark'];

  $page_url = $base_url . '/' . $page_url;

  // URL filter string
  $filter_options = arg(0+$page_url_length);
  $review_order = arg(1+$page_url_length);

  drupal_set_title(t('Browse Term Reviews !help', array('!help' => $show_help)));

  if (user_access('daedalus build term reviews') && !db_result(db_query("SELECT COUNT(*) FROM {dae_course}"))) {

    // If there are no courses, inform the
    // user a course must be created first.
    $url = $base_url . '/' .   $page_settings['build courses'];

    drupal_set_message(t('There are no courses created. <a href="@url"><b>Create a Course?</b></a>', array('@url' => url($url))));

  }

  elseif (user_access('daedalus build term reviews') && !db_result(db_query("SELECT COUNT(*) FROM {dae_term_review_form}"))) {

    // If there are no term reviews, inform the
    // user a term review must be created first.
    $url = $base_url . '/' .   $page_settings['build course codes'];

    drupal_set_message(t('There are no term reviews created. <a href="@url"><b>Create a Term Review?</b></a>', array('@url' => url($url))));

  }

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($help_url, 0, 1);

  // Displays the filter dropdown to
  // minimize the selected Term Reviews.
  if (!$filter_options) {

    $review_table  = '<tr><td><b>' . t('Ranking') . '</b></td>';
    $review_table .= '<td><b>' . t('Course') . '</b></td>';
    $review_table .= '<td><b>' . t('Instructor') . '</b></td>';
    $review_table .= '<td><b>' . t('Term') . '</b></td>';
    $review_table .= '<td><b>' . t('Academic Year') . '</b></td></tr>';

    $review = array();

    $result = db_query_range("SELECT * FROM {dae_term_review_form} ORDER BY year", 15);
    while ($row = db_fetch_array($result)) {
      $review[$row['id']]['course'] = $row['course'];
      $review[$row['id']]['term'] = $row['term'];
      $review[$row['id']]['year'] = $row['year'];
      $review[$row['id']]['instructor'] = $row['instructor'];
    }

    if ($review) {

      foreach ($review as $rid => $value) {

        $deleted_info = '';

        $url = $base_url . '/' .   $page_settings['manage term reviews'] . '/' . $rid;

        if (is_numeric($value['course'])) {

          $course = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $value['course'])) . ' - ' .
                db_result(db_query("SELECT course_name FROM {dae_course} WHERE id=%d", $value['course']));

        }

        else {

          $course = $value['course'];

          $deleted_info = ' <small><i>(' . t('course deleted') . ')</i></small>';

        }

        // Calculate the Average Coverage Rating for each Term Review
        $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $rid);
        $i = $total = 0;

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

        $review_table .= '<tr>
                            <td>' . $rating . '</td>
                            <td><a href="' . $url . '">' . $course . '</a>' . $deleted_info . '</td>
                            <td>' . $value['instructor'] . '</td>
                            <td>' . $value['term'] . '</td>
                            <td>' . $value['year'] . '</td>
                          </tr>';

      }

      // Form to display the Term Reviews
      $form[] = array(
        '#type' => 'item',
        '#value' => '<table>' . $review_table . '</table>',
      );

    }

    if ($review) {

      $year_options = array();

      // If no filter options are passed, give the year options
      // for every academic term and year found in the database.
      $result = db_query("SELECT DISTINCT year FROM {dae_term_review_form} ORDER BY year DESC");
      while ($row = db_fetch_array($result)) {
        $year_options[$row['year'] . '/' . ($row['year']+1)] = $row['year'] . '/' . ($row['year']+1);
      }

      // Incase there are no values in the dae_term_review_form database,
      // create a default date set to the academic current year.
      if (!$year_options) {

        // If it's not the new Academic Year start from the
        // previous Academic Year.
        if (Date('m') < 9) {
          $default_year = (Date('Y') - 1) . '/' . Date('Y');
          $year_options[$default_year] = $default_year;
        }
        else {
          $default_year = Date('Y') . '/' . (Date('Y')+1);
          $year_options[$default_year] = $default_year;
        }

      }

      // Field set to show additional settings
      $form['options'] = array(
        '#title' => t('Show filter options'),
        '#type' => 'fieldset',
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
      );

      // Filter button found at the top
      $form['options']['filter-top'] = array(
        '#type' => 'submit',
        '#value' => t('Filter view'),
        '#prefix' => '<blockquote>',
      );

      // Show all forms button
      $form['options']['view-all-top'] = array(
        '#type' => 'submit',
        '#value' => t('View all forms'),
        '#suffix' => '</blockquote>',
      );

      // Options to choose the year
      $form['options']['year-option'] = array(
        '#title' => t('Academic year option'),
        '#type' => 'select',
        '#options' => array(0 => t('All academic years'),
                            1 => t('Single academic year'),
                            2 => t('Reviewed after the academic year'),
                            3 => t('Reviewed before the academic year'),
                          ),
        '#default_value' => 0,
        '#description' => t('Select the option to narrow term reviews by the academic year'),
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // Options to choose the academic term
      $form['options']['year'] = array(
        '#type' => 'select',
        '#title' => t('Academic year'),
        '#options' => $year_options,
        '#default_value' => 0,
        '#description' =>  t('Select the academic year'),
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // Options to choose the term
      $form['options']['term'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Academic terms'),
        '#default_value' => array('Fall',
                                  'Winter',
                                  'Summer', ),
        '#options' => array('Fall' => 'Fall',
                          'Winter' => 'Winter',
                          'Summer' => 'Summer', ),
        '#description' =>  t('Select the academic terms you want to view'),
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // Option to filter course
      $form['options']['course'] = array(
        '#type' => 'textfield',
        '#title' => t('View term reviews by selecting a course'),
        '#autocomplete_path' => 'autocomp/course',
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // Option to filter instructor
      $form['options']['instructor'] = array(
        '#type' => 'textfield',
        '#title' => t('Filter term reviews by selecting a professor'),
        '#prefix' => '<blockquote>',
        '#suffix' => '</blockquote>',
      );

      // Submit button
      $form['options']['filter'] = array(
        '#type' => 'submit',
        '#value' => t('Filter view'),
        '#prefix' => '<blockquote>',
      );

      // Show all forms button
      $form['options']['view-all'] = array(
        '#type' => 'submit',
        '#value' => t('View all forms'),
        '#suffix' => '</blockquote>',
      );
    }

    // Submit hidden information to pass
    // to the validate and submit hooks.
    $form['pass-browse-term-reviews'] = array( '#type' => 'value', '#value' => $page_settings['browse term reviews'], );

    return $form;

  }

  // Table and data generation section
  $temp = explode('-', $review_order);

  // URL parameters
  // Either asc desc or null
  $ord = $temp[1];
  $one = $temp[0];

  // Generate the links for the headers.
  // If a user clicks the header it will alternate between asc and desc.
  $course_url     = $page_url . '/' . $filter_options . '/course-asc';
  $instructor_url = $page_url . '/' . $filter_options . '/instructor-asc';
  $term_url       = $page_url . '/' . $filter_options . '/term-asc';
  $year_url       = $page_url . '/' . $filter_options . '/year-asc';

  if ($ord != 'desc') {

    switch ($one) {

      case 'course':
        $course_url = str_replace('asc', 'desc', $course_url);
      break;

      case 'instructor':
        $instructor_url = str_replace('asc', 'desc', $instructor_url);
      break;

      case 'term':
        $term_url = str_replace('asc', 'desc', $term_url);
      break;

      case 'year':
        $year_url = str_replace('asc', 'desc', $year_url);
      break;

    }

  }

  $table_header  = '<table><tr><td><b>' . t('Ranking') . '</b></td>';
  $table_header .= '<td><a href="' . $course_url . '"><b>' . t('Course') . '</b></a></td>';
  $table_header .= '<td><a href="' . $instructor_url . '"><b>' . t('Instructor') . '</b></a></td>';
  $table_header .= '<td><a href="' . $term_url . '"><b>' . t('Term') . '</b></a></td>';
  $table_header .= '<td><a href="' . $year_url . '"><b>' . t('Academic Year') . '</b></a></td></tr>';

  // Can filter everything except the course by sql though - and will
  // Seperate the filter option into an array deliniated by the double underscore.
  $filter_options_array = explode('__', $filter_options);

  $settings = array();

  // Right now the array has integer keys. This isn't useful. The keys should be the
  // setting. So we're going to go through every setting passed, and if it's a proper
  // setting, make it a new entry in the setting array, and remove the old one.
  foreach ($filter_options_array as $key => $values) {
    $temp = explode('_', $values);
    $settings[$temp[0]] = $temp[1];
  }

  // Make a query, everything but courses
  $default_query = "SELECT * FROM {dae_term_review_form} ";
  $query = $default_query;
  $query_term = '';

  // When the "View all forms" button is selected bypass the
  // filter query construction and use the default query only.
  if ($filter_options != 'all') {

    // If not all three terms, no need to search by term
    if (!(ereg('Summer', $settings['term']) && ereg('Winter', $settings['term']) && ereg('Fall', $settings['term']))) {

      // Create the Academic Term query
      if (ereg('Summer', $settings['term'])) {
        $query_term .= "term LIKE 'summer' ";
      }

      if (ereg('Winter', $settings['term'])) {

        if ($query_term) {
          $query_term .= ' OR ';
        }

        $query_term .= "term LIKE 'winter' ";

      }

      if (ereg('Fall', $settings['term'])) {

        if ($query_term) {
          $query_term .= ' OR ';
        }

        $query_term .= " term LIKE 'fall' ";

      }

    }

    if ($query_term) {
      $query_term = 'WHERE (' . $query_term . ')';
    }

    $query .= $query_term;

    // Create the Academic Year query
    if (ereg('eq', $settings['year'])) {
      $query_year = '(year = ' . str_replace('eq', '', $settings['year']) . ')';
    }
    elseif (ereg('gt', $settings['year'])) {
      $query_year = '(year > ' . str_replace('gt', '', $settings['year']) . ')';
    }
    elseif (ereg('lt', $settings['year'])) {
      $query_year = '(year < ' . str_replace('lt', '', $settings['year']) . ')';
    }

    if (!ereg('all', $settings['year'])) {

      if ($query == $default_query) {
        $query_year = ' WHERE ' . $query_year . ' ';
      }
      else {
        $query_year = ' AND ' . $query_year . ' ';
      }

      $query .= $query_year;

    }

    // Select the course id and filter reviews.
    if ($settings['course'] != 'all') {

      $course_id = db_result(db_query("SELECT id FROM {dae_course} WHERE course='%s'", $settings['course']));

      if ($query == $default_query) {
        $query_course = ' WHERE (course = "' . $course_id . '")';
      }
      else {
        $query_course = ' AND (course = "' . $course_id . '")';
      }

      $query .= $query_course;

    }

    // The last SQL query for instructor
    if ($settings['instructor'] != 'all') {

      if ($query == $default_query) {
        $query_instructor = " WHERE (instructor LIKE '%s')";
      }
      else {
        $query_instructor = " AND (instructor LIKE '%s')";
      }

      $args = '%' . $settings['instructor'] . '%';

      $query .= $query_instructor;

    }

  }

  // Regardless if the "View all forms" button is selected do not bypass
  // the sorting options. Always add these to the default query.

  // If no sorting options is set.
  if ($review_order == '') {
    $review_order = 'year-DESC';
  }
  else {

    if (sizeof(explode('-', $review_order)) == 1) {
      $review_order .= '-ASC';
    }

  }

  $order = explode('-', $review_order);

  switch ($order[0]) {

    case 'year':
    case 'instructor':
    case 'term':

      $query .= ' ORDER BY ' . $order[0] . ' ' . $order[1];

    break;

  }

  // Now we have the settings, make the virutal table out of arrays. The
  // info is discluded. Each column is the name of the column, but plural.
  $ids = $courses = $instructors = $terms = $years = $urls = array();

  // Retrieve the results from the constructed query.
  if ($args) {
    $result = db_query($query, $args);
  }
  else {
    $result = db_query($query);
  }


  // Loop through the values to display each Term Review
  while ($row = db_fetch_array($result)) {

    $url = $base_url . '/' . $page_settings['manage term reviews'] . '/' . $row['id'];

    if (is_numeric($row['course'])) {
      $course = db_result(db_query("SELECT course FROM {dae_course} WHERE id=%d", $row['course'])) . ' - ' .
            db_result(db_query("SELECT course_name FROM {dae_course} WHERE id=%d", $row['course']));
    }
    else {
      $course = $row['course'] . ' <small><i>(' . t('course deleted') . ')</i></small>';
    }

    $ids[] = $row['id'];
    $urls[] = $url;
    $courses[] = $course;
    $terms[] = $row['term'];
    $instructors[] = $row['instructor'];
    $years[] = $row['year'] . '/' . ($row['year']+1);

  }

  $temp = explode('-', $review_order);

  if ($temp[0] == 'course') {

    $value = $courses;
    natsort($value);

    foreach ($value as $key => $x) {

      $id = $ids[$key];
      $course = $courses[$key];
      $instructor = $instructors[$key];
      $term = $terms[$key];
      $year = $years[$key];
      $form_link = $urls[$key];

      // Calculate the Average Coverage Rating for each Term Review
      $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $id);
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

      // For desc, put it at the
      // beginning, asc end.
      if ($temp[1] == 'asc') {

        $table .= '<tr><td>' . $rating . '</td>';
        $table .= '<td><a href="' . $form_link . '>' . $course . '</a></td>';
        $table .= '<td>' . $instructor . '</td>';
        $table .= '<td>' . $term . '</td>';
        $table .= '<td>' . $year . '</td></tr>';

      }
      else {

        // Reverses row position, puts row
        // on the top instead of the bottom
        $table = '<tr><td>' . $rating . '</td>' .
                 '<td><a href="' . $form_link . '">' . $course . '</a></td>' .
                 '<td>' . $instructor . '</td>' .
                 '<td>' . $term . '</td>' .
                 '<td>' . $year . '</td></tr>' . $table; // Add the existing table to the end

      }

    }

  }
  else {

    $value = $ids;

    foreach ($value as $key => $x) {

      $id = $ids[$key];
      $course = $courses[$key];
      $instructor = $instructors[$key];
      $term = $terms[$key];
      $year = $years[$key];
      $form_link = $urls[$key];

      // Calculate the Average Coverage Rating for each Term Review
      $result = db_query("SELECT rating FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $id);
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

      $table .= '<tr><td>' . $rating . '</td>';
      $table .= '<td><a href="' . $form_link . '>' . $course . '</a></td>';
      $table .= '<td>' . $instructor . '</td>';
      $table .= '<td>' . $term . '</td>';
      $table .= '<td>' . $year . '</td></tr>';

    }

  }

  if ($value) {
    $table_output = $table_header . $table . '</table>';
  }
  else {
    $table_output = '<ul><li><i>' . t('No matches found') . '</i></li></ul>';
  }

  // Output the term review information
  $form[] = array(
    '#type' => 'item',
    '#value' => $table_output,
  );

  // Submit hidden information to pass
  // to the validate and submit hooks.
  $form['pass-browse-term-reviews'] = array( '#type' => 'value', '#value' => $page_settings['browse term reviews'], );

  return $form;

}


/**
 * Implementation of hook_submit().
 */
function daedalus_browse_review_form_submit($form, &$form_state) {

  // Get daedalus base url
  global $base_url;

  // Set's the pages title
  $page_url = $form_state['values']['pass-browse-term-reviews'];
  $page_url_length = sizeof(explode('/', $page_url));

  $filter_options = arg(0+$page_url_length);
  $review_order = arg(1+$page_url_length);

  switch ($form_state['values']['op']) {

    case $form_state['values']['view-all-top']:
    case $form_state['values']['view-all']:

      drupal_goto($base_url . '/' . $page_url . '/all');
      break;

    case $form_state['values']['filter-top']:
    case $form_state['values']['filter']:

      // Make default settings for what ever wasn't chosen.
      // the default settings will match everything

      $term_string = 'term_';

      if ($form_state['values']['term']['Fall'] == TRUE) {
        $term_string .= $form_state['values']['term']['Fall'];
      }

      if ($form_state['values']['term']['Winter'] == TRUE) {
        $term_string .= $form_state['values']['term']['Winter'];
      }

      if ($form_state['values']['term']['Summer'] == TRUE) {
        $term_string .= $form_state['values']['term']['Summer'];
      }

      if (($term_string == 'term_')) {
        $term_string .= 'all';
      }

      // Split the year up from the 2010/2011 format to preserve the URL
      $year_values = explode('/', $form_state['values']['year']);

      $year = $year_values[0];

      switch ($form_state['values']['year-option']) {
        case 0:
          $year_string = 'year_all';
          break;
        case 1:
          $year_string = 'year_eq' . $year;
          break;
        case 2:
          $year_string = 'year_gt' . $year;
          break;
        case 3:
          $year_string = 'year_lt' . $year;
          break;
      }

      if ($form_state['values']['course']) {
        $course_string = 'course_' . $form_state['values']['course'];
      }
      else {
        $course_string = 'course_all';
      }

      if ($form_state['values']['instructor']) {
        $instructor_string = 'instructor_' . $form_state['values']['instructor'];
      }
      else {
        $instructor_string = 'instructor_all';
      }

      // Delimit the filters with underscores to prevent breaking the URL
      $filters = $year_string . '__' . $course_string . '__' . $term_string . '__' . $instructor_string;

      // Simply add the created filter string to the URL
      drupal_goto($base_url . '/' . $page_url . '/' . $filters);

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

        if ($filter_options && $review_order) {
          drupal_goto($base_url . '/' . $page_url . '/' . $filter_options . '/' . $review_order);
        }
        elseif ($filter_options) {
          drupal_goto($base_url . '/' . $page_url . '/' . $filter_options);
        }
        else {
          drupal_goto($base_url . '/' . $page_url);
        }

      }

      break;

  }

}
