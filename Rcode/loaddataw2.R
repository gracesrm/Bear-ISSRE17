# Last modified 5/4/2016.
# 
# Overview: This loads the datw2a and displays some results that I use to manually 
# correct before running the logistic regressions.
#
# Note that some of the code is to be used for my own understanding. 

rm(dat,datw2,dat50,dat90,test,trainIndex,training,Control,cos,cos1,cos2,lr,lr2,lr1)

# Load datw2a
datw2<-read.csv(file = "~/bear/Res/Resw2.csv", check.names=FALSE,
                header = F, col.names=c('program','syscall','stratNo','st','et','y'), quote = "")
datw2$program <- NULL
datw2$stratNo <- factor(datw2$stratNo)

datw2$y <- relevel(datw2$y, ref = "N")
datw2$syscall <- relevel(datw2$syscall, ref = "mmap")
datw2$stratNo <- relevel(datw2$stratNo, ref = "2")

# Label the strategy number as a categorical variable.
# datw2IO$stratNo <- factor(datw2IO$stratNo)

# datw2CPU<-read.csv(file = "~/newdatw2a/format_datw2a/datw2a10_cpu.csv", check.names=FALSE,
#                  header = F, col.names=c('y','program','syscall','stratNo'), quote = "")
# datw2CPU$stratNo <- factor(datw2CPU$stratNo)

# Create a new vector that helps identify which type of program it is
# datw2IO$progType <- rep("io",length(datw2IO$y))
# datw2CPU$progType <- rep("cpu",length(datw2CPU$y))

#concatonate
# datw2 <- rbind(datw2IO, datw2CPU);

#make results binary
# datw2$progType <- factor(datw2$progType)

#load correct package
library(plyr)
# redefine variables for simpilfication in the model
# datw2$y<-revalue(datw2$y, c("abort" = "crash"))
# datw2IO$y<-revalue(datw2IO$y, c("abort" = "crash"))
# datw2CPU$y<-revalue(datw2CPU$y, c("abort" = "crash"))
# datw2$stratNo<-revalue(datw2$stratNo, c("3" = "2"))
# datw2IO$stratNo<-revalue(datw2IO$stratNo, c("3" = "2"))
# datw2CPU$stratNo<-revalue(datw2CPU$stratNo, c("3" = "2"))

# sanity check to make sure I did everything correctly
# levels(datw2$y)

#datw2IO.clean <- datw2IO[which(datw2IO$syscall != 'lstat'),]
#datw2IO.clean <- datw2IO.clean[which(datw2IO.clean$syscall != 'mmap'),]
#datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'wait4'),]
#datw2IO.clean$syscall <- factor(datw2IO.clean$syscall) #refactor

table(datw2[which(datw2$stratNo =='1'),]$y)
table(datw2[which(datw2$stratNo =='2'),]$y)
table(datw2[which(datw2$stratNo =='3'),]$y)
table(datw2[which(datw2$stratNo =='4'),]$y)
table(datw2[which(datw2$stratNo =='5'),]$y)
table(datw2[which(datw2$stratNo =='6'),]$y)
table(datw2[which(datw2$stratNo =='7'),]$y)
table(datw2[which(datw2$stratNo =='8'),]$y)
table(datw2[which(datw2$stratNo =='9'),]$y)
table(datw2[which(datw2$stratNo =='10'),]$y)

datw2.clean <- datw2[which(datw2$stratNo != '0'),]
datw2.clean <- datw2.clean[which(datw2.clean$stratNo != '7'),]
datw2.clean <- datw2.clean[which(datw2.clean$stratNo != '9'),]
datw2.clean <- datw2.clean[which(datw2.clean$stratNo != '10'),]
datw2.clean$stratNo <- factor(datw2.clean$stratNo) #refactor
datw2 <- datw2.clean

# datw2CPU.clean <- datw2CPU.clean[which(datw2CPU.clean$syscall != 'kill'),]
# datw2CPU.clean$syscall <- factor(datw2CPU.clean$syscall) #refactor

#for datw2 all

table(datw2[which(datw2$syscall =='mmap'),]$y)
table(datw2[which(datw2$syscall =='mprotect'),]$y)
table(datw2[which(datw2$syscall =='munmap'),]$y)
table(datw2[which(datw2$syscall =='mremap'),]$y)
table(datw2[which(datw2$syscall =='read'),]$y)
table(datw2[which(datw2$syscall =='pread64'),]$y)
table(datw2[which(datw2$syscall =='recvfrom'),]$y)
table(datw2[which(datw2$syscall =='rt_sigaction'),]$y)
table(datw2[which(datw2$syscall =='rt_sigprocmask'),]$y)
table(datw2[which(datw2$syscall =='rt_sigpending'),]$y)
table(datw2[which(datw2$syscall =='sendto'),]$y)
table(datw2[which(datw2$syscall =='write'),]$y)
table(datw2[which(datw2$syscall =='pwrite64'),]$y)
table(datw2[which(datw2$syscall =='kill'),]$y)
table(datw2[which(datw2$syscall =='access'),]$y)
table(datw2[which(datw2$syscall =='mq_notify'),]$y)
table(datw2[which(datw2$syscall =='setuid'),]$y)
table(datw2[which(datw2$syscall =='mq_notify'),]$y)
table(datw2[which(datw2$syscall =='mlock'),]$y)
table(datw2[which(datw2$syscall =='mlockall'),]$y)
table(datw2[which(datw2$syscall =='munlock'),]$y)
table(datw2[which(datw2$syscall =='munlockall'),]$y)

#we remove mmap, kill for being suspect to complete separation
# table(datw2[which(datw2$syscall =='mmap'),]$y)
# table(datw2[which(datw2$syscall =='kill'),]$y)
# 
datw2.clean <- datw2[which(datw2$syscall != 'mq_notify'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'munmap'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'mremap'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'pread64'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'pwrite64'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'recvfrom'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'sendto'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'kill'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'rt_sigpending'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'setuid'),]
datw2.clean <- datw2.clean[which(datw2.clean$syscall != 'access'),]
datw2 <- datw2.clean

#remove non good strategies
#none to remove
