<?php

/**
 * @file
 */

/**
 * Build form that return all existing exportable objects.
 */
function user_settings_property_list_form($form, &$form_state) {
  $form['#tree'] = TRUE;
  $result = user_settings_property_load_all();

  foreach ($result as $object) {
    $form[$object->machine_name]['name']['#markup'] = check_plain($object->name);
    if ($object->is_overwritten) {
      $form[$object->machine_name]['name']['#markup'] .= ' <sup>' . check_plain($object->type) . '</sup>';
    }
    $form[$object->machine_name]['machine_name']['#markup'] = check_plain($object->machine_name);

    $operations = array();
    if ($object->disabled == TRUE) {
      $operations[] = array(
        'title' => t('Enable'),
        'href' => 'admin/structure/user-settings/' . $object->machine_name . '/enable',
      );
    }
    $operations[] = array(
      'title' => t('Edit'),
      'href' => 'admin/structure/user-settings/' . $object->machine_name . '/edit',
    );
    $operations[] = array(
      'title' => t('Export'),
      'href' => 'admin/structure/user-settings/' . $object->machine_name . '/export',
    );
    // If object are able to store not only in code we could delete (or revert) it.
    // If object was created with hook_user_settings_property_settings_info() it could be only reverted.
    if (empty($object->in_code_only)) {
      $operations[] = array(
        'title' => $object->is_overwritten ? t('Revert') : t('Delete'),
        'href' => 'admin/structure/user-settings/' . $object->machine_name . '/delete',
      );
    }
    if ($object->disabled == FALSE) {
      $operations[] = array(
        'title' => t('Disable'),
        'href' => 'admin/structure/user-settings/' . $object->machine_name . '/disable',
      );
    }
    // Render operation links as CTools dropdown links. They will be processes with dropdown javasctipt.
    $form[$object->machine_name]['actions']['#markup'] = theme('links__ctools_dropbutton',
      array('links' => $operations, 'attributes' => array('class' => array('links', 'inline'))));

    $form[$object->machine_name]['disabled'] = array(
      '#type' => 'value',
      '#value' => $object->disabled,
    );
  }
  $form['#attached']['css'][] = drupal_get_path('module', 'user_settings') . '/css/user_settings.admin.css';

  return $form;
}

/**
 * Example form for create/edit exportable object.
 */
function user_settings_property_edit_form($form, &$form_state, $exportable_object = NULL) {
  $form['#tree'] = TRUE;
  $form['#exportable_object'] = $exportable_object;
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Property name'),
    '#description' => t('Human-readable name of item'),
    '#required' => TRUE,
    '#maxlength' => 64,
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 64,
    '#machine_name' => array(
      'exists' => 'user_settings_property_load',
      'source' => array('name'),
    ),
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#maxlength' => 255,
  );
  $form['data'] = array(
    '#type' => 'fieldset',
    '#title' => 'Settings',
  );
  $widgets = user_settings_get_widgets();
  $option_list = array();
  foreach ($widgets as $name => $widget) {
    $option_list[$name] = $widget['title'];
  }
  $form['data']['type'] = array(
    '#type' => 'select',
    '#title' => t('Widget type'),
    '#options' => $option_list,
    '#required' => TRUE,
  );
  $form['data']['group'] = array(
    '#type' => 'textfield',
    '#title' => t('Group'),
  );
  $form['data']['selector'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Selector'),
    '#description' => t('Eg. %selector', array('%selector' => '.sidebar a')),
  );
  $form['data']['property'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Property'),
    '#description' => t('Eg. %property', array('%property' => 'background-color')),
  );
  $form['data']['default'] = array(
    '#type' => 'textfield',
    '#title' => t('Default value'),
  );
  $form['actions'] = array(
    '#type' => 'action',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create'),
  );

  if (!empty($exportable_object)) {
    // Fill the default form values from existing object.
    $form['name']['#default_value']         = $exportable_object->name;
    $form['machine_name']['#default_value'] = $exportable_object->machine_name;
    $form['machine_name']['#disabled']      = TRUE; // Machine name can't be changed.
    foreach ($exportable_object->data as $key => $value) {
      $form['data'][$key]['#default_value'] = $value;
    }

    // Replace submit value from 'Save' to 'Update'.
    $form['actions']['submit']['#value'] = t('Update');

    // Keep the object in form.
    $form['exportable_object'] = array(
      '#type' => 'value',
      '#value' => $exportable_object,
    );
  }

  return $form;
}

/**
 * Submit callback for exportable object create/edit form.
 */
function user_settings_property_edit_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  // Load or create new exportable object.
  $exportable_object = !empty($values['exportable_object']) ? $values['exportable_object'] : new stdClass() ;

  $exportable_object->name          = check_plain($values['name']);
  $exportable_object->machine_name  = check_plain($values['machine_name']);
  $exportable_object->data          = $values['data'];

  // Create/update exportable object.
  $saved = user_settings_property_save($exportable_object);
  if ($saved) {
    $message = empty($values['exportable_object']) ? 'New item @name was added.' : 'Item @name was updated.';
    drupal_set_message(t($message, array('@name' => $values['name'])));
  }
  else {
    drupal_set_message(t('Could not save object. Something bad happened.'), 'error');
  }

  // Redirect user to list with all exportable objects.
  $form_state['redirect'] = 'admin/structure/user-settings';
}

/**
 * Return exportable code for a singe exportable object.
 */
function user_settings_property_export_form($form, &$form_state, $exportable_object) {
  drupal_set_title(filter_xss(t('Export object "!name"', array('!name' => $exportable_object->name))));
  ctools_include('export');
  $code = ctools_export_crud_export('user_settings_properties', $exportable_object);

  // Display parsed code in the textarea.
  $lines = substr_count($code, "\n");
  $form['export'] = array(
    '#title' => t('Export data'),
    '#type' => 'textarea',
    '#value' => $code,
    '#rows' => $lines,
    '#description' => t('Copy the export text and paste it into import area.'),
  );

  return $form;
}

/**
 * Form for import code with exportable objects.
 */
function user_settings_property_import_form($form, $form_state) {
  $form['import'] = array(
    '#type' => 'textarea',
    '#title' => t('Paste code here to import object'),
  );
  $form['actions'] = array(
    '#type' => 'action',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );

  return $form;
}

/**
 * Validate callback for exporable object import form.
 * Checks whether exportable object can be imported.
 */
function user_settings_property_import_form_validate($form, &$form_state) {
  ctools_include('export');
  $code = $form_state['values']['import'];

  // Try to loaded object with import code.
  $exportable_object_raw = ctools_export_crud_import('user_settings_properties', $code);
  if (is_string($exportable_object_raw)) {
    form_set_error('import', t('Unable to get an import from the code. Errors reported: @errors', array('@errors' => $exportable_object_raw)));
  }
  else {
    $form_state['storage']['exportable_object_raw'] = $exportable_object_raw;
  }
}

/**
 * Submit callback for exportable object import form.
 * Saves imported objects.
 */
function user_settings_property_import_form_submit($form, &$form_state) {
  ctools_include('export');
  $exportable_object_raw = $form_state['storage']['exportable_object_raw'];
  $exportable_object = user_settings_property_load_prepare($exportable_object_raw);
  $exportable_object_original = user_settings_property_load($exportable_object->machine_name);

  // If object with the same machine name exists - delete it.
  if (!empty($exportable_object_original)) {
    user_settings_property_delete($exportable_object_original->machine_name);
  }

  // Save imported object in database.
  if (user_settings_property_save($exportable_object)) {
    // Set up a message depends on recent action.
    if (!empty($exportable_object_original)) {
      drupal_set_message(filter_xss(t('Object "!name" was successfully updated.', array('!name' => $exportable_object->name))));
    }
    else {
      drupal_set_message(filter_xss(t('Object "!name" was successfully imported.', array('!name' => $exportable_object->name))));
    }
  }

  $form_state['redirect'] = 'admin/structure/user-settings';
}

/**
 * Form allows user to delete exportable object from database.
 */
function user_settings_property_delete_form($form, &$form_state, $exportable_object) {
  $form['exportable_object'] = array(
    '#type' => 'value',
    '#value' => $exportable_object,
  );
  if ($exportable_object->is_overwritten) {
    $form['message'] = array(
      '#markup' => t('Are you sure that you want to revert %name?', array('%name' => $exportable_object->name))
    );
  }
  else {
    $form['message'] = array(
      '#markup' => t('Are you sure that you want to delete %name from database?', array('%name' => $exportable_object->name))
    );
  }

  $form['actions'] = array(
    '#type' => 'actions'
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $exportable_object->is_overwritten ? t('Revert') : t('Delete'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Submit callback for exportable object delete form.
 * Removes object from database.
 */
function user_settings_property_delete_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  // Delete exportable object from database.
  if ($values['op'] == $values['submit']) {
    $exportable_object = $values['exportable_object'];
    user_settings_property_delete($exportable_object->machine_name);
    drupal_set_message(t('Exportable object %name was deleted.', array('%name' => $exportable_object->name)));
  }

  // Redirect user to list with all exportable objects.
  $form_state['redirect'] = 'admin/structure/user-settings';
}

/**
 * Page callback. Disables selected exportable object.
 *
 * @param $exportable_object
 *   Exportable object loaded from database.
 */
function user_settings_property_disable_page($exportable_object) {
  ctools_include('export');
  ctools_export_crud_disable('user_settings_properties', $exportable_object);
  drupal_set_message(t('Exportable object %name was disabled.', array('%name' => $exportable_object->name)));
  drupal_goto('admin/structure/user-settings');
}

/**
 * Page callback. Enables selected exportable object.
 *
 * @param $exportable_object
 *   Exportable object loaded from database.
 */
function user_settings_property_enable_page($exportable_object) {
  ctools_include('export');
  ctools_export_crud_enable('user_settings_properties', $exportable_object);
  drupal_set_message(t('Exportable object %name was enabled.', array('%name' => $exportable_object->name)));
  drupal_goto('admin/structure/user-settings');
}
