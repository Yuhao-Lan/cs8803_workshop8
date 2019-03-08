#!/bin/bash
echo "tickTime=2000
dataDir=/var/zookeeper
clientPort=2181" > /etc/zookeeper/conf/zoo.cfg

/etc/init.d/zookeeper stop
/usr/share/zookeeper/bin/zkServer.sh stop
/usr/share/zookeeper/bin/zkServer.sh start
