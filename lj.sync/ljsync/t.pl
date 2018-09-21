#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use XML::Parser;
use XML::Parser::EasyTree;

   $XML::Parser::Easytree::Noempty=1;
 my $p=new XML::Parser(Style=>'EasyTree');
 my $tree=$p->parsefile('comments.xml');
 
print Dumper($tree);