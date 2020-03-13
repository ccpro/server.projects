#!/usr/local/perl -w

use strict;
use BSON;
use Data::Dumper;

use MIME::Base64 qw( decode_base64 encode_base64 );

use JSON::MaybeXS;

my $base64str = 'NQAAAAJhAAQAAAAxMjMAEGIA3gAAAANjABsAAAAQY2MA6gAAAAJjYgAGAAAANTU1NTUAAAA=';
my $data = decode_base64($base64str);

my $codec = BSON->new;

my $obj = $codec->decode_one($data);
print Dumper($obj);
