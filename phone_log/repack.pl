#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use DBI;
use lib qw'/usr/local/www/Modules/';
use DBConfig;

=pod
user_id     | integer                     | NOT NULL
date        | timestamp without time zone | NOT NULL
destination | character varying(32)       |
number      | character varying(16)       |
type        | character varying(1)        |
minutes     | integer                     |
airtime     | character varying(20)       |
toll        | character varying(20)       |
total       | character varying(20)       |
=cut

my $str; # = qq^12/23/2013, 2:56 PM Incoming (425) 765-8920 7 --- $0.00 \n^;

my $sql;
my $dbi = DBI->connect(@{DBConfig::get_db_params()}) || die "Error: Can not connect to db\n";

my $regexp = '^(.*[AM|PM])?\s.*?\s(\(\d\d\d\) \d\d\d-\d\d\d\d)\s([0-9]){1,2}';
#my $regexp = '^(.*\sAM|PM)\s.*?(\(\d\d\d\)\s\d\d\d-\d\d\d\d)?(0-9){1,2}.*$';


my $user_id = int($ARGV[0]);

die "user_id is undefined" if ($user_id == 0);


my $date;
my $phone;
my $minutes;

while(<>) {

s/\n|\r//g;

if ($regexp) {
   print "'$1' '$2' '$3'\n";
   
   $date =  $dbh->quote($1);
   $phone = $dbh->quote($2);
   $minutes = int($3);
      
   $sql = "insert into phone_call values($user_id, )";
   print "$sql\n";
   #$dbi->do($sql);   
   
}
}