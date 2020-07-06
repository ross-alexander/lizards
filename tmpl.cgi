#!/usr/bin/perl

use XML::LibXML;
use CGI;

sub SVG {
    my $parser = XML::LibXML->new();
    my $doc = $parser->parse_file("/home/ralexand/source/java/image.svg");
    return $doc->getDocumentElement()->cloneNode(1);
}

sub HTML {
    my $doc = XML::LibXML::Document->new("1.0", "utf8");
    my $html = $doc->createElement("html");
    $html->setAttribute("xmlns", "http://www.w3.org/1999/xhtml");
    $doc->setDocumentElement($html);

    my $head = $html->appendChild($doc->createElement("head"));
    my $body = $html->appendChild($doc->createElement("body"));

    $body->appendChild(SVG());

    print $doc->toString(1);
}

my $cgi = new CGI;
print $cgi->header(-type=>"application/xhtml+xml");
&HTML();
exit 0;
