<?php
// $Id: ljsync_parser.cls.php,v 1.4 2010/08/26 13:54:44 noelbush Exp $

require_once(drupal_get_path('module', 'ljsync') .'/ljsync.inc');

define ('LJSYNC_OUTSIDE_DATA', 0);
define ('LJSYNC_INSIDE_ENTRY', 1);
define ('LJSYNC_INSIDE_COMMENT' , 2);

/**
 * A parser for files produced by jbackup.pl.
 */
class LJSyncParser {
  
  protected $uid;
  protected $lj_uname;
  protected $sync;
  protected $max_create;
  protected $stop_parsing = FALSE;

  protected $current_tag;
  protected $current_entry;
  protected $current_comment;
  protected $parser_state;
  protected $inside_data;
  protected $created_count;
  
  /**
   * Import new entries from the given XML,
   * which should be in the format produced by
   * ljexport.
   * 
   */
  public function import_xml($path, $uid, $sync, $max_create, $lj_uname) {
    $this->uid = $uid;
    $this->lj_uname = $lj_uname;
    $this->sync = $sync;
    $this->max_create = $max_create;
    
    $xml_parser = xml_parser_create('UTF-8');
    xml_parser_set_option($xml_parser, XML_OPTION_CASE_FOLDING, FALSE);
    xml_set_element_handler($xml_parser, array(&$this, "start_element"), array(&$this, "end_element"));
    xml_set_character_data_handler($xml_parser, array(&$this, "character_data"));
    
    if (!($fp = fopen($path, 'r'))) {
      drupal_set_message('Could not open file ' . $path . '.', 'error');
      return;
    }
    
    while (!$this->stop_parsing && $data = fread($fp, 4096)) {
      xml_parse($xml_parser, $data, feof($fp));
    }
    xml_parser_free($xml_parser);
    
    if ($this->created_count > 0) {
      drupal_set_message("Created {$this->created_count} new nodes.");
    }
  }
  
  private function start_element($parser, $name, $attrs) {
    $this->current_tag[] = $name;
    switch ($name) {
      case 'entry' :
        $this->current_entry = $attrs;
        $this->parser_state = LJSYNC_INSIDE_ENTRY;
        break;
        
      case 'prop' :
        if ($attrs['name'] == 'taglist') {
          $tags = $attrs['value'];
          $this->current_entry['tags'] = $tags; 
        }
        break;
        
      case 'comment' :
        $this->current_comment = $attrs;
        $this->parser_state = LJSYNC_INSIDE_COMMENT;
        break;
        
      default :
        $this->inside_data = TRUE;
        break;
    }
  }
  
  private function end_element() {
    switch (end($this->current_tag)) {
      case 'entry' :
        $this->process_entry($this->current_entry);
        $this->parser_state = LJSYNC_OUTSIDE_DATA;
        break;
        
      case 'comment' :
        $this->current_entry['comments'][] = $this->current_comment;
        $this->parser_state = LJSYNC_INSIDE_ENTRY;
        break;
        
      default :
        break;
    }
  
    $this->inside_data = FALSE;
    array_pop($this->current_tag);
  }
  
  private function process_entry($spec) {
    static $vid;
    $vid = variable_get('ljsync_tags_vid', LJSYNC_DEFAULT_TAGS_VID);
    $jitemid = $spec['jitemid'];
    
    $node_is_local = false;
    
    // If this was created locally and crossposted by ljxp, make sure it has an entry in our table.
    // (Note that ljxp is required by this module, so we don't have to check that it is enabled.)
    $query = db_query('SELECT nid from {ljxp_node} WHERE ljid = %d', $jitemid);
    // Entries from different users may have the same id, so we must load each node and check the user.
    while ($result = db_fetch_array($query)) {
      $nid = $result['nid'];
      $node = node_load($nid);
      if ($node->uid == $this->uid) {      
        // We found a match, so create a record, marking it as local origin.
        $node_is_local = true;
        db_query('REPLACE INTO {ljsync_node} (nid, jitemid, anum, ditemid, origin) VALUES (%d, %d, %d, %d, %d)',
                 $nid, $jitemid, $spec['anum'], $spec['ditemid'], LJSYNC_ORIGIN_LOCAL);
      }
      else {
        unset($node);
      }
    }
    
    // Since the node is not local, we will either find it in our table, or it is a new node (for us).
    if (!$node_is_local) {
      $query = db_query('SELECT nid FROM {ljsync_node} WHERE jitemid = %d', $jitemid);
      while ($result = db_fetch_array($query)) {
        $nid = $result['nid'];
        $node = node_load($nid);
      }
    }
    
    // We did not find the node, so we need to create it.
    if (!isset($node)) {
      $nid = $this->new_node($spec);
      watchdog('ljsync', "new node: %nid.", array('%nid' => $nid), WATCHDOG_DEBUG);
    }
    
    // Add tags, if any are specified.
    if (isset($spec['tags'])) {
      $terms['tags'][$vid] = $spec['tags'];
      if (!isset($node)) {
        $node = node_load($nid);
      }
      taxonomy_node_save($node, $terms);
    }
    
    // Synchronize comments.
    if ($spec['comments']) {
      foreach ($spec['comments'] as $cspec) {
        $jtalkid = $cspec['jtalkid'];
        $query = db_query('SELECT cid FROM {ljsync_comments} WHERE jtalkid = %d', $jtalkid);
        $result = db_fetch_array($query);
        
        // Create our copy of the comment if it does not already exist.
        if (empty($result)) {
          $cid = $this->new_comment($cspec, $nid);
          watchdog('ljsync', "new comment: %cid.", array('%cid' => $cid), WATCHDOG_DEBUG);
        }
      }
    }
  }
  
  /**
   * Create a new node that contains a copy of a remote post.
   */
  private function new_node($spec) {
    $node = new StdClass();
    $node->type = variable_get('ljsync_node_type', 'blog');
    $node->uid = $this->uid;
    $node->status = 1;
    $node->promote = 1;
    $node->format = variable_get('ljsync_node_format', 1);
    $node->comment = 2;
    $node->title = (!isset($spec['title']) ? '(no subject)' : $spec['title']);
    
    $node->body = $this->clean_body($spec['body']);
    _ljsync_fix_lj_links($node, $this->lj_uname);
    _ljsync_fetch_lj_embeds($node, $this->lj_uname, $spec['ditemid']);
    $node->created = $spec['created'];
    $node->changed = $spec['changed'];
  
    $node->teaser = node_teaser($node->body, 3);
    
    // Make sure pathauto will run on this (if enabled).
    $node->pathauto_usersubmitted = TRUE;
    $node->validated = TRUE;
    
    node_save($node);
    $nid = $node->nid;
    
    db_query("INSERT INTO {ljsync_node} (nid, jitemid, anum, ditemid, origin) VALUES (%d, %d, %d, %d, %d)",
             $nid, $spec['jitemid'], $spec['anum'], $spec['ditemid'], LJSYNC_ORIGIN_REMOTE);
    
    $this->created_count++;
    if ($this->max_create > 0 && $this->created_count == $this->max_create) {
      drupal_set_message("Stopped at specified maximum of {$this->max_create} created nodes.");
      $this->stop_parsing = TRUE;
    }
  
    return $nid;
  }
  
  private function new_comment($cspec, $nid) {
    $poster = $cspec['poster'];
    if ($poster == $this->lj_uname) {
      $poster_id = $this->uid;
    }
    else if ($poster == '') {
      $poster_id = 0;
    }
    else {
      if (strpos($poster, 'ext_') === 0) {
        $non_lj_user = TRUE;
      }
      $authname = $non_lj_user ? $poster : "http://$poster.livejournal.com";
      $user = user_load(array('name' => $poster));
      if ($user == null) {
        $uspec = array(
          'name' => $poster,
          'mail' => variable_get('openid_dummy_email', OPENID_DUMMY_EMAIL),
          'status' => 1,
          'pass' => user_password(),
        );
        $user = user_save('', $uspec);
        db_query("INSERT INTO {authmap} (uid, authname, module) VALUES (%d, '%s', 'openid')", $user->uid, $authname);
        watchdog('ljsync', "Created new account for %poster.", array('%poster' => $poster), WATCHDOG_DEBUG);
        _ljsync_fetch_userpic($user);
      }
      $poster_id = $user->uid;
    }
    
    // Duplicated/adapted from comment.module (too bad it's not a separate function there!)
    if ($cspec['parentid'] == 0) {
      $parent_id = 0;
      $max = db_result(db_query('SELECT MAX(thread) FROM {comments} WHERE nid = %d', $nid));
      $max = rtrim($max, '/');
      $thread = int2vancode(vancode2int($max) + 1) .'/';
    }
    else {
      $parent_id = db_result(db_query('SELECT cid FROM {ljsync_comments} WHERE jtalkid = %d', $cspec['parentid']));
      $parent = _comment_load($parent_id);
      $parent->thread = (string) rtrim((string) $parent->thread, '/');
      $max = db_result(db_query("SELECT MAX(thread) FROM {comments} WHERE thread LIKE '%s.%%' AND nid = %d", $parent->thread, $nid));
      if ($max == '') {
        $thread = $parent->thread .'.'. int2vancode(0) .'/';
      }
      else {
        $max = rtrim($max, '/');
        $parts = explode('.', $max);
        $parent_depth = count(explode('.', $parent->thread));
        $last = $parts[$parent_depth];
        $thread = $parent->thread .'.'. int2vancode(vancode2int($last) + 1) .'/';
      }
    }
    
    $edit = array(
      'pid' => $parent_id,
      'nid' => $nid,
      'uid' => $poster_id,
      'subject' => $cspec['subject'],
      'comment' => $cspec['comment'],
      'hostname' => 'livejournal.com',
      'timestamp' => $cspec['timestamp'],
      'status' => COMMENT_PUBLISHED,
      'format' => variable_get('ljsync_comment_format', 1),
      'thread' => $thread,
      'name' => $cspec['poster'],
    );
    db_query("INSERT INTO {comments} (pid, nid, uid, subject, comment, hostname, timestamp, status, format, thread, name) VALUES (%d, %d, %d, '%s', '%s', '%s', %d, %d, %d, '%s', '%s')",
      $edit['pid'],
      $edit['nid'],
      $edit['uid'],
      $edit['subject'],
      $edit['comment'],
      $edit['hostname'],
      $edit['timestamp'],
      $edit['status'],
      $edit['format'],
      $edit['thread'],
      $edit['name']
    );
    $cid = db_last_insert_id('comments', 'cid');
    db_query('REPLACE INTO {ljsync_comments} (jtalkid, cid) VALUES (%d, %d)', $cspec['jtalkid'], $cid);
    _comment_update_node_statistics($nid);
    comment_invoke_comment($edit, 'insert');
    return $cid;
  }
  
  private function character_data($parser, $data) {
    if ($this->inside_data) {
      switch ($this->parser_state) {
        case LJSYNC_INSIDE_ENTRY :
          switch (end($this->current_tag)) {
            case 'date' :
              $this->current_entry['created'] = strtotime($data);
              break;
              
            case 'subject' :
              $this->current_entry['title'] .= $data;
              break;
              
            case 'event' :
              $this->current_entry['body'] .= $data;
              break;
          }
          break;
          
        case LJSYNC_INSIDE_COMMENT :
          switch (end($this->current_tag)) {
            case 'date' :
              $this->current_comment['timestamp'] = strtotime($data);
              break;
              
            case 'subject' :
              $this->current_comment['subject'] .= $data;
              break;
              
            case 'body' :
              $this->current_comment['comment'] .= $data;
              break;
          }
          break;
          
        break;
      }
    }
  }
  
  function clean_body($content) {
    static $body_fixes = array(
      '/<p class="MsoNormal".*?>/s' => '<p>',
      '/^<br\s*\/?>/' => '',
      '/(<br\s*\/?>|\n)/s' => '</p><p>',
      '/<div.*?>/s' => '',
      '/<\/div>/s' => '',
      '/<span (style=""|lang="RU")>(.+?)<\/span>/s' => '\2',
    );
    $content = '<p>' . trim($content) . '</p>';
    $content = preg_replace('/<lj-cut>/', '<!--break-->', $content, 1);
    foreach($body_fixes as $find => $replace) {
      $content = preg_replace($find, $replace, $content);
    }
    static $config = array('bare'                        => true,
                           'clean'                       => true,
                           'doctype'                     => 'omit',
                           'drop-proprietary-attributes' => true,
                           'enclose-block-text'          => true,
                           'enclose-text'                => true,
                           'logical-emphasis'            => true,
                           'output-html'                 => true,
                           'show-body-only'              => true,
                           'word-2000'                   => true,
                           'wrap'                        => '0',
                           'new-blocklevel-tags'         => 'lj-embed',
                           'new-empty-tags'              => 'lj,param',
                           'new-inline-tags'             => 'lj,object');
    return tidy_repair_string($content, $config, 'utf8');
  }
}

/**
 * Changes links to other LJ posts by given user into links to
 * other imported nodes.
 * 
 * @param $node
 * @param $lj_uname
 * @return number of fixed links
 */
function _ljsync_fix_lj_links(&$node, $lj_uname) {
  preg_match_all('%href="http://(?:www\.livejournal\.com/users/' . $lj_uname . '|' .
                                 $lj_uname . '\.livejournal\.com)/([0-9]+).html(?:#cutid.+?)?"%s', $node->body, $matches, PREG_SET_ORDER);
  $count = count($matches);
  if ($count > 0) {
    foreach ($matches as $match) {
      $href = $match[0];
      $ditemid = $match[1];
      $query = db_query('SELECT nid FROM {ljsync_node} WHERE ditemid = %d', $ditemid);
      $result = db_fetch_array($query);
      $link_nid = $result['nid'];
      $node->body = str_replace($href, 'href="/node/' . $link_nid . '"', $node->body);
    }
  }
  return $count;
}

/**
 * Looks for <lj-embed/> elements and fetches the enclosed HTML
 * generated by LiveJournal's servers (since the XML-RPC interface
 * does not return this).
 * 
 * @param $node
 * @param $lj_uname
 * @param $ditemid
 */
function _ljsync_fetch_lj_embeds(&$node, $lj_uname, $ditemid) {
  preg_match_all('|<lj-embed id="(.+)"/?>|', $node->body, $embeds, PREG_SET_ORDER);
  $count = count($embeds);
  if ($count > 0) {
    $lj_entry_url = "http://$lj_uname.livejournal.com/$ditemid.html";
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $lj_entry_url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $lj_page = curl_exec($ch);
    curl_exec($ch);
    curl_close($ch);
    
    foreach ($embeds as $embed) {
      $embed_text = $embed[0];
      $embed_id = $embed[1];
      
      preg_match_all('|(<iframe.+?src="(.+?moduleid='. $embed_id .'.+?)".+?>)(</iframe>)|',
        $lj_page, $iframes, PREG_SET_ORDER);
      if (count($iframes) > 0) {
        $embed_src = str_replace('&amp;', '&', $iframes[0][2]);
        
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $embed_src);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_REFERER, $lj_entry_url);
        $embed_content = curl_exec($ch);
        curl_exec($ch);
        curl_close($ch);
        
        $node->body = str_replace($embed_text, $iframes[0][1] . $embed_content . $iframes[0][3], $node->body);
      }
    }
  }
}
