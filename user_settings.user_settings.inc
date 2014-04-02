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

