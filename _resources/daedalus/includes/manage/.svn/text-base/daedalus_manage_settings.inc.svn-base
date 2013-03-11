<?php


/**
 * @file
 * daedalus-menu-manage.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */


//------------------------------------------------------------------------------
// Daedalus->Manage->Daedalus Settings Callback Functions
//------------------------------------------------------------------------------
function daedalus_manage_settings_page() {
  return drupal_get_form('daedalus_manage_settings_form');
}


/**
 * Menu Location: Daedalus -> Manage -> Daedalus Settings
 * URL Location:  daedalus/manage/settings
 *
 * Displays: Modifiable settings page which change the
 *           behaviour daedalus and informs the user of
 *           the status of various components.
 */
function daedalus_manage_settings_form($form) {

  global $base_url;

  $form = array();

  $page_url = daedalus_get_setting('manage settings');
  $page_url_length = sizeof(explode('/', $page_url));
  $page_url = $base_url . '/' . $page_url;

  $param = array();

  $param[0] = arg(0+$page_url_length);
  $param[1] = arg(1+$page_url_length);

  // Determine if graphviz is installed.
  $installed = exec('find -name graphviz');

  $pwd = exec('pwd');

  $gviz_location = $pwd . '/graphviz';

  if ($installed) {

    $perm = drupal_substr(sprintf('%o', fileperms($gviz_location)), -4);

    if ($perm == '0777') {
      $perm_state = TRUE;
    }
    else {
      $perm_state = FALSE;
    }

  }

  $graphviz_path = daedalus_get_setting('graphviz path');

  if ($graphviz_path == '{DNE-graphviz path}') {
    $installed = $graphviz_path = FALSE;
  }

  // Small message at the end of each title where the user
  // can easily reset a setting to it's default value
  $reset_message = '<small><small>' . t('reset to default') . '</small></small>';

  // If there first url is rest, then it should be reset
  if ($param[0] == 'reset') {

    // Set the query to select all non-hidden
    // values to reset to the default value.
    switch ($param[1]) {

      case 'confirm':

        $form['confirm-reset'] = array(
          '#type' => 'submit',
          '#value' => t('Confirm Reset'),
          '#weight' => -20,
        );

        $form['cancel-reset'] = array(
          '#type' => 'submit',
          '#value' => t('Cancel Reset'),
          '#weight' => -20,
        );

        break;

      case 'all':

        $result = db_query("SELECT * FROM {dae_settings} ");

        $type = $row['type'];

        //reset the settings, and give a message saying it was reset
        while ($row = db_fetch_array($result)) {

          db_query("UPDATE {dae_settings} SET value='%s' WHERE id=%d", $row['default_value'], $row['id']);

          drupal_set_message(t('The "@type" setting "@setting" has been reset to its default value.',
                  array('@type' => $type, '@setting' => $row['setting'])));

        }

        // Rebuild the menu, and redirect to the settings page (the redirect
        // is important because the URL to the settings page may have changed)
        // and the menu_rebuild will make all the links accurate again.
        menu_rebuild();

        drupal_goto($page_url);

        break;

      // Reset the setting passed in the url
      default:

        $result = db_query("SELECT * FROM {dae_settings} WHERE setting='%s'", str_replace('-', ' ', $param[1]));

        while ($row = db_fetch_array($result)) {

          $type = $row['type'];

          if ($type == 'url') {
            $menu_flag = TRUE;
          }

          db_query("UPDATE {dae_settings} SET value='%s' WHERE id=%d", $row['default_value'], $row['id']);

          drupal_set_message(t('The "@type" setting "@setting" has been reset to its default value.',
                  array('@type' => $type, '@setting' => $row['setting'])));

        }

        // Update the menu if the selected setting is a url.
        if ($menu_flag) {

          // Rebuild the menu, and redirect to the settings page (the redirect
          // is important because the URL to the settings page may have changed)
          // and the menu_rebuild will make all the links accurate again.
          menu_rebuild();

          drupal_goto($page_url . '/defaultURL');

        }

        drupal_goto($page_url . '/default');

        break;

    }

  }

  $setting_links = '<br /><strong><a href="' . $page_url . '#root">' . t('Root User Settings') . '</a></strong>
                    <br /><strong><a href="' . $page_url . '#url">' . t('URL Settings') . '</a></strong>
                    <br /><strong><a href="' . $page_url . '#bool">' . t('Boolean Settings') . '</a></strong>
                    <br /><strong><a href="' . $page_url . '#str">' . t('String Settings') . '</a></strong>
                    <br /><strong><a href="' . $page_url . '#int">' . t('Integer Settings') . '</a></strong>
                    <br /><strong><a href="' . $page_url . '#img">' . t('Image Settings') . '</a></strong>';

  $warning_message  = '<br /><strong><i>' . t('Warning:') . '</i></strong><br />';
  $warning_message .= t('Please use caution when modifying the integer, URL address, image and boolean field settings.
      This may cause different aspects of the site to behave strangely. If you notice any problems after changing these
      settings try resetting the fields to their defualt values. If you have changed multiple settings and can not find
      the problem, you can click <a href="@reset-confirm">here</a> to reset each field to its default value.',
          array('@reset-confirm' => url($page_url . '/reset/confirm')));

  $warning_message .= '<br /><br /><strong><i>' . t('Note:') .'</i></strong><br />';
  $warning_message .= t('When changing a URL value this module is composed of four default groups:');
  $warning_message .= '<ul>';
  $warning_message .= '<li>' . t('Analyse') . '</li>';
  $warning_message .= '<li>' . t('Browse') . '</li>';
  $warning_message .= '<li>' . t('Build') . '</li>';
  $warning_message .= '<li>' . t('Manage') . '</li>';
  $warning_message .= '</ul>';
  $warning_message .= t('For example, the parent for the browse menu is "daedalus/browse" and a child is "daedalus/browse/courses".
    To change the Daedalus menu you must use the drupal administration tools. If you choose to do so it is recommended to change
    each group URL to correspond, however this is not a requirement.');
  $warning_message .= '<br /><br /><strong><i>' . t('Visual Maps:') . '</i></strong><br />';

  if ($installed && $perm_state && $graphviz_path) {

    $warning_message .= t('The visual map directory has been successfully installed.');

  }
  elseif (!$installed) {

    $warning_message .= t('A directory with full permissions is required to enable the visual map functionality.
                           You must place the directory "graphviz" here: %graphviz-location',
            array('%graphviz-location' => $pwd)) . '/<b>graphviz</b>';

  }
  elseif (!$perm_state) {

    $warning_message .= t('The current graphviz file permissions are set to "%perm". Please change this
                           setting to "%new-perm" here: %graphviz-location',
            array('%perm' => $perm, '%new-perm' => '0777', '%graphviz-location' => $pwd)) . '/<b>graphviz</b>';

  }
  elseif (!$graphviz_path) {

    $warning_message .= t('The Daedalus setting "graphviz path" is required to view the visual maps.') . '<br />';
    $warning_message .= t('You may determine the correct path by using the command "which dot" or "which neato".') . '<br />';
    $warning_message .= t('This setting may be updated below.');

  }

  $current_theme = db_result(db_query("SELECT value FROM {variable} WHERE name='theme_default'"));

  $temp = explode('"', $current_theme);

  $current_theme = $temp[1];

  $warning_message .= '<br /><br /><strong><i>' . t('Current Theme:') .'</i></strong><br />';
  $warning_message .= t('Change the "%current-theme" theme settings ', array('%current-theme' => ucwords($current_theme)));
  $warning_message .= '<a href="' . $base_url . '/admin/build/themes/settings/' . $current_theme . '">' . t('here') . '</a>';

  // Display the setting links.
  $form['setting-links'] = array(
    '#type' => 'item',
    '#value' => $setting_links,
    '#weight' => -15,
  );

  // Display warning
  $form['warning'] = array(
    '#type'   => 'item',
    '#value'  => $warning_message,
    '#weight' => -10,
  );

    //                  \\
   // ROOT USER SETTINGS \\
  //                      \\
  $form[] = array(
    '#type' => 'item',
    '#value' => '<h2>' . t('Root User Settings') . '</h2>',
    '#prefix' => '<a name="root" /></a><br /><br />',
    '#suffix' => '<div style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"></div>',
  );

  $root_settings = db_fetch_array(db_query("SELECT name, mail FROM {users} WHERE uid='1'"));
  $root_name = $root_settings['name'];
  $root_mail = $root_settings['mail'];

  $form['root-name'] = array(
    '#type' => 'textfield',
    '#title' => t('Current root user name'),
    '#default_value' => $root_name,
    '#required' => TRUE,
  );

  $form['root-mail'] = array(
    '#type' => 'textfield',
    '#title' => t('Current root e-mail address'),
    '#default_value' => $root_mail,
    '#required' => TRUE,
  );

    //            \\
   // URL SETTINGS \\
  //                \\
  $form[] = array(
    '#type' => 'item',
    '#value' => '<h2>' . t('URL Settings') .'</h2>',
    '#prefix' => '<a name="url"></a><br /><br />',
    '#suffix' => '<div style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"></div>',
  );

  $result = db_query("SELECT * FROM {dae_settings} WHERE type='url' ORDER BY id");

  while ($row = db_fetch_array($result)) {

    // The str_replace() function is used several times to replace the blank spaces with dashes. This is to
    // prevent the href from breaking due to white space as well as the $form['data-xxx'].
    $reset_string = ' - <a href="' . $page_url . '/reset/' . str_replace(' ', '-', $row['setting']) . '">' . $reset_message . '</a>';

    $url_title = '<big>' . t('%setting', array('%setting' => $row['setting'])) . ' ' . $reset_string . '</big>';
    $url_description = t('%description', array('%description' => $row['description']));

    $form[str_replace(' ', '-', $row['setting'])] = array(
      '#type' => 'textfield',
      '#title' => $url_title,
      '#default_value' => $row['value'],
      '#description' => $url_description,
      '#required' => TRUE,
    );

  }

  $form['reset-all-1'] = array(
    '#type' => 'submit',
    '#value' => t('Reset all to default'),
  );

  // Submit button and reset all settings button
  $form['save-settings-1'] = array(
    '#type' => 'submit',
    '#value' => t('Save all settings'),
  );

  // Return to the top of the page.
  $form['back-to-top-1'] = array(
    '#type' => 'submit',
    '#value' => t('Return to Top'),
  );

    //                \\
   // BOOLEAN SETTINGS \\
  //                    \\
  $form[] = array(
    '#type' => 'item',
    '#value' => '<h2>' . t('Boolean Settings') . '</h2>',
    '#prefix' => '<a name="bool"></a><br /><br />',
    '#suffix' => '<div style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"></div>',
  );

  $result = db_query("SELECT * FROM {dae_settings} WHERE type='bool' ORDER BY id");
  while ($row = db_fetch_array($result)) {

    $reset_string = ' - <a href="' . $page_url . '/reset/' . str_replace(' ', '-', $row['setting']) . '">' . $reset_message . '</a>';

    $bool_title = '<big>' . t('%setting', array('%setting' => $row['setting'])) . ' ' . $reset_string . '</big>';
    $bool_description = t('%description', array('%description' => $row['description']));

    $form[str_replace(' ', '-', $row['setting'])] = array(
      '#title' => $bool_title,
      '#type' => 'radios',                          // It is important that it is false, true. Otherwise if the setting is false
      '#options' => array( '0' => t('false'),       // it will default to "true". Right now it's just using the value
                           '1' => t('true') ),      // (which can only be 0 or 1). If it is false, the default value is the first
      '#default_value' => $row['value'],            // radio. So 0 will be the first, (false) and 1 will be the second (true).
      '#description' => $bool_description,
      '#required'  => TRUE,
    );

  }

  $form['reset-all-2'] = array(
    '#type' => 'submit',
    '#value' => t('Reset all to default'),
  );

  // Submit button and reset all settings button
  $form['save-settings-2'] = array(
    '#type' => 'submit',
    '#value' => t('Save all settings'),
  );

  // Return to the top of the page.
  $form['back-to-top-2'] = array(
    '#type' => 'submit',
    '#value' => t('Return to Top'),
  );

    //               \\
   // STRING SETTINGS \\
  //                   \\
  $form[] = array(
    '#type' => 'item',
    '#value' => '<h2>' . t('String Settings') . '</h2>',
    '#prefix' => '<a name="str"></a><br /><br />',
    '#suffix' => '<div style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"></div>',
  );

  $result = db_query("SELECT * FROM {dae_settings} WHERE type='str' ORDER BY id");
  while ($row = db_fetch_array($result)) {

    $reset_string = ' - <a href="' . $page_url . '/reset/' . str_replace(' ', '-', $row['setting']) . '">' . $reset_message . '</a>';

    $str_title = '<big>' . t('%setting', array('%setting' => $row['setting'])) . ' ' . $reset_string . '</big>';
    $str_description = t('%description', array('%description' => $row['description']));

    $form[str_replace(' ', '-', $row['setting'])] = array(
      '#type' => 'textfield',
      '#title' => $str_title,
      '#default_value' => $row['value'],
      '#description' => $str_description,
      '#required' => TRUE,
    );

  }

  $form['reset-all-3'] = array(
    '#type' => 'submit',
    '#value' => t('Reset all to default'),
  );

  // Submit button and reset all settings button
  $form['save-settings-3'] = array(
    '#type' => 'submit',
    '#value' => t('Save all settings'),
  );

  // Return to the top of the page.
  $form['back-to-top-3'] = array(
    '#type' => 'submit',
    '#value' => t('Return to Top'),
  );

    //                \\
   // INTEGER SETTINGS \\
  //                    \\
  $form[] = array(
    '#type' => 'item',
    '#value' => '<h2>' . t('Integer Settings') . '</h2>',
    '#prefix' => '<a name="int"></a><br /><br />',
    '#suffix' => '<div style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"></div>',
  );

  $result = db_query("SELECT * FROM {dae_settings} WHERE type='int' ORDER BY id");
  while ($row = db_fetch_array($result)) {

    $reset_string = ' - <a href="' . $page_url . '/reset/' . str_replace(' ', '-', $row['setting']) . '">' . $reset_message . '</a>';

    $int_title = '<big>' . t('%setting', array('%setting' => $row['setting'])) . ' ' . $reset_string . '</big>';
    $int_description = t('%description', array('%description' => $row['description']));

    $form[str_replace(' ', '-', $row['setting'])] = array(
      '#type' => 'textfield',
      '#title' => $int_title,
      '#default_value' => $row['value'],
      '#description' => $int_description,
      '#required' => TRUE,
    );

  }

  $form['reset-all-4'] = array(
    '#type' => 'submit',
    '#value' => t('Reset all to default'),
  );

  // Submit button and reset all settings button
  $form['save-settings-4'] = array(
    '#type' => 'submit',
    '#value' => t('Save all settings'),
  );

  // Return to the top of the page.
  $form['back-to-top-4'] = array(
    '#type' => 'submit',
    '#value' => t('Return to Top'),
  );

    //            \\
   // IMAGE UPLOAD \\
  //                \\
  if ($param[0] == 'upload') {

    $form[] = array(
      '#type' => 'item',
      '#value' => '<h2>' . t('Image Upload') . '</h2>',
      '#prefix' => '<br /><a name="upload" /></a><br /><br />',
      '#suffix' => '<div style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"></div>',
    );

    $image_setting = str_replace('-', ' ', $param[1]);

    // Upload the student learning outcomes.
    $form['new-image'] = array(
      '#type' => 'file',
      '#title' => t('Upload new ') . ucwords($image_setting) . t(' image'),
      '#size' => 50,
      '#description' => t('Upload ".jpg", ".bmp" or ".png" images.'),
      '#prefix' => '<br /><table><tr><td width=525px>',
      '#suffix' => '</td>',
    );

    $form['upload-image'] = array(
      '#type' => 'submit',
      '#value' => t('Upload '),
      '#prefix' => '<td width=10px>',
      '#suffix' => '</td>',
    );

    $form['cancel-reset'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
      '#prefix' => '<td>',
      '#suffix' => '</td></tr></table><table><tr><td></td></tr></table>',
    );

    $form['image-setting'] = array( '#type' => 'value', '#value' => $param[1], );

    // This is required for the '#type' => 'file' form item.
    $form['#attributes'] = array('enctype' => "multipart/form-data");

  }

    //              \\
   // IMAGE SETTINGS \\
  //                  \\
  $form[] = array(
    '#type' => 'item',
    '#value' => '<h2>' . t('Image Settings') . '</h2>',
    '#prefix' => '<a name="img"></a><br /><br />',
    '#suffix' => '<div style="border-top-style:solid; border-width:2px; border-color:#C0C0C0;"></div>',
  );

  $result = db_query("SELECT * FROM {dae_settings} WHERE type='image' ORDER BY id");
  while ($row = db_fetch_array($result)) {

    $setting = str_replace(' ', '-', $row['setting']);

    $img_reset_string  = ' - <a href="' . $page_url . '/upload/' . $setting . '#upload"><small><small>' . t('upload new image') . '</small></small></a>';
    $img_reset_string .= ' or <a href="' . $page_url . '/reset/' . $setting . '">' . $reset_message . '</a>';

    $img_src = $base_url . '/' . $row['value'];

    $img_title = '<big>' . t('%setting', array('%setting' => $row['setting'])) . ' ' . $img_reset_string . '</big>';

    $img_description  = t('%img-description', array('%img-description' => $row['description'])) . '<br/>';
    $img_description .= t('The current image is:') . '<br /><br />';
    $img_description .= '<blockquote><img src="' . $img_src . '" alt="image" /></blockquote>';

    $form[str_replace(' ', '-', $row['setting'])] = array(
      '#type' => 'textfield',
      '#title' => $img_title,
      '#default_value' => $row['value'],
      '#description' => $img_description,
      '#required' => TRUE,
      '#suffix' => '<br /><br />',
    );

  }

  $form['reset-all-5'] = array(
    '#type' => 'submit',
    '#value' => t('Reset all to default'),
  );

  // Submit button and reset
  // all settings button.
  $form['save-settings-5'] = array(
    '#type' => 'submit',
    '#value' => t('Save all settings'),
  );

  // Return to the top of the page.
  $form['back-to-top-5'] = array(
    '#type' => 'submit',
    '#value' => t('Return to Top'),
  );

  // Submit the original root info to compare
  // against the submitted root user values.
  $form['orig-name'] = array( '#type' => 'value', '#value' => $root_name, );
  $form['orig-mail'] = array( '#type' => 'value', '#value' => $root_mail, );

  return $form;

}


/*
 * Implementation of hook_validate().
 */
function daedalus_manage_settings_form_validate($form, &$form_state) {

  if ($form_state['values']['op'] == $form_state['values']['upload-image']) {

    $session_file = $_FILES['files']['name']['new-image'];

    if ($session_file) {

      $valid_ext = array('jpg', 'bmp', 'png');
      $file_info = explode('.', $session_file);
      $file_ext  = $file_info[1];

      // Validate the file types
      if (!in_array($file_ext, $valid_ext)) {
        form_set_error('new-image', t('Upload ".jpg", ".bmp" or ".png" file formats only.'));
      }

    }

    else {
      form_set_error('new-image', t('An image must be selected for upload.'));
    }

  }
  else {

    // URL's are only going to be allowed to have certian characters.
    // This will use a regular expression to determine if the URL is valid.
    $result = db_query("SELECT * FROM {dae_settings} WHERE type='url'");
    while ($row = db_fetch_array($result)) {

      $setting = $form_state['values'][str_replace(' ', '-', $row['setting'])];

      // Regular expression to allow only lowercase letters,
      // numbers, hyphens, underscores, and slashes.
      $pattern = "/(^[a-z0-9\-_\/]*$)/";

      if (preg_match($pattern, $setting) == 0) {
        form_set_error( str_replace(' ', '-', $row['setting']), t('The URL contains invalid charactars. Only lowercase letters, numbers, hyphens, underscores and backslashes are permitted.'));
      }

    }

    // Fields that are integers must be numeric
    $result = db_query("SELECT * FROM {dae_settings} WHERE type='int'");
    while ($row = db_fetch_array($result)) {

      $setting =  str_replace(' ', '-', $row['setting']);

      $value = $form_state['values'][$setting];

      if (!is_numeric($value)) {
        form_set_error($setting, t('Must be an integer value'));
      }

    }

  }

}

/**
 * Implementation of hook_submit().
 */
function daedalus_manage_settings_form_submit($form, &$form_state) {

  // Get daedalus base url
  global $base_url;

  $page_url = daedalus_get_setting('manage settings');

  switch ($form_state['values']['op']) {

    //delete the pcourse
    case $form_state['values']['confirm-reset']:

      // If reset all selected go to the reset all URL.
      drupal_goto($base_url . '/' . $page_url . '/reset/all');

      break;

    case $form_state['values']['cancel-reset']:
    case $form_state['values']['back-to-top-1']:
    case $form_state['values']['back-to-top-2']:
    case $form_state['values']['back-to-top-3']:
    case $form_state['values']['back-to-top-4']:
    case $form_state['values']['back-to-top-5']:

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['reset-all-1']:
    case $form_state['values']['reset-all-2']:
    case $form_state['values']['reset-all-3']:
    case $form_state['values']['reset-all-4']:
    case $form_state['values']['reset-all-5']:

      drupal_goto($base_url . '/' . $page_url . '/reset/confirm');

      break;

    case $form_state['values']['save-settings-1']:
    case $form_state['values']['save-settings-2']:
    case $form_state['values']['save-settings-3']:
    case $form_state['values']['save-settings-4']:
    case $form_state['values']['save-settings-5']:

      // Update the root user
      $orig_name = $form_state['values']['orig-name'];
      $orig_mail = $form_state['values']['orig-mail'];
      $root_name = $form_state['values']['root-name'];
      $root_mail = $form_state['values']['root-mail'];

      if ($orig_name != $root_name) {

        db_query("UPDATE {users} SET name='%s' WHERE uid='1'", $root_name);

        drupal_set_message(t('The root user name "@orig-name" has been changed to "@root-name".',
                array('@orig-name' => $orig_name, '@root-name' => $root_name)));

      }

      if ($orig_mail != $root_mail) {

        db_query("UPDATE {users} SET mail='%s' WHERE uid='1'", $root_mail);

        drupal_set_message(t('The root e-mail address "@orig-mail" has been changed to "@root-mail".',
                array('@orig-mail' => $orig_mail, '@root-mail' => $root_mail)));

      }

      // Update all other settings.
      $result = db_query("SELECT * FROM {dae_settings} ORDER BY id");

      while ($row = db_fetch_array($result)) {

        $type = $row['type'];

        // Set a flag to rebuild the daedalus menu
        if ($type == 'url') {
          $menu_flag = TRUE;
        }

        $setting = $form_state['values'][str_replace(' ', '-', $row['setting'])];

        // If the setting value has changed...
        // update it. And set a message.
        if ($setting != $row['value']) {

          db_query("UPDATE {dae_settings} set value='%s' WHERE id=%d", $setting, $row['id']);

          drupal_set_message(t('The @type setting "@old-setting" has been updated to "@new-setting".',
                  array('@type' => $type, '@old-setting' => $row['setting'], '@new-setting' => $setting)));

        }

      }

      // This function will clear and populate the {menu_router} table, add
      // entries to {menu_links} for new router items, then remove stale items
      // from {menu_links}. If called from update.php or install.php, it will
      // also schedule a call to itself on the first real page load from
      // menu_execute_active_handler(), because the maintenance page environment
      // is different and leaves stale data in the menu tables.
      // http://api.drupal.org/api/drupal/includes--menu.inc/function/menu_rebuild/6
      if ($menu_flag) {
        menu_rebuild();
      }

      drupal_goto($base_url . '/' . $page_url);

      break;

    case $form_state['values']['upload-image']:

      $setting = str_replace('-', ' ', $form_state['values']['image-setting']);

      $validators = array( 'file_validate_extensions' => array( 'png' ) );

      // Check if the directory has already been created
      $images_directory = file_directory_path() . '/daedalusimages';

      if (!file_exists($images_directory)) {

        // Create a new directory for uploaded images.
        exec('mkdir ' . $images_directory);

        // Modify the permissions of this new directory.
        exec('chmod 775 ' . $images_directory);

      }

      // Save new file uploads. I tried saving the image in the /daedalus/img
      // folder but the new files are unable to be written there or moved.
      $file = file_save_upload('new-image', $validators, $images_directory);

      if ($file) {

        // Determine the file name and location
        // and move it to the graviz folder.
        $filename = $file->filename;

        $new_value = $images_directory . '/' . $filename;

        // Update the image setting to the new value saved in the file directory path.
        db_query("UPDATE {dae_settings} set value='%s' WHERE setting='%s'", $new_value, $setting);

        drupal_set_message(t('File saved as "@filename" in "@images-directory/@filename"',
                array('@filename' => $filename, '@images-directory' => $images_directory)));

        file_set_status($file, FILE_STATUS_PERMANENT);

        drupal_goto($base_url . '/' . $page_url);

      }
      else {
        drupal_set_message(t('File not saved.'), 'warning');
      }

      break;

  }

}