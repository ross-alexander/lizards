my $top = 100;
my $left = -250;
my $scale = 0.25;

my $x_min = 1000;
my $x_max = -1000;
my $y_min = 1000;
my $y_max = -1000;

while(<>)
{
# Remove comments
    chomp $_;
    s/%.*$//;
    my @tokens = split(/ +/, $_);
    map {
	if(m:/[a-z]+:)
	{
	    substr($_, 0, 1) = "";
	    push(@stack, $_);
	}
	if (m:\{:)
	{
	    push(@stack, $_);
	    $path = "";
	    $stoke = "none";
	    $fill = "none";
	}
	if (m:\}:)
	{
	    my $left = pop(@stack);
	    if ($left ne "{")
	    {
		print "Error.\n";
		exit(1);
	    }
	}
	if (m:[0-9]+:)
	{
	    push(@stack, $_);
	}
	if (m:mt:)
	{
	    my $y = pop(@stack);
	    my $x = pop(@stack);
	    $x += $left;
	    $y = $top - $y;
	    $x *= $scale;
	    $y *= $scale;
	    $path .= "M $x $y ";

	    $x_min = $x_min < $x ? $x_min : $x;
	    $y_min = $y_min < $y ? $y_min : $y;
	    $x_max = $x_max > $x ? $x_max : $x;
	    $y_max = $y_max > $y ? $y_max : $y;

	}
	if (m:lt:)
	{
	    my $y = pop(@stack);
	    my $x = pop(@stack);
	    $x += $left;
	    $y = $top - $y;
	    $x *= $scale;
	    $y *= $scale;
	    $path .= "L $x $y ";

	    $x_min = $x_min < $x ? $x_min : $x;
	    $y_min = $y_min < $y ? $y_min : $y;
	    $x_max = $x_max > $x ? $x_max : $x;
	    $y_max = $y_max > $y ? $y_max : $y;

	}
	if (m:arcn?:)
	{
	    my $e = pop(@stack);
	    my $s = pop(@stack);
	    my $r = pop(@stack);
	    my $y = pop(@stack);
	    my $x = pop(@stack);

	    my $sx = $x + $r * cos($s * 3.1415927 / 180);
	    my $sy = $y + $r * sin($s * 3.1415927 / 180);

	    my $ex = $x + $r * cos($e * 3.1415927 / 180);
	    my $ey = $y + $r * sin($e * 3.1415927 / 180);

	    printf STDERR "%d %d %d %d -- %d %d\n", $x, $y, $r, $s, $sx, $sy;

	    $sx += $left;
	    $ex += $left;
	    $sy = $top - $sy;
	    $ey = $top - $ey;
	    $sx *= $scale;
	    $sy *= $scale;
	    $ex *= $scale;
	    $ey *= $scale;
	    $r *= $scale;

	    $path .= "M $sx $sy A $r $r 0 0,1 $ex $ey ";

#	    $x_min = $x_min < $x ? $x_min : $x;
#	    $y_min = $y_min < $y ? $y_min : $y;
#	    $x_max = $x_max > $x ? $x_max : $x;
#	    $y_max = $y_max > $y ? $y_max : $y;

	}
	if (m:fill:)
	{
	    $fill = "blue";
	}
	if (m:st:)
	{
	    $stoke = "white";
	}
	if (m:bd:)
	{
	    my $var = pop(@stack);
	    print "<path id=\"$var\" d=\"$path\"/>\n";
	    printf STDERR "%d %d %d %d\n", $x_min, $x_max, $y_min, $y_max;
	}
    } @tokens;
}

