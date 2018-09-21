#!/bin/perl

use strict;
use Socket;

my @str = (
  'Nov 25 20:12:54 ccpro pure-ftpd: (?@124.217.216.65) [WARNING] Authentication failed for user [Administrator]',
  'Nov 29 11:52:40 ccpro pure-ftpd: (?@ip-87-204-120-254.netia.com.pl) [WARNING] Authentication failed for user [Administrator]',
  'Jun 19 07:46:32 ccpro pure-ftpd: (?@70.89.54.126) [ERROR] Unable to set up secure anonymous FTP');
my $reg = '^.*@(.*)\)\s(\[WARNING\]|\[ERROR\]) Authentication failed.*$';
my $addr;

for my $s (@str) {
   if ( $s =~ /$reg/ ) {
      $addr = $1;
      if ( $addr =~ /^\d*\.\d*\.\d*\.\d*/ ) {
         print "IP : $addr\n";
      }
      else {
         print "$addr :: ".resolve_ip($addr)."\n";
      }
   }
}


sub resolve_ip($)
{
   my ($addr) = @_;
   my $ipaddr = gethostbyname($addr);
   inet_ntoa($ipaddr);
}
