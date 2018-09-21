#!/usr/local/bin/perl -w

use strict;
use Data::Dumper;

use Class::Date qw(:errors date localdate gmdate now -DateParse -EnvC);

my $born = date "1971-09-27";
my $now = now;

my $diff = $now - $born;

print "Days passed since $born : ".$diff->day."\n";

