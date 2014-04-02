<?php

/**
 *  Build the user settings form.
 */
function user_settings_form($form, $form_state, $account) {
  $form['#account'] = $account;

  /**
   *  @see get_db_values().
   */
  $select = get_db_values($account->uid);
  $settings = drupal_json_decode($select['settings']);
  $fonts_array = get_fonts('family');

  /* Import font-families to form. */
  $imports = '';
  foreach (get_fonts('url') as $key => $value) {
    $imports .= '@import url(' . $value . ');' . PHP_EOL;
  }

  /* Add farbtastic scripts and css. */
  drupal_add_css($imports, array('type' => 'inline', 'preprocess' => FALSE));
  drupal_add_js('misc/farbtastic/farbtastic.js');
  drupal_add_js(drupal_get_path('module', 'user_settings') . '/js/scripts.js');
  drupal_add_css('misc/farbtastic/farbtastic.css');
  drupal_add_css(drupal_get_path('module', 'color') . '/color.css', array('media' => 'all'));

  /**
   * Font settings.
   */
  $form['font'] = array(
    '#type' => 'fieldset',
    '#title' => t('Font settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  /* Font family */
  $form['font']['heading_font_family'] = array(
    '#type' => 'radios',
    '#title' => t('Heading font family'),
    '#options' => $fonts_array,
    '#default_value' => (isset($settings['heading_font_family']) ? $settings['heading_font_family'] : ''),
    '#attributes' => array(
      'class' => array('select-font'),
    ),
  );
  /* Font family */
  $form['font']['font_family'] = array(
    '#type' => 'radios',
    '#title' => t('Text font family'),
    '#options' => $fonts_array,
    '#default_value' => (isset($settings['font_family']) ? $settings['font_family'] : ''),
    '#attributes' => array(
      'class' => array('select-font'),
    ),
  );
  /* Font size */
  $form['font']['font_size'] = array(
    '#type' => 'select',
    '#title' => t('Font size'),
    '#description' => t('The font size is relative and affects all text.'),
    '#options' => array(
      '12' => '12',
      '13' => '13',
      '14' => '14',
      '16' => '16',
      '18' => '18',
      '20' => '20',
    ),
    '#default_value' => (isset($settings['font_size']) ? $settings['font_size'] : 16),
  );

  /**
   * Color settings.
   */
  $form['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Color settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  /* Background */
  $form['color']['background'] = array(
    '#type' => 'textfield',
    '#title' => t('Background color'),
    '#prefix' => '<div id="color-picker"></div>',
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['background']) ? $settings['background'] : '#e9e9e9'),
  );
  /* Content background */
  $form['color']['content_background'] = array(
    '#type' => 'textfield',
    '#title' => t('Content background color'),
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['content_background']) ? $settings['content_background'] : '#ffffff'),
  );
  /* Sidebar background */
  $form['color']['sidebar_background'] = array(
    '#type' => 'textfield',
    '#title' => t('Sidebar background color'),
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['sidebar_background']) ? $settings['sidebar_background'] : '#495e6e'),
  );
  $form['color']['sidebar_text_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Sidebar text color'),
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['sidebar_text_color']) ? $settings['sidebar_text_color'] : '#FFFFFF'),
  );
  /* Headings */
  $form['color']['color_heading'] = array(
    '#type' => 'textfield',
    '#title' => t('Heading color'),
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['color_heading']) ? $settings['color_heading'] : '#008891'),
  );
  /* Text */
  $form['color']['color_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Text color'),
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['color_text']) ? $settings['color_text'] : '#000'),
  );
  $form['color']['link_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Link color'),
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['link_color']) ? $settings['link_color'] : '#47c0c3'),
  );
  $form['color']['link_hover_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Link hover color'),
    '#attributes' => array(
      'class' => array('color'),
    ),
    '#size' => 10,
    '#default_value' => (isset($settings['link_hover_color']) ? $settings['link_hover_color'] : '#008891'),
  );

  /* Submit form. */
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
    '#submit' => array('user_settings_form_submit'),
  );
  /* Reset settings. */
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('user_settings_form_reset'),
  );

  return $form;
}
