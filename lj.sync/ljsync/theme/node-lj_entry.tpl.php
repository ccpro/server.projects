<?php

/**
 * @file node.tpl.php
 *
 * Theme implementation to display a node.
 * Modified from core node.tpl.php.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $picture: The author's picture of the node output from
 *   theme_user_picture().
 * - $date: Formatted creation date (use $created to reformat with
 *   format_date()).
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $submitted: themed submission information output from
 *   theme_node_submitted().
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $teaser: Flag for the teaser state.
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 * 
 * Added by ljsync:
 * - $formatted_date: creation date formatted a la livejournal
 * - $title_link: title of post, as link to itself
 * - $tags: list of tags assigned to post (if any)
 * - $lj_link: link to original lj post (if applicable)
 * - $lj_remote: whether the post was originally remote (on LJ)
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
?>
<div id="node-<?php print $node->nid; ?>" class="<?php print $node_classes ?>">
  <table class="blog-entry-header">
    <?php if (!$sticky): ?>
      <tr class="date">
        <td class="label date">Date:</td>
        <td class="value"><?php print $formatted_date ?></td>
      </tr>
    <?php endif; ?>
    <tr class="subject">
      <td class="label">Subject:</td>
      <td class="value"><?php print $title_link ?></td>
    </tr>
    <?php if ($tags): ?>
      <tr class="tags">
        <td class="label">Tags:</td>
        <td class="value"><?php print $tags ?></td>
      </tr>
    <?php endif; ?>
  </table>
  <div class="content">
    <?php print $content ?>
  </div>
  <?php if ($lj_link): ?>
    <div class="lj-link">
    <?php if ($lj_remote): ?>
      (Originally posted at
    <?php else: ?>
      (Cross-posted to
    <?php endif; ?>
    <?php print $lj_link ?>)</div>
  <?php endif; ?>
  <?php print $links ?>
</div> <!-- /#node-<?php print $node->nid ?> -->

