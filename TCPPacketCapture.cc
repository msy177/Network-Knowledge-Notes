/*
TCP的实战抓包

抓包工具：tcpdump wireshark

这俩配合使用比较好

tcpdump仅支持命令行使用，经常Linux服务器中使用
wireshark还额外提供了可视化界面

先用tcpdump抓包，把抓包的文件拖到windows上的wireshark来可视化分析

单纯的windows就直接使用wiresharek就可以了

抓包ping:
ping的数据包是ICMP协议，所以使用tcpdump抓取的时候 只抓icmp协议的数据包就行


使用命令：
tcpdump -i eth0 icmp and host 192.168.100.3 -nn 来抓包监控
然后使用ping -I eth0 -c 3 192.168.100.3 来发送3次icmp包 让tcpdump抓
但是由于我之前测试是本地环回测试：
本地环回测试是不走物理网卡的，所以不能指定这个网卡，让修改，然后还要修改tcpdump抓包网卡为lo也就是本地环回网卡
root@msy-virtual-machine:/home/msy# ip addr
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 00:0c:29:cd:ed:7e brd ff:ff:ff:ff:ff:ff
    altname enp2s1
    inet 192.168.100.3/24 brd 192.168.100.255 scope global noprefixroute ens33
       valid_lft forever preferred_lft forever
    inet6 fe80::373c:6076:46da:7207/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
后面我又搞了一台云服务器，使用我自己的机器ping到云服务器，让云服务器抓包
[root@kk-g76w8pjcs0br925dng6z ~]# tcpdump -i eth0 icmp and host 220.160.33.21 -nn
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on eth0, link-type EN10MB (Ethernet), capture size 262144 bytes
16:38:22.296134 IP 183.198.3.183 > 220.160.33.21: ICMP echo request, id 5823, seq 1, length 64
16:38:22.296151 IP 220.160.33.21 > 183.198.3.183: ICMP echo reply, id 5823, seq 1, length 64
16:38:23.298356 IP 183.198.3.183 > 220.160.33.21: ICMP echo request, id 5823, seq 2, length 64
16:38:23.298372 IP 220.160.33.21 > 183.198.3.183: ICMP echo reply, id 5823, seq 2, length 64
16:38:24.300976 IP 183.198.3.183 > 220.160.33.21: ICMP echo request, id 5823, seq 3, length 64
16:38:24.300991 IP 220.160.33.21 > 183.198.3.183: ICMP echo reply, id 5823, seq 3, length 64
16:40:46.717750 IP 168.138.77.168 > 220.160.33.21: ICMP echo request, id 32641, seq 0, length 14
16:40:46.717767 IP 220.160.33.21 > 168.138.77.168: ICMP echo reply, id 32641, seq 0, length 14
16:41:08.097183 IP 220.160.33.21 > 199.127.61.135: ICMP 220.160.33.21 udp port 62060 unreachable, length 450
这是抓到的内容
我的虚拟机时192.168.100.3，经过NAT转换为公网的183.198.3.193然后和云服务器进行的通信
可以在浏览器搜索公网ip查询自己电脑的公网ip 我电脑的就是183.198.3.193

接下来分析抓包内容，ping一共发了3个icmp包
我们把抓包内容放到windows里的wireshark里面去分析
TODO..................


*/