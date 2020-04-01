#!/usr/bin/perl -w

sub compact_lj_counter($$);
sub compact_visitors($);

use strict;
use Data::Dumper;

use DBI;

use lib('/usr/local/www/Modules');
use DBConfig;

my $dbh = DBI->connect(@{DBConfig::get_db_params()}) || die "Error: Can not connect to db\n";

my %counter_types = (
   1	=> 'Live Journal',
   2	=> 'Dream Width',
);

compact_lj_counter($dbh, 1);
compact_lj_counter($dbh, 2);
compact_visitors($dbh);

$dbh->do("vacuum full");

$dbh->disconnect();


sub compact_lj_counter($$)
{
 my ($dbh, $counter_type) = @_;

 my $select_sql = qq^
SELECT lj_item as item, ip, Date(date), sum(total_hits) as count
FROM lj_counter
WHERE counter_type = $counter_type
GROUP BY lj_item, ip, Date(date)
ORDER BY lj_item^;

print $select_sql."\n";
 my $sth = $dbh->prepare($select_sql);

 my @data;

   $sth->execute();
   while (my $s = $sth->fetchrow_hashref()) {
      push @data, {
                  count => int $s->{count},
                  date => $s->{date},
                  ip => $s->{ip},
                  item => int $s->{item}
               };
   }
   $sth->finish();

 my @sql;
 my $sql;
 my $item = -1;

   print "$counter_types{$counter_type}\n";

   foreach my $d (@data) {
      @sql = ();
      push @sql, qq^delete from lj_counter where lj_item = $d->{item} and ip = '$d->{ip}' and date(date) = '$d->{date}' and counter_type = $counter_type^;
      push @sql, qq^insert into lj_counter (lj_item, ip, date, total_hits, counter_type) values($d->{item}, '$d->{ip}', '$d->{date}', $d->{count}, $counter_type)^;

      if ($item != $d->{item}) {
         $item = $d->{item};
         print "ITEM: $item\n";
      }

      local $dbh->{AutoCommit} = 0;
      local $dbh->{RaiseError} = 1;
       
      eval {
         $dbh->do($sql = $_) foreach (@sql);
         $dbh->commit();
      };
      if ($@) {
         die "Error: $@\n $sql";
         $dbh->rollback();
      }
   }
}

sub compact_visitors($)
{
 my ($dbh) = @_;

 my $select_sql = qq^
SELECT vdate, ip, url_path, sum(total_hits) as count 
FROM visitors
WHERE done = 0 AND vdate < CURRENT_DATE -1
GROUP BY vdate, ip, url_path
ORDER BY vdate^;
 my $sth = $dbh->prepare($select_sql);

 my @data;

   $sth->execute();
   while (my $s = $sth->fetchrow_hashref()) {
      push @data, {
                  count => int $s->{count},
                  vdate => $s->{vdate},
                  ip => $s->{ip},
                  url_path => $s->{url_path}
               };
   }
   $sth->finish();

 my @sql;
 my $sql;
 my $vdate = '';

   foreach my $d (@data) {
      @sql = ();
      push @sql, qq^delete from visitors where url_path = '$d->{url_path}' and ip = '$d->{ip}' and vdate = '$d->{vdate}' and done = 0^;
      push @sql, qq^insert into visitors (url_path, ip, vdate, total_hits, done) values('$d->{url_path}', '$d->{ip}', '$d->{vdate}', $d->{count}, 1)^;

      if ($vdate ne $d->{vdate}) {
         $vdate = $d->{vdate};
         print "Date: $vdate\n";
      }

      local $dbh->{AutoCommit} = 0;
      local $dbh->{RaiseError} = 1;
       
      eval {
         $dbh->do($sql = $_) foreach (@sql);
         $dbh->do("update visitors set done = 1 where done = 0 and vdate < CURRENT_DATE - 1");
         $dbh->commit();
      };
      if ($@) {
         die "Error: $@\n $sql";
         $dbh->rollback();
      }
   }
}