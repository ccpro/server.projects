#!/usr/bin/perl -w

use strict;
use Data::Dumper;

use Class::Date qw(:errors date localdate gmdate now -DateParse);

use DBI;

use lib('/usr/local/www/Modules');
use DBConfig;

sub check_emails();
sub check_phones();
sub check_websites();
sub update_accounts();
sub trim($);

my @header = qw(firstname middlename lastname birthday email_1 email_2 phone_1 phone_2 address website);
my @data;

my @array;
my $i = 0;
<>;
while(<>) {
   my %data;

   s/\n|\r//gis;
   @array = split("\t");

   $i =0;
   for my $f (@header) {
      $data{$f} = $array[$i++] || '';
   }   
   
   push @data, \%data;
}

check_emails();
check_phones();
check_websites();

update_accounts();


sub check_emails()
{
 my $tmp;

   for my $d (@data) {

      if (($d->{email_1}||'') eq '' && ($d->{email_2}||'') ne '') {
         $d->{email_1} = lc $d->{email_2};
         $d->{email_2} = '';
      }
      elsif (($d->{email_2}||'') =~ /\@gmail.com$/ ) {
         $tmp = $d->{email_1};
         $d->{email_1} = lc $d->{email_2};
         $d->{email_2} = lc $tmp;
      }

   }
}


sub check_phones()
{
   for my $d (@data) {
      $d->{phone_1} =~ s/\D//g if (($d->{phone_1}||'') ne '');
      $d->{phone_2} =~ s/\D//g if (($d->{phone_2}||'') ne '');
   }
}

sub check_websites()
{
   for my $d (@data) {
      if (($d->{website}||'') ne '' && $d->{website} =~ /google/) {
         $d->{website} = '';
      }
   }
}

sub format_phone($)
{
 my ($phone) = @_;
 my $new_phone;

   return '' if ($phone eq '');
   
   $new_phone = $phone;
   $new_phone =~ s/\D//g;
   $new_phone =~ s/^(.*)(\d\d\d)(\d\d\d)(\d\d\d\d)/$2-$3-$4/;
   
   $new_phone;
}

sub update_account($$$);
sub insert_account($$);
sub find_correct_one($$);

sub update_accounts()
{
 my $dbh = DBI->connect(@{DBConfig::get_db_params()}) || die "Error: Can not connect to db\n";
 my $sth;
 my $sql;
 my $phone1;
 my $phone2;
 my $found_data;

   for my $d (@data) {
      $phone1 = format_phone($d->{phone_1});
      $phone2 = format_phone($d->{phone_2});
      $sql = qq^SELECT * FROM persons WHERE (firstname = '$d->{firstname}' And lastname = '$d->{lastname}')^;
      if ($phone1 ne '') {
         $sql .= " Or work_phone = '$phone1' Or home_phone = '$phone1' Or other = '$phone1'";
      }
      if ($phone2 ne '') {
         $sql .= " Or work_phone = '$phone2' Or home_phone = '$phone2' Or other = '$phone2'";
      }
      if ($d->{email_1} ne '') {
         $sql .= " Or Lower(email) = Lower('$d->{email_1}')";
      }
      if ($d->{email_2} ne '') {
         $sql .= " Or Lower(email) = Lower('$d->{email_2}')";
      }
      if ($d->{birthday} ne '') {
         $sql .= " Or Date(birthday) = date('$d->{birthday}')";
      }

#      $sth = $dbh->prepare($sql) || warn "$@\n $sql\n";
#      $sth->execute() || warn "$@\n $sql\n";
      
      $found_data = $dbh->selectall_arrayref($sql);

      if (scalar(@$found_data) == 1) {
         print "\n\nFound: $found_data->[0][0]\n";
         update_account($dbh, $found_data->[0], $d);
      }
      elsif (scalar(@$found_data) > 1)
      {
         print "\n\nFound few accounts $d->{firstname} $d->{lastname}\n$sql\n".Dumper($found_data);
         my $correct = find_correct_one($found_data, $d);
         
         print "\n\nFound good one:".Dumper($correct);
         
         update_account($dbh, $correct, $d) if (defined $correct);
      }
      elsif (scalar(@$found_data) == 0) {
         print "\n\nNot found ($sql)\n".Dumper($d);
         insert_account($dbh, $d);
      }
      
#      $sth->finish();
      
#      print Dumper($found_data);
   }


   $dbh->disconnect();
}

sub insert_account($$)
{
 my ($dbh, $data) = @_;
 my $sql;
 my @fields;
 my @values;
 my @note;
 my $rc = 0;

   if( ($data->{lastname}||'') eq '' || ($data->{firstname}||'') eq '') {
      print "Can not insert account ".Dumper($data);
      return;
   }

   if (($data->{middlename}||'') ne '') {
      $data->{firstname} = "$data->{firstname}, $data->{middlename}";
   }

   push @fields, 'lastname', 'firstname';
   push @values, $dbh->quote($data->{lastname}), $dbh->quote($data->{firstname});

   if (($data->{birthday}||'') ne '') {
      push @fields, 'birthday';
      push @values, $dbh->quote($data->{birthday});
   }

   if (($data->{email_1}||'') ne '') {
      push @fields, 'email';
      push @values, $dbh->quote($data->{email_1});
   }

   if (($data->{email_2}||'') ne '') {
     push @note, $data->{email_2};
   }

   if (($data->{website}||'') ne '') {
     push @note, $data->{website};
   }

   if (($data->{phone_1}||'') ne '') {
      push @fields, 'other';
      push @values, $dbh->quote(format_phone $data->{phone_1});
   }

   if (($data->{phone_2}||'') ne '') {
      push @fields, 'home_phone';
      push @values, $dbh->quote(format_phone $data->{phone_2});
   }

   if (($data->{address}||'') ne '') {
      push @fields, 'address';
      push @values, $dbh->quote($data->{address});
   }

   if (scalar @note > 0) {
      push @fields, 'note';
      push @values, $dbh->quote(join(',',@note));
   }

   $sql = "INSERT INTO persons(".(join(',', @fields)).") VALUES(".(join(',',@values)).")";

   local $dbh->{AutoCommit} = 0;
   local $dbh->{RaiseError} = 1;

   eval {
      $dbh->do($sql);
      $dbh->commit();
      
      $rc = 1;
   };
   if ($@) {
      $dbh->rollback();
      die "Error: $@\n $sql";
   }
print "$sql\n";

   $rc;
}

sub find_correct_one($$)
{
 my ($data, $new_data) = @_;
 my $email;

   for my $d(@$data) {
      $email = $d->[20] || '';

      if (($new_data->{email_1}||'') ne '' && $email eq $new_data->{email_1}) {
         return $d;
      }
      if (($new_data->{email_2}||'') ne '' && $email eq $new_data->{email_2}) {
         return $d;
      }
   }
   
   undef;
}

sub update_account($$$)
{
 my ($dbh, $user_data, $new_data) = @_;
 my $id = $user_data->[0];
 my @new_values;
 my $sql;
 my $note = $user_data->[17];
 
 my $update_log = '';

   if ((trim $user_data->[8]||'') ne ''  && (trim $new_data->{phone_1}||'') ne '' && 
       format_phone(trim $user_data->[8]) ne format_phone(trim $new_data->{phone_1}) ) {
print format_phone($user_data->[8]).' :: '.format_phone($new_data->{phone_1})."\n";
      $update_log .= qq^old cell value:'$user_data->[8]'\n^;
      push @new_values, 'other = '.$dbh->quote(format_phone($new_data->{phone_1}));
   }
   if ((trim $user_data->[6]||'') ne ''  && (trim $new_data->{phone_2}||'') ne '' && 
       format_phone(trim $user_data->[6]) ne format_phone(trim $new_data->{phone_2}) ) {
print format_phone($user_data->[6]).' :: '.format_phone($new_data->{phone_2})."\n";
      $update_log .= qq^old home phone value:'$user_data->[6]'\n^;
      push @new_values, 'home_phone = '.$dbh->quote(format_phone($new_data->{phone_1}));
   }
   if ((trim $user_data->[20]||'') ne ''  && (trim $new_data->{email_1}||'') ne '' && 
       lc trim $user_data->[20] ne lc trim $new_data->{email_1} ) {
print "'$user_data->[20]' :: '$new_data->{email_1}'\n";
      unless ($note =~ /$new_data->{email_1}/si) {
         $update_log .= qq^old email value:'$user_data->[20]'\n^;
      }
      push @new_values, 'email = '.$dbh->quote($new_data->{email_1});
   }
   if ((trim $new_data->{email_2}||'') ne '') {
      unless ($note =~ /$new_data->{email_2}/si) {
         $update_log .= "second email value:'$new_data->{email_2}'\n";
      }
   }
   if ((trim $user_data->[19]||'') ne ''  && (trim $new_data->{birthday}||'') ne '') {
    my $db_date = date $user_data->[19];
    my $new_date = date $new_data->{birthday};

      if ( (split(' ',$db_date))[0] ne (split(' ',$new_date))[0] ) {
         $update_log .= qq^old birthday value:'$user_data->[19]'\n^;
         push @new_values, 'birthday = '.$dbh->quote($new_data->{birthday});
      }
   }
   if ((trim $new_data->{website}||'') ne '') {
      unless ($note =~ /$new_data->{website}/si) {
         $update_log .= "website value:'$new_data->{website}'\n";
      }
   }

   if (trim $update_log ne '') {
      $update_log = ($user_data->[17]||'')."\nUpdate :" . localtime()."\n$update_log";
      push @new_values, 'note = '.$dbh->quote($update_log);
   }

   if (scalar @new_values > 0) {   
      $sql = "UPDATE persons SET ".(join(',', @new_values))." WHERE person_id = $id";

      local $dbh->{AutoCommit} = 0;
      local $dbh->{RaiseError} = 1;
       
      eval {
         $dbh->do($sql);
         $dbh->commit();
      };
      if ($@) {
         $dbh->rollback();
         die "Error: $@\n $sql";
      }

 print "$sql\n";
   }
}

sub trim($)
{
 my $string = shift || '';

   $string =~ s/^\s+//;
   $string =~ s/\s+$//;

   $string;
}

=pod
 [
    229,
    'Aaron',
    'Logue',
    undef,
    'Ygnition',
    '206-574-3376',
    '206-523-8831',
    undef, <-- fax
    undef, <-- cell
    '7530 1st ave NE',
    'Seattle',
    'WA',
    '98115',
    'USA',
    undef,
    'http://www.cryogenius.com',
    undef,
    'note',
    0,
    undef,
    'gyro@cryogenius.com'
 ]
                                                                                                                                                                                                                                                                       

   Column   |          Type          |                        Modifiers
------------+------------------------+---------------------------------------------------------
 person_id  | integer                | not null default nextval(('person_id'::text)::regclass)
 firstname  | character varying(35)  |
 lastname   | character varying(35)  |
 title      | character varying(32)  |
 company    | character varying(50)  |
 work_phone | character varying(20)  |
 home_phone | character varying(20)  |
 fax        | character varying(20)  |
 other      | character varying(20)  |
 address    | character varying(70)  |
 city       | character varying(32)  |
 state      | character varying(5)   |
 zip        | character varying(10)  |
 country    | character varying(32)  |
 custom2    | character varying(255) |
 custom3    | character varying(255) |
 custom4    | character varying(255) |
 note       | text                   |
 privateyn  | smallint               | not null default 0
 birthday   | date                   |
 email      | character varying(64)  |
                        
=cut
