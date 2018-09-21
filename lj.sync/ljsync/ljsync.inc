<?php
// $Id: ljsync.inc,v 1.5 2010/08/26 13:54:44 noelbush Exp $

/**
 * @file
 * Utility functions for ljsync.module.
 */

define('LJSYNC_ORIGIN_LOCAL', 0);
define('LJSYNC_ORIGIN_REMOTE', 1);

/**
 * Load saved ljsync settings for a given user.  Default settings
 * will be returned if none exist for the given user.
 * 
 * @param $uid the id of the user whose settings are wanted
 * @return array of settings
 */
function ljsync_load_settings($uid = NULL) {
  if (!$uid) {
    global $user;
    $uid = $user->uid;
  }

  $result = db_query("SELECT * FROM {ljsync} WHERE uid = %d", $uid);
  $settings = db_fetch_object($result);
  if (!$settings) {
    $settings = ljsync_load_defaults($uid);
  }

  return $settings;
}

/**
 * Return default ljsync settings for a given uid.
 * 
 * @param $uid the id of the user whose settings are wanted
 * @return array of settings
 */
function ljsync_load_defaults($uid) {
  $user = user_load(array('uid' => $uid));
  $settings = new stdClass();
  $settings->uid = $uid;
  $settings->lj_uname = $user->name;
  $settings->active = TRUE;
  $settings->tags = TRUE;
  return $settings;
}

/**
 * Save ljsync settings for a user.
 * 
 * @param $settings array with the settings to save (includes uid)
 */
function ljsync_save_settings($settings) {
  if (isset($settings->lj_pass)) {
    db_query("REPLACE INTO {ljsync} (uid, lj_uname, lj_pass, active, tags) " .
             "VALUES (%d, '%s', '%s', %d, '%s')",
      $settings->uid,
      $settings->lj_uname,
      md5($settings->lj_pass),
      $settings->active,
      $settings->tags
    );
  }
  else {
    db_query("REPLACE INTO {ljsync} (uid, lj_uname, active, tags) " .
             "VALUES (%d, '%s', %d, '%s')",
      $settings->uid,
      $settings->lj_uname,
      $settings->active,
      $settings->tags
    );
  }
}

/**
 * Return whether the given user is or is not a livejournal user.
 * 
 * @param $user a Drupal user object
 * @return boolean
 */
function ljsync_is_lj_user($user) {
  return (db_result(db_query("SELECT * FROM {authmap} WHERE uid = %d AND module = 'openid' AND authname = 'http://%s.livejournal.com'", $user->uid, $user->name)) !== FALSE);
}

/**
 * Retrieve a userpic from LiveJournal for the given user
 * and attach it to the user.
 * 
 * @param $user a Drupal user object.
 */
function _ljsync_fetch_userpic($user) {
  $settings = ljsync_load_settings($user->uid);
  $username = $settings->lj_uname;
  if ($username == '' || strpos($username, 'ext_') === 0) {
    return;
  }
  $img_filepath = file_create_path(variable_get('user_picture_path', 'pictures') .'/'. $username);
  if (file_exists($img_filepath)) {
    user_save($user, array('picture' => $img_filepath));
  }
  else {
    $foaf_url = "http://$username.livejournal.com/data/foaf";
    $ch = curl_init($foaf_url);
    curl_setopt($ch, CURLOPT_USERAGENT, variable_get('ljsync_useragent', LJSYNC_DEFAULT_USERAGENT));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    if (microtime(1) * 1000 < $next_lj_query) {
      watchdog('ljsync', "Throttling requests to LJ.", NULL, WATCHDOG_DEBUG);
      usleep(200000);
    }
    $next_lj_query = microtime(1) * 1000 + 201;
    $contents = curl_exec($ch);
    curl_close($ch);
    $matches = array();
    if (preg_match('/<foaf:img rdf:resource="(.+?)"/', $contents, $matches) > 0) {
      $img_url = $matches[1];
      if (isset($img_url) && $img_url != '') {
        $fp = fopen($img_filepath, 'w');
        $ch = curl_init($img_url);
        curl_setopt($ch, CURLOPT_USERAGENT, variable_get('ljsync_useragent', LJSYNC_DEFAULT_USERAGENT));
        curl_setopt($ch, CURLOPT_HEADER, FALSE);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
        curl_setopt($ch, CURLOPT_BINARYTRANSFER, TRUE);
        curl_setopt($ch, CURLOPT_FILE, $fp);
        if (microtime(1) * 1000 < $next_lj_query) {
          watchdog('ljsync', "Throttling requests to LJ.", NULL, WATCHDOG_DEBUG);
          usleep(200000);
        }
        $success = curl_exec($ch);
        $next_lj_query = microtime(1) * 1000 + 201;
        fclose($fp);
        curl_close($ch);
        if ($success) {
          watchdog('ljsync', "Downloaded userpic for %user_link from %img_link (as given at %foaf_link) to %img_path.",
            array('%user_link' => l($username, 'user/'. $user->uid),
                  '%img_link' => l($img_url, $img_url),
                  '%foaf_link' => l($foaf_url, $foaf_url),
                  '%img_path' => $img_filepath),
            WATCHDOG_DEBUG);
          $user->picture = $img_filepath;
          user_save($user);
        }
        else {
          watchdog('ljsync', "Could not download userpic for %user_link from %img_link.",
            array('%user_link' => l($username, 'user/'. $user->uid),
                  '%img_link' => l($img_url, $img_url)),
            WATCHDOG_DEBUG);
        }
      }
      else {
        watchdog('ljsync', "Could not parse userpic URL for %user_link.",
          array('%user_link' => l($username, 'user/'. $user->uid)),
          WATCHDOG_DEBUG);
      }
    }
  }
}

function ljsync_perform_import($path, $uid, $lj_uname, $sync, $max_create) {
  require_once(drupal_get_path('module', 'ljsync') .'/ljsync_parser.cls.php');
  $ljsync = new LJSyncParser();
  $ljsync->import_xml($path, $uid, $sync, $max_create, $lj_uname);
}
