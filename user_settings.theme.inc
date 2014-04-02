<?php

/**
 * @file
 */

/**
 * Create table from exportable objects list form.
 *
 * @param $vars
 *   Array that contains list form.
 * @return string
 *   Rendered output.
 */
function theme_user_settings_property_list_form($vars) {
  $form = $vars['form'];

  $rows = array();
  foreach (element_children($form) as $object_name) {
    if (!empty($form[$object_name]['machine_name'])) {

      // Build table row parts from form values.
      $row   = array();
      $row[] = render($form[$object_name]['name']);
      $row[] = render($form[$object_name]['machine_name']);
      $row[] = render($form[$object_name]['actions']);

      // Add class that shows status of exportable object: enabled or disabled.
      $class = 'enabled';
      if ($form[$object_name]['disabled']['#value'] == TRUE) {
        $class = 'disabled';
      }

      // Build table row from from values.
      $rows[] = array(
        'data' => $row,
        'class' => array($class),
      );
    }
  }

  // If no exportable objects in database we should show some empty text.
  if (empty($rows)) {
    $url = url('admin/structure/user-settings/create');
    $rows = array(array(
      'data' => array(array(
        'data' => t('There are no created objects yet. <a href="@url">Add one</a>', array('@url' => $url)),
        'colspan' => 3
      )),
    ));
  }

  // Render table with exportable objects.
  $header = array(t('Name'), t('Machine name'), t('Actions'));
  $output = theme('table', array('header' => $header, 'rows' => $rows));

  // Return rendered table and rendered rest form parts.
  return $output . drupal_render_children($form);
}
