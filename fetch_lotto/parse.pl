#!/usr/bin/perl -w

use Data::Dumper;
use strict;

#<div  class="numberItem" title="MegaMillion Results">
#<div  class="numberItem" title="Lotto Results">

my $str = '';
while (<>) {
   $str .= $_;
}

my $mega_million = '';
my @balls;

if ( $str =~ /\<div.*?title=\"MegaMillion Results\"\>(.*?)\<\/div>/isg) {
   $mega_million = $1;
   @balls = $mega_million =~ />(\d\d)\</isg;
}

print Dumper(\@balls);
