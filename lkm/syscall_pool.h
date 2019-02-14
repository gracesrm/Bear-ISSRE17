#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/syscalls.h>

void **syscall_table;

//----------- File IO -----------------
long (*orig_sys_open)(const char __user *filename, int flags, int mode);
long (*orig_sys_close)(unsigned int fd);
ssize_t (*orig_sys_write)(unsigned int fd, char * buf, size_t count);
ssize_t (*orig_sys_read)(int fd, void *buf, size_t count);
long (*orig_sys_writev)(unsigned long fd, const struct iovec __user *vec, unsigned long vlen);
long (*orig_sys_readv)(unsigned long fd, const struct iovec __user *vec, unsigned long vlen);
off_t (*orig_sys_lseek)(int fd, off_t offset, int whence);
long (*orig_sys_pread64)(unsigned int fd, char __user *buf, size_t count, loff_t pos);
long (*orig_sys_pwrite64)(unsigned int fd, const char __user *buf, size_t count, loff_t pos);
long (*orig_sys_lstat)(const char __user *filename, struct __old_kernel_stat __user *statbuf);
long (*orig_sys_fstat)(unsigned int fd, struct __old_kernel_stat __user *statbuf);
long (*orig_sys_link)(const char __user *oldname, const char __user *newname);
long (*orig_sys_unlink)(const char __user *pathname);
long (*orig_sys_access)(const char __user *filename, int mode);

//------------ Network -----------------
int (*orig_sys_connect)(int sockfd, struct sockaddr *addr, int addrlen);
int (*orig_sys_bind)(int sockfd, struct sockaddr *my_addr, int addrlen);
int (*orig_sys_listen)(int sockfd, int backlog);
//---------------------------------
int (*orig_sys_accept)(int sockfd, struct sockaddr *addr, int addrlen);
ssize_t (*orig_sys_sendto)(int sockfd, void *buf, size_t len, int flags, const struct sockaddr *dest_addr, int addrlen);
ssize_t (*orig_sys_recvfrom)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen);
ssize_t (*orig_sys_sendmsg)(int sockfd, const struct msghdr *msg, int flags);
ssize_t (*orig_sys_recvmsg)(int sockfd, struct msghdr *msg, int flags);
long (*orig_sys_socket)(int, int, int);
long (*orig_sys_sendfile)(int out_fd, int in_fd, off_t __user *offset, size_t count);
long (*orig_sys_shutdown)(int, int);

//------------- Process----------------
long (*orig_sys_getpid)(void);
long (*orig_sys_getppid)(void);
long (*orig_sys_wait4)(pid_t pid, int __user *stat_addr, int options, struct rusage __user *ru);
long (*orig_sys_waitid)(int which, pid_t pid, struct siginfo __user *infop, int options, struct rusage __user *ru);
long (*orig_sys_exit)(int error_code);
long (*orig_sys_exit_group)(int error_code);
//---------------------------
long (*orig_sys_setuid)(uid_t uid);
long (*orig_sys_mq_notify)(mqd_t mqdes, const struct sigevent __user *notification);


//------------- Memory ----------------
long (*orig_sys_brk)(unsigned long brk);
long (*orig_sys_mprotect)(unsigned long start, size_t len, unsigned long prot);
long (*orig_sys_mremap)(unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr);
long (*orig_sys_munmap)(unsigned long addr, size_t len);
long (*orig_sys_mmap)(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, off_t pgoff);

//------------- Signal ----------------
long (*orig_sys_rt_sigaction)(int, const struct sigaction __user *, struct sigaction __user *, size_t);
long (*orig_sys_rt_sigprocmask)(int how, sigset_t __user *set, sigset_t __user *oset, size_t sigsetsize);
long (*orig_sys_rt_sigpending)(sigset_t __user *set, size_t sigsetsize);
long (*orig_sys_kill)(int pid, int sig);

long (*orig_sys_mlock)(unsigned long start, size_t len);
long (*orig_sys_munlock)(unsigned long start, size_t len);
long (*orig_sys_mlockall)(int flags);
long (*orig_sys_munlockall)(void);

//--------------------------------------------------------------
//---------------------------------------------------------------

long my_sys_open(const char __user *filename, int flags, int mode);
long mask_sys_open(const char __user *filename, int flags, int mode);
long my_sys_close(unsigned int fd);
long mask_sys_close(unsigned int fd);
ssize_t my_sys_write(unsigned int fd, char *buf, size_t count);
ssize_t my_sys_read(int fd, void *buf, size_t count);
long my_sys_writev(unsigned long fd, const struct iovec __user *vec, unsigned long vlen);
long my_sys_readv(unsigned long fd, struct iovec __user *vec, unsigned long vlen);
off_t my_sys_lseek(int fd, off_t offset, int whence);
long my_sys_pread64(unsigned int fd, char __user *buf, size_t count, loff_t pos);
long my_sys_pwrite64(unsigned int fd, char __user *buf, size_t count, loff_t pos);
long my_sys_lstat(char __user *filename, struct __old_kernel_stat __user *statbuf);
long my_sys_fstat(unsigned int fd, struct __old_kernel_stat __user *statbuf);
long my_sys_link(char __user *oldname, char __user *newname);
long my_sys_unlink(char __user *pathname);
long my_sys_access(const char __user *filename, int mode);


//------------ Network -----------------
int my_sys_connect(int sockfd, struct sockaddr *addr, int addrlen);
int my_sys_bind(int sockfd, struct sockaddr *my_addr, int addrlen);
int my_sys_listen(int sockfd, int backlog);
//-------------------------
int my_sys_accept(int sockfd, struct sockaddr *addr, int addrlen);
ssize_t my_sys_sendto(int sockfd, void *buf, size_t len, int flags, struct sockaddr *dest_addr, int addrlen);
ssize_t my_sys_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen);
ssize_t my_sys_sendmsg(int sockfd, struct msghdr *msg, int flags);
ssize_t my_sys_recvmsg(int sockfd, struct msghdr *msg, int flags);
long my_sys_socket(int, int, int);
long my_sys_sendfile(int out_fd, int in_fd, off_t __user *offset, size_t count);
long my_sys_shutdown(int, int);

//------------- Process----------------
long my_sys_getpid(void);
long my_sys_getppid(void);
long my_sys_wait4(pid_t pid, int __user *stat_addr, int options, struct rusage __user *ru);
long my_sys_waitid(int which, pid_t pid, struct siginfo __user *infop, int options, struct rusage __user *ru);
long my_sys_exit(int error_code);
long my_sys_exit_group(int error_code);

//-------------------------------
long my_sys_setuid(uid_t uid);
long my_sys_mq_notify(mqd_t mqdes, const struct sigevent __user *notification);

//------------- Memory ----------------
long my_sys_brk(unsigned long brk);
long my_sys_mprotect(unsigned long start, size_t len, unsigned long prot);
long my_sys_mremap(unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr);
long my_sys_munmap(unsigned long addr, size_t len);
long my_sys_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, off_t pgoff);

//------------- Signal ----------------
long my_sys_rt_sigaction(int, struct sigaction __user *, struct sigaction __user *, size_t);
long my_sys_rt_sigprocmask(int how, sigset_t __user *set, sigset_t __user *oset, size_t sigsetsize);
long my_sys_rt_sigpending(sigset_t __user *set, size_t sigsetsize);
long my_sys_kill(int pid, int sig);


long my_sys_mlock(unsigned long start, size_t len);
long my_sys_munlock(unsigned long start, size_t len);
long my_sys_mlockall(int flags);
long my_sys_munlockall(void);
//--------------------------------------------------------------
//---------------------------------------------------------------

void syscall_update(void);
void restore_syscall(void);
void dynamic_syscall_update(const char *callname);
void dynamic_restore_syscall(const char *callname);
