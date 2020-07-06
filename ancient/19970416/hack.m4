changequote(<,>)
define(<Header>,dnl
</Gameletter {($1)} def
/Turnnumber {($2)} def
/NTN {(eval($2 + 1))} def
/Date {($6)} def
/saveobj save def
/Shadow_Code {($4)} def
PageOne
($3) Player_Number
($5) Clan_Full_Name
Write
[
>)

define(<Phase>, <ifelse($1, <World>, <() Linef Startf (WORLD PHASE (all Den Growth and Settling).) Endf>, <>)
]>)

define(<ClanID>, <>)dnl
