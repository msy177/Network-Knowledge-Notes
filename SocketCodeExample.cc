/*

客户端：
socket-->connect read write close
服务端：
socket-->bind-->listen-->accept read write close

当一方断开连接close后，另一方读取的时候就会读取到EOF，等待数据处理完，服务端就调用close，连接关闭

连接断开流程：
客户端调用close，客户端没有数据需要发送了，发送服务端FIN报文，进入FIN_WAIT1状态
服务端收到FIN报文，TCP协议栈会在FIN包内插入一个文件结束符EOF到接收缓冲区，应用通过read读取可以感知到这个FIN包
这个EOF会被放在已排队等候的其他已接收的数据之后，在EOF后就没有任何数据了，此时服务端进入CLOSE_WAIT状态

一端主动断开，触发EOF，另一端读取到EOF后就close也断开

*/