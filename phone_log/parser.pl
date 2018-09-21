#!/usr/bin/perl -w

use strict;
use Data::Dumper;

my $user_id = 0;
my @data;
my @store;

use DBI;
use lib qw'/usr/local/www/Modules/';
use DBConfig;

my $sql;
my $dbi = DBI->connect(@{DBConfig::get_db_params()}) || die "Error: Can not connect to db\n";
my $year;
my $phone;

$user_id = 4; #$ARGV[0] || die 'user_id parameter needed';


while (<>) {
print "$_";

   s/\n|\r//g;
   
   s/, WA/ WA/;
   
   @data = split(',',$_);
   for my $i (@data) {
      $i =~ s/^"(.*)$/$1/;
      $i =~ s/^(.*)"$/$1/;
   }

   if ($data[0] =~ /^(.*?)\/(.*?)\/(.*?)$/) {
#      $year = int($3)+2000;
      $data[0] = sprintf("%d-%02d-%02d", $3, $1, $2);
   }

   $store[0] = $dbi->quote("$data[0] $data[1]");  #date
   $store[1] = $dbi->quote($data[2]);             #destination
   
   $phone = $data[3];
   $phone =~ s/\D//g;
   
   if (length($phone) == 10) {
      $phone =~ s/(\d\d\d)(\d\d\d)(\d\d\d\d)/$1-$2-$3/;
   }
   $store[2] = $phone;				#phone number
   
   
   $store[4] = int($data[4]) ;             	#minutes
   $store[3] = $dbi->quote($data[4]);		#type
   $store[6] = $dbi->quote($data[6]);		#toll
   
   for (my $i=0; $i<8; ++$i) {
      $store[$i] ||= 'NULL';
   }

=pod
ccpro_noip_org=# \d phone_call
Таблица "public.phone_call"
   Колонка   |             Тип             | Модификаторы
-------------+-----------------------------+--------------
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

   $sql = "insert into phone_call values($user_id, ".join(',', @store).")";
   print "$sql\n";

   $dbi->do($sql);
}
$dbi->disconnect();
