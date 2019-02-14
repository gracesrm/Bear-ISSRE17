# Last modified 5/4/2016.
# 
# Overview: This loads the dat10a and displays some results that I use to manually 
# correct before running the logistic regressions.
#
# Note that some of the code is to be used for my own understanding. 

rm(dat,dat10,dat50,dat90,test,trainIndex,training,Control,cos,cos1,cos2,lr,lr2,lr1)

# Load dat10a
dat10<-read.csv(file = "~/bear/Res/Res10.csv", check.names=FALSE,
                header = F, col.names=c('program','syscall','stratNo','st','et','y'), quote = "")
dat10$program <- NULL
dat10$stratNo <- factor(dat10$stratNo)

dat10$y <- relevel(dat10$y, ref = "N")
dat10$syscall <- relevel(dat10$syscall, ref = "mmap")
dat10$stratNo <- relevel(dat10$stratNo, ref = "2")

# Label the strategy number as a categorical variable.
# dat10IO$stratNo <- factor(dat10IO$stratNo)

# dat10CPU<-read.csv(file = "~/newdat10a/format_dat10a/dat10a10_cpu.csv", check.names=FALSE,
#                  header = F, col.names=c('y','program','syscall','stratNo'), quote = "")
# dat10CPU$stratNo <- factor(dat10CPU$stratNo)

# Create a new vector that helps identify which type of program it is
# dat10IO$progType <- rep("io",length(dat10IO$y))
# dat10CPU$progType <- rep("cpu",length(dat10CPU$y))

#concatonate
# dat10 <- rbind(dat10IO, dat10CPU);

#make results binary
# dat10$progType <- factor(dat10$progType)

#load correct package
library(plyr)
# redefine variables for simpilfication in the model
# dat10$y<-revalue(dat10$y, c("abort" = "crash"))
# dat10IO$y<-revalue(dat10IO$y, c("abort" = "crash"))
# dat10CPU$y<-revalue(dat10CPU$y, c("abort" = "crash"))
# dat10$stratNo<-revalue(dat10$stratNo, c("3" = "2"))
# dat10IO$stratNo<-revalue(dat10IO$stratNo, c("3" = "2"))
# dat10CPU$stratNo<-revalue(dat10CPU$stratNo, c("3" = "2"))

# sanity check to make sure I did everything correctly
# levels(dat10$y)

#dat10IO.clean <- dat10IO[which(dat10IO$syscall != 'lstat'),]
#dat10IO.clean <- dat10IO.clean[which(dat10IO.clean$syscall != 'mmap'),]
#dat10.clean <- dat10.clean[which(dat10.clean$syscall != 'wait4'),]
#dat10IO.clean$syscall <- factor(dat10IO.clean$syscall) #refactor

table(dat10[which(dat10$stratNo =='1'),]$y)
table(dat10[which(dat10$stratNo =='2'),]$y)
table(dat10[which(dat10$stratNo =='3'),]$y)
table(dat10[which(dat10$stratNo =='4'),]$y)
table(dat10[which(dat10$stratNo =='5'),]$y)
table(dat10[which(dat10$stratNo =='6'),]$y)
table(dat10[which(dat10$stratNo =='7'),]$y)
table(dat10[which(dat10$stratNo =='8'),]$y)
table(dat10[which(dat10$stratNo =='9'),]$y)
table(dat10[which(dat10$stratNo =='10'),]$y)


# dat10CPU.clean <- dat10CPU.clean[which(dat10CPU.clean$syscall != 'kill'),]
# dat10CPU.clean$syscall <- factor(dat10CPU.clean$syscall) #refactor

#for dat10 all

table(dat10[which(dat10$syscall =='mmap'),]$y)
table(dat10[which(dat10$syscall =='mprotect'),]$y)
table(dat10[which(dat10$syscall =='munmap'),]$y)
table(dat10[which(dat10$syscall =='mremap'),]$y)
table(dat10[which(dat10$syscall =='read'),]$y)
table(dat10[which(dat10$syscall =='pread64'),]$y)
table(dat10[which(dat10$syscall =='recvfrom'),]$y)
table(dat10[which(dat10$syscall =='rt_sigaction'),]$y)
table(dat10[which(dat10$syscall =='rt_sigprocmask'),]$y)
table(dat10[which(dat10$syscall =='rt_sigpending'),]$y)
table(dat10[which(dat10$syscall =='sendto'),]$y)
table(dat10[which(dat10$syscall =='write'),]$y)
table(dat10[which(dat10$syscall =='pwrite64'),]$y)
table(dat10[which(dat10$syscall =='kill'),]$y)
table(dat10[which(dat10$syscall =='access'),]$y)
table(dat10[which(dat10$syscall =='mq_notify'),]$y)
table(dat10[which(dat10$syscall =='setuid'),]$y)
table(dat10[which(dat10$syscall =='mq_notify'),]$y)
table(dat10[which(dat10$syscall =='mlock'),]$y)
table(dat10[which(dat10$syscall =='mlockall'),]$y)
table(dat10[which(dat10$syscall =='munlock'),]$y)
table(dat10[which(dat10$syscall =='munlockall'),]$y)

#we remove mmap, kill for being suspect to complete separation
# table(dat10[which(dat10$syscall =='mmap'),]$y)
# table(dat10[which(dat10$syscall =='kill'),]$y)
# 
# dat10.clean <- dat10[which(dat10$syscall != 'kill'),]
# dat10.clean <- dat10.clean[which(dat10.clean$syscall != 'mmap'),]
# dat10.clean$syscall <- factor(dat10.clean$syscall) #refactor


#remove non good strategies
#none to remove
