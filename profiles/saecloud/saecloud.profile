<?php

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Allows the profile to alter the site configuration form.
 */
function saecloud_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the app name.
  $form['site_information']['site_name']['#default_value'] = $_SERVER['HTTP_APPNAME'];
  $form['smtp_settings'] = array(
    '#type' => 'fieldset', 
    '#title' => st('SMTP settings'), 
    '#collapsible' => FALSE,
    '#weight' => 10,
    '#element_validate' => array('_saecloud_smtp_validate'),
  );
  $form['smtp_settings']['smtp_host'] = array(
    '#type' => 'textfield',
    '#title' => st('Primary SMTP server'),
    '#default_value' => '',
    '#description' => st('The host name or IP address of your primary SMTP server. (Leave blank if use sina,mail,163,265,netease,qq,sohu,yahoo smtp service)', array('!gmail-smtp' => '<code>smtp.gmail.com</code>')),
  );
  $form['smtp_settings']['smtp_port'] = array(
    '#type' => 'textfield',
    '#title' => st('SMTP port'),
    '#size' => 5,
    '#maxlength' => 5,
    '#default_value' => '',
    '#description' => st('The standard SMTP port is 25, for Google Mail use 465. (Leave blank if use sina,mail,163,265,netease,qq,sohu,yahoo smtp service)'),
  );
  $form['smtp_settings']['smtp_tls'] = array(
    '#type' => 'checkbox',
    '#title' => st('Use TLS protocol'),
    '#default_value' => '',
    '#description' => st('Whether to use TLS encrypted connection to communicate with the SMTP server. (Leave blank if use sina,mail,163,265,netease,qq,sohu,yahoo smtp service)'),
  );
  $form['smtp_settings']['smtp_username'] = array(
    '#type' => 'textfield',
    '#title' => st('SMTP username'),
    '#default_value' => '',
    '#required' => TRUE,
    '#description' => st('Always enter your username including mail adress like "example@gmail.com".'),
  );
  $form['smtp_settings']['smtp_password'] = array(
    '#type' => 'password',
    '#title' => st('SMTP password'),
    '#description' => st('Password of smtp server, usually is your email password.'),
    '#required' => TRUE,
  );
}

/**
 * Implements hook_install_tasks().
 */
function saecloud_install_tasks($install_state) {
  //drupal_set_message(st('Please go to %link set smtp server for send mail.', array('%link' => l('SAE mailer config page', 'admin/config/system/saemailer'))));
}

/**
 * Implements hook_install_tasks_alter().
 */
function saecloud_install_tasks_alter(&$tasks, $install_state) {
  $tasks['install_verify_requirements']['function'] = 'saecloud_verify_requirements';
}

/**
 * Modify default verify requuirement
 *
 * There we know SAE can't check directory if writable, so we must bypass these 
 * error when check basic file path.
 */
function saecloud_verify_requirements(&$install_state) {
  $status_report = install_verify_requirements($install_state);
  // If file system error, passby it.
  $error_msg = '<div title="Error"><span class="element-invisible">Error</span></div></td><td class="status-title">' . st('File system') . '</td>';
  if (strstr($status_report, $error_msg)) {
    return;
  }
  return $status_report;
}

/**
 * Validate and Save smtp info in install configure page
 */
function _saecloud_smtp_validate($element, &$form_state) {
  if (!valid_email_address($form_state['values']['smtp_username'])) {
    form_set_error('smtp_settings][smtp_username', t('The e-mail address %mail is not valid.', array('%mail' => $form_state['values']['smtp_username'])));
  }
  else {
    variable_set('smtp_host', $form_state['values']['smtp_host']);
    variable_set('smtp_port', $form_state['values']['smtp_port']);
    variable_set('smtp_tls', $form_state['values']['smtp_tls']);
    variable_set('smtp_username', $form_state['values']['smtp_username']);
    variable_set('smtp_password', $form_state['values']['smtp_password']);
  }
}

