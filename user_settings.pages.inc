<?php

/**
 *  Build the user settings form.
 */
function user_settings_form($form, $form_state, $account) {
  $form['properties'] = array(
    '#type' => 'vertical_tabs',
  );

  $properties = user_settings_property_load_all();
  $settings = user_settings_load();
  $form['#properties'] = $properties;
  $form['#user_settings'] = $settings;
  $groups = array();
  $widget_callbacks = array();
  // Figure out which module defines which widget.
  foreach (module_implements('user_settings_widget_info') as $module) {
    $widget_info = module_invoke($module, 'user_settings_widget_info');
    foreach($widget_info as $widget_name => $widget) {
      $widget_callbacks[$widget_name] = $module;
    }
  }

  foreach ($properties as $machine_name => $property) {
    $widget_name = $property->data['type'];
    $widget_module = $widget_callbacks[$widget_name];
    // The .inc file is already loaded due to module_invoke.
    $function = $widget_module . '_user_settings_widget_' . $widget_name;
    $value = isset($settings['properties'][$machine_name]) ? $settings['properties'][$machine_name] : $property->data['default'];

    if (!empty($property->data['group'])) {
      $group_name = $property->data['group'];
      $group = drupal_clean_css_identifier($group_name);
      if (!isset($form[$group])) {
        $form[$group] = array(
          '#type' => 'fieldset',
          '#title' => t($group_name),
          '#tree' => FALSE,
          '#collapsible' => FALSE,
          '#collapsed' => FALSE,
          '#group' => 'properties',
        );
      }
      $element = $function($value, $property, $form);
      $form[$group][$machine_name] = $element;
    }
    else {
      $element = $function($value, $property, $form);
      $form[$machine_name] = $element;
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('user_settings_form_reset'),
  );

  return $form;
}

/**
 *  Form submit actions.
 */
function user_settings_form_submit($form, &$form_state) {
  $properties = $form['#properties'];
  $settings = $form['#user_settings'];
  foreach ($properties as $key => $property) {
    $settings['properties'][$key] = trim($form_state['values'][$key]);
  }

  user_settings_save($settings);
  drupal_set_message(t('Settings saved.'));
}

/**
 * Reset user settings.
 */
function user_settings_form_reset($form, &$form_state) {
  user_settings_delete();
  drupal_set_message(t('Settings restored.'));
}
