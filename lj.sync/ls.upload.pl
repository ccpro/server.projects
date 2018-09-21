#!/usr/bin/perl -w

use strict;
use Data::Dumper;
use LJ::Simple;
use DBI;

use Socket;
use Geo::IP;

use lib qw(/usr/local/www/modules);
use DBConfig;

my Geo::IP $gi = Geo::IP->new(GEOIP_STANDARD);

sub check_ip($);
sub download_post($);
sub statistic($);
sub download_images($);

sub timestamp(;$)
{
 my $_time = shift;

   POSIX::strftime("%m-%d-%Y %H:%M:%S", localtime($_time||time()));
}

my $images;
my $dbh = DBI->connect(@{DBConfig->get_db_params()}) || die "Error: Can not connect to db\n";
my $flag = 0;

use constant STAT_ONLY	=> 1;

foreach my $k (@ARGV) {
   $flag |= STAT_ONLY if ( $k eq '-s' );
}

if ( $flag == STAT_ONLY ) {
   print "statistic only\n";
   statistic($dbh);
}
else {
   $images = download_post($dbh);
   download_images($images);
   statistic($dbh);
}

$dbh->disconnect();

sub massage_time($)
{
 my $t = shift;

   if ($t =~/^(.*)-(.*)-(.*) (.*)$/) {
      $t = "$3-$1-$2 $4";
   }

   $t;
}

sub download_post($)
{
   my ($dbh) = @_;
   my @images;
   my $str;
   my $sql;

   my LJ::Simple $lj = LJ::Simple->new ({
      user =>      "ccpro",
      pass =>      "whi1t3r",
      site =>      undef,
   });

   my ($count, @tags) = $lj->GetTags();

   my %events = ();
   (defined $lj->GetEntries(\%events,undef,"sync",time()-(30*24*3600))) ||
      die "$0: Failed to get entries - $LJ::Simple::error\n";

   for my $itemid (sort {$a <=> $b} keys %events) {

    my $count = $dbh->selectrow_array("
       select count(*) from live_journal
       where itemid = $events{$itemid}{__htmlid}");

      if (length($events{$itemid}{prop_current_mood}||0) > 128) {
         $events{$itemid}{prop_current_mood} = 
            substr($events{$itemid}{prop_current_mood}, 0, 124);
            $events{$itemid}{prop_current_mood} .= '...';
      }
      if (length($events{$itemid}{prop_current_music}||0) > 128) {
         $events{$itemid}{prop_current_music} = 
            substr($events{$itemid}{prop_current_music}, 0, 124);
            $events{$itemid}{prop_current_music} .= '...';
      }

      $events{$itemid}{event} = "Subject: $events{$itemid}{subject}\n".
                                   $events{$itemid}{event};


      my ($protect,@prot_opt)=$lj->GetProtect($events{$itemid});
      if (defined $protect) {
         if ($protect eq "public") {
            $events{$itemid}{security} = "public";
         } elsif ($protect eq "friends") {
            $events{$itemid}{security} = "friends only";
         } elsif ($protect eq "groups") {
            $events{$itemid}{security} = join("","only groups - ",
                                         join(", ",@prot_opt));
         } elsif ($protect eq "private") {
            $events{$itemid}{security} = "private";
         }
      }

      $str = $events{$itemid}{event}||'';

      while ( length($str) ) {
         if ( $str =~ /\<img\s?src='(http:\/\/.*?)'/si ) {
            push @images, $1; 
            $str = $';
         } else {
            last;
         }
      }                                            

      $events{$itemid}{prop_taglist} ||= '';

      if ($count) {
         print "Updating : $itemid\n";
         $sql = "UPDATE live_journal
                 SET current_mood  = ".$dbh->quote($events{$itemid}{prop_current_mood}).",
                     current_music = ".$dbh->quote($events{$itemid}{prop_current_music}).",
                     subject       = ".$dbh->quote($events{$itemid}{subject}).",
                     allow_mask    = ".($events{$itemid}{allow_mask}||0).",
                     event         = ".$dbh->quote($events{$itemid}{event}).",
                     eventtime     = ".$dbh->quote(massage_time(timestamp($events{$itemid}{__timet}))).",
                     tags          = ".$dbh->quote($events{$itemid}{prop_taglist}).",
                     logtime       = ".$dbh->quote(massage_time(timestamp($events{$itemid}{__timet}))).",
                     security       = ".($dbh->quote($events{$itemid}{security} || 0))."
                 WHERE itemid = ".$events{$itemid}{__htmlid};
      } else {
         print "Inserting : $itemid\n";
         $sql = "INSERT INTO live_journal (
                    itemid, current_mood, current_music, subject,
                    allow_mask, event, eventtime, tags, logtime, security)
                 VALUES(".$events{$itemid}{__htmlid}.",".$dbh->quote($events{$itemid}{prop_current_mood}).",".
                          $dbh->quote($events{$itemid}{prop_current_music}).",".
                          $dbh->quote($events{$itemid}{subject}).",".
                          ($events{$itemid}{allow_mask}||0).",".
                          $dbh->quote($events{$itemid}{event}).",".
                          $dbh->quote(massage_time(timestamp($events{$itemid}{__timet}))).",".
                          $dbh->quote($events{$itemid}{prop_taglist}).",".
                          $dbh->quote(massage_time(timestamp($events{$itemid}{__timet}))).",".
                          ($dbh->quote($events{$itemid}{security}||0)).")";
      }

print "$sql\n";
      $dbh->do($sql);
   }
   \@images;
}

sub statistic($)
{
   my ($dbh) = @_;

   $dbh->do("delete from lj_counter where ip = '204.89.54.2'");
   $dbh->do("delete from lj_counter where ip LIKE '10.%'");

   my %data;
   my $sql = "SELECT DISTINCT lc.lj_item, date(lc.date), lc.ip, lj.subject, lc.total_hits
        FROM lj_counter lc 
        LEFT JOIN live_journal lj ON lj.itemid = lc.lj_item 
        WHERE lc.date > CURRENT_DATE - 7 and couter_type = 1
        ORDER BY date(lc.date), lc.lj_item, lc.ip";

   my $sth = $dbh->prepare($sql);
   $sth->execute();

   print "EXECUTE\n";

   while (my $z = $sth->fetch()) {
      push @{$data{$z->[1]}{$z->[0]}}, [$z->[3], check_ip($z->[2]), int $z->[4]];
   }
   $sth->finish();

   for my $k (sort keys %data) {
      print "$k\n";
      for my $d (sort keys %{$data{$k}}) {
         print "\t$d :: $data{$k}{$d}->[0][0]\n";
         for my $a (@{$data{$k}{$d}}) {
            print "\t\t".($a->[1]||'').", hits: $a->[2]\n";
         }
      }
   }
}

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

      unless ( -f $save_name ) {
         print "going to wget '$_'\n";
         `/usr/local/bin/wget $_ -O "$save_name"`;
      }
      
      shift @save_path;
      my $old_name = join('_', @save_path);
      
      if(-f $old_name) {
         unlink $old_name;
      }
   }
}

sub check_ip($)
{
 my $ip = shift || return 'none';

 my ($name, $aliases, $addrtype, $length, @addrs, %names);
 my $country;

   if ($ip =~ /^([0-9]*)\.([0-9]*)\.([0-9]*)\.([0-9]*)$/) {
    my $address = pack( "C4", $1, $2, $3, $4 );

      # Do a lookup
      if(exists($names{$address})) {
         # Use the hashed name
         $name = $names{$address};
      } else {
         ($name, $aliases, $addrtype, $length, @addrs) = 
            gethostbyaddr( $address, &AF_INET );
         if (!$name) {
            $name = "$1.$2.$3.$4";
         }
         $names{$address} = $name;
      }
      $country = ($gi->country_name_by_name($name)||'');
      return "$name ($country)";
   }

   $country = ($gi->country_name_by_addr($ip)||'');
   return "$ip ($country)";
}
