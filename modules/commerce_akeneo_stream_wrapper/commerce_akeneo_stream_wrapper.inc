<?php

/**
 * @file
 * Commerce Akeneo Stream Wrapper class file.
 */

/**
 * Stream wrapper to support local files.
 */
class DrupalAkeneoStreamWrapper implements DrupalStreamWrapperInterface {
  /**
   * Stream context resource.
   *
   * @var Resource
   */
  public $context;

  /**
   * Instance URI (stream).
   *
   * A stream is referenced as "scheme://target".
   *
   * @var String
   */
  protected $uri;

  /**
   * The content of the file.
   *
   * @var String
   */
  protected $stream_content = NULL;

  /**
   * The pointer to the next read or write within the content variable.
   *
   * @var Integer
   */
  protected $stream_pointer;

  /**
   * A generic resource handle.
   *
   * @var array
   */
  protected $stream_response = NULL;

  /**
   * Base implementation of setUri().
   */
  function setUri($uri) {
    $this->uri = $uri;
  }

  /**
   * Base implementation of getUri().
   */
  function getUri() {
    return $this->uri;
  }

  /**
   * Base implementation of getMimeType().
   */
  public static function getMimeType($uri, $mapping = NULL) {
    $wrapper      = new self();
    $wrapper->uri = $uri;

    if ($wrapper->getStreamContent() && isset($wrapper->stream_response->mimeType)) {
      return $wrapper->stream_response->mimeType;
    }

    return 'application/octet-stream';
  }

  /**
   * Implements chmod().
   *
   * Returns a TRUE result since this is a read-only stream wrapper.
   */
  function chmod($mode) {
    return TRUE;
  }

  /**
   * Implements realpath().
   */
  function realpath() {
    return $this->getLocalPath();
  }

  /**
   * Support for fopen(), file_get_contents(), file_put_contents() etc.
   *
   * @param $uri
   *   A string containing the URI to the file to open.
   * @param $mode
   *   The file mode ("r", "wb" etc.).
   * @param $options
   *   A bit mask of STREAM_USE_PATH and STREAM_REPORT_ERRORS.
   * @param &$opened_path
   *   A string containing the path actually opened.
   *
   * @return bool
   *   Returns TRUE if file was opened successfully.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-open.php
   */
  public function stream_open($uri, $mode, $options, &$opened_path) {
    $this->uri = $uri;

    $allowed_modes = array('r', 'rb');
    if (!in_array($mode, $allowed_modes)) {
      return FALSE;
    }

    // Attempt to fetch the URL's data using drupal_http_request().
    if (!$this->getStreamContent()) {
      return FALSE;
    }

    // Reset the stream pointer since this is an open.
    $this->stream_pointer = 0;
    return TRUE;
  }

  /**
   * @return mixed
   */
  public function getDetails() {
    if ($this->getStreamContent()) {
      return $this->stream_response;
    }

    return FALSE;
  }

  /**
   * Support for flock().
   *
   * @param $operation
   *     One of the following:
   *     - LOCK_SH to acquire a shared lock (reader).
   *     - LOCK_EX to acquire an exclusive lock (writer).
   *     - LOCK_UN to release a lock (shared or exclusive).
   *     - LOCK_NB if you don't want flock() to block while locking (not
   *     supported on Windows).
   *
   * @return bool
   *   Always returns TRUE at the present time.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-lock.php
   */
  public function stream_lock($operation) {
    return TRUE;
  }

  /**
   * Support for fread(), file_get_contents() etc.
   *
   * @param $count
   *   Maximum number of bytes to be read.
   *
   * @return string|bool
   *   The string that was read, or FALSE in case of an error.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-read.php
   */
  public function stream_read($count) {
    if (is_string($this->stream_content)) {
      $remaining_chars = strlen($this->stream_content) - $this->stream_pointer;
      $number_to_read  = min($count, $remaining_chars);
      if ($remaining_chars > 0) {
        $buffer = substr($this->stream_content, $this->stream_pointer, $number_to_read);
        $this->stream_pointer += $number_to_read;
        return $buffer;
      }
    }
    return FALSE;
  }

  /**
   * Support for fwrite(), file_put_contents() etc.
   *
   * @param $data
   *   The string to be written.
   *
   * @return bool
   *   The number of bytes written (integer).
   *
   * @see http://php.net/manual/en/streamwrapper.stream-write.php
   */
  public function stream_write($data) {
    return FALSE;
  }

  /**
   * Support for feof().
   *
   * @return bool
   *   TRUE if end-of-file has been reached.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-eof.php
   */
  public function stream_eof() {
    return $this->stream_pointer == strlen($this->stream_content);
  }

  /**
   * Support for fseek().
   *
   * @param $offset
   *   The byte offset to got to.
   * @param $whence
   *   SEEK_SET, SEEK_CUR, or SEEK_END.
   *
   * @return bool
   *   TRUE on success.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-seek.php
   */
  public function stream_seek($offset, $whence) {
    if (strlen($this->stream_content) >= $offset) {
      $this->stream_pointer = $offset;
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Support for fflush().
   *
   * @return bool
   *   TRUE if data was successfully stored (or there was no data to store).
   *
   * @see http://php.net/manual/en/streamwrapper.stream-flush.php
   */
  public function stream_flush() {
    return TRUE;
  }

  /**
   * Support for ftell().
   *
   * @return int
   *   The current offset in bytes from the beginning of file.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-tell.php
   */
  public function stream_tell() {
    return $this->stream_pointer;
  }

  /**
   * Support for fstat().
   *
   * @return array|bool
   *   An array with file status, or FALSE in case of an error - see fstat()
   *   for a description of this array.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-stat.php
   */
  public function stream_stat() {
    $stat = array();

    if ($this->getStreamContent() && isset($this->stream_response->mimeType)) {
      $stat['size'] = $this->stream_response->size;
    }

    return !empty($stat) ? $this->getStat($stat) : FALSE;
  }

  /**
   * Support for fclose().
   *
   * @return bool
   *   TRUE if stream was successfully closed.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-close.php
   */
  public function stream_close() {
    $this->stream_pointer  = 0;
    $this->stream_content  = NULL;
    $this->stream_response = NULL;
    return TRUE;
  }

  /**
   * Support for unlink().
   *
   * @param $uri
   *   A string containing the uri to the resource to delete.
   *
   * @return bool
   *   TRUE if resource was successfully deleted.
   *
   * @see http://php.net/manual/en/streamwrapper.unlink.php
   */
  public function unlink($uri) {
    // We return FALSE rather than TRUE so that managed file records can be
    // deleted.
    return TRUE;
  }

  /**
   * Support for rename().
   *
   * @param $from_uri ,
   *                  The uri to the file to rename.
   * @param $to_uri
   *                  The new uri for file.
   *
   * @return bool
   *   TRUE if file was successfully renamed.
   *
   * @see http://php.net/manual/en/streamwrapper.rename.php
   */
  public function rename($from_uri, $to_uri) {
    return FALSE;
  }

  /**
   * Support for mkdir().
   *
   * @param $uri
   *   A string containing the URI to the directory to create.
   * @param $mode
   *   Permission flags - see mkdir().
   * @param $options
   *   A bit mask of STREAM_REPORT_ERRORS and STREAM_MKDIR_RECURSIVE.
   *
   * @return bool
   *   TRUE if directory was successfully created.
   *
   * @see http://php.net/manual/en/streamwrapper.mkdir.php
   */
  public function mkdir($uri, $mode, $options) {
    return FALSE;
  }

  /**
   * Support for rmdir().
   *
   * @param $uri
   *   A string containing the URI to the directory to delete.
   * @param $options
   *   A bit mask of STREAM_REPORT_ERRORS.
   *
   * @return bool
   *   TRUE if directory was successfully removed.
   *
   * @see http://php.net/manual/en/streamwrapper.rmdir.php
   */
  public function rmdir($uri, $options) {
    return FALSE;
  }

  /**
   * Support for stat().
   *
   * @param $uri
   *   A string containing the URI to get information about.
   * @param $flags
   *   A bit mask of STREAM_URL_STAT_LINK and STREAM_URL_STAT_QUIET.
   *
   * @return bool
   *   An array with file status, or FALSE in case of an error - see fstat()
   *   for a description of this array.
   *
   * @see http://php.net/manual/en/streamwrapper.url-stat.php
   */
  public function url_stat($uri, $flags) {
    $this->uri = $uri;

    if ($flags & STREAM_URL_STAT_QUIET) {
      return @$this->stream_stat();
    }
    else {
      return $this->stream_stat();
    }
  }

  /**
   * Support for opendir().
   *
   * @param $uri
   *   A string containing the URI to the directory to open.
   * @param $options
   *   Unknown (parameter is not documented in PHP Manual).
   *
   * @return bool
   *   TRUE on success.
   *
   * @see http://php.net/manual/en/streamwrapper.dir-opendir.php
   */
  public function dir_opendir($uri, $options) {
    return FALSE;
  }

  /**
   * Support for readdir().
   *
   * @return bool
   *   The next filename, or FALSE if there are no more files in the directory.
   * @see http://php.net/manual/en/streamwrapper.dir-readdir.php
   */
  public function dir_readdir() {
    return FALSE;
  }

  /**
   * Support for rewinddir().
   *
   * @return bool
   *   TRUE on success.
   * @see http://php.net/manual/en/streamwrapper.dir-rewinddir.php
   */
  public function dir_rewinddir() {
    return FALSE;
  }

  /**
   * Support for closedir().
   *
   * @return bool
   *   TRUE on success.
   * @see http://php.net/manual/en/streamwrapper.dir-closedir.php
   */
  public function dir_closedir() {
    return FALSE;
  }

  /**
   * Implements abstract public function getDirectoryPath()
   */
  public function getDirectoryPath() {
    return '';
  }

  /**
   * Overrides getExternalUrl().
   *
   * Return the HTML URL of a Twitpic image.
   * TODO: check if the good thing.
   */
  function getExternalUrl() {
    return $this->uri;
  }

  /**
   * Gets the name of the directory from a given path.
   *
   * This method is usually accessed through drupal_dirname(), which wraps
   * around the PHP dirname() function because it does not support stream
   * wrappers.
   *
   * @param $uri
   *   A URI or path.
   *
   * @return string
   *   A string containing the directory name.
   *
   * @see drupal_dirname()
   */
  public function dirname($uri = NULL) {
    list($scheme, $target) = explode('://', $uri, 2);
    $dirname = dirname($target);

    if ($dirname == '.') {
      $dirname = '';
    }

    return $scheme . '://' . $dirname;
  }

  /**
   * Return the local filesystem path.
   *
   * @param string $uri
   *   Optional URI, supplied when doing a move or rename.
   *
   * @return string
   */
  function getLocalPath($uri = NULL) {
    if (!isset($uri)) {
      $uri = $this->uri;
    }

    return $uri;
  }

  /**
   * Helper function to return a full array for stat functions.
   */
  protected function getStat(array $stat = array()) {
    $defaults = array(
      'dev'     => 0,      // device number
      'ino'     => 0,      // inode number
      'mode'    => 0100000 | 0444,     // inode protectio
      'nlink'   => 0,    // number of links
      'uid'     => 0,      // userid of owner
      'gid'     => 0,      // groupid of owner
      'rdev'    => -1,    // device type, if inode device *
      'size'    => 0,     // size in bytes
      'atime'   => 0,    // time of last access (Unix timestamp)
      'mtime'   => 0,    // time of last modification (Unix timestamp)
      'ctime'   => 0,    // time of last inode change (Unix timestamp)
      'blksize' => -1, // blocksize of filesystem IO
      'blocks'  => -1,  // number of blocks allocated
    );

    $return = array();
    foreach (array_keys($defaults) as $index => $key) {
      if (!isset($stat[$key])) {
        $return[$index] = $defaults[$key];
        $return[$key]   = $defaults[$key];
      }
      else {
        $return[$index] = $stat[$key];
        $return[$key]   = $stat[$key];
      }
    }

    return $return;
  }

  /**
   * Fetch the content of the file using drupal_http_request().
   */
  protected function getStreamContent() {
    if (!isset($this->stream_response)) {
      $this->stream_response = NULL;
      $this->stream_content  = NULL;

      if (!$headers = $this->getSecurityHeaders()) {
        watchdog(
          'akeneo',
          t('Missing security parameters for Akeneo Stream Wrapper.'),
          array(),
          WATCHDOG_ERROR,
          url('admin/config/services/akeneo')
        );

        return NULL;
      }

      $options            = array();
      $options['headers'] = $headers;
      $options['timeout'] = variable_get(
        'commerce_akeneo_stream_wrapper_timeout',
        COMMERCE_AKENEO_STREAM_WRAPPER_DEFAULT_TIMEOUT
      );

      // Restore correct webservice url.
      $uri = preg_replace('/^akeneo/i', 'http', $this->uri) . '.json';
      // Todo: remove debug
      $uri = 'http://local.akeneo-pim/api/rest/media/10041266330001/6';

      $request = drupal_http_request($uri, $options);

      if (empty($request->error) && !empty($request->data)) {
        $this->stream_response = json_decode($request->data);
        $this->stream_content  = base64_decode($this->stream_response->content);
        unset($this->stream_response->content);
      }
      else {
        watchdog(
          'akeneo',
          t(
            'An error occurs: "@error" (code: @code) while using Akeneo Stream Wrapper for @uri.',
            array('@error' => $request->error, '@code' => $request->code, '@uri' => $uri)
          ),
          array(),
          WATCHDOG_ERROR
        );
      }
    }

    return $this->stream_content;
  }

  /**
   * @return array|bool
   */
  protected function getSecurityHeaders() {
    $username = variable_get('commerce_akeneo_stream_wrapper_username', NULL);
    $apiKey   = variable_get('commerce_akeneo_stream_wrapper_apikey', NULL);
    $salt     = variable_get('commerce_akeneo_stream_wrapper_salt', NULL);
    //admin
    //'650b33e492b258a7786dc081535941aefb6274be';
    //'8b87ixiszrkso0s0wsosk0004s480gs';

    if (empty($username) || empty($apiKey) || empty($salt)) {
      return FALSE;
    }

    $nonce   = uniqid();
    $created = date('c');
    $digest  = base64_encode(sha1(base64_decode($nonce) . $created . $apiKey . '{' . $salt . '}', TRUE));
    $token   = sprintf(
      'X-WSSE: UsernameToken Username="%s", PasswordDigest="%s", Nonce="%s", Created="%s"',
      $username,
      $digest,
      $nonce,
      $created
    );

    $headers = array(
      'Content-Type'  => 'application/json',
      'Authorization' => 'WSSE profile="UsernameToken"',
      'X-WSSE'        => $token,
    );

    return $headers;
  }
}
