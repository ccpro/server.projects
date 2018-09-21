#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use DBI;
use lib qw'/usr/local/www/Modules/';
use DBConfig;

sub main($);
sub read_ips($);

my $cfg = {
   ips   => [],
};

main($cfg);

sub main($)
{
   my ($cfg) = @_;

   $cfg->{dbh} = DBI->connect(@{DBConfig::get_db_params()}) || die 'can not connect to db';
   read_ips($);
   
   if ( scalar(@{$cfg->{ip}}) ) {
   }
}

sub read_ips($)
{
   my ($cfg) = @_;
   my $sql = "SELECT ip, Date(date) 
              FROM fw_ip 
              WHERE Date(date) = current_date AND whiteyn = 0";
   my $sth;

   $sth = $cfg->{dbh}->prepare($sql);
   $sth->execute();
   
   while ( my $s = $sth->fetch() ) {
      push @{$cfg->{ip}}, [$s->[1], $s->[0]];
   }
   $sth->finish();
   
   
}
