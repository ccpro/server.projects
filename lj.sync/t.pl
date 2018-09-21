#!/usr/bin/perl -w

use strict;
use Data::Dumper;

my $image = 'http://prozhogin.org/lj/2009/10/09/asdfg.jpg';
my $name;
my @path;
my $path;

if ($image =~ /^.*:\/\/(.*)\/(.*?)$/) {
   $path = $1;
   $name = $2;

   @path = split('/', $path);
}

my @p;
for my $p (@path) {
   push @p, $p;
   $path = join('/', @p);
   unless (-d $path) {
      mkdir ($path);
   }
}

push @path, $name;
$name = join('/', @path);

print Dumper(\@path, $name);

shift @path;

my $old_name = join('_', @path);
print Dumper($old_name);

print "'$old_name' exists!\n\n" if (-f $old_name);

print "'$image'   '$name'\n";
