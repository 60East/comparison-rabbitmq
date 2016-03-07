#!/bin/bash
PUBS=$1
SUBS=$2
COUNT=$3
SIZE=$4
PUB_COUNT=$(($COUNT / $PUBS))
SUB_COUNT=$(($COUNT / $SUBS))
CONN_PARAMS="localhost"

PUB_PARAMS="$CONN_PARAMS $PUB_COUNT $SIZE"
SUB_PARAMS="$CONN_PARAMS $SUB_COUNT"


for i in $(seq 1 $PUBS)
do
  ./amqp_publisher $PUB_PARAMS &
done

for i in $(seq 1 $SUBS)
do
  ./amqp_subscriber $SUB_PARAMS &
done

sleep 5
echo "starting ${PUBS}p/${SUBS}s $COUNT messages $SIZE bytes"
pgrep amqp_ | xargs kill -USR1
wait
