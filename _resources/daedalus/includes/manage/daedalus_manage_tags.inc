<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage->Tags Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_tags_page() {
  return drupal_get_form('daedalus_manage_tags_form');
}


/**
 * Menu Location: Daedalus -> Manage -> Tags
 * URL Location:  daedalus/manage/tags
 *
 * Displays: Tag management merge, delete and rename.
 */
function daedalus_manage_tags_form($form) {

  // Get all setting for the page.
  $result = db_query("SELECT setting, value
                        FROM {dae_settings}
                       WHERE setting='manage tags' OR
                             setting='question mark'");

  $page_settings = array();
  while ($row = db_fetch_array($result)) {
    $page_settings[$row['setting']] = $row['value'];
  }

  global $base_url;

  drupal_set_title(t('Manage Tags') );

  $page_url = $help_url = $page_settings['manage tags'];
  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);
  $param[2] = arg(2+$page_url_length);
  $param[3] = arg(3+$page_url_length);

  $show_help = '<img class="show-help" src="' . $base_url . '/' . $page_settings['question mark'] . '" alt="?" />';

  drupal_set_title(t('Manage Tags !help', array('!help' => $show_help)));

  $page_url = $base_url . '/' . $page_url;

  $confirm = 'confirm=true';

  $form = array();

  // Add the hidden help form. Paramaters are
  // (help url, show border, show break).
  $form = daedalus_help_form($help_url, 1, 1);

  // Give the confirmation page if it should be given.
  if ($param[0] == 'delete' && $param[1] != '' && $param[2] != $confirm) {

    drupal_set_title(t('Delete Confirmation !help', array('!help' => $show_help)));

    $tag_ids = explode('_', $param[1]);

    $tag_placeholders = implode(' OR ', array_fill(0, count($tag_ids), "id=%d"));

    // Query {dae_tag} for the tag labels
    // from the id's passed through the URL.
    $tag_query = 'SELECT tag_label FROM {dae_tag}
      WHERE ' . $tag_placeholders . ' ORDER BY tag_label';

    $tag_labels = array();

    $result = db_query($tag_query, $tag_ids);
    while ($row = db_fetch_array($result)) {
      $tag_labels[] = $row['tag_label'];
    }

    if (sizeof($tag_ids) > 1) {
      $delete_total = sizeof($tag_ids) . ' tags';
    }
    else {
      $delete_total = 'tag';
    }

    $delete_message  = t('Are you sure you want to delete the following');
    $delete_message .= ' <strong>' . t('@deltot', array('@deltot' => $delete_total)) . '</strong>?';
    $delete_message .= 'This can not be undone.<br /><br /><ul>';

    foreach ($tag_labels as $label) {
      $delete_message .= '<li>' . t('!label', array('!label' => $label)) . '</li>';
    }

    $delete_message .= '</ul>' . t('Are you sure you want to continue?') . '<br /><br />';

    $form[] = array(
      '#type' => 'item',
      '#value' => $delete_message,
    );

    $form['submit-delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete Tags'),
    );

    $form[] = array(
      '#type' => 'item',
      '#value' => '<small><a href="' . $page_url . '">' . t('cancel delete') . '</a></small><br/><br/><hr><br/>',
    );

    return $form;

  }

  elseif ($param[0] == 'split' && $param[1] != '') {

    $label = db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id=%d", $param[1]));

    drupal_set_title(t('Split the tag "!label" !help', array('!label' => $label, '!help' => $show_help)));

    $form['split-a'] = array(
      '#type' => 'textfield',
      '#title' => t('New tag #1'),
    );

    $form['split-b'] = array(
      '#type' => 'textfield',
      '#title' => t('New tag #2'),
    );

    $form['submit-split'] = array(
      '#type' => 'submit',
      '#value' => t('Split'),
    );

    // Hidden value
    $form['delete-label'] = array( '#type' => 'value', '#value' => $label );

    $form[] = array(
      '#type' => 'item',
      '#value' => '<small><a href="' . $page_url . '">cancel ' . $param[0] . '</a></small><br/><br/><hr><br/>',
    );

    return $form;

  }

  elseif ($param[0] == 'merge' && $param[1] != '') {

    $ids = explode('_', $param[1]);
    $label_1 = db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id=%d", $ids[0]));
    $label_2 = db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id=%d", $ids[1]));

    drupal_set_title(t('Merge the tags "@label1" and "@label2"', array('@label1' => $label_1, '@label2' => $label_2)) . $show_help );

    $form['merge'] = array(
      '#title' => t('Enter new tag'),
      '#type' => 'textfield',
      '#description' => t('You may enter an entirely new tag label or one of the merging tag labels.'),
    );

    $form['submit_merge'] = array(
      '#type' => 'submit',
      '#value' => t('Merge Values'),
    );

    $form[] = array(
      '#type' => 'item',
      '#value' => '<small><a href="' . $page_url . '">' . t('cancel merge') . '</a></small><br/><br/><hr><br/>',
    );

    $form['merge-a'] = array( '#type' => 'value', '#value' => $label_1, );
    $form['merge-b'] = array( '#type' => 'value', '#value' => $label_2, );

    return $form;

  }

  elseif ($param[0] == 'rename' && $param[1] != '') {

    $label = db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id=%d", $param[1]));

    drupal_set_title(t('Rename the tag "@label"', array('@label' => $label)) . $show_help );

    $form['tag-rename'] = array(
      '#title' => t('Enter new name'),
      '#type' => 'textfield',
    );

    $form['submit-rename'] = array(
      '#type' => 'submit',
      '#value' => t('Rename'),
    );

    $form[] = array(
      '#type' => 'item',
      '#value' => '<small><a href="' . $page_url . '">' . t('cancel rename') . '</a></small><br/><br/><hr><br/>',
    );

    $form['tag-id'] = array( '#type' => 'value', '#value' => $param[1], );

    return $form;

  }

  // Create specialized queries to collect
  // the tags from the database.
  $default_query = 'SELECT * FROM {dae_tag}';

  $query = $default_query;
  $args = array(); $filters = array();

  if ($param[0] == 'opt1') {
    $query .= " WHERE tag_label='%s' ";
    $args = str_replace('_', ' ', $param[1]);
    $filter_state = FALSE;
    $search_state = 'opt1';
  }
  elseif ($param[0] == 'opt2') {
    $query .= " WHERE tag_label LIKE '%s' ";
    $args = '%' . str_replace('_', ' ', $param[1]) . '%';
    $filter_state = FALSE;
    $search_state = 'opt2';
  }
  elseif ($param[0] == 'opt3') {

    $args = $filters = explode('_', $param[1]);

    for ($i=0; $i < count($filters); $i++) {

      if ($i == 0) {
        $query .= " WHERE tag_label LIKE '%s' ";
      }
      else {
        $query .= " OR tag_label LIKE '%s'";
      }

      $args[$i] = '%' . $filters[$i] . '%';

    }

    $filter_state = FALSE;
    $search_state = 'opt3';

  }
  elseif ($param[0] == 'opt4') {
    $query .= " WHERE upper(substring(`tag_label`,1,1))='%s'";
    $args = drupal_substr($param[1], 0, 1);
    $filter_state = FALSE;
    $search_state = 'opt4';
  }
  else {
    $filter_state = TRUE;
    $search_state = 'opt2';
  }

  // List tags in alphabetical order
  $query .= ' ORDER BY tag_label';

  $selected_tags = array();

  $tag_count = 0;

  // The query and checkbox display
  $result = db_query($query, $args);
  while ($row = db_fetch_array($result)) {

    $slo_count = db_result(db_query("SELECT COUNT(*) FROM {dae_slo_tag} WHERE tag_id=%d", $row['id']));

    $selected_tags[$row['id']] = $row['tag_label'] . ' <font style="font-size:xx-small;">(' . $slo_count . ')</font>';

    $tag_count++;

  }

  if ($selected_tags) {

    $form['checkboxes'] = array(
      '#title' => t('Selected tags'),
      '#type' => 'checkboxes',
      '#options' => $selected_tags,
    );

  }
  else {

    $form['checkboxes'] = array(
      '#title' => t('Selected tags'),
      '#type' => 'item',
      '#value' => '<ul><li><i>' . t('No matches found') . '</i></li></ul>',
    );

  }

  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter options'),
    '#weight' => -20,
    '#collapsible' => TRUE,
    '#collapsed' => $filter_state,
  );

  // Textfield for input. The span class is added to set the warning '*'
  // The #required field sets a internal Drupal warning that disables the
  // functionality of the 'view all' button.
  $form['filter']['filter_string'] = array(
    '#title' => t('Enter filter string') . '<span class="form-required" title="This Field is Required.">*</span>',
    '#type' => 'textfield',
    '#default_value' => $param[1],
    '#weight' => -18,
    '#prefix' => '<blockquote>',
    '#suffix' => '</blockquote>',
  );

  // Functions to filter the list (since the list of tags can be exhaustive)
  $form['filter']['method'] = array(
    '#type' => 'select',
    '#title' => t('Filter type'),
    '#options' => array('opt1' => t('List tags that match the filter string.'),
                        'opt2' => t('List tags that are "like" the filter string.'),
                        'opt3' => t('List tags containing any of the filter string words.'),
                        'opt4' => t('List tags that begin with a given character.'),
                      ),
    '#default_value' => $search_state,
    '#weight' => -16,
    '#prefix' => '<blockquote>',
    '#suffix' => '</blockquote>',
  );

  if ($param[0]) {

    $form['filter']['filter-submit'] = array(
      '#type' => 'submit',
      '#value' => t('Filter tags'),
      '#weight' => -14,
      '#prefix' => '<blockquote>',
    );

    $form['filter']['filter-view-all'] = array(
      '#type' => 'submit',
      '#value' => t('View all'),
      '#weight' => -14,
      '#suffix' => '</blockquote>',
    );

  }
  else {

    $form['filter']['filter-submit'] = array(
      '#type' => 'submit',
      '#value' => t('Filter tags'),
      '#weight' => -14,
      '#prefix' => '<blockquote>',
      '#suffix' => '</blockquote>',
    );

  }

  if ($tag_count > 0) {

    // Top Buttons
    $form['top-merge-button'] = array(
      '#type' => 'submit',
      '#value' => t('Merge'),
      '#weight' => -10,
    );

    $form['top-delete-button'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => -8,
    );

    $form['top-split-button'] = array(
      '#type' => 'submit',
      '#value' => t('Split'),
      '#weight' => -6,
    );

    $form['top-rename-button'] = array(
      '#type' => 'submit',
      '#value' => t('Rename'),
      '#weight' => -4,
    );

    if ($tag_count >= 10) {

      // Bottom Buttons
      $form['bot-merge-button'] = array(
        '#type' => 'submit',
        '#value' => t('Merge'),
        '#weight' => 20,
      );

      $form['bot-delete-button'] = array(
        '#type' => 'submit',
        '#value' => t('Delete'),
        '#weight' => 30,
      );

      $form['bot-split-button'] = array(
        '#type' => 'submit',
        '#value' => t('Split'),
        '#weight' => 40,
      );

      $form['bot-rename-button'] = array(
        '#type' => 'submit',
        '#value' => t('Rename'),
        '#weight' => 50,
      );

      if ($param[0]) {

        $form['filter-view-all'] = array(
          '#type' => 'submit',
          '#value' => t('View all'),
          '#weight' => 60,
        );

      }

    }

  }

  return $form;

}


/*
 * Implementation of hook_validate().
 */
function daedalus_manage_tags_form_validate($form, &$form_state) {

  global $base_url;

  $selected_tags = array();

  // Get the selected checkboxes and make a list of the tag ids
  if ($form_state['values']['checkboxes']) {
    foreach ($form_state['values']['checkboxes'] as $tid => $label) {
      if ($label) {
        $selected_tags[] = $tid;
      }
    }
  }

  // Determine if the proper amout of tags have been selected
  switch ($form_state['values']['op']) {

    case $form_state['values']['filter-submit']:

      $filter_string = $form_state['values']['filter_string'];

      if (!$filter_string) {
        form_set_error('filter_string', t('Please enter a filter string or filter character'));
      }

      break;

    case $form_state['values']['top-split-button']:
    case $form_state['values']['bot-split-button']:

      if (sizeof($selected_tags) != 1) {
        form_set_error('', t('You must select 1 tag to split'));
      }

      break;

    case $form_state['values']['top-rename-button']:
    case $form_state['values']['bot-rename-button']:

      if (sizeof($selected_tags) != 1) {
        form_set_error('', t('You must select 1 tag to rename'));
      }

      break;

    case $form_state['values']['top-merge-button']:
    case $form_state['values']['bot-merge-button']:

      if (sizeof($selected_tags) != 2) {
        form_set_error('', t('You must select 2 tags to merge'));
      }

      break;

    case $form_state['values']['top-delete-button']:
    case $form_state['values']['bot-delete-button']:

      if (sizeof($selected_tags) <= 0) {
        form_set_error('', t('You must select at least 1 tag to delete'));
      }

      break;

    case $form_state['values']['submit-rename']:

      $new_tag = $form_state['values']['tag-rename'];
      $tag_id = $form_state['values']['tag-id'];

      if (!$new_tag) {
        form_set_error('tag-rename', t('Enter new name field is required.'));
      }

      if (db_result(db_query("SELECT id FROM {dae_tag} WHERE tag_label='%s' AND id<>%d", $new_tag, $tag_id))) {
        form_set_error($new_tag, t('This tag already exists.'));
      }

      break;

    case $form_state['values']['submit_merge']:

      if (!$form_state['values']['merge']) {
        form_set_error('merge', t('The "Enter new tag" field is required.'));
      }
      else {

        $existing_tag = db_result(db_query("SELECT tag_label FROM {dae_tag}
          WHERE tag_label = '%s'", $form_state['values']['merge']));

        if ($existing_tag) {

          $label_a = $form_state['values']['merge-a'];
          $label_b = $form_state['values']['merge-b'];

          if ($existing_tag != $label_a && $existing_tag != $label_b) {
            form_set_error('merge', t('The tag label entered already exists.'));
          }

        }

      }

      break;

    case $form_state['values']['submit-split']:

      $page_url = daedalus_get_setting('manage tags');
      $page_url_length = sizeof(explode('/', $page_url));

      $param = array();
      $param[0] = arg(0+$page_url_length);
      $param[1] = arg(1+$page_url_length);

      if (!$form_state['values']['split-a']) {
        form_set_error('split-a', t('"New tag #1" field is required.'));
      }

      if (!$form_state['values']['split-b']) {
        form_set_error('split-b', t('"New tag #2" field is required.'));
      }

      // Validate the first split-a
      if (db_result(db_query("SELECT COUNT(*) FROM {dae_tag} WHERE id <> %d AND tag_label LIKE '%s'", $param[1], $form_state['values']['split-a'])) > 0) {
        form_set_error('split-a', t('"New tag #1" already exists.'));
      }

      // Validate the first split-b
      if (db_result(db_query("SELECT COUNT(*) FROM {dae_tag} WHERE id <> %d AND tag_label LIKE '%s'", $param[1], $form_state['values']['split-b'])) > 0) {
        form_set_error('split-b', t('"New tag #2" already exists.'));
      }

      break;

  }

}


/**
 * Implementation of hook_submit().
 */
function daedalus_manage_tags_form_submit($form, &$form_state) {

  global $base_url;

  $page_url = daedalus_get_setting('manage tags');
  $page_url_length = sizeof(explode('/', $page_url));

  $param = array();
  $param[0] = arg(0+$page_url_length);//delete/merge/rename
  $param[1] = arg(1+$page_url_length);//ids

  $id_list = '';

  // Get the selected checkboxes and make a list of the tag ids
  if ($form_state['values']['checkboxes']) {
    foreach ($form_state['values']['checkboxes'] as $tid => $label) {
      if ($label != '0') {
        if ($id_list) {
            $id_list .= '_';
        }
        $id_list .= $tid;
      }
    }
  }

  switch ($form_state['values']['op']) {

    // Delete Tags
    case $form_state['values']['submit-delete']:

      $tag_ids = explode('_', $param[1]);

      // Create the query to remove the tags from the {dae_tag} table.
      $tag_placeholders = implode(' OR ', array_fill(0, count($tag_ids), "id=%d"));

      $tag_query = 'DELETE FROM {dae_tag} WHERE ' . $tag_placeholders;

      db_query($tag_query, $tag_ids);

      // Create the query to remove the tags from the {dae_slo_tag} table.
      $slo_tag_placeholders = implode(' OR ', array_fill(0, count($tag_ids), "tag_id=%d"));

      $slo_tag_query = 'DELETE FROM {dae_slo_tag} WHERE ' . $slo_tag_placeholders;

      db_query($slo_tag_query, $tag_ids);

      if (sizeof($tag_ids) > 1) {
        drupal_set_message(t('The tags have been deleted.'));
      }
      else {
        drupal_set_message(t('The tag has been deleted.'));
      }

      drupal_goto($base_url . '/' . $page_url);

      break;

    // Split tags
    case $form_state['values']['submit-split']:

      $orig_id = $param[1];

      $label = $form_state['values']['delete-label'];

      $split_a = $form_state['values']['split-a'];
      $split_b = $form_state['values']['split-b'];

      // Get array of all slos using the origional tag
      $slos_using = array();

      $result = db_query("SELECT * FROM {dae_slo_tag} WHERE tag_id=%d", $orig_id);
      while ($row = db_fetch_array($result)) {
        $slos_using[] = $row['slo_id'];
      }

      // Update the original tag to the value for split_a
      db_query("UPDATE {dae_tag} SET tag_label='%s' WHERE id=%d", $split_a, $orig_id);

      // Create the new tag for the other value
      db_query("INSERT INTO {dae_tag} (tag_label) VALUES('%s')", $split_b);

      // Get the newly created tags id
      $split_b_id = db_result(db_query("SELECT id FROM {dae_tag} WHERE tag_label='%s'", $split_b));

      // Now insert the new tag into each associated SLO
      foreach ($slos_using as $id) {
        db_query("INSERT INTO {dae_slo_tag} (slo_id,tag_id) VALUES(%d,%d)", $id, $split_b_id);
      }

      drupal_set_message(t('Tag "@label" has been split into "@splitA" & "@splitB"',
              array('@label' => $label, '@splitA' => $split_a, '@splitB' => $split_b)));

      drupal_goto($base_url . '/' . $page_url);

      break;


    case $form_state['values']['submit-rename']:

      $new_tag = $form_state['values']['tag-rename'];
      $old_tag = db_result(db_query("SELECT tag_label FROM {dae_tag} WHERE id=%d", $param[1]));

      db_query("UPDATE {dae_tag} SET tag_label='%s' WHERE id=%d", $new_tag, $param[1]);

      drupal_set_message(t('Tag "@old" has been renamed "@new"',
              array('@old' => $old_tag, '@new' => $new_tag)));

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['submit_merge']:

      //make an array of id's passed
      $pass_ids = explode('_', $param[1]);

      $merge_label = $form_state['values']['merge'];
      $label_a = $form_state['values']['merge-a'];
      $label_b = $form_state['values']['merge-b'];

      // If the new merging label is the same as the first label being merged,
      // pass the list of slo's back into the first label which is param[0].
      if ($merge_label == $label_a) {

        $slo_ids = array();

        // First collect the list of SLOs associated with the two merging tags.
        $result = db_query("SELECT DISTINCT slo_id FROM {dae_slo_tag}
          WHERE tag_id=%d OR tag_id=%d", $pass_ids[0], $pass_ids[1]);

        while ($row = db_fetch_array($result)) {
          $slo_ids[] = $row['slo_id'];
        }

        // Delete the second merging tag.
        db_query("DELETE FROM {dae_tag} WHERE id=%d", $pass_ids[1]);

        // Delete all of the associated SLOs for both merging tags.
        db_query("DELETE FROM {dae_slo_tag} WHERE tag_id=%d", $pass_ids[0]);
        db_query("DELETE FROM {dae_slo_tag} WHERE tag_id=%d", $pass_ids[1]);

        // Add the associated SLOs to the one tag.
        $query = "INSERT INTO {dae_slo_tag} (slo_id, tag_id) VALUES (%d,%d)";
        for ($i=0; $i < count($slo_ids); $i++) {
          db_query($query, $slo_ids[$i], $pass_ids[0]);
        }

        drupal_set_message(t('Tags "@first" & "@second" have been merged into "@merge"',
                array('@first' => $label_a, '@second' => $label_b, '@merge' => $merge_label)));

        drupal_goto($base_url . '/' . $page_url);

      }
      elseif ($merge_label == $label_b) {

        $result = db_query("SELECT DISTINCT slo_id FROM {dae_slo_tag}
          WHERE tag_id=%d OR tag_id=%d", $pass_ids[0], $pass_ids[1]);

        while ($row = db_fetch_array($result)) {
          $slo_ids[] = $row['slo_id'];
        }

        db_query("DELETE FROM {dae_tag} WHERE id=%d", $pass_ids[0]);
        db_query("DELETE FROM {dae_slo_tag} WHERE tag_id=%d", $pass_ids[0]);
        db_query("DELETE FROM {dae_slo_tag} WHERE tag_id=%d", $pass_ids[1]);

        for ($i=0; $i < count($slo_ids); $i++) {
          db_query("INSERT INTO {dae_slo_tag} (slo_id, tag_id) VALUES (%d,%d)", $slo_ids[$i], $pass_ids[1]);
        }

        drupal_set_message(t('Tags "@first" & "@second" have been merged into "@merge"',
                array('@first' => $label_a, '@second' => $label_b, '@merge' => $merge_label)));

        drupal_goto($base_url . '/' . $page_url);

      }
      else {

        $result = db_query("SELECT DISTINCT slo_id FROM {dae_slo_tag}
          WHERE tag_id=%d OR tag_id=%d", $pass_ids[0], $pass_ids[1]);

        while ($row = db_fetch_array($result)) {
          $slo_ids[] = $row['slo_id'];
        }

        db_query("DELETE FROM {dae_tag} WHERE id=%d", $pass_ids[1]);
        db_query("DELETE FROM {dae_slo_tag} WHERE tag_id=%d", $pass_ids[0]);
        db_query("DELETE FROM {dae_slo_tag} WHERE tag_id=%d", $pass_ids[1]);

        // If the new tag is not equal to either of the merging tags, update
        // one of the tags with the new value instead of deleting both.
        db_query("UPDATE {dae_tag} SET tag_label='%s' WHERE id=%d", $merge_label, $pass_ids[0]);

        $query = "INSERT INTO {dae_slo_tag} (slo_id, tag_id) VALUES (%d,%d)";
        for ($i=0; $i < count($slo_ids); $i++) {
          db_query($query, $slo_ids[$i], $pass_ids[0]);
        }

        drupal_set_message(t('Tags "@first" & "@second" have been merged into "@merge"',
                array('@first' => $label_a, '@second' => $label_b, '@merge' => $merge_label)));

        drupal_goto($base_url . '/' . $page_url);

      }

      break;

    // Filtering the results, redirect to the filtered list.
    case $form_state['values']['filter-submit']:

      $filter_string = $form_state['values']['filter_string'];

      $filter_string = str_replace(' ', '_', $filter_string);

      $filter_method = $form_state['values']['method'];

      drupal_goto($base_url . '/' . $page_url . '/' . $filter_method . '/' . $filter_string);

      break;

    // Reset the filter
    case $form_state['values']['filter-view-all']:
      drupal_goto($base_url . '/' . $page_url);

      break;

    // Goto Split tags.
    case $form_state['values']['top-split-button']:
    case $form_state['values']['bot-split-button']:

      drupal_goto($base_url . '/' . $page_url . '/split/' . $id_list);

      break;

    // Goto Delete tags.
    case $form_state['values']['top-delete-button']:
    case $form_state['values']['bot-delete-button']:

      drupal_goto($base_url . '/' . $page_url . '/delete/' . $id_list);

      break;

    // Goto Merge tags.
    case $form_state['values']['top-merge-button']:
    case $form_state['values']['bot-merge-button']:

      drupal_goto($base_url . '/' . $page_url . '/merge/' . $id_list);

      break;

    // Goto Rename tags.
    case $form_state['values']['top-rename-button']:
    case $form_state['values']['bot-rename-button']:

      drupal_goto($base_url . '/' . $page_url . '/rename/' . $id_list);

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