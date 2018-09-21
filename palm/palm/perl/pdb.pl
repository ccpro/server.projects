#!/usr/local/bin/perl -w

use strict;
use Data::Dumper;

use Palm::PDB;
use Palm::Raw;

&Palm::PDB::RegisterPRCHandlers("Palm::Raw", "");

my $pdb = new Palm::PDB;
$pdb->Load("pdb1.pdb");

print Dumper($pdb);