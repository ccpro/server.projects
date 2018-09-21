#!/usr/local/bin/perl -w

use strict;
use Data::Dumper;

#use GD;
#use Image::ExifTool qw(:Public);
#use Image::Size 'attr_imgsize';

use DBI;
use lib qw'/usr/local/www/Modules/';
use DBConfig;

use File::Copy;

sub get_list_of_images_with_info($$$);
sub get_image_comment($$$);
sub get_album_name($);
sub make_photo_dir($$$);
sub recursive_mkdir($);
sub copy_image_with_comments($$$$);
sub main($);
sub get_photo_dirs($);

my $cfg = {
  path      => '/usr/home/ccpro/www/photos',
  save_path => './photos/',
  ignore    => ['.', '..', '1968', '1985', '1986', '1988', '1990', '1993', '1994', '1996', '1999', '2000', 'vlad', 'live8march'],
  convert   => '/usr/local/bin/convert',
};

$cfg->{dbi} = DBI->connect(@{DBConfig::get_db_params()}) || die "Error: Can not connect to db\n";
main($cfg);
$cfg->{dbi}->disconnect();

##
## entry point
##
sub main($)
{
 my ($cfg) = @_;

 my $photo_dirs = get_photo_dirs($cfg);
 my $list_of_images;
 my $album_name;
 my $new_photo_dir;

   for my $year (sort { $a <=> $b } keys(%$photo_dirs)) {

      print "Going to handle '$year'\n";

      for my $path (@{$photo_dirs->{$year}}) {

         ($list_of_images, $album_name) = get_list_of_images_with_info($cfg, $year, $path);

         $new_photo_dir = make_photo_dir($cfg, $path, $album_name);

         for my $image (sort keys %$list_of_images) {
            copy_image_with_comments($cfg, $list_of_images->{$image}, $path, $new_photo_dir);
         }
      }
   
#      print "$year ".Dumper($photo_dirs->{$year})."\n";
   }
}

sub get_photo_dirs($)
{
 my ($cfg) = @_;
 
 my %dirs;

  opendir (DIR, $cfg->{path}) or die "Can not open dir: $!";
  while (my $item = readdir(DIR)) {
   my @found = grep {$_ eq $item} @{$cfg->{ignore}};

     if ( -d "$cfg->{path}/$item" && scalar(@found) == 0) {
        $dirs{$item} = get_sub_dirs("$cfg->{path}/$item");
     }
  }
  closedir(DIR);
  
  \%dirs;
}

sub get_sub_dirs($)
{
 my ($sub_path) = @_;

 my @sub_dirs = ();

   opendir(SDIR, $sub_path) or die "Can not open sub dir ($sub_path): $!";
   while (my $item = readdir(SDIR)) {

      if (-d "$sub_path/$item/images") {
         push @sub_dirs, "$sub_path/$item/images";
      }
      else {

         if (-d "$sub_path/$item") {

            opendir(SSDIR, "$sub_path/$item") or die "Can not open sub dir ($sub_path/$item): $!";
            while (my $sitem = readdir(SSDIR)) {

               if (-d "$sub_path/$item/images") {
                  push @sub_dirs, "$sub_path/$item/$sitem/images";
               }
            }
            closedir(SSDIR);
         }
      }
   }
   closedir(SDIR);

   \@sub_dirs;
}

sub copy_image_with_comments($$$$)
{
 my ($cfg, $image, $path_from, $path_to) = @_;
 my $file_access_time;
 my $format = "$cfg->{convert} \"%s\" -pointsize 20 -background white -fill black -gravity South -splice %s -annotate +0+2 %s \"%s\"";
 my $source = "$path_from/$image->{file_name}";
 my $target = "$path_to/$image->{file_name}";


   $file_access_time = (stat($source))[8];

   if (!(-f $target) || (-f $target && $file_access_time > (stat($target))[8])) {
      print "'$source' is newer then '$target'";

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
