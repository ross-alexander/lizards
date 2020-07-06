while(<>)
{
    chomp $_;
    s/([0-9]+) ([0-9]+) mt /M $1 $2\n/g;
    s/([0-9]+) ([0-9]+) lt /L $1 $2\n/g;
    s/st/Z\n/g;
    print $_;
}
