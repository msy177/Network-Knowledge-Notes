/*

如何优化TCP？？

先看下 tcp一堆内核参数
[root@kk-g76w8pjcs0br925dng6z ~]# ls -l /proc/sys/net/ipv4/tcp*
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_abort_on_overflow
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_adv_win_scale
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_allowed_congestion_control
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_app_win
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_autocorking
-r--r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_available_congestion_control
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_base_mss
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_challenge_ack_limit
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_congestion_control
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_dsack
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_early_retrans
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_ecn
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_fack
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_fastopen
-rw-------. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_fastopen_key
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_fin_timeout
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_frto
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_invalid_ratelimit
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_keepalive_intvl
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_keepalive_probes
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_keepalive_time
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_limit_output_bytes
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_low_latency
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_max_orphans
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_max_ssthresh
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_max_syn_backlog
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_max_tw_buckets
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_mem
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_min_tso_segs
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_moderate_rcvbuf
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_mtu_probing
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_no_metrics_save
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_notsent_lowat
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_orphan_retries
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_reordering
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_retrans_collapse
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_retries1
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_retries2
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_rfc1337
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_rmem
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_sack
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_slow_start_after_idle
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_stdurg
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_synack_retries
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_syncookies
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_syn_retries
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_thin_dupack
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_thin_linear_timeouts
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_timestamps
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_tso_win_divisor
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_tw_recycle
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_tw_reuse
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_window_scaling
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_wmem
-rw-r--r--. 1 root root 0 Dec  3 17:05 /proc/sys/net/ipv4/tcp_workaround_signed_windows

如何正确的使用这些参数来优化TCP呢？？

首先TCP要三次握手，那就先从三次握手开始优化
如果三次握手在HTTP请求中时间占比10%以上，网络拥堵，高并发性能就会差很多
客户端优化：客户端主要需要同步序列号，只有同步了序列号才能有效传输，很多特性的底层支持 比如流量控制 窗口等
客户端需要发送SYN包，等待服务端的SYN+ACK，如果长时间没有收到，就会重传SYN包，那么我们就可以
1.修改tcp_syn_retries参数，默认是5次，让重传快点
  5次的话大概就是第一次1秒，第二次2秒，第三次4秒，第四次，8秒，第五次16秒，第五次后会再等32秒，加起来一分钟左右终止三次握手
  可以适当降低次数，快速的中断连接，而不是一直保持
服务端优化：服务端需要回复SYN+ACK 表示收到了客户端的序列号，把自己的也发过去
然后服务端进入半连接状态 SYN_RECV
我们可以调整半连接队列大小优化


如何绕过三次握手呢？
三次握手在HTTP的缺陷就是，每次的请求都需要经过一个RTT(从客户端到服务器的一个往返时间)后才能正常数据通信
第三次握手才能开始传递数据
不过在Linux3.7内核版本，提供了TCP_FAST_OPEN功能
1. 客户端发送 SYN 报文，该报文包含「数据」(对于非 TFO 的普通 TCP 握手过程，SYN 报文中不包含
「数据」) 以及此前记录的 Cookie;
2. 支持 TCP Fast Open 的服务器会对收到 Cookie 进行校验：如果 (Cookie 有效，服务器将在 SYN-ACK 报
文中对 SYN 和「数据」进行确认，服务器随后将「数据」递进送至相应的应用程序；如果 Cookie 无效，
服务器将丢弃 SYN 报文中包含的「数据」, 且其随后发出的 S\YN-ACK 报文将只确认 SYN 的对应序列
号；
3. 如果服务器接受了 SYN 报文中的「数据」, 服务器可在握手完成之前发送「数据」, 这就减少了握手带
来的 1 个 RTT 的时间消耗；
4. 客户端将发送 ACK 确认服务器发回的 SYN 以及「数据」, 但如果客户端在初始的 SYN 报文中发送的
「数据」没有被确认，则客户端将重新发送「数据」;
5. 此后的 TCP 连接的数据传输过程和非 TFO 的正常情况一致。
所以，之后发起 HTTPGET 请求的时候，可以绕过三次握手，这就减少了握手带来的 1 个 RTT 的时间消
耗。

cookie是在TCP报头里的选项字段里面

相当于第一次还是正常的，后续客户端跟服务端的连接 都在第一次握手和第二次握手就已经传递数据了，降低了一次RTT

通过echo 3 > /proc/sys/net/ipv4/tcp_fastopen可以开启这个功能 0关闭 1 作为客户端才使用 2 服务端才使用 3 都使用


优化三次握手的策略
策略
TCP 内核参数
调整 SYN 报文的重传次数
tcp_syn_retries
调整 SYN 半连接队列长度
tcp_max_syn_backlog、somaxconn、backlog
调整 SYN+ACK 报文的重传次数
tcp_synack_retries
调整 accpet 队列长度
min (backlog, somaxconn)
绕过三次握手
tcp_fastopen


接下来就是四次挥手
如何优化四次挥手呢？

关闭连接的方式：RST报文和FIN报文
RST直接不走挥手流程，暴力关闭连接
安全关闭必须通过close和shutdown函数发起FIN报文
close: 完全断开连接，不能发送任何数据，使用close关闭的连接是不优雅的，于是就有了shutdown
shutdown：可以控制只关闭一个方向的连接 int shutdown(int sock,int howto); 
            第二个参数 SHUT_RD(0) 关闭连接的读方向，接收缓冲区有的话直接丢弃，后续接收的数据 都会ACK回复，但是数据会悄悄丢弃
            对端能正常发也能收到ACK，但是数据丢弃了已经
            SHUT_WR(1) 关闭写方向，发送缓冲区里未发送的会立即发送并且发送FIN报文给对端
            SHUT_RDWR(2) 相当于上面两个各自操作一遍，关闭套接字的读和写两个方向

主动方发送FIN后进入FIN_WAIT1状态，如果对方一直不回复ACK就一直陷入FIN_WAIT状态，然后重传 由内核参数决定
如果FIN_WAIT1状态的连接过多就需要降低重传次数了，快速断开连接

然后就是FIN_WAIT2状态了，收到对方的ACK后变成FIN_WAIT2状态，如果是close关闭的 不会一直处于FIN_WAIT2状态，超时就直接关闭连接了
这个超时时间跟TIMEMSL 60秒一样
如果是shutdown关闭的就可以一直保持FIN_WAIT2状态，等待对方的关闭请求FIN
我们可以调整一下TODO....


TIME_WAIT优化
修改内核参数 超过一定数量的TIME_WAIT连接 下次的TIME_WAIT直接关闭 不经过TIME_WAIT
或者调整这个最大数量


*/