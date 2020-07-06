while(<>)
{
    chomp $_;
    if (/([M|L]) ([0-9]+) ([0-9]+)/)
    {
	$x = $2 - 250;
	$y = 150 - $3;
	print "$1 $x $y";
    }
    else
    {
	print $_;
    }
    print "\n";
}
