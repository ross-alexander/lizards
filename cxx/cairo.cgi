#!/usr/bin/perl

use CGI qw(:html4);

my $svgfile = "tmp/cairo.svg";

my $q = CGI->new();

print $q->header();
print $q->start_html('Lizards! maps');

if ($q->param('map'))
{
    my $map = $q->param('map');
    if (-f $map)
    {
	my $res = `./cairo -c $map -o $svgfile`;
	if ($! == 0 && -f $svgfile)
	{
	    print $q->pre($res);
        }
    }
}
print $q->object({data=>$svgfile, type=>'image/svg+xml'});
print $q->end_html();
