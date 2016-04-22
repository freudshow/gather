#!/bin/sh
echo "======start gather app======"
cd /app
./gather_V0
while [ $? == 1 ]; do
    echo "======gather app aborted, re-run again======"
    sleep 3
    cd /app
	./gather_V0
done
