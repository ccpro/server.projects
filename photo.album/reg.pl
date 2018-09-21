#!/usr/bin/perl -w

use strict;

my $find = 'var comment';
my $title1 = "var comment = \"After &amp; Radio <s>asdasd</s> recording <br>Session<br/> (2002/Oct/12)\";";

unless ($title1 =~ s/^\s{0,}$find\s+=\s+"(.*)\s+\([0-9]{1,4}\/[0-9A-Za-z?]{1,3}\/[0-9?]{1,2}\)";/$1/igs)
#unless ($title1 =~ s/^\s+$find\s+=\s+"(.*)\s+\([0-9]{1,4}\/[0-9]{1,3}\/[0-9]{1,2}\)";/$1/igs)
{
 print "can not parse\n";
}

$title1 =~ s/<s>.*<\/s>//gis;
$title1 =~ s/<br.?>//gis;
$title1 =~ s/&amp;/and/gis;

print "'$title1'\n";

