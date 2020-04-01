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

#my $sql = "SELECT reference, id FROM movie_review";
my $sql = "SELECT reference, id FROM movie_review WHERE length(coalesce(imdb_actors, '')) = 0 order by date desc";
my $upd_sql = "UPDATE movie_review SET imdb_actors = ? WHERE id = ?";

my $sth = $dbh->prepare($sql);
$sth->execute();
while (my $s = $sth->fetchrow_hashref()) {
    my $imdb = '';

    if (($s->{reference} || '') ne '' && $s->{reference} =~ /\/title\/(tt.*?)\//) {
        $imdb = $1;

print Dumper($s->{reference}, $s->{id}, $imdb);
        my $url = "https://www.omdbapi.com/?i=$imdb&apikey=6314df89";
        my $response = HTTP::Tiny->new->get($url);
        if ($response->{success}) {
            my $data = $json->decode($response->{content});

            if (lc ($data->{Response}||'') eq 'false') {
                print "$url\n";
                print "$data->{Error}\n";
                next;
            }

            my @params = (lc($data->{Actors}), $s->{id});
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
}
$sth->finish();

$dbh->disconnect();
