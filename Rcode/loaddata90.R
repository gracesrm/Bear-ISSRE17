# Last modified 5/4/2016.
# 
# Overview: This loads the dat90a and displays some results that I use to manually 
# correct before running the logistic regressions.
#
# Note that some of the code is to be used for my own understanding. 


# Load dat90a
dat90<-read.csv(file = "~/bear/Res/Res90.csv", check.names=FALSE,
                header = F, col.names=c('program','syscall','stratNo','st','et','y'), quote = "")
dat90$program <- NULL
dat90$stratNo <- factor(dat90$stratNo)

dat90$y <- relevel(dat90$y, ref = "N")
dat90$syscall <- relevel(dat90$syscall, ref = "mmap")
dat90$stratNo <- relevel(dat90$stratNo, ref = "2")

# Label the strategy number as a categorical variable.
# dat90IO$stratNo <- factor(dat90IO$stratNo)

# dat90CPU<-read.csv(file = "~/newdat90a/format_dat90a/dat90a90_cpu.csv", check.names=FALSE,
#                  header = F, col.names=c('y','program','syscall','stratNo'), quote = "")
# dat90CPU$stratNo <- factor(dat90CPU$stratNo)

# Create a new vector that helps identify which type of program it is
# dat90IO$progType <- rep("io",length(dat90IO$y))
# dat90CPU$progType <- rep("cpu",length(dat90CPU$y))

#concatonate
# dat90 <- rbind(dat90IO, dat90CPU);

#make results binary
# dat90$progType <- factor(dat90$progType)

#load correct package
library(plyr)
# redefine variables for simpilfication in the model
# dat90$y<-revalue(dat90$y, c("abort" = "crash"))
# dat90IO$y<-revalue(dat90IO$y, c("abort" = "crash"))
# dat90CPU$y<-revalue(dat90CPU$y, c("abort" = "crash"))
# dat90$stratNo<-revalue(dat90$stratNo, c("3" = "2"))
# dat90IO$stratNo<-revalue(dat90IO$stratNo, c("3" = "2"))
# dat90CPU$stratNo<-revalue(dat90CPU$stratNo, c("3" = "2"))

# sanity check to make sure I did everything correctly
# levels(dat90$y)

#dat90IO.clean <- dat90IO[which(dat90IO$syscall != 'lstat'),]
#dat90IO.clean <- dat90IO.clean[which(dat90IO.clean$syscall != 'mmap'),]
#dat90.clean <- dat90.clean[which(dat90.clean$syscall != 'wait4'),]
#dat90IO.clean$syscall <- factor(dat90IO.clean$syscall) #refactor

table(dat90[which(dat90$stratNo =='1'),]$y)
table(dat90[which(dat90$stratNo =='2'),]$y)
table(dat90[which(dat90$stratNo =='3'),]$y)
table(dat90[which(dat90$stratNo =='4'),]$y)
table(dat90[which(dat90$stratNo =='5'),]$y)
table(dat90[which(dat90$stratNo =='6'),]$y)
table(dat90[which(dat90$stratNo =='7'),]$y)
table(dat90[which(dat90$stratNo =='8'),]$y)
table(dat90[which(dat90$stratNo =='9'),]$y)
table(dat90[which(dat90$stratNo =='10'),]$y)


# dat90CPU.clean <- dat90CPU.clean[which(dat90CPU.clean$syscall != 'kill'),]
# dat90CPU.clean$syscall <- factor(dat90CPU.clean$syscall) #refactor

#for dat90 all

table(dat90[which(dat90$syscall =='mmap'),]$y)
table(dat90[which(dat90$syscall =='mprotect'),]$y)
table(dat90[which(dat90$syscall =='munmap'),]$y)
table(dat90[which(dat90$syscall =='mremap'),]$y)
table(dat90[which(dat90$syscall =='read'),]$y)
table(dat90[which(dat90$syscall =='pread64'),]$y)
table(dat90[which(dat90$syscall =='recvfrom'),]$y)
table(dat90[which(dat90$syscall =='rt_sigaction'),]$y)
table(dat90[which(dat90$syscall =='rt_sigprocmask'),]$y)
table(dat90[which(dat90$syscall =='rt_sigpending'),]$y)
table(dat90[which(dat90$syscall =='sendto'),]$y)
table(dat90[which(dat90$syscall =='write'),]$y)
table(dat90[which(dat90$syscall =='pwrite64'),]$y)
table(dat90[which(dat90$syscall =='kill'),]$y)
table(dat90[which(dat90$syscall =='access'),]$y)
table(dat90[which(dat90$syscall =='mq_notify'),]$y)
table(dat90[which(dat90$syscall =='setuid'),]$y)
table(dat90[which(dat90$syscall =='mq_notify'),]$y)
table(dat90[which(dat90$syscall =='mlock'),]$y)
table(dat90[which(dat90$syscall =='mlockall'),]$y)
table(dat90[which(dat90$syscall =='munlock'),]$y)
table(dat90[which(dat90$syscall =='munlockall'),]$y)

#we remove mmap, kill for being suspect to complete separation
# table(dat90[which(dat90$syscall =='mmap'),]$y)
# table(dat90[which(dat90$syscall =='kill'),]$y)
# 
# dat90.clean <- dat90[which(dat90$syscall != 'kill'),]
# dat90.clean <- dat90.clean[which(dat90.clean$syscall != 'mmap'),]
# dat90.clean$syscall <- factor(dat90.clean$syscall) #refactor


#remove non good strategies
#none to remove

