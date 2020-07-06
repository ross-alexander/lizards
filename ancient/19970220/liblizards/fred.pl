#!/usr/local/bin/perl

while(<>)
{
	s/player_home_den[ ]*\[([^\]]+)\]/player[$1].home_den/;
	s/player_clan_name[ ]*\[([^\]]+)\]/player[$1].clan_name/;
	print $_;
}
