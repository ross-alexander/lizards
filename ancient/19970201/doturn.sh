#!/bin/sh -x

if [ $# -lt 3 ]; then
    echo "Not enough paramters"
    exit 1
fi

DATA=$1
GAME=$2
TURN=$3

LOG="turn.log"

if [ -f $LOG ]; then
    rm -f $LOG
fi
touch $LOG

PATH=$PATH:.

touch $DATA/LORDS$TURN.$GAME

echo $DATA $GAME $TURN

tordbord $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

terrmast $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

bordshuf $DATA $TURN $GAME >> $LOG
if [ $? != 0 ]; then exit 1; fi

psworldopen $DATA $TURN $GAME >> $LOG
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
