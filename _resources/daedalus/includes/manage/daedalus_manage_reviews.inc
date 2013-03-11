<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage->Term Reviews Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_reviews_page() {
    return drupal_get_form('daedalus_manage_reviews_form');
}


/**
 * Menu Location: Daedalus -> Manage -> Term Reviews
 * URL Location:  daedalus/manage/term-review
 *
 * Displays
 *
 * Right now there's nothing you can do to "manage" a submitted form, per say, but this lets you see it.
 * It could later have features for deleting a review term.
 */
function daedalus_manage_reviews_form($form) {
//TODO Go through this section and redo!!
  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                      WHERE setting='manage term reviews' OR
                            setting='question mark' OR
                            setting='browse term reviews' OR
                            setting='manage learning outcomes' OR
                            setting='checkmark' OR
                            setting='exclamation mark' OR
                            setting='manage courses'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  //general settings
  global $base_url;

  $page_url = $help_url = $page_settings['manage term reviews'];

  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($help_url, 1, 1);

  // If no form id has been passed
  // redirect the user to select a
  // term review.
  if ($param[0] == '') {

    drupal_set_message(t('Redirected to the browse term reviews page.') );

    drupal_goto($base_url . '/' . $page_settings['browse term reviews']);

  }

  elseif (is_numeric($param[0]) && $param[1] == 'delete' && user_access('daedalus delete term reviews')) {

    switch ($param[1]) {

      case 'delete':

        drupal_set_title(t('Delete Confirmation !help', array('!help' => $show_help)));

        $course_id = db_result(db_query("SELECT course FROM {dae_term_review_form} WHERE id=%d", $param[0]));

        $result = db_query("SELECT course, course_name FROM {dae_course} WHERE id=%d", $course_id);
        $row = db_fetch_array($result);

        $delete_message  = '<br />';
        $delete_message .= t('Are you sure you want to delete the term review for "%course %course-name"? This can not be undone.',
                array('%course' => $row['course'], '%course-name' => $row['course_name']) );
        $delete_message .= '<br /><br /><ul><li>' . t('This will delete the term review and all associated coments') . '</li></ul><br />';
        $delete_message .= t('Are you sure you want to continue?');

        $form[] = array(
          '#type' => 'item',
          '#value' => $delete_message,
        );

        $form['delete-id'] = array( '#type' => 'value', '#value' => $param[0] );

        $form["delete-forward"] = array(
          '#type' => 'submit',
          '#value' => t('I understand the consequences. Delete this term review forever.'),
        );

        $form['delete-reverse'] = array(
          '#type' => 'submit',
          '#value' => t('On second thought, I better not. Take me back'),
        );

        return $form;

        break;

    }

  }

  else {

    //a paramater has passed
    $id = $param[0];

    //make sure that feedback form exists
    if (!db_result(db_query("SELECT id FROM {dae_term_review_form} WHERE id=%d", $id))) {

      drupal_set_message(t('You have requested a feedback form which does not exist.') );

      drupal_set_message(t('This should instead redirect to the browse page') );

      drupal_goto($page_url);

    }

    // now a for a row with id $id exists
    $row = db_fetch_array(db_query("SELECT * FROM {dae_term_review_form} WHERE id = %d", $id));

    if (!$row) {

      drupal_set_message(t('The term review form which you have requested either does not exist, or has been deleted.') );

      return $form;

    }

    $preslo_list = '<ul>';
    $any = FALSE;

    //first get all the prereq ones (accidently put wrong list in each variable, so the prelist is actually not prereq
    $result = db_query("SELECT * FROM {dae_term_review_ratings} WHERE term_review_form_id=%d and type='prereq'", $id);
    while ($row = db_fetch_array($result)) {

      $comment = db_result(db_query("SELECT comment FROM {dae_term_review_ratings} WHERE term_review_form_id=%d AND slo='%s'", $id, $row['slo']));

      $any = TRUE;
      if (is_numeric($row['slo'])) {

        $truple = db_fetch_array(db_query("SELECT * FROM {dae_slo} WHERE id=%d", $row['slo']));

        $url = $base_url . '/' . $page_settings['manage learning outcomes'] . '/' . $truple['id'];

        $row['slo'] = '<a href="' . $url . '">' . $truple['slo_text'] . '</a>';

      }

      $preslo_list .= '<li>';

      // Append the checkmark or exclamation to the list
      $image_string = '';

      $x = $row['rating'];

      if ($x >= 1) {
        $image = $page_settings['checkmark'];
        $alt = '?';
      }
      else {
        $image = $page_settings['exclamation mark'];
        $alt = 'x';
      }

      // Determine if it's an internal or external url
      if (drupal_substr($image, 0, 4) != 'http') {
        $image = $base_url . '/' . $image;
      }

      // Format the image string
      if ($x == 2) {
        $image_string .= '<img src="' . $image . '" alt="' . $alt . '" /><img src="' . $image . '" alt="$alt" />';
      }
      else {
        $image_string .= '<img src="' . $image . '" alt="' . $alt . '" />';
      }

      //only show a comment if there is a comment, there probably won't be a lot
      //of comments, so there's no point in always having "No comment", etc
      if ($comment != '') {
        $comment_string = '<br/><b>' . t('Comment') . ':</b> <i>' . $comment . '</i>';
      }
      else {
        $comment_string = '';
      }

      $preslo_list .= $row['slo'] . '&nbsp;&nbsp;' . $image_string . '' . $comment_string;

      $preslo_list .= '</li>';

    }

    if (!$any) {
        $preslo_list .= '<li><i>' . t('This course had no prerequisite learning outcomes') . '</i></li>';
    }

    $preslo_list .= '</ul>';

    $slo_list = '<ul>';

    $any = FALSE;

    // Do the exact same thing as above, except for taught learning outcomes.
    // For commenting, just see above, this is basically copy and paste.
    $result = db_query("SELECT * FROM {dae_term_review_ratings} WHERE term_review_form_id=%d and type='taught'", $id);
    while ($row = db_fetch_array($result)) {

      $comment = db_result(db_query("SELECT comment FROM {dae_term_review_ratings} WHERE term_review_form_id=%d AND slo='%s'", $id, $row['slo']));

      $any = TRUE;

      if (is_numeric($row['slo'])) {

        $truple = db_fetch_array(db_query("SELECT * FROM {dae_slo} WHERE id=%d", $row['slo']));

        $url = $base_url . '/' . $page_settings['manage learning outcomes'] . '/' . $truple['id'];

        $row['slo'] = '<a href="' . $url . '">' . $truple['slo_text'] . '</a>';

      }

      if ($comment != '') {
        $comment_string = '<br/><b>' . t('Comment') . ':</b> <i>' . $comment . '</i>';
      }
      else {
        $comment_string = '';
      }

      $image_string="";
      $x = $row['rating'];

      if ($x >= 1) {
        $image = $page_settings['checkmark'];
        $alt = '?';
      }
      else {
        $image = $page_settings['exclamation mark'];
        $alt = '!';
      }

      // Determine if it's an internal or external url
      if (drupal_substr($image, 0, 4) != 'http') {
        $image = $base_url . '/' . $image;
      }

      // Format the image string
      if ($x == 2) {
        $image_string .= '<img src="' . $image . '" alt="' . $alt . '" /><img src="' . $image . '" alt="' . $alt . '" />';
      }
      else {
        $image_string .= '<img src="' . $image . '" alt="' . $alt . '" />';
      }

      $slo_list .= '<li>';
      $slo_list .= $row['slo'] . '&nbsp;&nbsp;' . $image_string . '' . $comment_string;
      $slo_list .= '</li>';

    }

    if (!$any) {
      $slo_list .= '<li><i>' . t('This course had no learning outcomes.') . '</i></li>';
    }

    $slo_list .= '</ul>';

    $row = db_fetch_array(db_query("SELECT * FROM {dae_term_review_form} WHERE id = %d", $id));

    $review_value  = t('Academic term: ') . $row['term'] . '<br />';
    $review_value .= t('Academic year: ') . $row['year'] . '/' . ($row['year']+1) . '<br /><br />';
    $review_value .= t('Submitted by: ') .  $row['instructor'] . '<br /><br />';
    $review_value .= '<strong>' . t('Student Learning Outcomes') . '</strong>' . $slo_list;
    $review_value .= '<strong>' . t('Assumed Learning Outcomes') . '</strong>' . $preslo_list;

    // Create the title
    if (is_numeric($row['course'])) {

      $course_info = db_fetch_array(db_query("SELECT course, course_code, course_number, course_name FROM {dae_course} WHERE id=%d", $row['course']));

      $url = $base_url . '/' . $page_settings['manage courses'] . "/" . $course_info['course_code'] . "/" . $course_info['course_number'];

      $row['course'] = '<a href="' . $url . '">' . $course_info['course'] . ' - ' . $course_info['course_name'] . '</a>';

    }
    else {
      $row['course'] .= ' <small><small><small><i>(course deleted)</i></small></small></small>';
    }

    drupal_set_title(t($row['course'] . $show_help) );

    $form[] = array(
      '#type'  => 'item',
      '#value' => $review_value,
    );

    //and show the extra comments on the course
    $info = db_result(db_query("SELECT info FROM {dae_term_review_form} WHERE id=%d", $id));

    $form[] = array(
      '#title' => t('Additional term reveiw comments'),
      '#type' => 'item',
      '#value' => $info
    );

    // If the user can see the button to delete
    // an slo then give the button
    if (user_access('daedalus delete term reviews')) {

      $form['delete-id'] = array( '#type' => 'value', '#value' => $param[0] );

      $form['delete-term-review'] = array(
        '#value' => t('Delete this term review'),
        '#type' => 'submit',
        '#prefix' => '<br/>',
      );

    }

    return $form;

  }

}


/**
 * Implementation of hook_validate().
 */
function daedalus_manage_reviews_form_validate($form, &$form_state) {

  if (!is_numeric($form_state['values']['delete-id'])) {
    form_set_error('', t('A valid term review ID was not registered.'));
  }

}


/**
 * Implementation of hook_submit().
 */
function daedalus_manage_reviews_form_submit($form, &$form_state) {

  global $base_url;

  $page_url = daedalus_get_setting('manage term reviews');

  switch ($form_state['values']['op']) {

    case $form_state['values']['delete-term-review']:

      drupal_goto($base_url . '/' . $page_url . '/' . $form_state['values']['delete-id'] . '/delete');
      break;

    case $form_state['values']['delete-forward']:

      db_query("DELETE FROM {dae_term_review_form} WHERE id=%d", $form_state['values']['delete-id']);
      db_query("DELETE FROM {dae_term_review_ratings} WHERE term_review_form_id=%d", $form_state['values']['delete-id']);

      drupal_set_message(t('Term review deleted') );

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['delete-reverse']:

      drupal_goto($base_url . '/' . $page_url . '/' . $form_state['values']['delete-id']);

      break;

    case $form_state['values']['dae-help-submit']:

      $help_text = $form_state['values']['dae-help'];

      $page_url_length = sizeof(explode('/', $page_url));

      $param = array();
      $param[0] = arg(0+$page_url_length);
      $param[1] = arg(1+$page_url_length);

      if ($help_text) {

        if (db_result(db_query("SELECT COUNT(*) FROM {dae_page_help} WHERE page_url='%s'", $page_url))) {
          db_query("UPDATE {dae_page_help} SET help_text='%s' WHERE page_url='%s'", $help_text, $page_url);
        }
        else {
          db_query("INSERT INTO {dae_page_help} (page_url, help_text) VALUES ('%s', '%s')", $page_url, $help_text);
        }

        drupal_set_message(t('Help information saved.') );

        if ($param[0] && $param[1]) {
          drupal_goto($base_url . '/' . $page_url . '/' . $param[0] . '/' . $param[1]);
        }
        else {
          drupal_goto($base_url . '/' . $page_url . '/' . $param[0]);
        }

      }

      break;

  }

}