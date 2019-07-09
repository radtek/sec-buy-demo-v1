#!/bin/sh
sudo sysctl -w net.ipv4.tcp_max_syn_backlog=65535
sudo sysctl -w net.core.somaxconn=65535
sudo sysctl -w fs.file-max=10485760
sudo sysctl -w net.ipv4.ip_local_port_range='1024 65535'
sudo sysctl -w net.ipv4.tcp_tw_reuse=1
sudo sysctl -w net.ipv4.tcp_timestamps=1
ulimit -n 1048576
#echo '* soft nofile 1048576' >> /etc/security/limits.conf
#echo '* hard nofile 1048576' >> /etc/security/limits.conf
#sysctl -w net.ipv4.tcp_rmem=1024
#sysctl -w net.ipv4.tcp_wmem=1024
