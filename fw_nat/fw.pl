#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use Socket;
use DBI;
use lib qw'/usr/local/www/Modules/';
use DBConfig;
use Class::Date qw(:errors date -DateParse);
#use Class::Date qw(:errors date localdate gmdate now -DateParse -EnvC);

sub main($);
sub read_ips($);
sub scan_logs($);
sub run_sql($$);
sub update_ip($$);
sub insert_ip($$);
sub rebuild_fw_rules($);
sub resolve_ip($);
sub save_max_date($$);
sub get_max_date($);

use constant WHITE_IP => 1;
use constant ALWAYS_BLACK_IP => 2;

my $cfg = {
   ip	=> {},
   log	=> {
      '/var/log/messages' => 
      [
         '^.*@(.*)\)\s\[WARNING\]\sAuthentication\sfailed.*$',
         '^.*@(.*)\)\s\[ERROR\] Unable to set up secure anonymous FTP$',
      ],
      '/var/log/access_log_%year_%month' => 
      [
         '^(.*)\s-.*GET \/(.*?)\/.*404.*$',
      ],
      '/var/log/error_log_%year_%month.log' => 
      [
         '/^\[(.*)\].*?\[error\] \[client (.*?)\] File does not exist:.*?\.php$/',
         '/^\[(.*)\].*?\[error\] \[client (.*?)\] File does not exist:.*?w00tw00t.*?$/',
      ]
   },
   iface => 'fxp0',
   natd  => '/usr/local/etc/rc.d/fw.sh',
   msg   => [],
};

main($cfg);

sub main($)
{
   my ($cfg) = @_;
   my $new_ips;

   $cfg->{dbh} = DBI->connect(@{DBConfig::get_db_params()}) || die 'can not connect to db';

   $new_ips = scan_logs($cfg);
   read_ips($cfg);

   for my $ip (keys %$new_ips) {
      if ( ($cfg->{ip}{$ip}{white}||0) == WHITE_IP) {
         print "White IP '$ip' " .Dumper($cfg->{ip}{$ip}). "\n";
      }
      if ( exists $cfg->{ip}{$ip} ) {
         if ( !$cfg->{ip}{$ip}{new} ) {
            update_ip($cfg, $ip);
            $cfg->{ip}{$ip}{new} = 1;
            push @{$cfg->{msg}}, "Updated '$ip'";
         }
      }
      else {
         insert_ip($cfg, $ip);
         $cfg->{ip}{$ip}{new} = 1;
         push @{$cfg->{msg}}, "Inserted '$ip'";
      }
   }
   
   rebuild_fw_rules($cfg) if ( scalar(@{$cfg->{msg}}) );
   
   print "$_\n" for ( @{$cfg->{msg}} );
}

sub rebuild_fw_rules($)
{
   my ($cfg) = @_;
   my $time = time();
   my $head = 
qq^#!/bin/sh
   
#/sbin/natd -config /etc/natd.conf
   
/sbin/ipfw -f flush
/sbin/ipfw add divert natd all from any to any via $cfg->{iface}\n\n^;
   my $tail =
qq^

#/sbin/ipfw add deny tcp from 76.121.207.219 21 to any in recv $cfg->{iface}
#/sbin/ipfw add deny tcp from 76.121.207.219 80 to any in recv $cfg->{iface}

#/sbin/ipfw add deny tcp from 24.19.218.149 21 to 24.19.218.149 in recv $cfg->{iface}
#/sbin/ipfw add deny tcp from 24.19.218.149 80 to 24.19.218.149 in recv $cfg->{iface}

/sbin/ipfw add allow gre from any to any keep-state
/sbin/ipfw add pass all from any to any\n^;

   my $rule = "/sbin/ipfw add reject all from %s to any via $cfg->{iface}\n";
   my $deny_port = "/sbin/ipfw add deny tcp from %s %d to any in recv $cfg->{iface}";

   open FH, ">/tmp/natd.sh" or die "can not open file $!\n";
   print FH $head;
   
   for my $ip (%{$cfg->{ip}}) {
      next if ($ip =~ /^10\.*./);
      if ( $cfg->{ip}{$ip}{new} && ($cfg->{ip}{$ip}{white}||0) != WHITE_IP) {
         print FH sprintf $rule, $ip;
      }
   }
   print FH $tail;
   close FH;
   
   `mv $cfg->{natd} $cfg->{natd}.$time.~`;
   `mv /tmp/natd.sh $cfg->{natd}`; 
   `chmod u+x $cfg->{natd}`;
   `$cfg->{natd}`;
}

sub run_sql($$)
{
   my ($db, $sql) = @_;

   $db->do($sql);
}

sub update_ip($$)
{
   my ($cfg, $ip) = @_;
   my $sql = "UPDATE fw_ip SET date = current_date WHERE ip = '$ip'";

   run_sql($cfg->{dbh}, $sql);
}

sub insert_ip($$)
{
   my ($cfg, $ip) = @_;
   my $sql = "INSERT INTO fw_ip (ip, date, whiteyn) VALUES('$ip', current_date, 0)";
   
   run_sql($cfg->{dbh}, $sql);
}

sub scan_logs($)
{
   my ($cfg) = @_;
   my %new_ips;
   my $ip;
   my $fname;
   my ($day, $mon, $year) = (localtime(time))[3..5];

   $mon = sprintf "%02d", $mon + 1;
   $year += 1900;

   for my $log (keys %{$cfg->{log}}) {
      $log =~ s/%year/$year/gi;
      $log =~ s/%month/$mon/gi;
      $log =~ s/%day/$day/gi;
   
      open FH, $log or do {print "Can not open '$log'\n"; next;};
      while (<FH>) {
         chomp;
         for my $str (@{$cfg->{log}{$log}}) {
            if ( /$str/ ) {
               $ip = $1;
               if ( $ip =~ /^\d*\.\d*\.\d*\.\d*$/ ) {
                  $new_ips{$ip} = 1;
               }
               else {
                  $ip = resolve_ip($ip);
                  $new_ips{$ip} = 1;
               }
            }
         }
      }
      close FH;
   }

my $max_date = get_max_date($cfg->{dbh});
my $date;
my $month;
   ($month, $year) = (localtime)[4,5];
   $fname = sprintf "/var/log/error_log_%4d_%02d.log", $year + 1900, $month+1;

   open(FH, '<', $fname) or return \%new_ips;

   while (<FH>) {
      if (/^\[(.*)\].*?\[error\] \[client (.*?)\] Invalid.*$/ ||
          /^\[(.*)\].*?\[error\] \[client (.*?)\] request failed: error reading the headers$/) {

         $date = date $1;
         next if ($max_date > $date);
         
         $max_date = $date;
         
         $ip = $2;
         if ( $ip =~ /^\d*\.\d*\.\d*\.\d*$/ ) {
            $new_ips{$ip} = 1;
         }
         else {
            $ip = resolve_ip($ip);
            $new_ips{$ip} = 1;
         }
      }
   }
   close FH;

   save_max_date($cfg->{dbh}, $max_date);

   \%new_ips;
}

sub save_max_date($$)
{
 my ($dbh, $max_date) = @_;
 my @sql = ('delete from fw_last_scan', 'insert into fw_last_scan values('.$dbh->quote($max_date).')');
 my $sql;

   {
      local $dbh->{AutoCommit} = 0;
      local $dbh->{RaiseError} = 1;
   eval {
   
      foreach $sql (@sql) {
         $dbh->do($sql);
      }
      $dbh->commit();
   };
   if ($@) {
      print "Error: $@\n$sql\n";
      $dbh->rollback();
   }
   }
}

sub get_max_date($)
{
 my ($dbh) = @_;
 my $sql = "select date::timestamp(0) from fw_last_scan";
 my $max_date;
 my $default_date = date "2001-11-12 07:13:12";

 my $sth;

   $sth = $dbh->prepare($sql) || return $default_date;
   $sth->execute();
   if (my $s = $sth->fetch) {
      $max_date = date $s->[0];
   }
   else {
      $max_date = $default_date;
   }
   $sth->finish();

   $max_date;
}

sub read_ips($)
{
   my ($cfg) = @_;
   my $sql = "SELECT ip, CASE WHEN whiteyn = 2 Then true ELSE date > current_date-40 END, whiteyn, port FROM fw_ip";
   my $sth;

      $sth = $cfg->{dbh}->prepare($sql);
      $sth->execute();
      while ( my $s = $sth->fetch() ) {
         $cfg->{ip}{$s->[0]}{new} = int($s->[1]);
         $cfg->{ip}{$s->[0]}{white} = int($s->[2]);
         $cfg->{ip}{$s->[0]}{port} = int($s->[3]);
      }
      $sth->finish();
}

sub resolve_ip($)
{
   my ($addr) = @_;
   my $ipaddr = gethostbyname($addr);
   inet_ntoa($ipaddr);
}
