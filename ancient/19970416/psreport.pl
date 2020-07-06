#!/usr/local/bin/perl

if (@ARGV < 3)
{
    die "Not enought parameters.\n";
}


$DATA = @ARGV[0];
$GAME = @ARGV[1];
$TURN = @ARGV[2];

open(PLAYERS, "$DATA/$GAME.LPLAYER") || die "Cannot open player file";

@players = <PLAYERS>;
close(PLAYERS);

push(@id, "CIR");
push(@player, "Circle Games");
push(@clan, "Circle Games");

for $i (@players)
{
    ($id, $player, $clan, $street, $suburb, $city, $country, $credit, $output) = split(/,/, $i);
    push(@id, $id);
    push(@player, $player);
    push(@clan, $clan);
}

for $count (0 ... @players)
{
    if ($count < 10)
    {
	if ($count < 10)
	{
	    $filename = "$DATA/$GAME.LP0$count\_$TURN";
	}
	else
	{
	    $filename = "$DATA/LP$count\_$TURN.$GAME";
	}
	print "$filename\n";
	open(INPUT, "$filename") || die "Cannot open player's file.\n";
	open(OUTPUT, ">$filename.tmp") || die "Cannot open player's output file.\n";
    }
    while(<INPUT>)
    {
	if(/[A-Za-z0-9]\(.*\)/)
	{
	    print OUTPUT $_;
	}
    }
#    rename("$filename.tmp", "$filename");
}
