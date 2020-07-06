define(`LizColour', `ifelse($1, 0, `Unseattled',dnl
ifelse($1, 1, `Red',dnl
ifelse($1, 2, `Green',dnl
ifelse($1, 3, `Grey',dnl
ifelse($1, 4, `Black',dnl
ifelse($1, 5, `Yellow'))))))')dnl
define(`Header', `<!doctype html public "-//W3C//HTML DTD 3.2//EN">
<html><head><title>Turn $2 of Game $1</title></head>')
define(`ClanID', `<body>
<h1>You are clan <strong>$2</strong></h1>')
define(`Spawning', `')
define(`Phase',`
define(`world', `<h2><strong>World `Phase'</strong> (all Den Growth and Settling)</h2>')dnl
define(`artillery', `<h2><strong>Artillery ``Phase''</strong> (all Chant, Lob and Free orders)</h2>')dnl
define(`movement', `<h2><strong>Movement ``Phase''</strong> (all Move, Split, Sail and Sneak order)</h2>')dnl
define(`development', `<h2><strong>Development ``Phase''</strong> (all Builds, Destroy, Recruit and Give orders)</h2>')dnl
<hr>$1')
define(`ClanReport',`<hr><h2>Clan Report for Clan $1</h2>')
define(`Den', `ifdef(dtable, `', `define(dtable)<table>')
<tr><td>$1<td><$2></tr>')
define(`HomeDen', `<p>In hex $1 you have a <strong>Home den</strong> with $2 LizColour($3) lizards and Band($4, $5, $6)<br>')
define(`Trailer', `ifdef(dtable, `</table>')</body></html>')
