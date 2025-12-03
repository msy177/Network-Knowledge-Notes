#! /bin/bash
TARGET_IP="127.0.0.1"
TARGET_PORT="9999"
CONCURRENT=100 # 并发连接数

for((i=1;i<=$CONCURRENT;i++));do
    echo "Client $i :Hello" | nc -v -w 2 -q 60 $TARGET_IP $TARGET_PORT &
    if((i%10==0));then
        sleep 0.1
    fi
done
wait
echo "所有并发连接已完成"