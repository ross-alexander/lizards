while(<>)
{
    chomp $_;
    if (/^([0-9]+) ([0-9]+) mt/)
    {
	my $x = $1 - 250;
	my $y = 180 - $2;
	print "M $x $y\n";
    }
    elsif (/^([0-9]+) ([0-9]+) lt/)
    {
	my $x = $1 - 250;
	my $y = 180 - $2;
	print "L $x $y\n";
    }
    else
    {
	print $_,"\n";
    }
}
