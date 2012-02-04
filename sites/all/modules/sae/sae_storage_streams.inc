<?php

/**
 * @file
 * Wrapper class for use SAE Storage System.
 */

class SaeStorageStreamWrapper implements DrupalStreamWrapperInterface {
  private $writen = TRUE;
  public $context;
  public $handle = NULL;
  protected $uri;
  private $i = 0;

  /**
   * Constructor.
   */
  public function __construct() {
    $this->stor = new SaeStorage();
  }

  /**
   * Get or create SAE sotrage object
   *
   * @return object
   *   The sotrage object.
   */
  public function stor() {
    if (!isset($this->stor)) {
      $this->stor = new SaeStorage();
    }
  }

  /**
   * Set file uri
   *
   * @param string $uri
   *   Uri of file that current been operated.
   */
  public function setUri($uri) {
    $this->uri = $uri;
  }

  /**
   * Get current file uri
   *
   * @return string
   *   Current file uri
   */
  public function getUri() {
    return $this->uri;
  }

  /**
   * Get external url of current file
   *
   * @return string
   *   The external url of current file
   */
  public function getExternalUrl() {
    self::stor();
    $path = $this->getLocalPath();
    $pathinfo = parse_url($path);
    $this->domain = $pathinfo['host'];
    $this->file = ltrim(strstr($path, $pathinfo['path']), '/\\');
    return $this->stor->getUrl($this->domain, $this->file);
  }

  /**
   * Set file mimetype
   *
   * @param string $uri
   *   Uri of file
   * @param Mimetype $mapping
   *   Mimetype mapping
   *
   * @return string
   *   Mime type
   */
  public static function getMimeType($uri, $mapping = NULL) {
    if (!isset($mapping)) {
      // The default file map, defined in file.mimetypes.inc is quite big.
      // We only load it when necessary.
      include_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';
      $mapping = file_mimetype_mapping();
    }

    $extension = '';
    $file_parts = explode('.', basename(self::getTarget($uri)));

    // Remove the first part: a full filename should not match an extension.
    array_shift($file_parts);
    // Iterate over the file parts, trying to find a match.
    // For my.awesome.image.jpeg, we try:
    // - jpeg
    // - image.jpeg, and
    // - awesome.image.jpeg
    while ($additional_part = array_pop($file_parts)) {
      $extension = drupal_strtolower($additional_part . ($extension ? '.' . $extension : ''));
      if (isset($mapping['extensions'][$extension])) {
        return $mapping['mimetypes'][$mapping['extensions'][$extension]];
      }
    }
    return 'application/octet-stream';
  }

  /**
   * Implements of chmod
   *
   * @return bool
   *   Always return true
   */
  public function chmod($mode) {
    // Always return true bease no need chmod for SAE Storage.
    return TRUE;
  }

  /**
   * Get local realpath for current file
   *
   * @return string
   *   The file's local storage path with out schema.
   */
  public function realpath() {
    $path = $this->getDirectoryPath() . '/' . $this->getTarget($this->uri);
    return $path;
  }

  /**
   * Implements of dirname
   *
   * @param string $uri
   *   Uri of file
   *
   * @return string
   *   Directory name with schema of file
   */
  public function dirname($uri = NULL) {
    list($scheme, $target) = explode('://', $uri, 2);
    $target  = $this->getTarget($uri);
    $dirname = dirname($target);

    if ($dirname == '.') {
      $dirname = '';
    }

    return $scheme . '://' . $dirname;
  }

  /**
   * Get base path of file storage
   *
   * @return string
   *   The file's local storage path with out schema.
   */
  public function getDirectoryPath() {
    return variable_get('file_public_path', conf_path() . '/files');
  }

  /**
   * Get local real path with schema
   *
   * @param string $uri
   *   Uri of file
   *
   * @return string
   *   Local real path with schema
   */
  public function getLocalPath($uri = NULL) {
    if (!isset($uri)) {
      $uri = $this->uri;
    }

    $path = $this->getDirectoryPath() . '/' . $this->getTarget($uri);

    list($scheme, $target) = explode('://', $uri, 2);
    return $scheme . '://' . $path;
  }

  /**
   * Get relative path of base path
   *
   * @param string $uri
   *   Uri of file
   *
   * @return string
   *   Relative path of base path without schema
   */
  public function getTarget($uri = NULL) {
    if (!isset($uri)) {
      $uri = $this->uri;
    }

    list($scheme, $target) = explode('://', $uri, 2);

    // Remove erroneous leading or trailing, forward-slashes and backslashes.
    return trim($target, '\/');
  }

  /**
   * Implements of stream_open
   */
  public function stream_open($uri, $mode, $options, &$opened_path) {
    $this->uri = $uri;
    $path = $this->getLocalPath();
    $pathinfo = parse_url($path);
    $this->domain = $pathinfo['host'];
    $this->file = ltrim(strstr($path, $pathinfo['path']), '/\\');
    $this->position = 0;
    $this->mode = $mode;
    $this->options = $options;

    if (in_array($this->mode, array('r', 'r+', 'rb'))) {
      if ($this->fcontent = $this->stor->read($this->domain, $this->file)) {
      }
      else {
        trigger_error(t("fopen({%path}): failed to read from Storage: No such domain or file.", array('%path' => $path)), E_USER_WARNING);
        return FALSE;
      }
    }
    elseif (in_array($this->mode, array('a', 'a+', 'ab'))) {
      trigger_error(t("fopen({%path}): Sorry, saestor does not support appending", array('%path' => $path)), E_USER_WARNING);
      if ($this->fcontent = $this->stor->read($this->domain, $this->file)) {
      }
      else {
        trigger_error(t("fopen({%path}): failed to read from Storage: No such domain or file.", array('%path' => $path)), E_USER_WARNING);
        return FALSE;
      }
    }
    elseif (in_array($this->mode, array('x', 'x+', 'xb'))) {
      if (!$this->stor->getAttr($this->domain, $this->file)) {
        $this->fcontent = '';
      }
      else {
        trigger_error(t("fopen({%path}): failed to create at Storage: File exists.", array('%path' => $path)), E_USER_WARNING);
        return FALSE;
      }
    }
    elseif (in_array($this->mode, array('w', 'w+', 'wb'))) {
      $this->fcontent = '';
    }
    else {
      $this->fcontent = $this->stor->read($this->domain, $this->file);
    }

    return TRUE;
  }

  /**
   * Implements of stream_read
   */
  public function stream_read($count) {
    if (in_array($this->mode, array('w', 'x', 'a', 'wb', 'xb', 'ab'))) {
      return FALSE;
    }

    $ret = substr($this->fcontent, $this->position, $count);
    $this->position += strlen($ret);

    return $ret;
  }

  /**
   * Implements of stream_write
   */
  public function stream_write($data) {
    if (in_array($this->mode, array('r', 'rb'))) {
      return FALSE;
    }

    $left = substr($this->fcontent, 0, $this->position);
    $right = substr($this->fcontent, $this->position + strlen($data));
    $this->fcontent = $left . $data . $right;

    $this->position += strlen($data);
    if (strlen($data) > 0) {
      $this->writen = FALSE;
    }

    return strlen($data);
  }

  /**
   * Implements of stream_close
   */
  public function stream_close() {
    if (!$this->writen) {
      $this->stor->write($this->domain, $this->file, $this->fcontent);
      $this->writen = TRUE;
    }
  }

  /**
   * Implements of stream_eof
   */
  public function stream_eof() {
    return $this->position >= strlen($this->fcontent);
  }

  /**
   * Implements of stream_tell
   */
  public function stream_tell() {
    return $this->position;
  }

  /**
   * Implements of stream_seek
   */
  public function stream_seek($offset , $whence = SEEK_SET) {
    switch ($whence) {
      case SEEK_SET:
        if ($offset < strlen($this->fcontent) && $offset >= 0) {
          $this->position = $offset;
          return TRUE;
        }
        else {
          return FALSE;
        }
        break;
      case SEEK_CUR:
        if ($offset >= 0) {
          $this->position += $offset;
          return TRUE;
        }
        else {
          return FALSE;
        }
        break;
      case SEEK_END:
        if (strlen($this->fcontent) + $offset >= 0) {
          $this->position = strlen($this->fcontent) + $offset;
          return TRUE;
        }
        else {
          return FALSE;
        }
        break;
      default:
        return FALSE;
    }
  }

  /**
   * Implements of unlink
   */
  public function unlink($uri) {
    $this->uri = $uri;
    $path = $this->getLocalPath();
    self::stor();
    $pathinfo = parse_url($path);
    $this->domain = $pathinfo['host'];
    $this->file = ltrim(strstr($path, $pathinfo['path']), '/\\');

    clearstatcache(TRUE);
    return $this->stor->delete($this->domain, $this->file);
  }

  /**
   * Implements of stream_flush
   */
  public function stream_flush() {
    if (!$this->writen) {
      $this->stor->write($this->domain, $this->file, $this->fcontent);
      $this->writen = TRUE;
    }
    return $this->writen;
  }

  /**
   * Implements of stream_stat
   */
  public function stream_stat() {
    return array();
  }

  /**
   * Implements of url_stat
   */
  public function url_stat($uri, $flags) {
    $this->uri = $uri;
    $path = $this->getLocalPath();
    self::stor();
    $pathinfo = parse_url($path);
    $this->domain = $pathinfo['host'];
    $this->file = ltrim(strstr($path, $pathinfo['path']), '/\\');

    if ($attr = $this->stor->getAttr($this->domain, $this->file)) {
      $stat = array();
      $stat['dev'] = $stat[0] = 0;
      $stat['ino'] = $stat[1] = 0;
      // 0100000 + 0777;
      $stat['mode'] = $stat[2] = 33279;
      $stat['nlink'] = $stat[3] = 0;
      $stat['uid'] = $stat[4] = 0;
      $stat['gid'] = $stat[5] = 0;
      $stat['rdev'] = $stat[6] = 0;
      $stat['size'] = $stat[7] = $attr['length'];
      $stat['atime'] = $stat[8] = 0;
      $stat['mtime'] = $stat[9] = $attr['datetime'];
      $stat['ctime'] = $stat[10] = $attr['datetime'];
      $stat['blksize'] = $stat[11] = 0;
      $stat['blocks'] = $stat[12] = 0;
      return $stat;
    }
    elseif (pathinfo($pathinfo['path'], PATHINFO_EXTENSION)) {
      // File not exist.
      return FALSE;
    }
    else {
      // Otherwise it is a directory, but we no need check if exist.
      $stat = array();
      $stat['dev'] = $stat[0] = 0;
      $stat['ino'] = $stat[1] = 0;
      // 040000 + 0777;for directories.
      $stat['mode'] = $stat[2] = 16895;
      $stat['nlink'] = $stat[3] = 0;
      $stat['uid'] = $stat[4] = 0;
      $stat['gid'] = $stat[5] = 0;
      $stat['rdev'] = $stat[6] = 0;
      $stat['size'] = $stat[7] = 0;
      $stat['atime'] = $stat[8] = 0;
      $stat['mtime'] = $stat[9] = time();
      $stat['ctime'] = $stat[10] = time();
      $stat['blksize'] = $stat[11] = 0;
      $stat['blocks'] = $stat[12] = 0;
      return $stat;
    }
  }

  /**
   * Implements of closedir
   */
  public function dir_closedir() {
    unset($this->total_index[$this->uri]);
    array_pop($this->search_uris);
    $this->uri = end($this->search_uris);
    return TRUE;
  }

  /**
   * Implements of opendir
   */
  public function dir_opendir($uri, $options) {
    $this->uri = $uri;
    $this->search_uris[$uri] = $uri;
    $this->dir_index[$this->uri] = 0;
    $this->file_index[$this->uri] = 0;
    $this->options = $options;
    return TRUE;
  }

  /**
   * Implements of readdir
   */
  public function dir_readdir() {
    self::stor();
    $path = $this->getLocalPath();
    $pathinfo = parse_url($path);
    $this->domain = $pathinfo['host'];
    $this->file = ltrim(strstr($path, $pathinfo['path']), '/\\');

    static $lists = array();

    if (!isset($lists[$this->uri])) {
      $lists[$this->uri] = $this->stor->getListByPath($this->domain, $this->file, 1000);
    }

    $dir_index = $this->dir_index[$this->uri];
    $file_index = $this->file_index[$this->uri];
    if (isset($lists[$this->uri]['dirs'][$dir_index])) {
      $dirname = $lists[$this->uri]['dirs'][$dir_index]['name'];
      $this->dir_index[$this->uri]++;
      return $dirname;
    }
    elseif (isset($lists[$this->uri]['files'][$file_index])) {
      $filename = $lists[$this->uri]['files'][$file_index]['Name'];
      $this->file_index[$this->uri]++;
      return $filename;
    }
    return FALSE;
  }

  /**
   * Implements of rewinddir
   */
  public function dir_rewinddir() {
    if (isset($this->search_offset[$this->uri])) {
      $this->search_offset[$this->uri] = 0;
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Implements of mkdir
   */
  public function mkdir($uri, $mode, $options) {
    // Always return true because we do not need create directories manually.
    return TRUE;
  }

  /**
   * Implements of rename
   */
  public function rename($path_from, $path_to) {
    return FALSE;
  }

  /**
   * Implements of rmdir
   */
  public function rmdir($uri, $options) {
    return FALSE;
  }

  /**
   * Implements of stream_cast
   */
  public function stream_cast($cast_as) {
    return FALSE;
  }

  /**
   * Implements of stream_lock
   */
  public function stream_lock($operation) {
    return FALSE;
  }

  /**
   * Implements of stream_set_option
   */
  public function stream_set_option($option, $arg1, $arg2) {
    return FALSE;
  }
}
