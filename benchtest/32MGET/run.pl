#!/usr/bin/perl

use v5.28;
use utf8;

use strict;
use warnings;
use bignum;

use File::Temp ();
use File::Basename;
use Cwd;

sub help{
    say "usage: $0 [upstream|ringbuf] [time|profile]";
    exit;
}

my $iterations = 2**10;
my $tmpFile;
my $testName = basename(getcwd());

`make clean`;

$ARGV[0] = $ARGV[0] // '';
if($ARGV[0] eq 'upstream'){
    `make $testName.upstream.exe`;
    `mv $testName.upstream.exe $testName.exe`;
    my @so_name = `readelf -d $testName.exe`;
    my $so_name = (grep {m/hiredis/} @so_name)[0];
    $so_name =~ s/.*\[(.*)\].*/$1/;
    my @upstream_path = `cat Makefile`;
    my $upstream_path = (grep {m/upstream_path/} @upstream_path)[0];
    $upstream_path =~ s/.*:=\s*(.*)\s*$/$1/;
    `ln -s $upstream_path/libhiredis.so $so_name`;
}
elsif($ARGV[0] eq 'ringbuf'){
    `make $testName.ringbuf.exe`;
    `mv $testName.ringbuf.exe $testName.exe`;
    my @so_name = `readelf -d $testName.exe`;
    my $so_name = (grep {m/hiredis/} @so_name)[0];
    $so_name =~ s/.*\[(.*)\].*/$1/;
    my @ringbuf_path = `cat Makefile`;
    my $ringbuf_path = (grep {m/ringbuf_path/} @ringbuf_path)[0];
    $ringbuf_path =~ s/.*:=\s*(.*)\s*$/$1/;
    `ln -s $ringbuf_path/libhiredis.so $so_name`;
}
else{
    &help();
}

$ARGV[1] = $ARGV[1] // '';
if($ARGV[1] eq 'time'){
    my $acc = 0;
    foreach (1 .. $iterations){
        my $tmpFileName = $tmpFile->filename;
        my $time = `./$testName.exe < $tmpFileName`;
        if($? >> 8){
            die 'error running test';
        }
        $acc += $time;
    }
    $acc /= $iterations;
    say $acc;
}
elsif($ARGV[1] eq 'profile'){
    my $tmpFileName = $tmpFile->filename;
    `valgrind --tool=callgrind --instr-atstart=no --dump-instr=yes ./$testName.exe < $tmpFileName`;
}
else{
    &help();
}

BEGIN{
    my $longString = '';
    foreach (0 .. 2**25){
        $longString .= chr(ord('!') + int(rand(ord('~') - ord('!'))));
    }
    $tmpFile = File::Temp->new(SUFFIX => '.txt');
    print $tmpFile $longString . '\n';
}
