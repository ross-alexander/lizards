#!/usr/bin/perl

print("Content-Type: text/plain\n\n");

open(STREAM, "groups.dat");
while(<STREAM>)
{
  print $_;
}
close(STREAM);
