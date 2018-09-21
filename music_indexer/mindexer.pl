#!/usr/bin/perl -w

use strict;
use Data::Dumper;

sub readDir($);
sub parseIndex($);

sub buildImageList($)
{
   my ($path) = @_;
   my @image_list;

   opendir(DIR, $path) || die "can't opendir $path: $!";
   while (my $f = readdir(DIR)) {
      push @image_list, "$path/$f" if ( $f =~ /^.*?\.(jpg|jpeg|tiff|tif|gif|png)$/i );
   }
   closedir DIR;

   \@image_list;
} 

sub splitPathAndName($)
{
   my ($full_path) = @_;

   if ( $full_path =~ /^(.*)\/(.*)$/ ) {
      return [$1, $2];
   }
}

sub readDir($)
{
   my ($path) = @_;
   my @dir_list;
   my $image_list;
   my $index_time = 0;
   my $index_xml_time = 0;
   my $rebuild_images;
   my $image_time;
   my $tmp;
   my $thmb_name;
   my $thmb_time;

#print "Going to open ".Dumper($path);

   opendir(DIR, $path) or die "can not open dir '$path'";
   while (my $f = readdir(DIR)) {

#print "readdir '$path' :: '$f'\n";
      chomp $f;

      if ( -d "$path/$f" && substr($f, 0, 1) ne '.' ) {
         push @dir_list, $f;
      }
      elsif ( $f eq '.index' ) {
#print "index time '$f'\n";
         $index_time = (stat("$path/$f"))[9];
#print localtime($index_time)."\n";
      }
      elsif ( $f eq '.index.xml' ) {
#print "index.xml time '$f'\n";
         $index_xml_time = (stat("$path/$f"))[9];
#print localtime($index_xml_time)."\n";
      }
   }
   closedir(DIR);

   $image_list = buildImageList($path);

   $rebuild_images = 0;
   for my $image (@$image_list) {
      $image_time = (stat($image))[9];
      $tmp = splitPathAndName($image);
      $thmb_name = "$tmp->[0]/.thumbnails/$tmp->[1]";

#print "'$thmb_name'\n";

      if ( -f $thmb_name ) {
         $thmb_time = (stat($thmb_name))[9] || 0;

#print "$image_time :: $thmb_time\n";
         if ( $image_time > $thmb_time ) {
            $rebuild_images = 1;
            last;
         }
      }
      else {
         $rebuild_images = 1;
         last;
      }
   }

   if ( !$index_time && $index_xml_time ) {
      print "Missing .index in $path\n";
   }

   $rebuild_images = 1;
   if ( ($index_time > 0 && $index_time > $index_xml_time) || $rebuild_images ) {
      print "$path\n";
      buildXMLIndex($path, parseIndex("$path/.index"), $image_list);
   }

   readDir("$path/$_") foreach (@dir_list);
}

sub makeThumbnail($)
{
   my ($fname) = @_;
   my $new_fname;
   my $rc;

=pod
   img = Magick::Image::read(fname).first

   scale = 0.
   if img.columns > img.rows
      scale = img.columns/250.
   else
      scale = img.rows/250.
   end
=cut
   $new_fname = splitPathAndName($fname);

   mkdir "$new_fname->[0]/.thumbnails" unless ( -d "$new_fname->[0]/.thumbnails");
   $rc = `/usr/local/bin/convert -antialias -strip -thumbnail 300 "$fname" "$new_fname->[0]/.thumbnails/$new_fname->[1]"`;

   print "Error!: $rc\n" if ( $rc ne '' );

   $new_fname->[1];
}

sub buildXMLIndex($$$)
{
   my ($path, $data, $image_list) = @_;
   my $desc;

   open XML, ">$path/.index.xml" or die "can not open '$path/.index.xml'";
   
   print XML "<?xml version='1.0' encoding='utf-8'?>\n";
   print XML "<!-- created by $0 -->\n";

   print XML "<Data ".($data->{'cdrip'}? "Source='CDRIP'":"")." Created='".localtime(time)."'>\n";
   
   delete $data->{'cdrip'};
   
   $desc = '';
   if (exists $data->{'description'} && length($data->{'description'}) ) {
      $desc = 
qq^\t<Description>
\t<\![CDATA[
$data->{"description"}
\t]]>
</Description>\n^;
      delete $data->{"description"};
   }
   
   if ( exists $data->{'genres'} && scalar (@{$data->{'genres'}}) ) {
      print XML "\t<Genres>\n";
      print XML "\t\t<Genre ID='$_' />\n" foreach (@{$data->{'genres'}});
      print XML "\t</Genres>\n";
      delete $data->{'genres'};
   }

   foreach my $d (keys %$data) {
      if ( ref($data->{$d}) eq 'ARRAY' ) {
         print XML "\t<$d Value='$_' />\n" foreach (@{$data->{$d}});
      }
      else {
         print XML "\t<$d Value='$data->{$d}' />\n";
      }
   }

   print XML $desc;

   if ( scalar(@$image_list) ) {
      print XML "\t<Thumbnails>\n";
      print XML "\t\t<Thumbnail Name='".(makeThumbnail($_))."' />\n" foreach(@$image_list);
      print XML "\t</Thumbnails>\n";
   }

   print XML "</Data>\n";

   close XML;
}

sub parseIndex($)
{
   my ($fileName) = @_;
   my %data;
   my $desc_flag;

   $data{'genres'} = [];
   $desc_flag = 0;
   
   open FH, $fileName and  do {
      while (<FH>) {
         chomp;
         if ( $desc_flag ) {
            $data{'description'} .= "$_\n";
         }
         elsif ( lc($_) eq 'description' ) {
            $desc_flag = 1;
         }
         elsif ( lc($_) eq 'cdrip' ) {
            $data{'cdrip'} = 1;
         }
         elsif ( /^(.*)=\"?(.*?)\"?$/ ) {
            if ( exists($data{$1}) ) {
               if ( ref($data{$1}) eq 'ARRAY' ) {
                  push @{$data{$1}}, $2;
               }
               else {
                  my $val = $data{$1};
                  
                  $data{$1} = undef;
                  push @{$data{$1}}, $val;
                  push @{$data{$1}}, $2;
               }
            } 
            else {
               $data{$1} = $2;
            }
         }
         else {
            push @{$data{'genres'}}, $_;
         }
      }
   };

   \%data;
}

readDir($_) foreach (@ARGV);
