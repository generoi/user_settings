<?php

/**
 * Implements hook_user_settings_properties_info().
 */
function user_settings_user_settings_properties_info() {
  return array();
}

/**
 * Implements hook_user_settings_widget_info().
 */
function user_settings_user_settings_widget_info() {
  return array(
    'color' => array(
      'title' => t('Color picker'),
    ),
    'typeface' => array(
      'title' => t('Typeface selector'),
    ),
    'select' => array(
      'title' => t('Select list'),
    ),
    'upload' => array(
      'title' => t('Image upload'),
    ),
    'checkbox' => array(
      'title' => t('Checkbox'),
    ),
    'variable' => array(
      'title' => t('Variable'),
      'skip stylesheet' => TRUE,
    ),
  );
}

/**
 * Implements hook_user_settings_widget_WIDGET().
 */
function user_settings_user_settings_widget_color($value, $property, &$form) {
  static $attached = FALSE;
  if (!$attached) {
    $form['#attached']['library'][] = array('system', 'farbtastic');
    $form['#attached']['js'][] = drupal_get_path('module', 'user_settings') . '/js/user_settings.js';
    $form['#attached']['css'][] = drupal_get_path('module', 'color') . '/color.css';
    $attached = TRUE;
  }

  if (!empty($property->data['group'])) {
    $group_name = $property->data['group'];
    $group = drupal_clean_css_identifier($group_name);
    // @TODO multiple groups unique ids
    $form[$group]['color_picker'] = array('#weight' => -1, '#markup' => '<div id="color-picker"></div>');
  }
  else {
    $form['color_picker'] = array('#weight' => -1, '#markup' => '<div id="color-picker"></div>');
  }

  $element = array(
    '#type' => 'textfield',
    '#title' => $property->name,
    '#description' => $property->description,
    '#attributes' => array('class' => array('color')),
    '#size' => 10,
    '#default_value' => $value,
  );
  return $element;
}

/**
 * Implements hook_user_settings_widget_WIDGET().
 */
function user_settings_user_settings_widget_select($value, $property, $form) {
  $options = explode(',', $property->data['options']);
  foreach ($options as $option) {
    list($machine_name, $name) = explode('|', $option);
    $option_list[trim($machine_name)] = trim($name);
  }

  $element = array(
    '#type' => 'select',
    '#title' => $property->name,
    '#description' => $property->description,
    '#options' => $option_list,
    '#default_value' => $value,
  );
  return $element;
}

/**
 * Implements hook_user_settings_widget_WIDGET().
 */
function user_settings_user_settings_widget_upload($value, $property, &$form) {
  $element = array(
    '#type' => 'managed_file',
    '#title' => $property->name,
    '#description' => $property->description,
    '#upload_location' => 'public://user_settings/',
    '#default_value' => $value,
  );
  $form['#widget_upload_fields'][] = $property->machine_name;
  $form['#submit'][] = 'user_settings_widget_upload_submit';
  return $element;
}

/**
 * Implements hook_user_settings_widget_WIDGET().
 */
function user_settings_user_settings_widget_variable($value, $property, &$form) {
  $element = array(
    '#type' => 'textfield',
    '#title' => $property->name,
    '#description' => $property->description,
    '#default_value' => variable_get($property->data['variable_name'], $value),
  );
  $form['#widget_variable_fields'][] = $property->machine_name;
  $form['#submit'][] = 'user_settings_widget_variable_submit';
  return $element;
}

/**
 * Implements hook_user_settings_widget_WIDGET().
 */
function user_settings_user_settings_widget_typeface($value, $property, &$form) {
  $options = explode(',', $property->data['typeface_options']);
  $urls = array();
  foreach ($options as $option) {
    list($url, $name) = explode('|', $option);
    $url = trim($url);
    $name = trim($name);
    $option_list[$name] = $name;
    $form['#attached']['css'][$url] = array('type' => 'external');
  }

  $element = array(
    '#type' => 'radios',
    '#title' => $property->name,
    '#description' => $property->description,
    '#options' => $option_list,
    '#default_value' => $value,
    '#attributes' => array('class' => array('select-font')),
  );
  return $element;
}

/**
 * Implements hook_user_settings_widget_WIDGET().
 */
function user_settings_user_settings_widget_checkbox($value, $property, $form) {
  $element = array(
    '#type' => 'checkbox',
    '#title' => $property->name,
    '#description' => $property->description,
    '#default_value' => $value,
  );
  return $element;
}

/**
 * Implements hook_user_settings_widget_admin_form().
 *
 * Add additional options for our widgets.
 */
function user_settings_user_settings_widget_admin_form(&$form, $exportable_object) {
  // Select widget
  $form['options'] = array(
    '#type' => 'textarea',
    '#title' => t('Options'),
    '#description' => t('Comma separated options in the form value|Human readable'),
    '#states' => array(
      'visible' => array(
        ':input[name="data[type]"]' => array('value' => 'select'),
      ),
    ),
  );
  // Typeface widget
  $form['typeface_options'] = array(
    '#type' => 'textarea',
    '#title' => t('Options'),
    '#description' => t('Comma separated options in the form http://fonts.googleapis.com/css?family=Belleza|Belleza'),
    '#states' => array(
      'visible' => array(
        ':input[name="data[type]"]' => array('value' => 'typeface'),
      ),
    ),
  );
  // Upload widget
  $form['upload_image_style'] = array(
    '#type' => 'select',
    '#title' => t('Image style'),
    '#options' => image_style_options(),
    '#states' => array(
      'visible' => array(
        ':input[name="data[type]"]' => array('value' => 'upload'),
      ),
    ),
  );
  // Checkbox widget
  $form['on_state'] = array(
    '#type' => 'textfield',
    '#title' => t('Checked state property value'),
    '#states' => array(
      'visible' => array(
        ':input[name="data[type]"]' => array('value' => 'checkbox'),
      ),
    ),
  );
  $form['off_state'] = array(
    '#type' => 'textfield',
    '#title' => t('Unhecked state property value'),
    '#states' => array(
      'visible' => array(
        ':input[name="data[type]"]' => array('value' => 'checkbox'),
      ),
    ),
  );
  $form['variable_name'] = array(
    '#type' => 'textfield',
    '#title' => t('The variable name'),
    '#states' => array(
      'visible' => array(
        ':input[name="data[type]"]' => array('value' => 'variable'),
      ),
    ),
  );
}

/**
 * Implements hook_user_settings_stylesheet_rule_alter().
 */
function user_settings_user_settings_stylesheet_rule_alter(&$rule, $context) {
  $property = $context['property'];
  $rules = &$context['rules'];
  // Import the font for typeface widgets.
  if ($context['widget'] == 'typeface') {
    $options = explode(',', $property->data['typeface_options']);
    foreach ($options as $option) {
      list($url, $name) = explode('|', $option);
      $url = trim($url);
      $name = trim($name);
      if ($name == $rule->value && filter_var($url, FILTER_VALIDATE_URL)) {
        $import_rule = '@import "' . $url . '";';
        if (!in_array($import_rule, $rules)) {
          array_unshift($rules, $import_rule);
        }
      }
    }
  }

  // Output the styled image path for image uploads.
  if ($context['widget'] == 'upload') {
    $image_style = $property->data['upload_image_style'];
    if ($file = file_load($rule->value)) {
      $rule->value = 'url("' . image_style_url($image_style, $file->uri) . '")';
    }
  }
}

/**
 * Delegated submit callback for upload widgets.
 */
function _user_settings_widget_upload_submit($form, $form_state) {
  $account = $form['#account'];
  $settings = $form['#user_settings'];
  $fields = array_unique($form['#widget_upload_fields']);
  foreach ($fields as $field) {
    $fid = $form_state['values'][$field];
    $previous_fid = isset($settings['properties'][$field]) ? $settings['properties'][$field] : FALSE;
    if ($file = file_load($form_state['values'][$field])) {
      // If the file hasnt changed skip.
      if ($file->fid == $previous_fid) {
        continue;
      }
      // If the file has changed and a previous version exists, remove the
      // previous.
      else if ($previous_fid && ($previous = file_load($previous_fid))) {
        file_usage_delete($previous, 'user_settings', 'user_settings', $account->uid);
        file_delete($previous);
      }
      $file->status = FILE_STATUS_PERMANENT;
      file_save($file);
      file_usage_add($file, 'user_settings', 'user_settings', $account->uid);
    }
  }
}

/**
 * Delegated submit callback for variable widgets.
 */
function _user_settings_widget_variable_submit($form, $form_state) {
  $properties = $form['#properties'];
  $fields = array_unique($form['#widget_variable_fields']);
  foreach ($fields as $field) {
    $value = $form_state['values'][$field];
    $property = $properties[$field];
    $variable_name = $property->data['variable_name'];
    variable_set($variable_name, $value);
  }
}
