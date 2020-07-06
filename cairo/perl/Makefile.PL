use ExtUtils::MakeMaker;

$CC = 'c++';
$LIBS =`pkg-config --libs cairomm-1.0 librsvg-2.0 json-c`;

WriteMakefile(
	'NAME' => 'lizards',
	'XSOPT' => '-C++',
	'OBJECT' => 'lizards_wrap.o ../liblizards.a',
	'CC'	=> $CC,
	'LD'	=> '$(CC)',
	'CCFLAGS' => "`pkg-config --cflags cairomm-1.0 librsvg-2.0 json-c` -I..",
	'LIBS' => ["$LIBS"],
	'depend' => { 'lizards_wrap.cxx' => '../lizards.swg' },
);
