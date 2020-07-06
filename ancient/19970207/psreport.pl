#!/usr/local/bin/perl

if (@ARGV < 3)
{
    die "Not enought parameters.\n";
}


$DATA = @ARGV[0];
$GAME = @ARGV[1];
$TURN = @ARGV[2];

open(PLAYERS, "$DATA/LPLAYER.$GAME") || die "Cannot open player file";

$returndate = <PLAYERS>;
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
    print $count;
    print "\n";
    if ($count < 10)
    {
	if ($count < 10)
	{
	    $filename = "$DATA/LP0$count\_$TURN.$GAME";
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
	if (/^([A-Za-z]+):([ ]*[A-Za-z0-9]+)$/)
	{
	    $tag = $1;
	    $values = $2;
	    if ($tag =~ /Header/)
	    {
		print OUTPUT "PageOne\n";
		print OUTPUT "($count) Player_Number\n";

#      credit = atof (player_cred [work]);
#      fprintf (fptr, "($ %0.2f) Credit\n", credit);

#      fprintf (fptr, "Address\n");
#      fprintf (fptr, "(%s) -18 Text\n", player_name [work]);
#      fprintf (fptr, "(%s) -18 Text\n", player_road [work]);
#      fprintf (fptr, "(%s) -18 Text\n", player_burb [work]);
#      fprintf (fptr, "(%s) -18 Text\n", player_city [work]);
#      fprintf (fptr, "(%s) -18 Text\n", player_country [work]);

		$clan = $clan[$count];
		print OUTPUT "($clan) Clan_Full_Name\n";
		print OUTPUT "Write\n";
	    }
	}
    }
    rename("$filename.tmp", "$filename");
}
