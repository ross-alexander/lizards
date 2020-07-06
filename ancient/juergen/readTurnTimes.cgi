#!/usr/bin/perl

print("Content-Type: text/plain\n\n");

open(STREAM, "lizTurnTimes.dat");
while(<STREAM>)
{
  print $_;
}
close(STREAM);
