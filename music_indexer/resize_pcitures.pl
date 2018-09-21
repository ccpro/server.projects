#!/usr/local/bin/perl -w
#

use strict;

use CGI qw(:standard :html3 -no_xhtml);
use GD;
use Image::Size 'attr_imgsize';

use Data::Dumper;

sub main($);
sub resize_images($);
sub scan_path();

if (scalar(@ARGV) == 0) {
   die "parameter missing: use directory as a parameter\n";
}

main($ARGV[0]);


sub main($)
{
 my $initial_path = $_[0];

   eval {
      chdir $initial_path;
      scan_path();
   };
   if ($@) {
      die "Error: $@\nCan not chdir to '$initial_path'";
   }
   
   
}

sub scan_path()
{
 my @image_list;
 my @dir_list;

    opendir(DIR, '.') || die "opendir for '.' failed: $!";
    while (my $f = readdir(DIR)) {
       push @image_list, $f if ( $f =~ /^.*?\.(jpg|jpeg|tiff|tif|gif|png|bmp)$/i );
       push @dir_list, $f if (-d $f && $f !~ /^\..*$/);
    }
    closedir DIR;

    if (scalar(@image_list) > 0) {
       resize_images(\@image_list);
    }
    
    
    for my $d (@dir_list) {
       print "going to '$d'\n";
       chdir($d);
       scan_path();
       chdir('..');
    }
}

sub resize_images($)
{
 my ($list) = @_;

 my $srcImage;
 my %attrs;
 my $coef;
 my $new_name;
 my $size = 3000;
 my $resize;
 my $rename;

   foreach my $fname (@$list) {
      $resize = 0;

      eval {

      %attrs = attr_imgsize($fname);
      
      };
      if ($@) {
         print "Error: '$fname' is incorrect\n";
         next;
      }

      $resize = ($attrs{-width} > $size || $attrs{-height} > $size);
      $rename = 1 if ( $fname =~ /^.*?\.(tiff|tif|gif|png|bmp)$/i );

      next if !$resize && !$rename;

      print "$fname is $attrs{-width} x $attrs{-height}\n";
      
      if ($attrs{-width} > $attrs{-height}) {
         $coef = $attrs{-width} / $size;
      }
      else {
         $coef = $attrs{-height} / $size;
      }

      $attrs{-width}  = int ($attrs{-width} / $coef);
      $attrs{-height} = int ($attrs{-height} / $coef);
      
      $new_name = $fname . ".resized.jpg";

      eval {      
       my $rc;
      
         if ($resize) {
            print "convert -quality 100% -antialias -strip -resize $attrs{-width}x$attrs{-height} \"$fname\" \"$new_name\"\n";
            $rc = `/usr/local/bin/convert -antialias -quality 100% -strip -resize $attrs{-width}x$attrs{-height} "$fname" "$new_name"`;
         }
         elsif ($rename) {
            print "convert -quality 100% -antialias -strip \"$fname\" \"$new_name\"\n";
            $rc = `/usr/local/bin/convert -antialias -quality 100% -strip "$fname" "$new_name"`;
         }

         print "convert result is: $rc\n";

         if (-f $new_name && !-z $new_name) {
            print "REMOVE $fname\n";
            unlink($fname);
         }
         
         if ($fname =~ s/^(.*?)\.(jpg|tiff|tif|gif|png)$/$1\.jpeg/i) {
            print "NEW NAME $fname\n";
         }
         
         print "RENAME $new_name -> $fname\n";
         rename($new_name, $fname);
      };
      if ($@) {
         print "Error $@\nCan not resize '$fname'\n";
      }
   }
}
