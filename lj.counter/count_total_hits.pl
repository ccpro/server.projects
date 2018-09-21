#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use DBI;

use lib('/usr/local/www/Modules');
use DBConfig;

my $select_sql = qq^select lj_item, ip, date(date), count(*) from lj_counter where total_hits = 1 and counter_type = 1 group by lj_item, ip, date(date) having count(*) > 1 ORDER BY lj_item^;

my $dbh =  DBI->connect(@{DBConfig::get_db_params()}) || die "Error: Can not connect to db\n";
my $sth = $dbh->prepare($select_sql);

my @data;

$sth->execute();
while (my $s = $sth->fetchrow_hashref()) {
   push @data, {
                  count => int $s->{count},
                  date => $s->{date},
                  ip => $s->{ip},
                  lj_item => int $s->{lj_item}
               };
}
$sth->finish();

my @sql;
my $sql;
my $lj_item = -1;
foreach my $d (@data) {
   @sql = ();
   push @sql, qq^delete from lj_counter where lj_item = $d->{lj_item} and ip = '$d->{ip}' and date(date) = '$d->{date} and counter_type = 1'^;
   push @sql, qq^insert into lj_counter (lj_item, ip, date, total_hits, counter_type) values($d->{lj_item}, '$d->{ip}', '$d->{date}', $d->{count}, 1)^;
#print Dumper(\@sql);    

   if ($lj_item != $d->{lj_item}) {
      $lj_item = $d->{lj_item};
      print "ITEM: $lj_item\n";
   }

   local $dbh->{AutoCommit} = 0;
   local $dbh->{RaiseError} = 1;
       
   eval {
      $dbh->do($sql = $_) foreach (@sql);
      $dbh->commit();
   };
   if ($@) {
      warn "Error: $@\n $sql";
      $dbh->rollback();
   }

}

my %data;
$sql = "SELECT DISTINCT lc.lj_item, date(lc.date), lc.ip, lj.subject, lc.total_hits
        FROM lj_counter lc 
        LEFT JOIN live_journal lj ON lj.itemid = lc.lj_item 
        WHERE lc.date > CURRENT_DATE - 7 And counter_type = 1
        ORDER BY date(lc.date), lc.lj_item, lc.ip";

   $sth = $dbh->prepare($sql);
   $sth->execute();

   print "EXECUTE\n";

   while (my $z = $sth->fetch()) {
      push @{$data{$z->[1]}{$z->[0]}}, [$z->[3], check_ip($z->[2]), int $z->[4]];
   }
   $sth->finish();

   for my $k (sort keys %data) {
      print "$k\n";
      for my $d (sort keys %{$data{$k}}) {
         print "\t$d\n";
         for my $a(@{$data{$k}{$d}}) {
            print "\t\t".($a->[0]||'')." :: ".($a->[1]||'').", hits: $a->[2]\n";
         }
      }
   }
$dbh->disconnect();

sub check_ip($)
{
   shift;
}
