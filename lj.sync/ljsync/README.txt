This module synchronizes LiveJournal entries and comments from a LiveJournal account to a Drupal site. It can be considered complementary to LiveJournal Crossposter. It integrates well with ljxp, and avoids re-copying entries that were crossposted from your site to LiveJournal by ljxp.

When copying comments to your site, this module will automatically create new openid-based user accounts for the LJ users who left the comments. It also copies their current LJ userpic to your site.

A regular cron job runs the included Perl script, which is a slightly modified version of the jbackup.pl script that can be found in various places (and states of functionality) on the web.[*], The ljsync module then parses the XML dump that this script creates, and inserts any new entries or comments that have been added.

You will need to be sure that Perl is available to you, as well as the following modules:

 - Getopt::Long
 - GDBM_File
 - Data::Dumper
 - XMLRPC::Lite
 - XML::Parser
 - Digest::MD5
 - Term::ReadKey
 - DBM_Filter
 - Encode
 
Many or all of these modules may already be installed on your server.

In addition, you will need htmltidy support for PHP installed.

[*] A note about licensing: jbackup.pl was released under the GNU GPL, and can be found at http://code.sixapart.com/svn/livejournal, among other places. I have combined features of a couple of different variants of this script that I have found, and so this version of the script may vary from any other you find elsewhere. I feel justified in doing this given that the script does not appear to be officially maintained anywhere at this time. The theme directory in this module also includes a couple of button images from LiveJournal, which are also released under the GNU GPL.

