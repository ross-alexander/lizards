my @lines = <STDIN>;

for my $line (@lines)
{
    my @bits = split(/ +/, $line);
    while (scalar(@bits))
    {
	if ($bits[0] eq "M" || $bits[0] eq "L")
	{
	    printf "%s(%d,%d);\n", $bits[0], $bits[1], $bits[2];
	}
	shift(@bits);
	shift(@bits);
	shift(@bits);
    }
}
