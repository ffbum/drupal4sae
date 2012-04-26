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

    $this->host = variable_get('sae_smtp_host', '');
    $this->port = variable_get('sae_smtp_port', '25');
    $this->smtpSecure = variable_get('sae_smtp_tls', FALSE) ? TRUE : FALSE;

    // Use SMTP authentication if both username and password are given.
    $this->userName = variable_get('sae_smtp_username', '');
    $this->password = variable_get('sae_smtp_password', '');
    $this->smtpAuth = (bool) ($this->userName != '' && $this->password != '');
  }

  /**
   * Send mail user SAE mail system
   *
   * @return bool
   *   If Succeed, return ture, else return false.
   */
  public function send() {
    $ret = $this->mailer->quickSend($this->to, $this->subject, $this->body, $this->userName, $this->password);
    $this->reset();
    return $ret;
  }

  /**
   * Clean mail status so we can send mail again
   */
  public function reset() {
    // Clean SAE Mail.
    $this->mailer->clean();
  }

  /**
   * Concatenates and wraps the e-mail body for plain-text mails.
   *
   * Mails contain HTML by default. When using PHPMailer without MimeMail
   * module, plain-text e-mails are sent, which require the same processing as
   * in the DefaultMailSystem implementation.
   *
   * @param string $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return string
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
   * @param array $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return bool
   *   TRUE if the mail was successfully accepted, otherwise FALSE.
   */
  public function mail(array $message) {
    $from = sae_mailer_parse_address($message['from']);
    $from = reset($from);
    $this->from = $from['mail'];
    unset($message['headers']['From']);

    $this->to = '';
    foreach (sae_mailer_parse_address($message['to']) as $address) {
      $this->to .= $address['mail'] . ',';
    }
    if (isset($message['headers']['Cc'])) {
      $this->cc = '';
      foreach (sae_mailer_parse_address($message['headers']['Cc']) as $address) {
        $this->cc .= $address['mail'] . ',';
      }
      unset($message['headers']['Cc']);
    }

    if (isset($message['headers']['Content-Type'])) {
      if ($message['headers']['Content-Type'] == 'TEXT' || $message['headers']['Content-Type'] == 'HTML') {
        $this->contentType = $message['headers']['Content-Type'];
      }
      unset($message['headers']['Content-Type']);
    }
    $line_endings = variable_get('mail_line_endings', MAIL_LINE_ENDINGS);
    // Prepare mail commands.
    $this->subject = $message['subject'];
    // Note: e-mail uses CRLF for line-endings. PHP's API requires LF
    // on Unix and CRLF on Windows. Drupal automatically guesses the
    // line-ending format appropriate for your system. If you need to
    // override this, adjust $conf['mail_line_endings'] in settings.php.
    $this->body = preg_replace('@\r?\n@', $line_endings, $message['body']);

    return $this->send();
  }
}
