#include "unpred.h"

int fdlist[FDSIZE];
int sysarray[SYSSIZE];
int topsysnum[NUMOFTOPSYS];

int reduce_size(size_t count){
	int t, tt;
    count= (count>0? count: 1);
    get_random_bytes(&t, sizeof(t)); //generate random number
    tt = t%count;
    tt = (tt >0 ? tt:(0-tt));
    tt = ((tt==0)? 1:tt);
    return tt;
}

int ret_strategy(int number){
	int t, tt;
	get_random_bytes(&t, sizeof(t));
	tt = t% number;
    tt = (tt >0 ? tt:(0-tt));
    return tt;
}

bool within_threshold(){
    int t, thresh;
    get_random_bytes(&t, sizeof(t));
    thresh = t % 100;
    thresh = (thresh >0? thresh: (0-thresh));
    // printk(KERN_INFO "Threshold is %d\n", thresh);
    return (thresh < THRESHOLD? true: false);
}

void init_fdlist(){
    int i;
    for(i=0; i<FDSIZE; i++)
        fdlist[i]= 0;
}

bool start_strategy(){
	struct timespec ts, ts1; 
    struct task_struct *iTask = current;
    long curr_time, delta_time, delta_ntime;

    getboottime(&ts1);
    getnstimeofday(&ts);
    curr_time = ts.tv_sec - ts1.tv_sec;
    delta_time = curr_time - iTask->start_time.tv_sec;
    delta_ntime = ts.tv_nsec - ts1.tv_nsec + iTask->start_time.tv_nsec;
    if(delta_time>= LAUNCHTIME){
        // printk(KERN_INFO "%s has been running for %ld nanoseconds\n", target, delta_ntime);
        // printk(KERN_INFO "timeOfday: %ld, bootime: %ld, psStartime: %ld\n", ts.tv_nsec, ts1.tv_nsec, current->start_time.tv_nsec);
        return true;
    } 
    else 
    	return false;
}

// ------------------------------------------
void add_fd(long fd, const char *filename){
	if(filename[0]== '/'){
		if ((strncmp(filename, "/home", 5)!=0)&&(strncmp("/tmp", filename, 4)!=0)){ //not under /home, key fd 
			fdlist[fd]= 1;
			// printk(KERN_INFO "fd %ld added in list %s\n", fd, filename);
		}
        else{
            printk(KERN_INFO "Play %s with fd %ld\n", filename, fd);
        }
        // else{
        //     if(filename[13]=='.'){
        //         fdlist[fd]= 1;
        //         // printk(KERN_INFO "fd %ld added in list %s\n", fd, filename);
        //     }
        //     else{
        //         fdlist[fd]= 0;
        //         // printk(KERN_INFO "Play %s with fd %ld\n", filename, fd);
        //     }
        // }
	}
    // if((!strncmp(filename, "/lib", 4)) || (!strncmp(filename, "/usr/lib", 8))){
    //     fdlist[fd] = 1;
    //     printk(KERN_INFO "fd %ld, %s added in list\n", fd, filename);
    // }
    // if((!strncmp(filename, "/bin", 4)) || (!strncmp(filename, "/usr/bin", 8))){
    //     fdlist[fd] = 1;
    //     printk(KERN_INFO "fd %ld, %s added in list\n", fd, filename);
    // }
}


void del_fd(long fd){
	fdlist[fd]= 0;
    // printk(KERN_INFO "fd %ld deleted\n", fd);
}

bool isnotKeyfd(long fd){   //
	return fdlist[fd]==1? false:true;
}

bool isnotKeyfile(const char *filename){
    if((!strncmp(filename, "/home", 5)) || (!strncmp(filename, "/tmp", 4))){
        return true;
    }
    // if(filename[13] == '.')
    //     return true;
    return false;
}

//-------------------------------------------------------------
//write.read.lseek.nanosleep.sendto.recvfrom.bind.listen.connct...
//0.....1....2.....3.........4......5........6....7......8.....
void initTopSysnum(void){
    int i;
    for(i= 0; i< SYSSIZE; i++)
        sysarray[i] = 0;
    for(i=0; i<NUMOFTOPSYS; i++)
        //initialize the top 5 frequently called syscall number
        topsysnum[i]= 100; //100: so the sysarray[topsysnum[i]] will be 0
}

void rankSyscall(int syscallnum){
    int i, j, tmp;
    sysarray[syscallnum]++;
    if(isRanked(syscallnum)){
        //sort the topsysnum;
        for(i= 0; i< NUMOFTOPSYS; i++){
            for(j=0; j< NUMOFTOPSYS-i-1; j++){
                if(sysarray[topsysnum[j]] < sysarray[topsysnum[j+1]]){
                    tmp= topsysnum[j+1];
                    topsysnum[j+1] = topsysnum[j];
                    topsysnum[j] = tmp;
                }
            }
        }
        // print current rank array
        // for(i= 0; i< NUMOFTOPSYS; i++){
        //     printk(KERN_INFO "%d, ", topsysnum[i]);
        // }
        return;
    }
    // not ranked: add the new callnum;
    for(i=0; i<NUMOFTOPSYS; i++){
        if(sysarray[syscallnum]> sysarray[topsysnum[i]]){
            for(j= NUMOFTOPSYS-1; j>i; j--){
                topsysnum[j] = topsysnum[j-1];
            }
            topsysnum[i]= syscallnum;
            printk(KERN_INFO "syscall %d added to toparray\n", syscallnum);
            return;
        }
    }
}


bool isRanked(int syscallnum){
    int i;
    for(i= 0; i<NUMOFTOPSYS; i++){
        if(topsysnum[i] == syscallnum){
            //printk(KERN_INFO "[Syscall %d]rank No.%d\n", syscallnum, i);
            return true;
        }
    }
    return false;
}


bool isMalSyscall(void){
    return true;
}