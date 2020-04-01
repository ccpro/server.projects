#!/usr/local/bin/perl -w

use strict;
use Data::Dumper;

use JSON;
use DBI;
use HTTP::Tiny;

use lib qw(/usr/local/www/modules);
use DBConfig;

my $json = JSON->new->allow_nonref();
my $dbh = DBI->connect(@{DBConfig->get_db_params()}) || die "Error: Can not connect to db\n";

for my $imdb (@ARGV) {

    my $id = $dbh->selectrow_arrayref("select id from movie_review where reference like '%$imdb%'");

    $id = $id->[0];
    next if (0 == $id);

    my $url = "https://www.omdbapi.com/?i=$imdb&apikey=6314df89";
    my $response = HTTP::Tiny->new->get($url);
    if ($response->{success}) {
        my $data = $json->decode($response->{content});

        if (lc ($data->{Response}||'') eq 'false') {
            print "$url\n";
            print "$data->{Error}\n";
            next;
        }

        $data->{imdbRating} = 0 if ('N/A' eq $data->{imdbRating});

        if ($data->{Runtime} eq 'N/A'){
            $data->{Runtime} = 0;
        } else {
            $data->{Runtime} = (split(' ', $data->{Runtime}))[0];
        }

        $data->{Year} =~ s/\D//g;
        $data->{Rated} = 'G' if ($data->{Rated} eq 'N/A');

        print Dumper($data);

        my $upd_sql = "UPDATE movie_review SET imdb_rate = ?, imdb_rated = ?, imdb_year = ?, imdb_runtime = ?, imdb_genre = ?, imdb_plot = ?, imdb_country = ? WHERE id = ?";
        my @params = ($data->{imdbRating} * 10, $data->{Rated}, $data->{Year}, $data->{Runtime}, $data->{Genre}, $data->{Plot}, $data->{Country}, $id);
            {
                local $dbh->{AutoCommit} = 0;
                local $dbh->{RaiseError} = 1;

                eval {
                    $dbh->do($upd_sql, undef, @params);
                    $dbh->commit();
                };

                if ($@) {
                    $dbh->rollback();
                    print Dumper($data);
                    print Dumper(\@params);
                    die "$@\n $upd_sql\n";
                }
            }
    } else {
        print Dumper($response);
    }
}

$dbh->disconnect();