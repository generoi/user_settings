<?php

/**
 * Implements hook_user_settings_properties_info().
 */
function user_settings_user_settings_properties_info() {
  $export = array();
  $property = new stdClass();
  $property->api_version = 1;
  $property->machine_name = 'font_size';
  $property->name = 'Font Size';
  $property->description = 'The font size is relative and affects all text.';
  $property->data = array(
    'type' => 'select',
    'group' => 'Font Settings',
    'selector' => 'html',
    'property' => 'font-size',
    'options' => '1em|12\r\n1.2em|14\r\n1.5em|16',
  );
  $export['font_size'] = $property;

  $property = new stdClass();
  $property->api_version = 1;
  $property->machine_name = 'background_color';
  $property->name = 'Background Color';
  $property->description = '';
  $property->data = array(
    'type' => 'color',
    'group' => 'Color Settings',
    'options' => '',
    'selector' => 'html, body',
    'property' => 'background-color',
  );
  $export['background_color'] = $property;

  return $export;
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
    '#options' => $option_list,
    '#default_value' => $value,
  );
  return $element;
}

/**
 * Implements hook_user_settings_widget_WIDGET().
 */
function user_settings_user_settings_widget_upload($value, $property, $form) {
  $element = array(
  );
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
}

/**
 * Implements hook_user_settings_stylesheet_rule_alter().
 */
function user_settings_user_settings_stylesheet_rule_alter($rule, $context) {
  // Import the font for typeface widgets.
  if ($context['widget'] == 'typeface') {
    $property = $context['property'];
    $rules = &$context['rules'];
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
}
