#!/usr/bin/perl -w

use strict;
use bytes;
use Data::Dumper;

use Encode::Encoder;

#my $s = '&#1055;&#1077;&#1088;&#1074;&#1086;&#1077; &#1076;&#1077;&#1085;&#1100; &#1088;&#1086;&#1078;&#1076;&#1077;&#1085;&#1080;&#1077; (&#1085;&#1077; &#1089;&#1074;&#1086;&#1105;)';
#my @s = split(/;/, $s);
my $s = $ARGV[0]||'';
my $c;

my %translate_table = (
   '1040'		=> 'А',
   '1072'		=> 'а',
   '1041'		=> 'Б',
   '1073'		=> 'б',
   '1042'		=> 'В',
   '1074'		=> 'в',
   '1043'		=> 'Г',
   '1075'		=> 'г',
   '1044'		=> 'Д',
   '1076'		=> 'д',
   '1045'		=> 'Е',
   '1077'		=> 'е',
   '1105'		=> 'ё',
   '1046'		=> 'Ж',
   '1078'		=> 'ж',
   '1047'		=> 'З',
   '1079'		=> 'з',
   '1048'		=> 'И',
   '1080'		=> 'и',
   '1049'		=> 'Й',
   '1081'		=> 'й',
   '1050'		=> 'К',
   '1082'		=> 'к',
   '1051'		=> 'Л',
   '1083'		=> 'л',
   '1052'		=> 'М',
   '1084'		=> 'м',
   '1053'		=> 'Н',
   '1085'		=> 'н',
   '1054'		=> 'О',
   '1086'		=> 'о',
   '1055'		=> 'П',
   '1087'		=> 'п',
   '1056'		=> 'Р',
   '1088'		=> 'р',
   '1057'		=> 'С',
   '1089'		=> 'с',
   '1058'		=> 'Т',
   '1090'		=> 'т',
   '1059'		=> 'У',
   '1091'		=> 'у',
   '1060'		=> 'Ф',
   '1092'		=> 'ф',
   '1061'		=> 'Х',
   '1093'		=> 'х',
   '1062'		=> 'Ц',
   '1094'		=> 'ц',
   '1063'		=> 'Ч',
   '1095'		=> 'ч',
   '1064'		=> 'Ш',
   '1096'		=> 'ш',
   '1065'		=> 'Щ',
   '1097'		=> 'щ',
   '1066'		=> 'Ъ',
   '1098'		=> 'ъ',
   '1067'		=> 'Ы',
   '1099'		=> 'ы',
   '1068'		=> 'Ь',
   '1100'		=> 'ь',
   '1069'		=> 'Э',
   '1101'		=> 'э',
   '1070'		=> 'Ю',
   '1102'		=> 'ю',
   '1071'		=> 'Я',
   '1103'		=> 'я',
);

my @s = $s =~ /&#(\d+);/gis;
my $ch;

for (@s) {
   $ch = $translate_table{$_} ||'?';
   $s =~ s/&#$_;/$ch/gis;
}

print "$s";
print "\n";
