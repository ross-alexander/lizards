my $x_min = 0;
my $x_max = 0;
my $y_min = 0;
my $y_max = 0;

my $scale = 0.4;
while(<>)
{
    chomp $_;
    my @a = split(/ +/, $_);
    my @b;
    my @c;
    map {
	if (/([LM])([0-9]+)/)
	{
	    push(@b, $1); push(@b, $2);
	}
	elsif (/([0-9]+)([LM])/)
	{
	    push(@b, $1); push(@b, $2);
	}
	else
	{
	    push(@b, $_);
	}
    } @a;
    my $i = 0;
    while ($i < scalar(@b))
    {
	my $cmd = $b[$i++];
	if ($cmd =~ /[LM]/)
	{
	    my $x = $b[$i++];
	    my $y = $b[$i++];
	    $y -= 100;
	    $x -= 250;
	    $x *= $scale;
	    $y *= $scale;

	    $x_min = $x_min < $x ? $x_min : $x;
	    $y_min = $y_min < $y ? $y_min : $y;
	    $x_max = $x_max > $x ? $x_max : $x;
	    $y_max = $y_max > $y ? $y_max : $y;

	    push(@c, $cmd);
	    push(@c, $x);
	    push(@c, $y);
	}
	else
	{
	    push(@c, $cmd);
	}
    }
    printf STDERR "%d %d %d %d\n", $x_min, $x_max, $y_min, $y_max;
    print join(" ", @c), "\n";
}
