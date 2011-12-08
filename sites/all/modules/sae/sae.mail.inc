<?php

/**
 * @file
 * Implements the base SAEMailSystem for Drupal class.
 */

/**
 * Base SAEMailSystem for Drupal implementation
 */
class SAEMailSystem implements MailSystemInterface {
  /**
   * Constructor.
   */
  public function __construct() {
    $this->mailer = new SaeMail();

    $this->Host = variable_get('smtp_host', '');
    $this->Port = variable_get('smtp_port', '25');
    $this->SMTPSecure = variable_get('smtp_tls', FALSE) ? TRUE : FALSE;

    // Use SMTP authentication if both username and password are given.
    $this->Username = variable_get('smtp_username', '');
    $this->Password = variable_get('smtp_password', '');
    $this->SMTPAuth = (bool)($this->Username != '' && $this->Password != '');
  }
  
  /**
   * Send mail user SAE mail system
   *
   * @return
   *   If Succeed, return ture, else return false.
   */
  public function send() {
    $ret = $this->mailer->quickSend($this->To , $this->Subject , $this->Body , $this->Username , $this->Password);
    $this->reset();
    return $ret;
  }
  
  /**
   * Clean mail status so we can send mail again
   */
  public function reset() {
    // Clean SAE Mail
    $this->mailer->clean();
  }
  
  /**
   * Concatenates and wraps the e-mail body for plain-text mails.
   *
   * Mails contain HTML by default. When using PHPMailer without MimeMail
   * module, plain-text e-mails are sent, which require the same processing as
   * in the DefaultMailSystem implementation.
   *
   * @see DefaultMailSystem::format()
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return
   *   The formatted $message.
   */
  public function format(array $message) {
    // Join the body array into one string.
    $message['body'] = implode("\n\n", $message['body']);
    // Convert any HTML to plain-text.
    $message['body'] = drupal_html_to_text($message['body']);
    // Wrap the mail body for sending.
    $message['body'] = drupal_wrap_mail($message['body']);
    return $message;
  }

  /**
   * Sends an e-mail message composed by drupal_mail().
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   * @return
   *   TRUE if the mail was successfully accepted, otherwise FALSE.
   *
   * @see PHPMailer::Send()
   */
  public function mail(array $message) {
    $from = sae_mailer_parse_address($message['from']);
    $from = reset($from);
    $this->From = $from['mail'];
    unset($message['headers']['From']);
    
    $this->To = '';
    foreach (sae_mailer_parse_address($message['to']) as $address) {
      $this->To .= $address['mail'] . ',';
    }
    if (isset($message['headers']['Cc'])) {
      $this->Cc = '';
      foreach (sae_mailer_parse_address($message['headers']['Cc']) as $address) {
        $this->Cc .= $address['mail'] . ',';
      }
      unset($message['headers']['Cc']);
    }
    
    if (isset($message['headers']['Content-Type'])) {
      if ($message['headers']['Content-Type'] == 'TEXT' || $message['headers']['Content-Type'] == 'HTML')
        $this->ContentType = $message['headers']['Content-Type'];
      unset($message['headers']['Content-Type']);
    }
    $line_endings = variable_get('mail_line_endings', MAIL_LINE_ENDINGS);
    // Prepare mail commands.
    $this->Subject = $message['subject'];
    // Note: e-mail uses CRLF for line-endings. PHP's API requires LF
    // on Unix and CRLF on Windows. Drupal automatically guesses the
    // line-ending format appropriate for your system. If you need to
    // override this, adjust $conf['mail_line_endings'] in settings.php.
    $this->Body = preg_replace('@\r?\n@', $line_endings, $message['body']);
    
    return $this->send();
  }
}

