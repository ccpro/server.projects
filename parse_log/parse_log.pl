#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use DBI;
use Socket;

sub parse_string($);
sub check_ip($);

my %names;

#my $dbh = DBI->connect('dbi:PgPP:dbname=ccpro_spb_ru', 'ccpro','',{RaiseError=>1}) ||
#        die "$!: can not connect to db";

my %data;
my %month = (
	'Jan'	=> '01',
	'Feb'	=> '02',
	'Mar'	=> '03',
	'Apr'	=> '04',
	'May'	=> '05',
	'Jun'	=> '06',
	'Jul'	=> '07',
	'Aug'	=> '08',
	'Sep'	=> '09',
	'Oct'	=> '10',
	'Nov'	=> '11',
	'Dec'	=> '12',
);

while(<>){
	chomp;
	my $d = parse_string($_);
#	grep $d->{date}, @data || push @data, $d->{date};

	$data{$d->{date}}{$d->{ip}} = check_ip($d->{ip});
}

my %bmonth;
foreach(keys %month){
	$bmonth{$month{$_}} = $_;
}

for my $date (sort keys %data){
#	print Dumper(\%data);

	my @d = split('-', $date);

	print "$d[0]-$bmonth{$d[1]}-$d[2]\n".Dumper($data{$date});
}

#$dbh->disconnect();

sub parse_string($)
{
	my $str = shift || return {};

	my $data;

	($data->{ip}, $data->{date}) = $str =~ /^(.*)\s.*\s.*\s\[(.*)\s.*]/;

	my @d = split(":|/", $data->{date});

	$data->{date} = "$d[2]-$month{$d[1]}-$d[0]";

	$data;
}

sub check_ip($)
{
        my $ip = shift || return 'none';

        my ($name, $aliases, $addrtype, $length, @addrs);

        if( $ip =~ /^([0-9]*)\.([0-9]*)\.([0-9]*)\.([0-9]*)$/ ){
                my $address = pack( "C4", $1, $2, $3, $4 );

                # Do a lookup
                if(exists($names{$address})){
                        # Use the hashed name
                        $name = $names{$address};
                } else {
                        ($name, $aliases, $addrtype, $length, @addrs) =
                                gethostbyaddr( $address, &AF_INET );
                        if( !$name ){
                                $name = "$1.$2.$3.$4";
                        }
                        $names{$address} = $name;
                }
                $name;
        } else {
                $ip;
        }
}
