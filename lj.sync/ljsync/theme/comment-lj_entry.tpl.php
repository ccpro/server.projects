<?php
// $Id: comment-lj_entry.tpl.php,v 1.1 2009/10/27 12:42:00 noelbush Exp $

/**
 * @file comment.tpl.php
 * Adapted from default theme implementation for comments.
 *
 * Available variables:
 * - $author: Comment author. Can be link or plain text.
 * - $content: Body of the post.
 * - $date: Date and time of posting.
 * - $links: Various operational links.
 * - $new: New comment marker.
 * - $picture: Authors picture.
 * - $signature: Authors signature.
 * - $status: Comment status. Possible values are:
 *   comment-unpublished, comment-published or comment-preview.
 * - $submitted: By line with date and time.
 * - $title: Linked title.
 *
 * These two variables are provided for context.
 * - $comment: Full comment object.
 * - $node: Node object the comments are attached to.
 * 
 * Added by ljsync:
 * - $subject: subject of comment (not linked)
 * - $poster: user object of comment poster
 * - $is_new: whether the comment is new
 * - $is_lj_user: whether the poster is an lj user
 * - $lj_profile_url: url to livejournal user's profile page
 * - $lj_user_page_url: url to livejournal user's page
 * - $lj_userinfo_icon: userinfo icon for livejournal users (themed image)
 * - $date: reformatted as simple date (not a link)
 * - $link: link to post with text "link"
 * - $delete_link: may or may not be set -- a link to delete the comment
 *
 * @see template_preprocess_comment()
 * @see ljsync_preprocess_comment()
 * @see theme_comment()
 */
?>
<div class="comment<?php print (($comment->new) ? ' comment-new' : '') .' '. $status .' '. $zebra ?> clear-block">
  <div class="header">
    
    <?php print $picture ?>
    
    <div class="postinfo">
      <div class="subject">
        <?php print $subject ?>
        <?php if ($is_new): ?>
          <span class="new"><?php print $new ?></span>
        <?php endif; ?>
      </div><!-- /.postinfo -->
      
      <div class="author">
        <?php if ($is_lj_user): ?>
          <span class="ljuser">
            <a href="<?php print $lj_profile_url ?>" title="<?php print t('LiveJournal user profile') ?>">
              <?php print $lj_userinfo_icon ?>
            </a>
            <a href="<?php print $lj_user_page_url ?>" title="<?php print t($poster->name .'\'s LiveJournal') ?>">
              <?php print $poster->name ?>
            </a>
          </span>
        <?php else: ?>
          <?php print theme('username', $poster) ?>
        <?php endif; ?>
      </div><!-- /.author -->
      
      <div class="coordinates-and-controls">
        <span class="coordinates">
          <?php print $date ?>
          (<?php print $link ?>)
          <?php if ($delete_link): ?>
            <span class="controls">
              <?php print $delete_link ?>
            </span>
          <?php endif; ?>
        </span>
      </div><!-- /.coordinates-and-controls -->
      
    </div><!-- /.postinfo -->
  </div><!-- /.header -->
  <div class="body">
    <?php print $content ?>
  </div><!-- /.body -->
  <div class="links">
    <?php print $links ?>
  </div>
</div><!-- /.comment -->

<?php $even = !$even; ?>