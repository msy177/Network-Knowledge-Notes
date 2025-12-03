/*


/proc/sys/net/ipv4/ 这个目录下存放着一堆tcp udp ip 的参数
tcp_max_syn_backlog文件里面是半连接队列的大小


tcp三次握手的时候会维护两个队列 半连接队列SYN和Accept全连接队列
服务端收到客户端的SYN请求，内核把该连接放入半连接队列SYN，并且向客户端响应SYN+ACK，接着客户端返回ACK，服务端收到
内核就把连接从半连接队列SYN移除，创建新的连接，添加到全连接队列Accept中，等待进程调用accept函数把连接取出来


这两个队列都有长度限制，超过限制，内核会丢弃或者返回RST包


ss命令可以查看两个队列的长度
说下ss命令，ss是Linux系统替代netstat的高性能网络连接统计工具，可以查看系统的TCP/UDP/UNIX等各类Socket连接状态
ss 默认显示所有已建立的TCP连接 类似于netstat -t
ss -t 仅显示TCP连接
ss -u 仅显示UDP连接
ss -x 仅显示UNIX域套接字 类似于netstat -x
ss -w 仅显示RAW套接字 如ping 网络工具 
ss -l 仅显示监听状态的连接 LISTEN 类似于netstat -l
ss -a 显示所有状态的连接 LISTEN ESTABLISHED TIME_WAIT 类似于netstat -a
ss -n 不解析域名/服务名 显示原始ip+端口 避免DNS域名解析的延迟
ss -p 显示连接对应的进程/程序名 需要root权限 类似于netstat -p
ss -e 显示额外的TCP连接信息 如UID 超时时间 重传次数
ss -m 显示socket的内存使用情况 接收/发送缓冲区大小
ss -o 显示TCP连接的计时器信息 如TIME_WAIT剩余时间，重传超时等
ss -r 解析IP为域名 与-n相反
ss -4/6 ipv4/ipv6
ss -K 强制关闭指定socket连接


ss命令的表头
msy@msy-virtual-machine:/proc/sys/net/ipv4$ ss
Netid      State        Recv-Q       Send-Q         Local Address:Port           Peer Address:Port       Process      
u_str      ESTAB        0            0                     * 19035                     * 20446                   
State(连接状态 LISTEN ESTABLISHED TIME_WAIT SYN_SENT SYN_RECV CLOSE_WAIT FIN_WAIT1/FIN_WAIT2)
Recv-Q 接收队列（接收缓冲区中未被应用层读取的字节数）如果持续非0，说明应用层处理速度慢
当是LISTEN监听状态的时候，显示的是   半连接队列的长度// 这句话错了   应该是全连接队列的当前就绪数 也就是还没有被accept上来的连接
Send-Q 发送队列（发送缓冲区中未被对方确认的字节数）如果持续非0，说明网络拥堵或者对方接收慢
当时LISTEN监听状态的时候，显示的是全连接队列的最大容量 如128
Local Address:Port 本地端地址：端口 0.0.0.0:22 监听所有IPV4地址的22端口（ssh服务）[::]:443 监听所有IPV6的443端口
Peer Address::Port 对端的ip地址以及端口


UDP没有state字段 Recv-Q和Send-Q就只是字节数了


如果全连接队列满了，不去accept接收 后续的就不行了
msy@msy-virtual-machine:/proc/sys/net/ipv4$ ss -ta
State              Recv-Q             Send-Q                         Local Address:Port                            Peer Address:Port             Process             
LISTEN             0                  4096                           127.0.0.53%lo:domain                               0.0.0.0:*                                    
LISTEN             0                  128                                127.0.0.1:6010                                 0.0.0.0:*                                    
LISTEN             0                  1024                               127.0.0.1:36765                                0.0.0.0:*                                    
LISTEN             0                  128                                127.0.0.1:ipp                                  0.0.0.0:*                                    
LISTEN             0                  128                                  0.0.0.0:ssh                                  0.0.0.0:*    
像这里的ssh服务就是最多128个，前提是没有accept接收，当然，一般我们的sshd服务都给我们accept接收上来了


看内核模块的源码 Linux2.6.32 net/ipv4/tcp_diag.c里面  
static void tcp_diag_get_info(struct sock *sk, struct inet_diag_msg *r,
			      void *_info)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	struct tcp_info *info = _info;

	if (sk->sk_state == TCP_LISTEN) {
		r->idiag_rqueue = sk->sk_ack_backlog;
		r->idiag_wqueue = sk->sk_max_ack_backlog;
	} else {
		r->idiag_rqueue = tp->rcv_nxt - tp->copied_seq;
		r->idiag_wqueue = tp->write_seq - tp->snd_una;
	}
	if (info != NULL)
		tcp_get_info(sk, info);
}
所以不同的state状态 Recv-Q和Send-Q表现出来的参数不一样


*/

/*
下面我们来模拟一下全连接队列满的情况：
我们写一个服务端然后再使用wrk工具来模拟客户端进行频繁连接测试
这里写错了，不能用wrk工具测试 wrk是针对http服务的，这里我们的服务端只是tcp服务
*/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>

int main(){

    // std::string ip = INADDR_ANY;//这里千万要注意 不能这样写，否则会ternerl错误 因为这是宏定义0 相当于NULL 赋给string类型
    std::string ip = "0.0.0.0";
    uint16_t port = 9999;
    
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if( sockfd == -1 ){
        perror("socket fail!");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    int n = bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if( n == -1 ){
        perror("bind fail!");
        return -1;
    }

    n = listen(sockfd,5000);//这里第三个参数是全连接队列的总容量 如果=0的话 三次握手可以成功建立连接，但是无法从里面accept取出连接
    if( n ==-1 ){
        perror("listen fail!");
        return -1;
    }
    sleep(15);
    while(1){
        socklen_t addrlen;
        struct sockaddr_in peeraddr;
        memset(&peeraddr,0,sizeof(sockaddr_in));
        int fd = accept(sockfd,(struct sockaddr*)&peeraddr,&addrlen);
        if(fd == -1){
            perror("accept a connetcion fail! \n");
        }
        else{
            std::cout<<"accept a connection success! fd is :"<<fd<<std::endl;
        }
        sleep(1);
    }
    

    //经过上面代码测试，发现ss 得到的Recv-Q 和 Send-Q 在ESTABLISH状态下分别是接收缓冲区数据大小和发送缓冲区数据大小
    //比如我这个代码跑起来，我使用telnet进行测试，然后一直发送数据，但是我这个服务端因为没有accept,所以Recv-Q一直在变大，因为缓冲区没有被处理
    //然后在LISTEN状态下是全连接队列的当前大小和全连接队列的总容量，我没有accept 使用一个telnet 所以Recv-Q就等于1，然后Send-Q是0
    //这里是因为我代码的Listen函数第三个参数是0，所以我修改成了128
    //LISTEN       0(当前全连接队列大小)            128(全连接队列总容量，超出无法accept成功，即使tcp连接成功)      0.0.0.0:9999                      0.0.0.0:* 
    //使用telent 后变成1(当前全连接队列大小) 但是无法accept上来，telnet发数据也没用


    //这些问题处理完就开始测试全连接队列满的情况了
    //使用nc工具进程测试 ，并发200个请求，然后发现  当前全连接队列大小保持在129了，不涨了，因为内核允许超过最大容量的1个或者几个连接
    //LISTEN     129    128           0.0.0.0:9999         0.0.0.0:* 
    //然后后续的连接都连接不上了，比如telnet就是一直trying 中了


    //然后出现了新的问题，我的nc工具已经关闭了，但是此时还是129
    //这是因为虽然nc关闭， 操作系统发送FIN给服务器，进行四次挥手断开连接，但是这些连接还在全连接队列
    //应用层没有accept上来，也就无法响应客户端发来的FIN包了，就无法关闭连接了
    //只有两种情况可以让Recv-Q下降：服务器调用accept捞上来，或者服务器进程退出，内核清理所有资源，队列清空
    

    //然后我发现ss 查看 我的服务器方一堆close_wait状态，这是因为我ctrl+c退出nc，nc发起FIN，然后我的服务器就收到然后进入close_wait状态，
    //但是我的代码没有close指令，所以就不会下次挥手了，就一直这种状态了


    //还有就是为什么我没有accept捞上来，还有一堆已建立的，是因为当全连接队列满了，内核会偷偷帮我们取上来几个连接


    //ss工具只显示已经accept上来的连接为ESTABLISH，虽然内核已经变成ESTABLISH，但是我们使用工具的时候只能看到已经accept分配文件描述符的连接
    //未 accept() 的连接：属于 LISTEN 状态 socket 的「附属资源」，没有独立的 connfd，内核不会把它作为 “独立连接” 暴露给工具，只会把数量统计在 LISTEN 的 Recv-Q 里；
    //已 accept() 的连接：内核会为它分配独立的 connfd，成为 “应用层可见的独立连接”，ss 才会把它单独显示为 ESTABLISHED 状态行。
    //但是内核帮我们取了一些 就ss看到了一些ESTABLISH了


    //还有一个问题？为什么内核要帮我们取出来一些连接？
    //因为tcp可靠性，防止丢失连接，取上来的连接已经存在进程的fd表了，只是代码没有调用accept，所以不知道这个fd的值
    //通过修改内核参数可以防止内核自动取出来，队列满了 就发送客户端RST包，连接失败 而不是擅自取 服务器也就不会产生close_wait状态了


    //被内核取上来的已经到了进程的fd表，我们可以查看一下
    //先通过pidof 进程名 拿到进程的pid 然后进入/proc/进程pid/fd 查看 TODO ....


    //我们还可以使用netstat -s | grep overflowed拿到因为队列满 丢失的连接

    
    // tcp_abort_on_overflow 可以设置这个内核参数 0表示满了就扔掉ACK，1表示满了就发生RST包给对方
    // 如果设置为0，可以提高连接成功概率，因为丢掉ACK后，就会触发重传，可能下次就成功了，
    //如果你认为长期溢出就可以设置成1 即使的通知对方 这边情况 而不是等重传

}

/*


TCP全连接队列的最大值取决于somaxconn的值和backlog的值的最小值
看源码可知 /net/socket.c
SYSCALL_DEFINE2(listen, int, fd, int, backlog)
{
	struct socket *sock;
	int err, fput_needed;
	int somaxconn;

	sock = sockfd_lookup_light(fd, &err, &fput_needed);
	if (sock) {
		somaxconn = sock_net(sock->sk)->core.sysctl_somaxconn;
		if ((unsigned)backlog > somaxconn)
			backlog = somaxconn; //如果backlog的值大于somaxconn系统参数  就改变一下 somaxconn是内核参数 一般默认128

		err = security_socket_listen(sock, backlog);
		if (!err)
			err = sock->ops->listen(sock, backlog);

		fput_light(sock->file, fput_needed);
	}
	return err;
}
所以就是说 我们是取两者最小值，所以你的backlog参数即使再大，如果超过了内核参数somaxconn也就无效了
可以通过指令查看我们当前的内核参数
msy@msy-virtual-machine:~/Network-Knowledge-Notes$ cat /proc/sys/net/core/somaxconn 
4096
msy@msy-virtual-machine:~/Network-Knowledge-Notes$ 
这里看到是4096 所以我的backlog设置5000 那么ss查看到的全连接队列长度 还是4096是不变的
也可以使用sysctl命令查看（sysctl是Linux用来管理内核参数的工具）
@msy-virtual-machine:~/Network-Knowledge-Notes$ sysctl net.core.somaxconn
net.core.somaxconn = 4096
msy@msy-virtual-machine:~/Network-Knowledge-Notes$ 
可以通过echo 5000 > /proc/sys/net/core/somaxconn 来改变内核参数


在万级高并发场景下，如果全连接队列长度仅为128 那么可能会有一堆连接失败
可以通过调大backlog，和调整内核参数的方式来解决，调到5000 一般就可以解决万级高并发了


*/



/*


全连接队列说了，下面我们来看半连接队列
半连接队列不能通过ss查看，但是我们可以通过查看SYN_RECV状态的连接 来拿到TCP半连接队列，因为进入SYN_RECV状态就是进入了半连接队列
msy@msy-virtual-machine:~/Network-Knowledge-Notes$ sudo netstat -natp | grep SYN_RECV | wc -l
0
msy@msy-virtual-machine:~/Network-Knowledge-Notes$ 
目前是0

我们可以通过模拟 出来半连接队列溢出的场景

比如客户端只发送SYN包，不发送ACK回应包，服务端就会有大量的SYN_RECV状态的半连接
这就是所谓的SYN攻击，DDos攻击

客户端使用hping3 模拟大量发送SYN包 不回应
sudo hping3 -S -p 9999 --flood 127.0.0.1

然后使用
msy@msy-virtual-machine:~/Network-Knowledge-Notes$ sudo netstat -natp | grep SYN_RECV | wc -l
2047
msy@msy-virtual-machine:~/Network-Knowledge-Notes$ sudo netstat -natp | grep SYN_RECV | wc -l
2047
查看SYN_RECV的数量大小

注意：客户端跟服务端同一台主机好像不行，必须两台不同的主机，因为可能tcp连接优化了‘

TODO.....


可以使用syncookie防范
cat /proc/sys/net/ipv4/tcp_syncookies
0 关闭
1 当SYN半连接队列放不下 启用
2 表示无条件启用
这样就可以在不使用SYN半连接队列情况下 成功建立连接
收到客户端SYN后，计算一个值，放在SYN+ACK包中，等待客户端返回ACK报文时，取出值验证，合法直接建立连接成功，中间不进入半连接队列
也就是给客户端一个值，你给我返回响应我就知道你是谁了，就直接建立连接，而不是先放入半连接队列等待你的ACK
这样可以防范攻击，满了也就不会丢弃新连接了 而且攻击方没有ACK 所以不会建立连接

几种方法 增大半连接队列长度 开启syncookies 减少SYN+ACK重传次数
但是增大半连接队列长度 不能单纯修改内核参数，还要增大somaxconn和backlog才行 否则无效单纯增大tcp_max_syn_backlog
减少SYN+ACK重传次数是因为处于SYN+RECV后对方不响应就会尝试重发SYN+ACK,达到上限就断开连接
所以修改次数为1次，这样快速的断开无效攻击连接
可以通过echo 1 > /proc/sys/net/ipv4/tcp_synack_retries来设置
msy@msy-virtual-machine:~$ cat /proc/sys/net/ipv4/tcp_synack_retries 
5
msy@msy-virtual-machine:~$ 
我的默认是5次

读者问：“syncookies 启用后就不需要半链接了？那请求的数据会存在哪里？”
syncookies = 1 时，半连接队列满后，后续的请求就不会存放到半连接队列了，
而是在第二次握手的时候，服务端会计算一个 cookie 值，放入到 SYN +ACK 包中的序列号发给客户端，客户端收到后并回 ack ，
服务端就会校验连接是否合法，合法就直接把连接放入到全连接队列。


还有一种办法处理SYN攻击，调大 netdev_max_backlog
当网卡接收数据包的速度大于内核处理速度，就有一个队列保存这些数据包 默认1000 可以调成10000



早期的linux内核 backlog是SYN队列 半连接队列大小
后面内核2.2后就变成全连接队列大小了
*/

