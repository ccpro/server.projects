#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use GD;
use GD::Graph;
use GD::Graph::bars;
use GD::Graph::lines;

use Class::Date qw(:errors date localdate gmdate now -DateParse -EnvC);

use DBI;
use lib qw'/usr/local/www/Modules/';
use DBConfig;

# required modules
use Net::IMAP::Simple;
use Email::Simple;
use IO::Socket::SSL;

sub get_gmail_spam();

sub check_years($);
sub check_months($);
sub make_graph($$);
sub get_data($$);
sub get_max($);


my $dbi = DBI->connect(@{DBConfig::get_db_params()}) || do{
   warn "$@::$!";
   return;
};

my @data;
my $years;
my $months;

eval {
   my $spam = get_gmail_spam();
   
   $dbi->begin_work();
   $dbi->do("INSERT INTO spam values(current_date, $spam)");
   $dbi->commit();
   
   print "Added new record with $spam into spam\n";
};
if ($@) {
#   $dbi->disconnect();
   $dbi->rollback();
   warn $@;
}

my $sth = $dbi->prepare("SELECT date, amount FROM spam WHERE extract(YEAR FROM date) >= Extract(YEAR FROM current_date)-1 ORDER BY date");

$sth->execute();
while (my $s = $sth->fetch()) {
   push @{$data[0]}, $s->[0];
   push @{$data[1]}, $s->[1];
}
$sth->finish();

$years = check_years(\@data);
$months = check_months($dbi);

print Dumper($years, $months);

make_graph(\@data, 'full spam stat');

for my $year (keys %$years) {
   make_graph(get_data(\@data, $year), "year $year");
}

my $date;
for my $d (keys %$months) {
   $date = date ("$d-01");

   make_graph(get_data(\@data, $d), 
              sprintf("year %04d month %s", $date->year, $date->month));
}

@data = ();

my $month_sql = "SELECT To_char(date, 'DD') AS day, To_char(date, 'MM') AS month, amount FROM spam WHERE date BETWEEN '%s' AND date('%s')-1 ORDER BY day";
my $sql;
my ($d1, $d2);
my $start_date;
for my $month (1..12) {

   $start_date = date now()->year."-$month-01";
#   $start_date->day = 1;
#   $start_date->month = $month;
#    year => now()->year, month => $month, day => 1);

   $d1 = sprintf("%d-%02d-%02d", $start_date->year, $start_date->month, 1);
   $d2 = sprintf("%d-%02d-%02d", $start_date->year, $start_date->month, $start_date->days_in_month);

   $sql = sprintf($month_sql, $d1, $d2);
   
#   print Dumper($month, $d1, $d2, $sql);

   $sth = $dbi->prepare($sql);

   $sth->execute();
   while (my $s = $sth->fetch()) {
      if ($month == 1) {
         push @{$data[0]}, sprintf("%d-XX-%02d", now()->year, int($s->[0]));
      }
      push @{$data[int($s->[1])]}, int($s->[2]);
   }
   $sth->finish();

#print Dumper(\@data)
}

make_graph(\@data, 
           sprintf("year %04d all months", now()->year));

$dbi->disconnect();

sub check_years($)
{
 my $d = shift;
 my %years = ();
 my $year;

   for (@$d) {
      $year = date ($_->[0])->year;
      unless (exists($years{$year})) {
         $years{$year} = 1;
      }
   }

   \%years;
}

sub check_months($)
{
 my $dbi = shift;
 my %months = ();
 my $sql = qq^
 select distinct to_char(date, 'YYYY') as year,  to_char(date, 'MM') as month
 from spam 
 where date >= date(to_char(current_date, 'YYYY')||'-'|| to_char(current_date, 'MM')||'-1') 
 order by year, month^;

 my $sth = $dbi->prepare($sql);

   $sth->execute();
   while( my $s = $sth->fetch()) {
      $months{"$s->[0]-$s->[1]"} = 1;
   }
   $sth->finish();

   \%months;
}

sub make_graph($$)
{
 my ($data, $title) = @_;
 my $max = 0;

   for my $i (1..scalar(@$data)) {
      for (@{$data->[$i]}) {
         $max = $_ if ($max < $_);
      }
   }
   $max = get_max($max);

print Dumper($title, $max);

 my GD::Graph::lines $chart = GD::Graph::lines->new(1000,550);
   $chart->set(
      title => $title,
      y_label     => 'spam letter',
      x_label     => 'date',
      y_max_value => $max,
      y_min_value => 0,
#x_tick_number => 5,
      x_label_skip => 40,
#cumulate    => 1,
   );
   $chart->set_title_font('arialbd', 24);
   $chart->set_x_axis_font('arial', 5);
   $chart->set_y_axis_font('arial', 5);

 my $gd = $chart->plot($data);

   open(GD, ">$title.jpg") or do {warn "open: $!"; return};
   binmode GD;
   print GD $gd->jpeg;
   close GD;
}

sub get_data($$)
{
 my ($data, $mask) = @_;
 my @new_data;

   for (my $i=0; $i < scalar(@{$data->[0]}); ++$i) {
#print "$data->[0][$i] :: $mask\n";
      if ($data->[0][$i] =~ /^$mask.*$/) {
         push @{$new_data[0]}, $data->[0][$i];
         push @{$new_data[1]}, $data->[1][$i];
      }
   }

   \@new_data;
}

sub get_max($)
{
 my ($m) = @_;
 my $length = 0;

print Dumper $m;

   return 10 if (int($m) == 0);

 my @m = split('', $m);
   $length = scalar(@m);

   for (my $i=1; $i<$length; ++$i) {
      $m /= 10;
   }

   $m = int($m) + 1;
   
   for (my $i=1; $i<$length; ++$i) {
      $m *= 10;
   }

   $m;
}

sub get_gmail_spam()
{
 my $username = 'sprozhogin@gmail.com';
 my $password = 'N1p@ssaran';
 my $mailhost = 'pop.gmail.com';

# Connect
 my $imap = Net::IMAP::Simple->new(
    $mailhost,
    port    => 993,
    use_ssl => 1,
 ) or die "Unable to connect to IMAP: $Net::IMAP::Simple::errstr\n";

# Log in
   if ( !$imap->login( $username, $password )) {
      die "Login failed: " . $imap->errstr;
   }
# Look in the the INBOX
 my $spam = $imap->select('[Gmail]/Spam');

# Disconnect
   $imap->quit;

   $spam;
}
