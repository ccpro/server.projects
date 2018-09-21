#!/usr/local/bin/perl -w

use strict;
use Data::Dumper;

#use GD;
#use Image::ExifTool qw(:Public);
#use Image::Size 'attr_imgsize';

use File::Copy;

use DBI;
use lib qw'/usr/local/www/Modules/';
use DBConfig;

sub get_list_of_images_with_info($$$);
sub get_image_comment($$$);
sub get_album_name($);
sub make_photo_dir($$$);
sub recursive_mkdir($);
sub copy_image_with_comments($$$$);
sub fix_string($);

my $cfg = {
  path      => '/usr/home/ccpro/www/photos',
  save_path => './photos/',
  ignore    => ['.', '..', '1968', '1985', '1986', '1988', '1990', '1993', '1994', '1996', '1999', '2000', 'vlad', 'live8march'],
  convert   => '/usr/local/bin/convert',
};
      
$cfg->{dbi} = DBI->connect(@{DBConfig::get_db_params()}) || die "Error: Can not connect to db\n";
      

#my $path = '/usr/home/ccpro/www/photos/2001/20010406/images';
#my $path = '/usr/home/ccpro/www/photos/2006/20060906/images';
my $path = '/usr/home/ccpro/www/photos/2001/20010526/images';
#
#2002radio
#
#my $path = '/usr/home/ccpro/www/photos/2007/20070906/images';
# 2001/20010623
#


my ($list_of_images, $album_name) = get_list_of_images_with_info($cfg, 2001, $path);

my $new_photo_dir = make_photo_dir($cfg, $path, $album_name);

   for my $image (sort keys %$list_of_images) {
      copy_image_with_comments($cfg, $list_of_images->{$image}, $path, $new_photo_dir);
   }

   $cfg->{dbi}->disconnect();

sub copy_image_with_comments($$$$)
{
 my ($cfg, $image, $path_from, $path_to) = @_;
 my $format = "$cfg->{convert} \"%s\" -pointsize 20 -background white -fill black -gravity South -splice %s -annotate +0+2 %s \"%s\"";
 my $source = "$path_from/$image->{file_name}";
 my $target = "$path_to/$image->{file_name}";

 my $file_is_missing = !(-f $target);
 my $file_access_time = (stat($source))[8];

   if ($file_is_missing || (-f $target && $file_access_time > (stat($target))[8])) {
      if ($file_is_missing) {
         print "'$target' is missing\n";
      }
      else {
         print "'$source' is newer then '$target'\n";
      }

      if (!length($image->{comment}||'')) {
         print " - going to copy over\n";
         copy $source, $target or print "Error: can not copy '$source' to '$target' : $!\n";
      }
      else {
       my $splice = '0x30';
       
         if (length($image->{comment}) > 60) {
            $splice = '0x45';
         }
         print " - going to copy over with comment\n";
       my $command = sprintf $format, $source, $splice, $cfg->{dbi}->quote($image->{comment}), $target;
         print "'$command'\n";
         `$command`;
      }
   }
}

sub make_photo_dir($$$)
{
 my ($cfg, $current_path, $album_name) = @_;

 my $new_path = $current_path;

   $new_path =~ s/$cfg->{path}\/|\/images//gi;
   $new_path =~ s/(.*)(\d\d\d\d)(\d\d)(\d\d)/$1$2.$3.$4/;
   $new_path = "$cfg->{save_path}$new_path - $album_name";

   unless (-d $new_path) {
      print "going to create new path '$new_path'\n";
      recursive_mkdir($new_path) or print "Directory creation error: $!\n";
   }
   else{
      print "directory '$new_path' is already exists\n";
   }

   $new_path;
}

sub recursive_mkdir($)
{
 my ($path) = @_;
 my @paths = split('/', $path);
 my $new_path = '.';

   for my $p (@paths) {
      next if ($p eq '.');

      $new_path .= "/$p";
      
      print "checking '$new_path'\n";
      unless (-d "$new_path") {
         print "going to create '$new_path'\n";
         mkdir "$new_path";
      }
   }
   
   die "Error: Failed to create new directory '$path'" unless (-d $path);
}

sub get_image_comment($$$)
{
 my ($cfg, $year, $image_name) = @_;

 my $sql = "select comment from photo_comments where year = $year AND num = ";
 my $name;

   ($name = $image_name) =~ s/\.jpg$//i;
   my ($comment) = $cfg->{dbi}->selectrow_array($sql.$cfg->{dbi}->quote($name));

   if (length($comment||'') > 0) {
      $comment =~ s/&copy;/(c)/gsi ;
      $comment =~ s/<s>.*<\/s>//gis;
      $comment =~ s/<br.?>//gis;
      $comment =~ s/&amp;/and/gis;
      $comment =~ s/&quot;/'/gis;
      $comment =~ s/\s+$//;
   }

print "'$comment'\n" if (($comment||'') ne '');
   
   $comment;
}

sub get_list_of_images_with_info($$$)
{
 my ($cfg, $year, $path) = @_;
 
 my %images = ();
 my $album_name;
 my %attrs;

   opendir (DIR, $path) or die "Can not open dir '$path': $!";
   while (my $item = readdir(DIR)) {
      if (-f "$path/$item" && $item =~ /\.jpg$/i) {
         $images{$item}{comment} = get_image_comment($cfg, $year, $item);
         $images{$item}{file_name} = $item;
#         %attrs = attr_imgsize("$path/$item");
#         $images{$item}{height} = $attrs{-height};
#         $images{$item}{width} = $attrs{-width};
      }
   }
   closedir(DIR);

   $album_name = get_album_name($path);

   (\%images, $album_name);
}

sub get_album_name($)
{
 my ($file_name) = @_;

 my $index = "$file_name/../index.shtml";
 my @lines;
 my $title = '';
 my $find = 'var comment';
 
   open FILE, "<$index" or die "Can not open '$index': $!";
   @lines = <FILE>;

   for (@lines) {
      if ($_ =~ /$find/) {
         chomp;
         $title = $_;
         unless ($title =~ s/^\s{0,}$find\s+=\s+["'](.*)\s{0,}\([0-9]{1,4}\/[0-9A-Za-z?]{1,3}\/[0-9A-Za-z?-]{1,5}\)["'];/$1/igs) {
            $cfg->{dbi}->disconnect();
            die "Can not parse '$title' in $file_name";
         }

         $title =~ s/<s>.*<\/s>//gis;
         $title =~ s/<br.?>//gis;
         $title =~ s/\//./gis;
         $title =~ s/&amp;/and/gis;
         $title =~ s/&quot;/'/gis;
         $title =~ s/\s+$//;
      }
   }
   
   $title;
}
