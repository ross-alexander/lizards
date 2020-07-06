#!/bin/sh -x

if [ $# -lt 2 ]; then
    echo "Not enough paramters"
    echo "[data dir] [game code]"
    exit 1
fi

DATA=$1
GAME=$2

PATH=$PATH:.

LOG="turn.log"

if [ -f $LOG ]; then
    rm -f $LOG
fi
touch $LOG

rm -f $DATA/$GAME.LP*_$TURN

echo $DATA $GAME

tordbord $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

terrmast $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

bordshuffle $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

worldopen $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

worldmast $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

worldshut $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

clanreport $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

clanrank $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

messapnd $DATA $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi
