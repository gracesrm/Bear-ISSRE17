#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include </usr/include/x86_64-linux-gnu/asm/unistd_64.h>
#include <linux/delay.h>    // loops_per_jiffy
#include <linux/syscalls.h>
#include <asm-generic/syscalls.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/random.h>
#include <linux/hashtable.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/file.h>
#include <linux/socket.h>
#include <linux/time.h>

// #define LOCAL_ADDR ((unsigned long int)0x0100007F)  //"127.0.0.1" attention on big/little endian
#define CR0_WP 0x00010000   // Write Protect Bit (CR0:16)
#define THRESHOLD 50
#define LAUNCHTIME 0
#define FDSIZE 1024
#define SYSSIZE 300  // system call size
#define NUMOFTOPSYS 100  //number of top system call
// #define TARGET "firefox"
//"soffice.bin"

int reduce_size(size_t count);
int ret_strategy(int number);
bool within_threshold(void);
void init_fdlist(void);
bool start_strategy(void);

void add_fd(long fd, const char *filename);
void del_fd(long fd);
bool isnotKeyfd(long fd);
bool isnotKeyfile(const char *filename);


//-------------------------------------------------------------
//write.read.lseek.nanosleep.sendto.recvfrom.bind.listen.connct...
//0.....1....2.....3.........4......5........6....7......8.....

//---------------------------------------------
void rankSyscall(int syscallnum);
void initTopSysnum(void);
bool isRanked(int syscallnum);
bool isMalSyscall(void);
//how to input unsure number of arguments?
