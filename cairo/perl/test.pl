#!/usr/bin/perl -Iblib/arch/auto/lizards

use 5.24.0;
use lizards;

my $layouts = lizards::layouts_t->new($lizards::FORMAT_FILE_JS, "../LAYOUT.JS");
my $template = lizards::template_t->new($layouts, $lizards::FORMAT_FILE_JS, "../CUST1.JS");

my $grid = $template->realize();
my $output = $grid->output();
$output->svg('test.svg', 0);

$template->setactive('04');
$template->save('test.js');
