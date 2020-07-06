#!/bin/sh -x

if [ $# -lt 3 ]; then
    echo "Not enough paramters"
    echo "[data dir] [game code] [turn]"
    exit 1
fi

DATA=$1
GAME=$2
TURN=$3

PATH=$PATH:.

LOG="turn.log"

if [ -f $LOG ]; then
    rm -f $LOG
fi
touch $LOG

touch $DATA/LORDS$TURN.$GAME
rm -f $DATA/$GAME.LP*_$TURN

echo $DATA $GAME $TURN

tordbord $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

terrmast $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

bordshuffle $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

worldopen $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

worldmast $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

worldshut $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

clanreport $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

clanrank $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

messapnd $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

psreport.pl $DATA $GAME $TURN >> $LOG
if [ $? != 0 ]; then exit 1; fi
