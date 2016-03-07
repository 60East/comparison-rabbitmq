#!/bin/bash

PUBS=$1
SUBS=$2
COUNT=$3
SIZE=$4
export STORAGE_DIR=./amps-storage
PUB_COUNT=$(($COUNT / $PUBS))
SUB_COUNT=$(($COUNT / $SUBS))
CONN_PARAMS="tcp://guest@localhost:9007/amps queue"

PUB_PARAMS="$CONN_PARAMS $PUB_COUNT $SIZE"
SUB_PARAMS="$CONN_PARAMS $SUB_COUNT"
DIR=result-$PUBS-$SUBS-$COUNT-$SIZE
mkdir -p $DIR
killall -9 ampServer >/dev/null
rm -f amps.log
sleep 5
rm -rf $STORAGE_DIR/*
(${AMPS_DIR}/bin/ampServer ./config.xml  >/dev/null &)
./wait-for-admin 8085 >/dev/null

for i in $(seq 1 $PUBS)
do
  ./q_publisher $PUB_PARAMS >>$DIR/pubs &
done

for i in $(seq 1 $SUBS)
do
  ./q_subscriber $SUB_PARAMS >>$DIR/subs &
done

sleep 5
echo "starting ${PUBS}p/${SUBS}s $COUNT messages $SIZE bytes"
pgrep "^q_.*" | xargs kill -USR1
wait

./q_clean $CONN_PARAMS

cat $DIR/pubs
cat $DIR/subs

#./stats *.out > $DIR/latency
#./sum $DIR/subs >> $DIR/subs
#./sum $DIR/pubs >> $DIR/pubs
mv *.out $DIR/.

killall -9 ampServer q_subscriber q_publisher 2>/dev/null
mv amps.log $DIR/.

