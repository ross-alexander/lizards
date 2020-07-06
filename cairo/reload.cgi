#!/usr/bin/perl -Iperl -Iperl/blib/arch/auto/lizards

use 5.24.0;
use CGI qw(:cgi);
use XML::LibXML;
use File::Basename;
use Encode;
use JSON;
use Data::Dumper;

use lizards;

# ----------------------------------------------------------------------
#
# make_xhtml
#
# ----------------------------------------------------------------------

sub make_xhtml {
    my $opts = {@_};
    my $doc = XML::LibXML::Document->new("1.0", "utf-8");
    my $dtd = $doc->createInternalSubset("html", undef, undef);
    my $html = $doc->createElement("html");
    $doc->setDocumentElement($html);
    my $head = $html->addNewChild("", "head");
    my $body = $html->addNewChild("", "body");
    
    $head->appendTextChild("title", "Lizards!");
    my $link = $head->addNewChild("", "link");
    $link->setAttribute("type", "text/css");
    $link->setAttribute("rel", "stylesheet");
    $link->setAttribute("href", "default.css");

    if ($opts->{script})
    {
	my $script = $head->addNewChild("", "script");
	$script->setAttribute("language", "JavaScript");
	$script->setAttribute("src", $opts->{script});
	$script->appendText(" ");
    }
    return ($doc, $html, $head, $body);
}

# ----------------------------------------------------------------------
#
# handle_post
#
# ----------------------------------------------------------------------

sub handle_post {
    my ($cgi) = @_;

    my $hidden = $cgi->param('hidden');
    my $res = decode_json($hidden);

    print STDERR $hidden, "\n";

    if ($res->{'function'} eq 'select_layout')
    {
	$res->{tmpl} = {
	    layout => $res->{id},
	    hexes => []};
	select_template($cgi, $res);
    }
    elsif ($res->{'function'} eq 'show_template')
    {
	select_template($cgi, $res);
    }
    else
    {

	print $cgi->header(-type => 'text/plain');
	my $vars = $cgi->Vars();
	
	print $cgi->param('hidden');
	
	while(my ($k, $v) = each(%$vars))
	{
	    printf("%s -> %s\n", $k, $v);
	}
    }
}

# ----------------------------------------------------------------------
#
# read_js_file
#
# ----------------------------------------------------------------------
sub read_js_file {
    my ($file) = @_;

    return undef if (! -f $file);
    my $stream;
    open($stream, "<:encoding(utf-8)", $file);
    my $text = join("", <$stream>);
    close($stream);

    return JSON->new->relaxed->decode(decode('utf-8', $text));
}

# ----------------------------------------------------------------------
#
# select_template
#
# ----------------------------------------------------------------------

sub select_template {
    my ($cgi, $full) = @_;

    my $icon = $full->{icon};
    my $tmpl = $full->{tmpl};
    my $active = $tmpl->{active};
    my $session = $cgi->param('session');

    # --------------------
    # Den types
    # --------------------

    my $den_types = {
	RED => 1,
	GRN => 1,
	GRY => 1,
	YEL => 1,
	BLK => 1
    };

# --------------------
# If an icon has been pressed
# --------------------

    if(defined($icon))
    {
	my $hex = undef;
	for my $i (0 .. length($tmpl->{hexes} - 1))
	{
	    my $h = $tmpl->{hexes}->[$i];
	    $hex = $h if ($h->{id} == $active);
	}

# --------------------
# If there is a hex there
# --------------------

	if ($hex)
	{

# --------------------
# If it is +/- for adding to bands
# --------------------

	    if ($icon =~ m:^([+-])([A-Z]+):)
	    {
		my $band_type = $2;

		print STDERR "Band $band_type\n";

		$hex->{band} //= {}; # Assigns unless definded $hex->{band}
 		$hex->{band}->{$band_type} += 10 if ($1 eq '+');
		$hex->{band}->{$band_type} -= 10 if ($1 eq '-');
	    }
	    elsif ($icon eq "HME")
	    {
		if($den_types->{$hex->{type}})
		{
		    $tmpl->{home} = $hex->{id};
		    printf(STDERR "Home is %s\n", $tmpl->{home});
		}
	    }
	    else
	    {
		$hex->{type} = $icon;
	    }
	}

# --------------------
# If no hex defined then create one
# --------------------

	else
	{
	    printf(STDERR "Adding hex %s %s\n", $active, $icon);
	    if (!($icon =~ m:^[+-]:))
	    {
		$hex = {'id' => $active, 'type' => $full->{icon}};
		push(@{$tmpl->{hexes}}, $hex);
	    }
	}
    }

    my $tmpl_file = "tmp/$session.js";
    my $svg_file = "tmp/$session.svg";

    my $stream;
    open($stream, ">", $tmpl_file);
    print $stream encode_json($tmpl);
    close($stream);

    printf(STDERR "Saved $tmpl_file\n");

    my $layouts = lizards::layouts_t->new($lizards::FORMAT_FILE_JS, "LAYOUT.JS");

    my $template = lizards::template_t->new($layouts, $lizards::FORMAT_FILE_JS, $tmpl_file);
    my $grid = $template->realize();
    my $output = $grid->output();

    my @text;
    
    if (exists($full->{x}) && exists($full->{y}) && exists($full->{width}) && exists($full->{height}))
    {
	push(@text, sprintf("x,y = %f, %f w,h = %f, %f\n", $full->{x}, $full->{y}, $full->{width}, $full->{height}));
	     
	my $hex = $output->hit($full->{x}, $full->{y}, $full->{width}, $full->{height});
	if (defined($hex))
	{
	    $tmpl->{active} = $hex->getid();
	    $template->setactive($hex->getid());
	    push(@text, sprintf("New hex %s\n", $hex->getid()));
	    printf(STDERR "-- %s %d\n", $hex->getid(), $hex->{active});
	}
    }
    elsif ($tmpl->{active})
    {
	$template->setactive($tmpl->{active});
    }
    $output->svg($svg_file, 0);
    $template->save($tmpl_file);

# -------------------
# Build base xml structure
# --------------------

    my ($doc, $html, $head, $body) = make_xhtml(script => "default.js");

# --------------------
# Stick a nice header up
# --------------------

    my $h1 = $body->addNewChild("", "h1");
    $h1->appendText("Lizards!");

    my $form = $body->addNewChild("", "form");
    $form->setAttribute("method", "POST");
    $form->setAttribute("action", "reload.cgi");
    $form->setAttribute("id", "bigpost");

    my $input = $form->addNewChild("", "input");
    $input->setAttribute('type', 'hidden');
    $input->setAttribute('id', 'hidden');
    $input->setAttribute('name', 'hidden');
    $input->setAttribute('value', 'default');
    
    my $input = $form->addNewChild("", "input");
    $input->setAttribute('type', 'hidden');
    $input->setAttribute('id', 'session');
    $input->setAttribute('name', 'session');
    $input->setAttribute('value', $session);

    my $table = $form->addNewChild("", "table");
    $table->setAttribute("class", "icons");


    sub add_row {
	my ($table, $row) = @_;

	my @imgs;

	my $tr = $table->addNewChild("", "tr");
	for my $i (@$row)
	{
	    my $td = $tr->addNewChild("", "td");
	    $td->setAttribute("class", "outer");
	    my $table_s = $td->addNewChild("", "table");
	    my $tr_i = $table_s->addNewChild("", "tr");
	    my $td_i = $tr_i->addNewChild("", "td");
	    if (exists($i->{f}))
	    {
		my $img = $td_i->addNewChild("", "img");
		$img->setAttribute("src", $i->{f});
		$img->setAttribute("class", "icon");
		if (exists($i->{t}))
		{
		    $img->setAttribute("icon_type", $i->{t});
		}
		push(@imgs, $img);
	    }
	    if (exists($i->{c}))
	    {
		my $tr_c = $table_s->addNewChild("", "tr");
		my $td_c = $tr_c->addNewChild("", "td");
		$td_c->appendText($i->{c});
	    }
		
	    for my $j ('class', 'id', 'rowspan', 'colspan')
	    {
		$td->setAttribute($j, $i->{$j}) if (exists($i->{$j}));
	    }
	}
	return @imgs;
    }

    my @row1 = add_row($table, [
			   { class => 'bigbox', rowspan => 5, f => $svg_file },
			   { class => 'smallbox', rowspan => 2, colspan => 3, id => 'sb' },
			   {c => 'Water', f => "images/WAT.svg", t => 'WAT'},
			   {c => 'Plains', f => "images/PLA.svg", t => 'PLA'},
			   {c => 'Fertile', f => "images/FER.svg", t => 'FER'},
			   {c => 'Scrub', f => "images/SCR.svg", t => 'SCR'},
		       ]);

    my @desc;
    for my $h (sort({$a->{id} <=> $b->{id}} @{$tmpl->{hexes}}))
    {
	my $dsc = sprintf("%02d - %s", $h->{id}, $h->{type});
	if (exists($h->{band}))
	{
	    while(my ($k, $v) = each(%{$h->{band}}))
	    {
		$dsc = $dsc . " $v $k ";
	    }
	}
	push(@desc, $dsc);
    }
    map { $_ = "<li>$_</li>\n" } @desc;
    $tmpl->{desc} = "<h2>Current</h2><ul>" . join("\n", @desc) . "</ul>\n" . sprintf("Cost %d", $template->cost());

# --------------------
# Attach json encoded template to bigbox
# --------------------

    my $tmpl_js = encode_json($tmpl);
    push(@text, $tmpl_js);

    my $bigbox = @row1[0];
    $bigbox->setAttribute("id", "bb");
    $bigbox->setAttribute("onclick", "bbpost(event)");
    $bigbox->setAttribute("tmpl", $tmpl_js);
    $bigbox->setAttribute("class", "bb");


    
    add_row($table, [
		{c => 'Whirlpool', f => "images/WHI.svg", t => 'WHI'},
		{c => 'Temple', f => "images/TEM.svg", t => 'TEM'},
		{c => 'Peak', f => "images/PEA.svg", t => 'PEA'},
		{c => 'Volcano', f => "images/VOL.svg", t => 'VOL'},
	    ]);
    add_row($table, [
		{c => 'Red Den', f => "images/RED.svg", t => 'RED'},
		{c => 'Green Den', f => "images/GRN.svg", t => 'GRN'},
		{c => 'Grey Den', f => "images/GRY.svg", t => 'GRY'},
		{c => 'Yellow Den', f => "images/YEL.svg", t => 'YEL'},
		{c => 'Black Den', f => "images/BLK.svg", t => 'BLK'},
		{c => 'Swamp', f => "images/SWA.svg", t => 'SWA'},
		{c => 'Ruins', f => "images/RUI.svg", t => 'RUI'},
	    ]);
    add_row($table, [
		{c => '+10 Red', f => "images/RED_add.svg", t => '+RED'},
		{c => '+10 Green', f => "images/GRN_add.svg", t => '+GRN'},
		{c => '+10 Grey', f => "images/GRY_add.svg", t => '+GRY'},
		{c => '+10 Yellow', f => "images/YEL_add.svg", t => '+YEL'},
		{c => '+10 Black', f => "images/BLK_add.svg", t => '+BLK'},
		{c => 'Fertile', f => "images/FER.svg", t => 'FER'},
		{c => 'Cursed', f => "images/CUR.svg", t => 'CUR'},
	    ]);
    add_row($table, [
		{c => '-10 Red', f => "images/RED_sub.svg", t => '-RED'},
		{c => '-10 Green', f => "images/GRN_sub.svg"},
		{c => '-10 Grey', f => "images/GRY_sub.svg"},
		{c => '-10 Yellow', f => "images/YEL_sub.svg"},
		{c => '-10 Black', f => "images/BLK_sub.svg"},
		{c => 'Home Den', f => "images/HOME.svg", t => 'HME'},
		{c => 'Accept', f => "images/check-mark.svg"},
	    ]);

    add_row($table, [
		{ c => join("\n", @text), class => 'logbox', colspan => 8, id => 'logbox'},
	    ]);
    
    print $cgi->header(-type => 'text/html', -charset=>'utf-8');
    print $doc->toString(1);
    print STDERR "----------------------------------------\n\n";
}


# ----------------------------------------------------------------------
#
# select_layout
#
# ----------------------------------------------------------------------

sub select_layout {
    my ($cgi) = @_;

# --------------------
# Load LAYOUT.JS
# --------------------

    my $layouts = read_js_file("LAYOUT.JS");

    for my $l (@$layouts)
    {
	my $id = $l->{id};
	if (!exists($l->{image}))
	{
	    my $image = sprintf("images/layout-%02d.svg", $id);
	    if (! -f $image)
	    {
		qx/lua layout.lua --input=LAYOUT.JS --layout=$id --output=$image/;
		my $rc = $?;
		die "Cannot create image $image" if ($rc != 0);
	    }
	    $l->{image} = $image;
	}
    }

# -------------------
# Build base xml structure
# --------------------

    my ($doc, $html, $head, $body) = make_xhtml(script => "default.js");

# --------------------
# Stick a nice header up
# --------------------

    my $h1 = $body->addNewChild("", "h1");
    $h1->appendText("Lizards!");

# --------------------
# Create table to hold img and caption
# --------------------

    my $form = $body->addNewChild("", "form");
    $form->setAttribute("method", "POST");
    $form->setAttribute("action", "reload.cgi");
    $form->setAttribute("id", "bigpost");

    my $input = $form->addNewChild("", "input");
    $input->setAttribute('type', 'hidden');
    $input->setAttribute('id', 'hidden');
    $input->setAttribute('name', 'hidden');
    $input->setAttribute('value', 'default');

    my $input = $form->addNewChild("", "input");
    $input->setAttribute('type', 'hidden');
    $input->setAttribute('id', 'session');
    $input->setAttribute('name', 'session');
    $input->setAttribute('value', $$);

    my $table = $form->addNewChild("", "table");

    my $tr_i = $table->addNewChild("", "tr");
    my $tr_c = $table->addNewChild("", "tr");

    my $td_i_l = $tr_i->addNewChild("", "td");
    my $td_i = $tr_i->addNewChild("", "td");
    my $td_i_r = $tr_i->addNewChild("", "td");

    my $img_l = $td_i_l->addNewChild("", "img");
    $img_l->setAttribute("src", "images/1leftarrow.svg");
    $img_l->setAttribute("onclick", "rotate(-1)");
    $td_i->setAttribute("class", "bigbox");

# --------------------
# main image
# --------------------

    my $img = $td_i->addNewChild("", "img");
    $img->setAttribute("class", "bigbox");
    $img->setAttribute("id", "bigbox");

    my @files = <layout*.svg>;

    my $flist = [];
    for my $l (@$layouts)
    {
	push(@$flist, {
	    path => $l->{image},
	    desc => sprintf("Cost: %d", $l->{cost}),
	    id => $l->{id},
	     });
    }
    $img->setAttribute("images", encode_json($flist));
    $img->setAttribute("onclick", "postie()");

# --------------------
# right arrow
# --------------------

    my $img_r = $td_i_r->addNewChild("", "img");
    $img_r->setAttribute("src", "images/1rightarrow.svg");
    $img_r->setAttribute("onclick", "rotate(1)");

    $tr_c->addNewChild("", "td");
    my $td_c = $tr_c->addNewChild("", "td");
    $td_c->setAttribute("id", "caption");
    $td_c->appendText("Caption competiton");
    $tr_c->addNewChild("", "td");
    print $cgi->header(-type => 'text/html', -charset=>'utf-8');
    print $doc->toString(1);
}

# ----------------------------------------------------------------------
#
# main
#
# ----------------------------------------------------------------------

sub main {
    my $cgi = new CGI;

    if ($ENV{REQUEST_METHOD} eq "POST")
    {
	handle_post($cgi);
    }
    else
    {
	select_layout($cgi);
    }
}

&main();
