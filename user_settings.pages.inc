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
  foreach ($properties as $machine_name => $property) {
    $widget_type = $property->data['type'];
    $widget = user_settings_get_widgets($widget_type);
    $value = isset($settings['properties'][$machine_name]) ? $settings['properties'][$machine_name] : $property->data['default'];
    $function = $widget['setting callback'];

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
