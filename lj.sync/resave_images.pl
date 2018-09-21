#!/usr/bin/perl -w

use strict;
use Data::Dumper;
use DBI;

use lib qw(/usr/local/www/modules);
use DBConfig;

sub download_images($);

my $dbh = DBI->connect(@{DBConfig->get_db_params()}) || die "Error: Can not connect to db\n";
my $sql = "select itemid, event from live_journal";
my $sth = $dbh->prepare($sql);
my $str;
my @images;

$sth->execute();

while (my $s = $sth->fetch()) {

   print "$s->[0]\n";

   $str = $s->[1];
   @images = ();

   while ( length($str) ) {
      if ( $str =~ /\<img\s?src='(http:\/\/.*?)'/si ) {
         push @images, $1;
         $str = $';
      } else {
         last;
      }
   }
   
   download_images(\@images);
}
$sth->finish();
$dbh->disconnect();

sub download_images($)
{
 my ($images) = @_;
 my $fname;
 my $save_name;
 my $save_path;
 my @save_path;
   

   chdir('images');
   for (@$images) {
      next if ( /lji|photos/ );
      
      if (/^.*:\/\/(.*)\/(.*?)$/) {
         $save_name = $2;
         $save_path = $1;
      }
      
      @save_path = split('/', $save_path);
      
      my @p;
      for my $p (@save_path) {
         push @p, $p;
         $save_path = join('/', @p);
         unless (-d $save_path) {
            print "created new dir '$save_path'\n";
            mkdir ($save_path);
         }
      }

      push @save_path, $save_name;
      $save_name = join('/', @save_path);

print "Image found: '$_' -> '$save_name'\n";

      unless ( -f $save_name ) {
         print "going to wget '$_'\n";
         `/usr/local/bin/wget -t 2 $_ -O "$save_name"`;
      }
      
      shift @save_path;
      my $old_name = join('_', @save_path);
      
      if(-f $old_name) {
         unlink $old_name;
      }
   }
}
