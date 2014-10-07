#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/delay.h>
#include <net/sock.h>

#include "ksocket.h"
#include "klog.h"

#define __SUBCOMPONENT__ "ksocket"
#define __LOGNAME__ "cd_ksocket.log"
int ksock_create(struct socket **sockp,
__u32 local_ip, int local_port)
{
struct sockaddr_in localaddr;
struct socket	*sock = NULL;
int error;
int option;

mm_segment_t	oldmm = get_fs();

error = sock_create(PF_INET, SOCK_STREAM, 0, &sock);

if (error) {
klog(KL_ERR, "sock_create err=%d", error);
goto out;	
}
klog(KL_DBG, "socket=%p", sock);
set_fs(KERNEL_DS);
option = 1;
error = sock_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
(char *)&option, sizeof(option));
set_fs(oldmm);
if (error) {
klog(KL_ERR, "sock_setsockopt err=%d", error);
goto out_sock_release;
}
if (local_ip != 0 || local_port != 0) {
memset(&localaddr, 0, sizeof(localaddr));
localaddr.sin_family = AF_INET;
localaddr.sin_port = htons(local_port);
localaddr.sin_addr.s_addr = (local_ip == 0) ?
INADDR_ANY : htonl(local_ip);
error = sock->ops->bind(sock, (struct sockaddr *)&localaddr,
sizeof(localaddr));
if (error == -EADDRINUSE) {
klog(KL_ERR, "port %d already in use", local_port);
goto out_sock_release;
}
if (error) {
klog(KL_ERR, "bind to port=%d err=%d", local_port,
error);
goto out_sock_release;
}
}
*sockp = sock;
return 0;
out_sock_release:
sock_release(sock);
out:
return error;
}
int ksock_set_sendbufsize(struct socket *sock, int size)
{
int option = size;
int error;
mm_segment_t oldmm = get_fs();
set_fs(KERNEL_DS);
error = sock_setsockopt(sock, SOL_SOCKET, SO_SNDBUF,
(char *)&option, sizeof(option));
set_fs(oldmm);
if (error) {
klog(KL_ERR, "cant set send buf size=%d for sock=%p",
size, sock);
}
return error;
}
int ksock_set_rcvbufsize(struct socket *sock, int size)
{
int option = size;
int error;
mm_segment_t oldmm = get_fs();
set_fs(KERNEL_DS);
error = sock_setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
(char *)&option, sizeof(option));
set_fs(oldmm);
if (error) {
klog(KL_ERR, "cant set rcv buf size=%d for sock=%p",
size, sock);
}
return error;
}
int ksock_connect(struct socket **sockp, __u32 local_ip, int local_port,
__u32 peer_ip, int peer_port)
{
struct sockaddr_in srvaddr;
int error;
struct socket *sock = NULL;
error = ksock_create(&sock, local_ip, local_port);
if (error) {
klog(KL_ERR, "sock create failed with err=%d", error);
goto out;
}
memset(&srvaddr, 0, sizeof(srvaddr));
srvaddr.sin_family = AF_INET;
srvaddr.sin_port = htons(peer_port);
srvaddr.sin_addr.s_addr = htonl(peer_ip);
error = sock->ops->connect(sock, (struct sockaddr *)&srvaddr,
sizeof(srvaddr), 0);
if (error) {
klog(KL_ERR, "connect failed with err=%d", error);
goto out_sock_release;
}
*sockp = sock;
return 0;
out_sock_release:
sock_release(sock);
out:
return error;
}
void ksock_release(struct socket *sock)
{
sock_release(sock);
}
int ksock_write_timeout(struct socket *sock, void *buffer, int nob, int timeout, int *pwrote)
{
int error;
long ticks = timeout*HZ;
unsigned long then;
struct timeval tv;
int wrote = 0;
mm_segment_t oldmm = get_fs();
BUG_ON(nob <= 0);
for (;;) {
struct iovec iov = {
.iov_base = buffer,
.iov_len = nob
};
struct msghdr msg = {
.msg_name = NULL,
.msg_namelen = 0,
.msg_iov = &iov,
.msg_iovlen = 1,
.msg_control = NULL,
.msg_controllen = 0,
.msg_flags = (ticks == 0) ? MSG_DONTWAIT : 0
};
if (ticks != 0) {
tv = (struct timeval) {
.tv_sec = ticks/HZ,
.tv_usec = ((ticks % HZ) * 1000000)/HZ
};
set_fs(KERNEL_DS);
error = sock_setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
(char *)&tv, sizeof(tv));
set_fs(oldmm);
if (error) {
klog(KL_ERR, "cant set sock timeout, err=%d",
error);
goto out;
}
}
then = jiffies;
set_fs(KERNEL_DS);
error = sock_sendmsg(sock, &msg, iov.iov_len);
set_fs(oldmm);
ticks-= jiffies - then;
if (error < 0) {
klog(KL_ERR, "send err=%d", error);
goto out;
}
if (error == 0) {
klog(KL_ERR, "send returned zero size");
error = -ECONNABORTED;
goto out;
}
if (error > 0)
wrote+= error;
buffer = (void *)((unsigned long)buffer + error);
nob-= error;
if (nob == 0) {
error = 0;
goto out;
}
if (ticks <= 0) {
klog(KL_ERR, "timeout reached");
error = -EAGAIN;
goto out;
}
}
out:
if (pwrote)
*pwrote = wrote;
return error;
}
int ksock_read_timeout(struct socket *sock, void *buffer, int nob, int timeout, int *pread)
{
int error;
long ticks = timeout*HZ;
unsigned long then;
struct timeval tv;
int read = 0;
mm_segment_t oldmm = get_fs();
BUG_ON(nob <= 0);
BUG_ON(ticks <= 0);
for (;;) {
struct iovec iov = {
.iov_base = buffer,
.iov_len = nob
};
struct msghdr msg = {
.msg_name = NULL,
.msg_namelen = 0,
.msg_iov = &iov,
.msg_iovlen = 1,
.msg_control = NULL,
.msg_controllen = 0,
.msg_flags = 0
};
tv = (struct timeval) {
.tv_sec = ticks/HZ,
.tv_usec = ((ticks % HZ) * 1000000)/HZ
};
set_fs(KERNEL_DS);
error = sock_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
(char *)&tv, sizeof(tv));
set_fs(oldmm);
if (error) {
klog(KL_ERR, "cant set sock timeout, err=%d",
error);
goto out;
}
then = jiffies;
set_fs(KERNEL_DS);
error = sock_recvmsg(sock, &msg, iov.iov_len, 0);
set_fs(oldmm);
ticks-= jiffies - then;
if (error < 0) {
klog(KL_ERR, "recv err=%d", error);
goto out;
}
if (error == 0) {
klog(KL_ERR, "recv returned zero size");
error = -ECONNRESET;
goto out;
}
if (error > 0)
read+= error;
buffer = (void *)((unsigned long)buffer + error);
nob-= error;
if (nob == 0) {
error = 0;
goto out;
}
if (ticks <= 0) {
klog(KL_ERR, "timeout reached");
error = -ETIMEDOUT;
goto out;
}
}
out:
if (pread)
*pread = read;
return error;
}
int ksock_listen(struct socket **sockp, __u32 local_ip, int local_port, int backlog)
{
int error;
struct socket *sock = NULL;
error = ksock_create(&sock, local_ip, local_port);
if (error) {
klog(KL_ERR, "csock_create err=%d", error);
return error;
}
klog(KL_DBG, "sock=%p, sock->ops=%p", sock, sock->ops);
error = sock->ops->listen(sock, backlog);
if (error) {
klog(KL_ERR, "listen failed err=%d", error);
goto out;
}
*sockp = sock;
return 0;
out:
sock_release(sock);
return error;
}
int ksock_accept(struct socket **newsockp, struct socket *sock)
{
wait_queue_t wait;
struct socket *newsock;
int error;
init_waitqueue_entry(&wait, current);
error = sock_create_lite(PF_PACKET, sock->type, IPPROTO_TCP, &newsock);
if (error) {
klog(KL_ERR, "sock_create_lite err=%d", error);
return error;
}
newsock->ops = sock->ops;
set_current_state(TASK_INTERRUPTIBLE);
add_wait_queue(sk_sleep(sock->sk), &wait);
error = sock->ops->accept(sock, newsock, O_NONBLOCK);
if (error == -EAGAIN) {
klog(KL_WRN, "accept returned %d", error);
schedule();
error = sock->ops->accept(sock, newsock, O_NONBLOCK);
}
remove_wait_queue(sk_sleep(sock->sk), &wait);
set_current_state(TASK_RUNNING);
if (error) {
if (error == -EAGAIN)
klog(KL_WRN, "accept error=%d", error);
else
klog(KL_ERR, "accept error=%d", error);
goto out;
}
*newsockp = newsock;
return 0;
out:
sock_release(newsock);
return error;
}
void ksock_abort_accept(struct socket *sock)
{
wake_up_all(sk_sleep(sock->sk));
}

  
