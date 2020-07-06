#!/usr/bin/perl

$ad = {
    "swamp" => {x => "-250", y => "-100" },
    "temple" => {x => "-250", y => "-100" },
    "fertile" => {x => "-250", y => "-100" },
    "hill" => {x => "-250", y => "-100" },
    "volcano" => {x => "-250", y => "-100" },
    "plain" => {x => "-250", y => "-100" },
    "cursed" => {x => "-250", y => "-100" },
    "raft" => {x => "-250", y => "-100" },
    "scrub" => {x => "-250", y => "-100" },
    "ruin" => {x => "-250", y => "-100" },
    "den" => {x => "-250", y => "-100" },
};

sub ax {
    my ($x) = @_;
    my $sym = $syms[scalar(@syms)-1];

    if (exists($ad->{$sym}) && exists($ad->{$sym}->{"x"}))
    {
	$x += $ad->{$sym}->{"x"};
    }
    return $x;
}

sub ay {
    my ($y) = @_;
    my $sym = $syms[scalar(@syms)-1];

    if (exists($ad->{$sym}) && exists($ad->{$sym}->{"y"}))
    {
	$y += $ad->{$sym}->{"y"};
    }
    return $y;
}

sub Proc {
    my ($in, $stack, $out) = @_;

    my $token = $in->[0];
    shift(@{$in});
    if ($token =~ m:/([A-Za-z0-9]+):)
    {
	my $cname = $1;
	push(@{$out}, sprintf "FN(%s)", $cname);
	push(@syms, $cname);
	Proc($in, $stack, $out);
    }
    elsif ($token eq "{")
    {
	push(@{$out}, sprintf "{");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "}")
    {
	push(@{$out}, sprintf "}");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "bd")
    {
	push(@{$out}, sprintf "");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "def")
    {
	push(@{$out}, sprintf "");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "cs")
    {
	push(@{$out}, sprintf "C();");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "st")
    {
	push(@{$out}, sprintf "ST();");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "fp")
    {
	push(@{$out}, sprintf "FP();");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "gsave")
    {
	push(@{$out}, sprintf "GS();");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "grestore")
    {
	push(@{$out}, sprintf "GR();");
	Proc($in, $stack, $out);
    }
    elsif ($token eq "fill")
    {
	push(@{$out}, sprintf "F();");
	Proc($in, $stack, $out);
    }
    elsif ($token =~ m/(("+"|"-")?([0-9]+(\.[0-9]+)?))/)
    {
	push(@{$stack}, $1);
	Proc($in, $stack, $out);
    }
    elsif ($token eq "setgray")
    {
	my $gray = pop(@{$stack});
	return;
    }
    elsif ($token eq "mt")
    {
	my $y = pop(@{$stack});
	my $x = pop(@{$stack});
	push(@{$out}, sprintf("M(%d, %d);", &ax($x), &ay($y)));
	return;
    }
    elsif ($token eq "lt")
    {
	my $y = pop(@{$stack});
	my $x = pop(@{$stack});
	push(@{$out}, sprintf("L(%d, %d);", &ax($x), &ay($y)));
	return;
    }
    elsif ($token eq "arc")
    {
	my $a2 = pop(@{$stack});
	my $a1 = pop(@{$stack});
	my $r = pop(@{$stack});
	my $y = pop(@{$stack});
	my $x = pop(@{$stack});
	push(@{$out}, sprintf("A(%d, %d, %d, %d, %d);", &ax($x), &ay($y), $r, $a1, $a2));
	return;
    } 
    elsif ($token eq "arcn")
    {
	my $a2 = pop(@{$stack});
	my $a1 = pop(@{$stack});
	my $r = pop(@{$stack});
	my $y = pop(@{$stack});
	my $x = pop(@{$stack});
	push(@{$out}, sprintf("AN(%d, %d, %d, %d, %d);\n", $x, $y, $r, $a1, $a2));
	return;
    } 
    elsif ($token eq "rgb")
    {
	my $b = pop(@{$stack});
	my $g = pop(@{$stack});
	my $r = pop(@{$stack});
	push(@{$out}, sprintf("RGB(%f, %f, %f);", $r, $g, $b));
	return;
    } 
    elsif ($token eq "scale")
    {
	my $y = pop(@{$stack});
	my $x = pop(@{$stack});
	push(@{$out}, sprintf("SC(%f, %f);", $x, $y));
	return;
    } 
   else
    {
	push(@{$out}, sprintf("++ %s\n", $token)) if (length($token));
    }
    Proc($in, $stack, $out) if (scalar(@{$in}));
    return;
}

my @lines = <STDIN>;

my @tokens;

map {
    chomp $_;
    s/%.*$//;
    push(@tokens, split(/[ \t]+/, $_));
} @lines;

my @stack;
my @out;

# push(@out, "#include <cairo.h>");
# push(@out, "#include \"symbols.h\"");

Proc(\@tokens, \@stack, \@out);

push(@out, sprintf("SYMN(%d)\n", scalar(@syms)));
push(@out, sprintf("SYMS(%s)\n", join(",", @syms)));

print join("\n", @out), "\n";
