#include "unpred.h"
#include "syscall_pool.h"

/* Just so we do not taint the kernel */
MODULE_LICENSE("GPL");
// #define CR0_WP 0x00010000   // Write Protect Bit (CR0:16)
// #define LOCAL_ADDR ((unsigned long int)0x0100007F)  //"127.0.0.1" attention on big/little endian

//-------- File IO ---------
static int opencnt;
static int closecnt;
static int readcnt;
static int writecnt;
static int readvcnt;
static int writevcnt;
static int pread64cnt;
static int pwrite64cnt;
static int lseekcnt;
static int lstatcnt;
static int fstatcnt;
static int linkcnt;
static int unlinkcnt;
 //------- Network ----------
static int connectcnt;
static int bindcnt;
static int listencnt;
static int acceptcnt;
static int socketcnt;
static int sendtocnt;
static int recvfromcnt;
static int sendmsgcnt;
static int recvmsgcnt;
static int sendfilecnt;
static int shutdowncnt;
//------- Process -----------
static int getpidcnt;
static int getppidcnt;
static int wait4cnt;
static int waitidcnt;
static int exitcnt;
static int exit_groupcnt;
//------- Memory -------------
static int mmapcnt;
static int munmapcnt;
static int mremapcnt;
static int brkcnt;
static int mprotectcnt;
 //-------- Siganl ------------
static int rt_sigactioncnt;
static int rt_sigprocmaskcnt;
static int rt_sigpendingcnt;
static int killcnt;



unsigned long **find_sys_call_table(void);

unsigned long **find_sys_call_table() {    
    unsigned long ptr;
    unsigned long *p;
    for (ptr = (unsigned long)sys_close;
         ptr < (unsigned long)&loops_per_jiffy;
         ptr += sizeof(void *)) {
             
        p = (unsigned long *)ptr;

        if (p[__NR_close] == (unsigned long)sys_close) {
            printk(KERN_DEBUG "Found the sys_call_table!!!\n");
            return (unsigned long **)p;
        }
    }   
    return NULL;
}

static int stratNo;
static char *TARGET;
static char *callname;
module_param(stratNo, int, 0644);
module_param(TARGET, charp, 0644);
module_param(callname, charp, 0644);

//  StratNo = 0, fail syscall;
long my_sys_open(const char __user *filename, int flags, int mode){
    if(!strcmp(current->comm, TARGET)){
        printk(KERN_INFO "OPEN CALLED with %s\n", filename);
        if((stratNo == 0) && within_threshold() &&(opencnt > 1) &&isnotKeyfile(filename)){
            opencnt++;
            printk(KERN_INFO "[open] failed %s\n", filename);
            return -1;
        }
    }
    return orig_sys_open(filename, flags, mode);
}

long mask_sys_open(const char __user *filename, int flags, int mode){
    long ret;
    if(!strcmp(current->comm, TARGET)){
        ret = orig_sys_open(filename, flags, mode);
        // printk(KERN_INFO "fd %ld opened for %s\n", ret, filename);
        if(ret > 0)
            add_fd(ret, filename);
        return ret;
    }
    return orig_sys_open(filename, flags, mode);
}

//  StratNo = 0, fail syscall;
long my_sys_close(unsigned int fd){
    if(!strcmp(current->comm, TARGET)){
        if((stratNo == 0) && within_threshold() &&isnotKeyfd(fd)){
            closecnt++;
            printk(KERN_INFO "[close] failed on %u\n", fd);
            return -1;
        }
        del_fd(fd);
    }
    return orig_sys_close(fd);
}

long mask_sys_close(unsigned int fd){
    if(!strcmp(current->comm, TARGET)){
        del_fd(fd);
    }
    return orig_sys_close(fd);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
ssize_t my_sys_write(unsigned int fd, char *buf, size_t count){
    int *tmp = 4;
    int rc;
    if(!strcmp(TARGET, current->comm)){
        writecnt++;
        if((stratNo == 8) && within_threshold() &&isnotKeyfd(fd)){
            // writecnt++;
	    rc = reduce_size(count);
            printk(KERN_INFO "[write] reduce len buffer\n");
            return orig_sys_write(fd, buf, rc);
        }
         
	if((stratNo == 5) && within_threshold() &&isnotKeyfd(fd)){
            // writecnt++;
            strcpy(buf, (char*)tmp);
            printk(KERN_INFO "[write] 5: diff type\n");
            return orig_sys_write(fd, buf, count+4);
        } 
	if((stratNo == 6) && within_threshold() &&isnotKeyfd(fd)){
            // writecnt++;
            strncat(buf, "xlmt", 4);
            printk(KERN_INFO "[write] 6: buffer overflow\n");
            return orig_sys_write(fd, buf, count+4);
        }
    }        
    return orig_sys_write(fd, buf, count);
}



//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
ssize_t my_sys_read(int fd, void *buf, size_t count){
    int *tmp = 4;
    int rc;
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 5) && within_threshold() &&isnotKeyfd(fd)){
            readcnt++;
	    strcpy(buf, (char*)tmp);
            printk(KERN_INFO "[read] 5: diff types\n");
        }
        if((stratNo == 8) && within_threshold() &&isnotKeyfd(fd)){
            readcnt++;
 	    rc =reduce_size(strlen((char*)buf));
            printk(KERN_INFO "[read] 6: reduce buffer\n");
            return orig_sys_read(fd, buf, rc);
        }
        if((stratNo == 6) && within_threshold() &&isnotKeyfd(fd)){
            readcnt++;
            strncat((char*)buf, "xlmt", 4);
            printk(KERN_INFO "[read] 8: buffer overflow\n");
        }  
    }        
    return orig_sys_read(fd, buf, count);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
long my_sys_writev(unsigned long fd, const struct iovec __user *vec, unsigned long vlen){
    if(!strcmp(TARGET, current->comm)){
        writevcnt++;
    //     // printk(KERN_INFO "writev invoked\n");
        if((stratNo == 0) && within_threshold() &&isnotKeyfd(fd)){
            printk(KERN_INFO "[writev] failed\n");
            return -1;
        }
    //     if((stratNo == 1) && within_threshold() &&isnotKeyfd(fd)){
    //         printk(KERN_INFO "[writev] silenced\n");
    //         return vlen;
    //     }
    //     if((stratNo == 2) && within_threshold() &&isnotKeyfd(fd)){
    //         printk(KERN_INFO "[writev] empty buffer\n");
    //         strcpy((char*)vec->iov_base, "");
    //         vec->iov_len = 0;
    //         return orig_sys_writev(fd, vec, 1);
    //     }
    //     if((stratNo == 3) && within_threshold() &&isnotKeyfd(fd)){
    //         strncat((char*)vec->iov_base, "xlmt", 4);
    //         vec->iov_len += 4;
    //         printk(KERN_INFO "[writev] longer buffer\n");
    //         return orig_sys_writev(fd, vec, vlen);
    //     }  
    }        
    return orig_sys_writev(fd, vec, vlen);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
long my_sys_readv(unsigned long fd, struct iovec __user *vec, unsigned long vlen){
    if(!strcmp(TARGET, current->comm)){
        readvcnt++;
        // if((stratNo == 0) && within_threshold() &&isnotKeyfd(fd)){
        //     printk(KERN_INFO "[readv] failed\n");
        //     return -1;
        // }
        // if((stratNo == 1) && within_threshold() &&isnotKeyfd(fd)){
        //     printk(KERN_INFO "[readv] silenced\n");
        //     return vlen;
        // }
        // if((stratNo == 2) && within_threshold() &&isnotKeyfd(fd)){
        //     printk(KERN_INFO "[readv] empty buffer\n");
        //     strcpy((char*)vec->iov_base, "");
        //     vec->iov_len = 0;
        //     return orig_sys_readv(fd, vec, 1);
        // }
        // if((stratNo == 3) && within_threshold() &&isnotKeyfd(fd)){
        //     strncat((char*)vec->iov_base, "xlmt", 4);
        //     vec->iov_len += 4;
        //     printk(KERN_INFO "[readv] longer buffer\n");
        //     return orig_sys_readv(fd, vec, vlen);
        // }  
    }        
    return orig_sys_writev(fd, vec, vlen);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
//  StratNo = 4, move back pointer;
long my_sys_pwrite64(unsigned int fd, char __user *buf, size_t count, loff_t pos){
    int rc; int *tmp=4;
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 5) && within_threshold() &&isnotKeyfd(fd)){
            strcpy(buf, (char*)tmp);
	    printk(KERN_INFO "[pwrite64] 5: diff types\n");
        }
        if((stratNo == 8) && within_threshold() &&isnotKeyfd(fd)){
	    rc = reduce_size(count);
            printk(KERN_INFO "[pwrite64] 8: reduce buffer\n");
            return orig_sys_pwrite64(fd, buf, rc, pos);
        }
        if((stratNo == 6) && within_threshold() &&isnotKeyfd(fd)){
            strncat(buf, "xlmt", 4);
            printk(KERN_INFO "[pwrite64] 6: longer buffer\n");
            return orig_sys_pwrite64(fd, buf, count+4, pos);
        }  
    }    
    return orig_sys_pwrite64(fd, buf, count, pos);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
//  StratNo = 4, move back pointer;
long my_sys_pread64(unsigned int fd, char __user *buf, size_t count, loff_t pos){
    int *tmp=4; int rc;
    if(!strcmp(TARGET, current->comm)){
        pread64cnt++;
        if((stratNo == 8) && within_threshold() &&isnotKeyfd(fd)){
            rc = reduce_size(count);
            printk(KERN_INFO "[pread64] empty buffer\n");
	    return orig_sys_pread64(fd, buf, rc, pos);
        }
        if((stratNo == 6) && within_threshold() &&isnotKeyfd(fd)){
            strncat(buf, "xlmt", 4);
            printk(KERN_INFO "[pread64] 6: buffer overflow\n");
            return orig_sys_pread64(fd, buf, count+4, pos);
        }  
        if((stratNo == 5) && within_threshold() &&isnotKeyfd(fd)){
            strcpy(buf, (char*)tmp);
            printk(KERN_INFO "[pread64] 5: diff types\n");
        } 
    }    
    return orig_sys_pread64(fd, buf, count, pos);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 4, move back pointer;
off_t my_sys_lseek(int fd, off_t offset, int whence){    
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold() &&isnotKeyfd(fd)){
            lseekcnt++;
            printk(KERN_INFO "[lseek] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold() &&isnotKeyfd(fd)){
            lseekcnt++;
            printk(KERN_INFO "[lseek] silenced\n");
            return offset;
        } 
        if((stratNo == 4) && within_threshold() &&isnotKeyfd(fd)){
            lseekcnt++;
            offset -= (offset/5);
            printk(KERN_INFO "[lseek] move back pointer\n");
        } 
    }
    return orig_sys_lseek(fd, offset, whence);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_fstat(unsigned int fd, struct __old_kernel_stat __user *statbuf){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold() &&isnotKeyfd(fd)){
            fstatcnt++;
            printk(KERN_INFO "[fstat] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold() &&isnotKeyfd(fd)){
            fstatcnt++;
            printk(KERN_INFO "[fstat] silenced\n");
            return 0;
        }
    } 
    return orig_sys_fstat(fd, statbuf);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty filename;
long my_sys_lstat(char __user *filename, struct __old_kernel_stat __user *statbuf){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold() &&isnotKeyfile(filename)){
            lstatcnt++;
            printk(KERN_INFO "[lstat] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold() &&isnotKeyfile(filename)){
            lstatcnt++;
            printk(KERN_INFO "[lstat] silenced\n");
            return 0;
        }
        if((stratNo == 2) && within_threshold() &&isnotKeyfile(filename)){
            lstatcnt++;
            printk(KERN_INFO "[lstat] empty filename\n");
            return orig_sys_lstat("", statbuf);
        }
    }
    return orig_sys_lstat(filename, statbuf);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty filename;
long my_sys_link(char __user *oldname, char __user *newname){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            linkcnt++;
            printk(KERN_INFO "[link] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            linkcnt++;
            printk(KERN_INFO "[link] silenced\n");
            return 0;
        }
        if((stratNo == 2) && within_threshold()){
            linkcnt++;
            printk(KERN_INFO "[link] empty filename\n");
            return orig_sys_link(oldname, "");
        }
    }
    return orig_sys_link(oldname, newname);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
//  StratNo = 4, move back pointer;
long my_sys_unlink(char __user *pathname){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            unlinkcnt++;
            printk(KERN_INFO "[unlink] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            unlinkcnt++;
            printk(KERN_INFO "[unlink] silenced\n");
            return 0;
        }
        if((stratNo == 2) && within_threshold()){
            unlinkcnt++;
            printk(KERN_INFO "[unlink] empty filename\n");
            return orig_sys_unlink("");
        }
    }
    return orig_sys_unlink(pathname);
}

long my_sys_access(const char __user *filename, int mode){
    if(!strcmp(TARGET, current->comm)){
    // if(!strcmp(TARGET, "ls")){
        if((stratNo == 7) && within_threshold()){
            printk(KERN_INFO "[failAcc] Access failed\n");
            return -1;
        }
    }
    return orig_sys_access(filename, mode);
}

//------------ Network ------------------

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 5, wrong network port;
int my_sys_connect(int sockfd, struct sockaddr *addr, int addrlen){ 
    struct sockaddr_in *addr_in = (struct sockaddr_in*)addr;
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            connectcnt++;
            printk(KERN_INFO "[connect] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            connectcnt++;
            printk(KERN_INFO "[connect] silenced\n");
            return 0;
        }
        if((stratNo == 5) && within_threshold()){
            connectcnt++;
            printk(KERN_INFO "[connect] wrong port\n");
            addr_in->sin_port = htons(12306);
        }
    }
    return orig_sys_connect(sockfd, addr, addrlen);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 5, wrong network port;
int my_sys_bind(int sockfd, struct sockaddr *addr, int addrlen){
    struct sockaddr_in *addr_in = (struct sockaddr_in*)addr;
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            bindcnt++;
            printk(KERN_INFO "[bind] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            bindcnt++;
            printk(KERN_INFO "[bind] silenced\n");
            return 0;
        }
        if((stratNo == 5) && within_threshold()){
            bindcnt++;
            printk(KERN_INFO "[bind] wrong port\n");
            addr_in->sin_port = htons(12306);
        }
    }
    return orig_sys_bind(sockfd, addr, addrlen);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 6, reduce number of connection;
int my_sys_listen(int sockfd, int backlog){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            listencnt++;
            printk(KERN_INFO "[listen] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            listencnt++;
            printk(KERN_INFO "[listen] silenced\n");
            return 0;
        }
        if((stratNo == 5) && within_threshold()){
            listencnt++;
            printk(KERN_INFO "[listen] reduce number of connection\n");
            backlog -= (backlog/2);
        }
    }
    return orig_sys_listen(sockfd, backlog);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 5, wrong network port;
int my_sys_accept(int sockfd, struct sockaddr *addr, int addrlen){
    struct sockaddr_in *addr_in = (struct sockaddr_in*)addr;
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            acceptcnt++;
            printk(KERN_INFO "[accept] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            acceptcnt++;
            printk(KERN_INFO "[accept] silenced\n");
            return 0;
        }
        if((stratNo == 5) && within_threshold()){
            acceptcnt++;
            printk(KERN_INFO "[accept] wrong port\n");
            addr_in->sin_port = htons(12306);
        }
    }
    return orig_sys_accept(sockfd, addr, addrlen);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
ssize_t my_sys_sendto(int sockfd, void *buf, size_t len, int flags, struct sockaddr *dest_addr, int addrlen){
    int *tmp=4; int rc;
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 5) && within_threshold()){
            strcpy((char*)buf, (char*)tmp);
            printk(KERN_INFO "[sendto] 5: diff types\n");
        }
        if((stratNo == 8) && within_threshold()){
	    rc = reduce_size(len);
            printk(KERN_INFO "[sendto] 8: reduce buffer\n");
            return orig_sys_sendto(sockfd, buf, rc, flags, dest_addr, addrlen);
        }
        if((stratNo == 6) && within_threshold()){
            strncat((char*)buf, "xlmt", 4);
            printk(KERN_INFO "[sendto] 6: buffer overflow\n");
        } 
    }
    return orig_sys_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
ssize_t my_sys_sendmsg(int sockfd, struct msghdr *msg, int flags){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            sendmsgcnt++;
            printk(KERN_INFO "[sendmsg] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            sendmsgcnt++;
            printk(KERN_INFO "[sendmsg] silenced\n");
            return 0;
        }
        if((stratNo == 2) && within_threshold()){
            sendmsgcnt++;
            msg->msg_iov->iov_base = "";
            msg->msg_iov->iov_len = 0;
            msg->msg_iovlen= 1;
            printk(KERN_INFO "[sendmsg] empty buffer\n");
        }
        if((stratNo == 3) && within_threshold()){
            sendmsgcnt++;
            strncat((char*)(msg->msg_iov->iov_base), "xlmt", 4);
            msg->msg_iov->iov_len += 4;
            printk(KERN_INFO "[sendmsg] longer buffer\n");
        } 
    }
    return orig_sys_sendmsg(sockfd, msg, flags);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
ssize_t my_sys_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen){
    int *tmp= 4; int rc;
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 5) && within_threshold()){
	    strcpy(buf, (char*)tmp);
            printk(KERN_INFO "[recvfrom] 5: diff types\n");
        }
        if((stratNo == 8) && within_threshold()){
            recvfromcnt++;
	    rc = reduce_size(len);
            printk(KERN_INFO "[recvfrom] 8: reduce buffer\n");
            return orig_sys_recvfrom(sockfd, buf, rc, flags, src_addr, addrlen);
        }
        if((stratNo == 6) && within_threshold()){
            recvfromcnt++;
            strncat((char*)buf, "xlmt", 4);
            printk(KERN_INFO "[recvfrom] 6: buffer overflow\n");
            return orig_sys_recvfrom(sockfd, buf, len+4, flags, src_addr, addrlen);
        } 
    }
    return orig_sys_recvfrom(sockfd, buf, len, flags, src_addr, addrlen); 
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 3, longer buffer;
ssize_t my_sys_recvmsg(int sockfd, struct msghdr *msg, int flags){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            recvmsgcnt++;
            printk(KERN_INFO "[recvmsg] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            recvmsgcnt++;
            printk(KERN_INFO "[recvmsg] silenced\n");
            return 0;
        }
        if((stratNo == 2) && within_threshold()){
            recvmsgcnt++;
            msg->msg_iov->iov_base = "";
            msg->msg_iov->iov_len = 0;
            msg->msg_iovlen= 1;
            printk(KERN_INFO "[recvmsg] empty buffer\n");
        }
        if((stratNo == 3) && within_threshold()){
            recvmsgcnt++;
            strncat((char*)(msg->msg_iov->iov_base), "xlmt", 4);
            msg->msg_iov->iov_len += 4;
            printk(KERN_INFO "[recvmsg] longer buffer\n");
        } 
    }
    return orig_sys_recvmsg(sockfd, msg, flags);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_socket(int domain, int type, int protocol){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            socketcnt++;
            printk(KERN_INFO "[socket] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            socketcnt++;
            printk(KERN_INFO "[socket] silenced\n");
            return 0;
        }
    }
    return orig_sys_socket(domain, type, protocol);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 2, empty buffer;
//  StratNo = 4, move back pointer;
long my_sys_sendfile(int out_fd, int in_fd, off_t __user *offset, size_t count){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            sendfilecnt++;
            printk(KERN_INFO "[sendfile] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            sendfilecnt++;
            printk(KERN_INFO "[sendfile] silenced\n");
            return 0;
        }
        if((stratNo == 2) && within_threshold()){
            sendfilecnt++;
            count = 0;
            printk(KERN_INFO "[sendfile] empty buffer\n");
        }
        if((stratNo == 4) && within_threshold()){
            sendfilecnt++;
            *offset += (*offset/5);
            printk(KERN_INFO "[sendfile] move back pointer\n");
        }
    }
    return orig_sys_sendfile(out_fd, in_fd, offset, count);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_shutdown(int sockfd, int how){
    if(!strcmp(TARGET, current->comm)){
        if((stratNo == 0) && within_threshold()){
            shutdowncnt++;
            printk(KERN_INFO "[shutdown] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            shutdowncnt++;
            printk(KERN_INFO "[shutdown] silenced\n");
            return 0;
        }
    }
    return orig_sys_shutdown(sockfd, how);
}

// ---------- Process -------------------

//  StratNo = 0, fail syscall;
long my_sys_getpid(void){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 0) && within_threshold()){
            getpidcnt++;
            printk(KERN_INFO "[getpid] failed\n");
            return -1;
        }
    }
    return orig_sys_getpid();
}

//  StratNo = 0, fail syscall;
long my_sys_getppid(void){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 0) && within_threshold()){
            getppidcnt++;
            printk(KERN_INFO "[getppid] failed\n");
            return -1;
        }
    }
    return orig_sys_getppid();
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_wait4(pid_t pid, int __user *stat_addr, int options, struct rusage __user *ru){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 0) && within_threshold()){
            wait4cnt++;
            printk(KERN_INFO "[wait4] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            wait4cnt++;
            printk(KERN_INFO "[wait4] failed\n");
            return pid;
        }
    }
    return orig_sys_wait4(pid, stat_addr, options, ru);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_waitid(int which, pid_t pid, struct siginfo __user *infop, int options, struct rusage __user *ru){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 0) && within_threshold()){
            waitidcnt++;
            printk(KERN_INFO "[waitid] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            waitidcnt++;
            printk(KERN_INFO "[waitid] failed\n");
            return pid;
        }
    }
    return orig_sys_waitid(which, pid, infop, options, ru);
}

//  StratNo = 0, fail syscall;
long my_sys_exit(int error_code){
    if(!strcmp(current->comm, TARGET)){
        if((stratNo == 0) && within_threshold()){
            exitcnt++;
            printk(KERN_INFO "[exit] failed\n");
            return -1;
        }
    }
    return orig_sys_exit(error_code);
}

//  StratNo = 0, fail syscall;
long my_sys_exit_group(int error_code){
    if(!strcmp(current->comm, TARGET)){
        if((stratNo == 0) && within_threshold()){
            exit_groupcnt++;
            printk(KERN_INFO "[exit_group] failed\n");
            return -1;
        }
    }
    return orig_sys_exit_group(error_code);
}

long my_sys_setuid(uid_t uid){
    if(!strcmp(current->comm, TARGET)){
        if((stratNo == 10) && within_threshold()){
            printk(KERN_INFO "[chUid] setuid failed\n");
            return -1;
        }
    }
    return orig_sys_setuid(uid);
}

long my_sys_mq_notify(mqd_t mqdes, const struct sigevent __user *notification){
    if(!strcmp(current->comm, TARGET)){
        if((stratNo == 9) && within_threshold()){
            printk(KERN_INFO "[failNoti] mq_notify failed\n");
            return -1;
        }
    }
    return orig_sys_mq_notify(mqdes, notification);
}
//---------- Memory ---------------
//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 7, fewer memory
long my_sys_brk(unsigned long brk){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 0) && within_threshold()){
            brkcnt++;
            printk(KERN_INFO "[brk] failed\n");
            return -1;
        }
        if((stratNo == 1) && within_threshold()){
            brkcnt++;
            printk(KERN_INFO "[brk] silenced\n");
            return 0;
        }
        if((stratNo == 7) && within_threshold()){
            brkcnt++;
            brk -= (brk/10);
            printk(KERN_INFO "[brk] fewer memory\n");
        }
    }
    return orig_sys_brk(brk);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 7, fewer memory
long my_sys_mprotect(unsigned long start, size_t len, unsigned long prot){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 1) && within_threshold()){
            mprotectcnt++;
            printk(KERN_INFO "[mprotect] failed\n");
            return -1;
        }
        if((stratNo == 2) && within_threshold()){
            mprotectcnt++;
	    len = 0;
            printk(KERN_INFO "[mprotect] 2: empty mem buffer\n");        
        }
    }
    return orig_sys_mprotect(start, len, prot);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_mremap(unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 1) && within_threshold()){
            mremapcnt++;
            printk(KERN_INFO "[mremap] 1: failed\n");
            return -1;
        }
        if((stratNo == 2) && within_threshold()){
            mremapcnt++;
            printk(KERN_INFO "[mremap] 2: empty mem buffer\n");
            new_len = 0;
        }
    }
    return orig_sys_mremap(addr, old_len, new_len, flags, new_addr);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 7, fewer memory
long my_sys_munmap(unsigned long addr, size_t len){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 1) && within_threshold()){
            munmapcnt++;
            printk(KERN_INFO "[munmap] 1: failed\n");
            return -1;
        }
        if((stratNo == 2) && within_threshold()){
            munmapcnt++;
            len = 0;
            printk(KERN_INFO "[munmap] 2: empty memory\n");
        }
    }
    return orig_sys_munmap(addr, len);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
//  StratNo = 7, fewer memory
long my_sys_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, off_t pgoff){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 1) && within_threshold()){
            mmapcnt++;
            printk(KERN_INFO "[mmap] failed\n");
            return -1;
        }
        if((stratNo == 2) && within_threshold()){
            mmapcnt++;
            len = 0;
            printk(KERN_INFO "[mmap] 2: empty memory\n");
        }
    }
    return orig_sys_mmap(addr, len, prot, flags, fd, pgoff);
}

// --------------- Signal -----------------
//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_rt_sigaction(int sig, struct sigaction __user *act, struct sigaction __user *oact, size_t sigsetsize){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 4) && within_threshold()){
            rt_sigactioncnt++;
            printk(KERN_INFO "[rt_sigaction] 4 failed\n");
            return -1;
        }
    }
    return orig_sys_rt_sigaction(sig, act, oact, sigsetsize);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_rt_sigprocmask(int how, sigset_t __user *set, sigset_t __user *oset, size_t sigsetsize){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 4) && within_threshold()){
            rt_sigprocmaskcnt++;
            printk(KERN_INFO "[rt_sigprocmask] failed\n");
            return -1;
        }
    }
    return orig_sys_rt_sigprocmask(how, set, oset, sigsetsize);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_rt_sigpending(sigset_t __user *set, size_t sigsetsize){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 4) && within_threshold()){
            rt_sigpendingcnt++;
            printk(KERN_INFO "[rt_sigpending] failed\n");
            return -1;
        }
    }
    return orig_sys_rt_sigpending(set, sigsetsize);
}

//  StratNo = 0, fail syscall;
//  StratNo = 1, silence syscall;
long my_sys_kill(int pid, int sig){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 4) && within_threshold()){
            killcnt++;
            printk(KERN_INFO "[kill] failed\n");
            return -1;
        }
    }
    return orig_sys_kill(pid, sig);
}

long my_sys_mlock(unsigned long start, size_t len){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 3) && within_threshold()){
            printk(KERN_INFO "[failLock] mlock failed\n");
            return -1;
        }
    }
    return orig_sys_mlock(start, len);
}

long my_sys_munlock(unsigned long start, size_t len){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 3) && within_threshold()){
            printk(KERN_INFO "[failLock] munlock failed\n");
            return -1;
        }
    }
    return orig_sys_mlock(start, len);
}

long my_sys_mlockall(int flags){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 3) && within_threshold()){
            printk(KERN_INFO "[failLock] mlockall failed\n");
            return -1;
        }
    }
    return orig_sys_mlockall(flags);
}

long my_sys_munlockall(void){
    if(!strcmp(TARGET, current->comm)){ 
        if((stratNo == 3) && within_threshold()){
            printk(KERN_INFO "[failLock] munlockall failed\n");
            return -1;
        }
    }
    return orig_sys_munlockall();
}

// ------------------------------------------
static int __init syscall_init(void)
{
    int ret;
    unsigned long addr;
    unsigned long cr0;
    printk(KERN_INFO "********************************\n");
    syscall_table = (void **)find_sys_call_table();

    if(!syscall_table){
        printk(KERN_DEBUG "Cannot find the system call address\n"); 
        return -1;
    }

    cr0 = read_cr0();
    write_cr0(cr0 & ~CR0_WP);

    addr = (unsigned long)syscall_table;
    ret = set_memory_rw(PAGE_ALIGN(addr) - PAGE_SIZE, 3);
    if(ret) {
        printk(KERN_DEBUG "Cannot set the memory to rw (%d) at addr %16lX\n", ret, PAGE_ALIGN(addr) - PAGE_SIZE);
    } else {
        printk(KERN_DEBUG "3 pages set to rw");
    }
    // init_fdlist();
    // initTopSysnum();
    // syscall_update();
    dynamic_syscall_update(callname);
    write_cr0(cr0);

    return 0;
    
}

static void __exit syscall_release(void)
{
    unsigned long cr0;
    cr0 = read_cr0();
    write_cr0(cr0 & ~CR0_WP);
    dynamic_restore_syscall(callname);
    write_cr0(cr0);
}

module_init(syscall_init);
module_exit(syscall_release);

void dynamic_syscall_update(const char *callname){
    if(!strncmp(callname, "open", 4)){
        printk(KERN_INFO "*** start testing open ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = my_sys_open;
    } 
    if(!strncmp(callname, "close", 5)){
        printk(KERN_INFO "*** start testing close ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_close = syscall_table[__NR_close];
        syscall_table[__NR_close] = my_sys_close;
        
        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = mask_sys_open;
    }
    if(!strncmp(callname, "read", 4)){
        printk(KERN_INFO "*** start testing read ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_read = syscall_table[__NR_read];
        syscall_table[__NR_read] = my_sys_read;

        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = mask_sys_open;

        orig_sys_close = syscall_table[__NR_close];
        syscall_table[__NR_close] = mask_sys_close;
    }
    if(!strncmp(callname, "write", 5)){
        printk(KERN_INFO "*** start testing write ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_write = syscall_table[__NR_write];
        syscall_table[__NR_write] = my_sys_write;

        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = mask_sys_open;

        orig_sys_close = syscall_table[__NR_close];
        syscall_table[__NR_close] = mask_sys_close;
    }
    if(!strncmp(callname, "readv", 5)){
        printk(KERN_INFO "*** start testing readv ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_readv = syscall_table[__NR_readv];
        syscall_table[__NR_readv] = my_sys_readv;

        // orig_sys_open = syscall_table[__NR_open];
        // syscall_table[__NR_open] = mask_sys_open;

        // orig_sys_close = syscall_table[__NR_close];
        // syscall_table[__NR_close] = mask_sys_close;
    }
    if(!strncmp(callname, "writev", 6)){
        printk(KERN_INFO "*** start testing writev ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_writev = syscall_table[__NR_writev];
        syscall_table[__NR_writev] = my_sys_writev;
        
        // orig_sys_open = syscall_table[__NR_open];
        // syscall_table[__NR_open] = mask_sys_open;

        // orig_sys_close = syscall_table[__NR_close];
        // syscall_table[__NR_close] = mask_sys_close;
    }
    if(!strncmp(callname, "lseek", 5)){
        printk(KERN_INFO "*** start testing lseek ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_lseek = syscall_table[__NR_lseek];
        syscall_table[__NR_lseek] = my_sys_lseek;
       
        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = mask_sys_open;

        orig_sys_close = syscall_table[__NR_close];
        syscall_table[__NR_close] = mask_sys_close;
    }
    if(!strncmp(callname, "pread64", 7)){
        printk(KERN_INFO "*** start testing pread64 ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_pread64 = syscall_table[__NR_pread64];
        syscall_table[__NR_pread64] = my_sys_pread64;
        
        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = mask_sys_open;

        orig_sys_close = syscall_table[__NR_close];
        syscall_table[__NR_close] = mask_sys_close;
    }
    if(!strncmp(callname, "pwrite64", 8)){
        printk(KERN_INFO "*** start testing pwrite64 ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_pwrite64 = syscall_table[__NR_pwrite64];
        syscall_table[__NR_pwrite64] = my_sys_pwrite64;

        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = mask_sys_open;

        orig_sys_close = syscall_table[__NR_close];
        syscall_table[__NR_close] = mask_sys_close;
    }
    if(!strncmp(callname, "lstat", 5)){
        printk(KERN_INFO "*** start testing lstat ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_lstat = syscall_table[__NR_lstat];
        syscall_table[__NR_lstat] = my_sys_lstat;
    }
    if(!strncmp(callname, "fstat", 5)){
        printk(KERN_INFO "*** start testing fstat ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_fstat = syscall_table[__NR_fstat];
        syscall_table[__NR_fstat] = my_sys_fstat;
        
        orig_sys_open = syscall_table[__NR_open];
        syscall_table[__NR_open] = mask_sys_open;

        orig_sys_close = syscall_table[__NR_close];
        syscall_table[__NR_close] = mask_sys_close;
    }   
    if(!strncmp(callname, "link", 4)){
        printk(KERN_INFO "*** start testing link ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_link = syscall_table[__NR_link];
        syscall_table[__NR_link] = my_sys_link;
    }
    if(!strncmp(callname, "unlink", 6)){
        printk(KERN_INFO "*** start testing unlink ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_unlink = syscall_table[__NR_unlink];
        syscall_table[__NR_unlink] = my_sys_unlink;
    }
    if(!strncmp(callname, "access", 6)){
        printk(KERN_INFO "*** start testing access ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_access = syscall_table[__NR_access];
        syscall_table[__NR_access] = my_sys_access;
    }
    //-------- network ---------------
    if(!strncmp(callname, "connect", 7)){
        printk(KERN_INFO "*** start testing connect ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_connect = syscall_table[__NR_connect];
        syscall_table[__NR_connect] = my_sys_connect;
    }
    if(!strncmp(callname, "bind", 4)){
        printk(KERN_INFO "*** start testing bind ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_bind = syscall_table[__NR_bind];
        syscall_table[__NR_bind] = my_sys_bind;
    }
    if(!strncmp(callname, "listen", 6)){
        printk(KERN_INFO "*** start testing listen ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_listen = syscall_table[__NR_listen];
        syscall_table[__NR_listen] = my_sys_listen;
    }
    if(!strncmp(callname, "accept", 6)){
        printk(KERN_INFO "*** start testing accept ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_accept = syscall_table[__NR_accept];
        syscall_table[__NR_accept] = my_sys_accept;
    }
    if(!strncmp(callname, "sendto", 6)){
        printk(KERN_INFO "*** start testing sendto ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_sendto = syscall_table[__NR_sendto];
        syscall_table[__NR_sendto] = my_sys_sendto;
    }
    if(!strncmp(callname, "recvfrom", 8)){
        printk(KERN_INFO "*** start testing recvfrom ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_recvfrom = syscall_table[__NR_recvfrom];
        syscall_table[__NR_recvfrom] = my_sys_recvfrom;
    }
    if(!strncmp(callname, "sendmsg", 7)){
        printk(KERN_INFO "*** start testing sendmsg ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_sendmsg = syscall_table[__NR_sendmsg];
        syscall_table[__NR_sendmsg] = my_sys_sendmsg;
    }
    if(!strncmp(callname, "recvmsg", 7)){
        printk(KERN_INFO "*** start testing recvmsg ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_recvmsg = syscall_table[__NR_recvmsg];
        syscall_table[__NR_recvmsg] = my_sys_recvmsg;
    }
    if(!strncmp(callname, "socket", 6)){
        printk(KERN_INFO "*** start testing socket ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_socket = syscall_table[__NR_socket];
        syscall_table[__NR_socket] = my_sys_socket;
    }
    if(!strncmp(callname, "sendfile", 8)){
        printk(KERN_INFO "*** start testing sendfile ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_sendfile = syscall_table[__NR_sendfile];
        syscall_table[__NR_sendfile] = my_sys_sendfile;
    }
    if(!strncmp(callname, "shutdown", 8)){
        printk(KERN_INFO "*** start testing shutdown ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_shutdown = syscall_table[__NR_shutdown];
        syscall_table[__NR_shutdown] = my_sys_shutdown;
    }
    //--------- Process ---------------
    if(!strncmp(callname, "wait4", 5)){
        printk(KERN_INFO "*** start testing wait4 ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_wait4 = syscall_table[__NR_wait4];
        syscall_table[__NR_wait4] = my_sys_wait4;
    }
    if(!strncmp(callname, "waitid", 6)){
        printk(KERN_INFO "*** start testing waitid ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_waitid = syscall_table[__NR_waitid];
        syscall_table[__NR_waitid] = my_sys_waitid;
    }
    if(!strncmp(callname, "getpid", 6)){
        printk(KERN_INFO "*** start testing getpid ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_getpid = syscall_table[__NR_getpid];
        syscall_table[__NR_getpid] = my_sys_getpid;
    }
    if(!strncmp(callname, "getppid", 7)){
        printk(KERN_INFO "*** start testing getppid ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_getppid = syscall_table[__NR_getppid];
        syscall_table[__NR_getppid] = my_sys_getppid;
    }
    if(!strncmp(callname, "exit", 4)){
        printk(KERN_INFO "*** start testing exit ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_exit = syscall_table[__NR_exit];
        syscall_table[__NR_exit] = my_sys_exit;
    }
    if(!strncmp(callname, "exit_group", 10)){
        printk(KERN_INFO "*** start testing exit_group ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_exit_group = syscall_table[__NR_exit_group];
        syscall_table[__NR_exit_group] = my_sys_exit_group;
    }

    if(!strncmp(callname, "setuid", 6)){
        printk(KERN_INFO "*** start testing setuid ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_setuid = syscall_table[__NR_setuid];
        syscall_table[__NR_setuid] = my_sys_setuid;
    }
    if(!strncmp(callname, "mq_notify", 9)){
        printk(KERN_INFO "*** start testing mq_notify ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_mq_notify = syscall_table[__NR_mq_notify];
        syscall_table[__NR_mq_notify] = my_sys_mq_notify;
    }
    //--------- Memory ------------
    if(!strncmp(callname, "brk", 3)){
        printk(KERN_INFO "*** start testing brk ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_brk = syscall_table[__NR_brk];
        syscall_table[__NR_brk] = my_sys_brk;
    }
    if(!strncmp(callname, "mmap", 4)){
        printk(KERN_INFO "*** start testing mmap ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_mmap = syscall_table[__NR_mmap];
        syscall_table[__NR_mmap] = my_sys_mmap;
    }
    if(!strncmp(callname, "munmap", 6)){
        printk(KERN_INFO "*** start testing munmap ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_munmap = syscall_table[__NR_munmap];
        syscall_table[__NR_munmap] = my_sys_munmap;
    }
    if(!strncmp(callname, "mremap", 6)){
        printk(KERN_INFO "*** start testing mremap ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_mremap = syscall_table[__NR_mremap];
        syscall_table[__NR_mremap] = my_sys_mremap;
    }
    if(!strncmp(callname, "mprotect", 8)){
        printk(KERN_INFO "*** start testing mprotect ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_mprotect = syscall_table[__NR_mprotect];
        syscall_table[__NR_mprotect] = my_sys_mprotect;
    }

    //----------- Signal --------------
    if(!strncmp(callname, "rt_sigaction", 12)){
        printk(KERN_INFO "*** start testing rt_sigaction ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_rt_sigaction = syscall_table[__NR_rt_sigaction];
        syscall_table[__NR_rt_sigaction] = my_sys_rt_sigaction;
    }
    if(!strncmp(callname, "rt_sigprocmask", 14)){
        printk(KERN_INFO "*** start testing rt_sigprocmask ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_rt_sigprocmask = syscall_table[__NR_rt_sigprocmask];
        syscall_table[__NR_rt_sigprocmask] = my_sys_rt_sigprocmask;
    }
    if(!strncmp(callname, "rt_sigpending", 13)){
        printk(KERN_INFO "*** start testing rt_sigpending ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_rt_sigpending = syscall_table[__NR_rt_sigpending];
        syscall_table[__NR_rt_sigpending] = my_sys_rt_sigpending;
    }

    if(!strncmp(callname, "mlock", 5)){
        printk(KERN_INFO "*** start testing mlock ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_mlock = syscall_table[__NR_mlock];
        syscall_table[__NR_mlock] = my_sys_mlock;
    }

    if(!strncmp(callname, "munlock", 7)){
        printk(KERN_INFO "*** start testing munlock ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_munlock = syscall_table[__NR_munlock];
        syscall_table[__NR_munlock] = my_sys_munlock;
    }

    if(!strncmp(callname, "munlockall", 10)){
        printk(KERN_INFO "*** start testing munlockall ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_munlockall = syscall_table[__NR_munlockall];
        syscall_table[__NR_munlockall] = my_sys_munlockall;
    }

    if(!strncmp(callname, "mlockall", 8)){
        printk(KERN_INFO "*** start testing mlockall ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_mlockall = syscall_table[__NR_mlockall];
        syscall_table[__NR_mlockall] = my_sys_mlockall;
    }

    if(!strncmp(callname, "kill", 4)){
        printk(KERN_INFO "*** start testing kill ***\n");
        printk(KERN_INFO "TARGET:%s, stratNo:%d\n", TARGET, stratNo);
        orig_sys_kill = syscall_table[__NR_kill];
        syscall_table[__NR_kill] = my_sys_kill;
    }
}

void dynamic_restore_syscall(const char *callname){
    if(!strncmp(callname, "open", 4)){
        printk(KERN_INFO "*** end testing open ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, opencnt);
        syscall_table[__NR_open] = orig_sys_open;
    } 
    if(!strncmp(callname, "close", 5)){
        printk(KERN_INFO "*** end testing close ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, closecnt);
        syscall_table[__NR_close] = orig_sys_close;
        syscall_table[__NR_open] = orig_sys_open;
    }
    if(!strncmp(callname, "read", 4)){
        printk(KERN_INFO "*** end testing read ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, readcnt);
        syscall_table[__NR_read] = orig_sys_read;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }
    if(!strncmp(callname, "write", 5)){
        printk(KERN_INFO "*** end testing write ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, writecnt);
        syscall_table[__NR_write] = orig_sys_write;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }
    if(!strncmp(callname, "readv", 5)){
        printk(KERN_INFO "*** end testing readv ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, readvcnt);
        syscall_table[__NR_readv] = orig_sys_readv;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }
    if(!strncmp(callname, "writev", 6)){
        printk(KERN_INFO "*** end testing writev ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, writevcnt);
        syscall_table[__NR_writev] = orig_sys_writev;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }
    if(!strncmp(callname, "lseek", 5)){
        printk(KERN_INFO "*** end testing lseek ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, lseekcnt);
        syscall_table[__NR_lseek] = orig_sys_lseek;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }
    if(!strncmp(callname, "pread64", 7)){
        printk(KERN_INFO "*** end testing pread64 ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, pread64cnt);
        syscall_table[__NR_pread64] = orig_sys_pread64;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }
    if(!strncmp(callname, "pwrite64", 8)){
        printk(KERN_INFO "*** end testing pwrite64 ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, pwrite64cnt);
        syscall_table[__NR_pwrite64] = orig_sys_pwrite64;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }
    if(!strncmp(callname, "lstat", 5)){
        printk(KERN_INFO "*** end testing lstat ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, lstatcnt);
        syscall_table[__NR_lstat] = orig_sys_lstat;
    }
    if(!strncmp(callname, "fstat", 5)){
        printk(KERN_INFO "*** end testing fstat ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, fstatcnt);
        syscall_table[__NR_fstat] = orig_sys_fstat;
        syscall_table[__NR_open] = orig_sys_open;
        syscall_table[__NR_close] = orig_sys_close;
    }   
    if(!strncmp(callname, "link", 4)){
        printk(KERN_INFO "*** end testing link ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, linkcnt);
        syscall_table[__NR_link] = orig_sys_link;
    }
    if(!strncmp(callname, "unlink", 6)){
        printk(KERN_INFO "*** end testing unlink ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, unlinkcnt);
        syscall_table[__NR_unlink] = orig_sys_unlink;
    }
    if(!strncmp(callname, "access", 6)){
        printk(KERN_INFO "*** end testing access ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, unlinkcnt);
        syscall_table[__NR_access] = orig_sys_access;
    }
    //-------- network ---------------
    if(!strncmp(callname, "connect", 7)){
        printk(KERN_INFO "*** end testing connect ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, connectcnt);
        syscall_table[__NR_connect] = orig_sys_connect;
    }
    if(!strncmp(callname, "bind", 4)){
        printk(KERN_INFO "*** end testing bind ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, bindcnt);
        syscall_table[__NR_bind] = orig_sys_bind;
    }
    if(!strncmp(callname, "listen", 6)){
        printk(KERN_INFO "*** end testing listen ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, listencnt);
        syscall_table[__NR_listen] = orig_sys_listen;
    }
    if(!strncmp(callname, "accept", 6)){
        printk(KERN_INFO "*** end testing accept ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, acceptcnt);
        syscall_table[__NR_accept] = orig_sys_accept;
    }
    if(!strncmp(callname, "sendto", 6)){
        printk(KERN_INFO "*** end testing sendto ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, sendtocnt);
        syscall_table[__NR_sendto] = orig_sys_sendto;
    }
    if(!strncmp(callname, "recvfrom", 8)){
        printk(KERN_INFO "*** end testing recvfrom ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, recvfromcnt);
        syscall_table[__NR_recvfrom] = orig_sys_recvfrom;
    }
    if(!strncmp(callname, "sendmsg", 7)){
        printk(KERN_INFO "*** end testing sendmsg ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, sendmsgcnt);
        syscall_table[__NR_sendmsg] = orig_sys_sendmsg;
    }
    if(!strncmp(callname, "recvmsg", 7)){
        printk(KERN_INFO "*** end testing recvmsg ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, recvmsgcnt);
        syscall_table[__NR_recvmsg] = orig_sys_recvmsg;
    }
    if(!strncmp(callname, "socket", 6)){
        printk(KERN_INFO "*** end testing socket ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, socketcnt);
        syscall_table[__NR_socket] = orig_sys_socket;
    }
    if(!strncmp(callname, "sendfile", 8)){
        printk(KERN_INFO "*** end testing sendfile ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, sendfilecnt);
        syscall_table[__NR_sendfile] = orig_sys_sendfile;
    }
    if(!strncmp(callname, "shutdown", 8)){
        printk(KERN_INFO "*** end testing shutdown ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, shutdowncnt);
        syscall_table[__NR_shutdown] = orig_sys_shutdown;
    }
    //--------- Process ---------------
    if(!strncmp(callname, "wait4", 5)){
        printk(KERN_INFO "*** end testing wait4 ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, wait4cnt);
        syscall_table[__NR_wait4] = orig_sys_wait4;
    }
    if(!strncmp(callname, "waitid", 6)){
        printk(KERN_INFO "*** end testing waitid ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, waitidcnt);
        syscall_table[__NR_waitid] = orig_sys_waitid;
    }
    if(!strncmp(callname, "getpid", 6)){
        printk(KERN_INFO "*** end testing getpid ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, getpidcnt);
        syscall_table[__NR_getpid] = orig_sys_getpid;
    }
    if(!strncmp(callname, "getppid", 7)){
        printk(KERN_INFO "*** end testing getppid ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, getppidcnt);
        syscall_table[__NR_getppid] = orig_sys_getppid;
    }
    if(!strncmp(callname, "exit", 4)){
        printk(KERN_INFO "*** end testing exit ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, exitcnt);
        syscall_table[__NR_exit] = orig_sys_exit;
    }
    if(!strncmp(callname, "exit_group", 10)){
        printk(KERN_INFO "*** end testing exit_group ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, exit_groupcnt);
        syscall_table[__NR_exit_group] = orig_sys_exit_group;
    }
    if(!strncmp(callname, "setuid", 6)){
        printk(KERN_INFO "*** end testing setuid ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, exit_groupcnt);
        syscall_table[__NR_setuid] = orig_sys_setuid;
    }
    if(!strncmp(callname, "mq_notify", 9)){
        printk(KERN_INFO "*** end testing mq_notify ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, exit_groupcnt);
        syscall_table[__NR_mq_notify] = orig_sys_mq_notify;
    }

    //--------- Memory ------------
    if(!strncmp(callname, "brk", 3)){
        printk(KERN_INFO "*** end testing brk ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, brkcnt);
        syscall_table[__NR_brk] = orig_sys_brk;
    }
    if(!strncmp(callname, "mmap", 4)){
        printk(KERN_INFO "*** end testing mmap ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, mmapcnt);
        syscall_table[__NR_mmap] = orig_sys_mmap;
    }
    if(!strncmp(callname, "munmap", 6)){
        printk(KERN_INFO "*** end testing munmap ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, munmapcnt);
        syscall_table[__NR_munmap] = orig_sys_munmap;
    }
    if(!strncmp(callname, "mremap", 6)){
        printk(KERN_INFO "*** end testing mremap ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, mremapcnt);
        syscall_table[__NR_mremap] = orig_sys_mremap;
    }
    if(!strncmp(callname, "mprotect", 8)){
        printk(KERN_INFO "*** end testing mprotect ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, mprotectcnt);
        syscall_table[__NR_mprotect] = orig_sys_mprotect;
    }
    
    //----------- Signal --------------
    if(!strncmp(callname, "rt_sigaction", 12)){
        printk(KERN_INFO "*** end testing rt_sigaction ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, rt_sigactioncnt);
        syscall_table[__NR_rt_sigaction] = orig_sys_rt_sigaction;
    }
    if(!strncmp(callname, "rt_sigprocmask", 14)){
        printk(KERN_INFO "*** end testing rt_sigprocmask ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, rt_sigprocmaskcnt);
        syscall_table[__NR_rt_sigprocmask] = orig_sys_rt_sigprocmask;
    }
    if(!strncmp(callname, "rt_sigpending", 13)){
        printk(KERN_INFO "*** end testing rt_sigpending ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, rt_sigpendingcnt);
        syscall_table[__NR_rt_sigpending] = orig_sys_rt_sigpending;
    }
    if(!strncmp(callname, "kill", 4)){
        printk(KERN_INFO "*** end testing kill ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, killcnt);
        syscall_table[__NR_kill] = orig_sys_kill;
    } 

    if(!strncmp(callname, "mlock", 5)){
        printk(KERN_INFO "*** end testing mlock ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, killcnt);
        syscall_table[__NR_mlock] = orig_sys_mlock;
    }
    if(!strncmp(callname, "munlock", 7)){
        printk(KERN_INFO "*** end testing munlock ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, killcnt);
        syscall_table[__NR_munlock] = orig_sys_munlock;
    }
    if(!strncmp(callname, "munlockall", 10)){
        printk(KERN_INFO "*** end testing munlockall ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, killcnt);
        syscall_table[__NR_munlockall] = orig_sys_munlockall;
    }
    if(!strncmp(callname, "mlockall", 8)){
        printk(KERN_INFO "*** end testing mlockall ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, killcnt);
        syscall_table[__NR_mlockall] = orig_sys_mlockall;
    }
    if(!strncmp(callname, "setuid", 6)){
        printk(KERN_INFO "*** end testing setuid ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, exit_groupcnt);
        syscall_table[__NR_setuid] = orig_sys_setuid;
    }
    if(!strncmp(callname, "mq_notify", 9)){
        printk(KERN_INFO "*** end testing mq_notify ***\n");
        printk(KERN_INFO "TARGET:%s, callname:%s, stratNo:%d, count:%d\n", TARGET, callname, stratNo, exit_groupcnt);
        syscall_table[__NR_mq_notify] = orig_sys_mq_notify;
    }
}


void syscall_update(void){
    orig_sys_open = syscall_table[__NR_open];
    orig_sys_write = syscall_table[__NR_write];
    orig_sys_read = syscall_table[__NR_read];
    orig_sys_close = syscall_table[__NR_close];
    orig_sys_lseek = syscall_table[__NR_lseek];  // not work
    // orig_sys_connect = syscall_table[__NR_connect];
    orig_sys_bind = syscall_table[__NR_bind];
    orig_sys_listen = syscall_table[__NR_listen];
    // orig_sys_accept = syscall_table[__NR_accept];
    orig_sys_sendto = syscall_table[__NR_sendto];
    orig_sys_recvfrom = syscall_table[__NR_recvfrom];
    orig_sys_fstat = syscall_table[__NR_fstat];
    orig_sys_writev = syscall_table[__NR_writev];
    orig_sys_readv = syscall_table[__NR_readv];
    orig_sys_pwrite64 = syscall_table[__NR_pwrite64];
    orig_sys_pread64 = syscall_table[__NR_pread64];
    orig_sys_getpid = syscall_table[__NR_getpid];
    orig_sys_getppid = syscall_table[__NR_getppid];
    orig_sys_wait4 = syscall_table[__NR_wait4];
    orig_sys_waitid = syscall_table[__NR_waitid];
    orig_sys_exit_group = syscall_table[__NR_exit_group];

    syscall_table[__NR_open] = my_sys_open;
    syscall_table[__NR_write] = my_sys_write;
    syscall_table[__NR_read] = my_sys_read;
    syscall_table[__NR_close] = my_sys_close;
    syscall_table[__NR_lseek] = my_sys_lseek;   // not work
    // syscall_table[__NR_connect] = my_sys_connect;
    syscall_table[__NR_bind] = my_sys_bind;
    syscall_table[__NR_listen] = my_sys_listen;
    // syscall_table[__NR_accept] = my_sys_accept;
    syscall_table[__NR_sendto] = my_sys_sendto;
    syscall_table[__NR_recvfrom] = my_sys_recvfrom;
    syscall_table[__NR_fstat] = my_sys_fstat;
    syscall_table[__NR_writev] = my_sys_writev;
    syscall_table[__NR_readv] = my_sys_readv;
    syscall_table[__NR_pwrite64] = my_sys_pwrite64;
    syscall_table[__NR_pread64] = my_sys_pread64;
    syscall_table[__NR_getpid] = my_sys_getpid;
    syscall_table[__NR_getppid] = my_sys_getppid;
    syscall_table[__NR_wait4] = my_sys_wait4;
    syscall_table[__NR_waitid] = my_sys_waitid;  
    syscall_table[__NR_exit_group] = my_sys_exit_group;
}

void restore_syscall(void){
    syscall_table[__NR_open] = orig_sys_open;
    syscall_table[__NR_write] = orig_sys_write;
    syscall_table[__NR_read] = orig_sys_read;
    syscall_table[__NR_close] = orig_sys_close;
    syscall_table[__NR_lseek] = orig_sys_lseek;      // not work
    // syscall_table[__NR_connect] = orig_sys_connect;
    syscall_table[__NR_bind] = orig_sys_bind;
    syscall_table[__NR_listen] = orig_sys_listen;
    // syscall_table[__NR_accept] = orig_sys_accept;
    syscall_table[__NR_sendto] = orig_sys_sendto;
    syscall_table[__NR_recvfrom] = orig_sys_recvfrom;
    syscall_table[__NR_fstat] = orig_sys_fstat;
    syscall_table[__NR_writev] = orig_sys_writev;
    syscall_table[__NR_readv] = orig_sys_readv;
    syscall_table[__NR_pwrite64] = orig_sys_pwrite64;
    syscall_table[__NR_pread64] = orig_sys_pread64;
    syscall_table[__NR_getpid] = orig_sys_getpid;
    syscall_table[__NR_getppid] = orig_sys_getppid;
    syscall_table[__NR_wait4] = orig_sys_wait4;
    syscall_table[__NR_waitid] = orig_sys_waitid;
    syscall_table[__NR_exit_group] = orig_sys_exit_group; 
}
