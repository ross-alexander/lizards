#!/usr/bin/perl

print("Content-Type: text/plain\n\n");

open(STREAM, "users.dat");
while(<STREAM>)
{
  print $_;
}
close(STREAM);
