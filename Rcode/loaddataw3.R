# Last modified 5/4/2016.
# 
# Overview: This loads the datw3a and displays some results that I use to manually 
# correct before running the logistic regressions.
#
# Note that some of the code is to be used for my own understanding. 

rm(dat,datw3,dat50,dat90,test,trainIndex,training,Control,cos,cos1,cos2,lr,lr2,lr1)

# Load datw3a
datw3<-read.csv(file = "/Users/ruimin/Dropbox/bear/Res/Resw3.csv", check.names=FALSE,
                header = F, col.names=c('program','syscall','stratNo','st','et','y'), quote = "")
datw3$program <- NULL
datw3$stratNo <- factor(datw3$stratNo)

datw3$y <- relevel(datw3$y, ref = "N")
datw3$syscall <- relevel(datw3$syscall, ref = "mmap")
datw3$stratNo <- relevel(datw3$stratNo, ref = "2")

# Label the strategy number as a categorical variable.
# datw3IO$stratNo <- factor(datw3IO$stratNo)

# datw3CPU<-read.csv(file = "/Users/AndrewLee/Documents/School/SeniorS1/Oliveira/Ruimin/newdatw3a/format_datw3a/datw3a10_cpu.csv", check.names=FALSE,
#                  header = F, col.names=c('y','program','syscall','stratNo'), quote = "")
# datw3CPU$stratNo <- factor(datw3CPU$stratNo)

# Create a new vector that helps identify which type of program it is
# datw3IO$progType <- rep("io",length(datw3IO$y))
# datw3CPU$progType <- rep("cpu",length(datw3CPU$y))

#concatonate
# datw3 <- rbind(datw3IO, datw3CPU);

#make results binary
# datw3$progType <- factor(datw3$progType)

#load correct package
library(plyr)
# redefine variables for simpilfication in the model
# datw3$y<-revalue(datw3$y, c("abort" = "crash"))
# datw3IO$y<-revalue(datw3IO$y, c("abort" = "crash"))
# datw3CPU$y<-revalue(datw3CPU$y, c("abort" = "crash"))
# datw3$stratNo<-revalue(datw3$stratNo, c("3" = "2"))
# datw3IO$stratNo<-revalue(datw3IO$stratNo, c("3" = "2"))
# datw3CPU$stratNo<-revalue(datw3CPU$stratNo, c("3" = "2"))

# sanity check to make sure I did everything correctly
# levels(datw3$y)

#datw3IO.clean <- datw3IO[which(datw3IO$syscall != 'lstat'),]
#datw3IO.clean <- datw3IO.clean[which(datw3IO.clean$syscall != 'mmap'),]
#datw3.clean <- datw3.clean[which(datw3.clean$syscall != 'wait4'),]
#datw3IO.clean$syscall <- factor(datw3IO.clean$syscall) #refactor

table(datw3[which(datw3$stratNo =='1'),]$y)
table(datw3[which(datw3$stratNo =='2'),]$y)
table(datw3[which(datw3$stratNo =='3'),]$y)
table(datw3[which(datw3$stratNo =='4'),]$y)
table(datw3[which(datw3$stratNo =='5'),]$y)
table(datw3[which(datw3$stratNo =='6'),]$y)
table(datw3[which(datw3$stratNo =='7'),]$y)
table(datw3[which(datw3$stratNo =='8'),]$y)
table(datw3[which(datw3$stratNo =='9'),]$y)
table(datw3[which(datw3$stratNo =='10'),]$y)

datw3.clean <- datw3[which(datw3$stratNo != '0'),]
datw3.clean <- datw3.clean[which(datw3.clean$stratNo != '9'),]
datw3.clean <- datw3.clean[which(datw3.clean$stratNo != '10'),]
datw3.clean$stratNo <- factor(datw3.clean$stratNo) #refactor
datw3 <- datw3.clean

# datw3CPU.clean <- datw3CPU.clean[which(datw3CPU.clean$syscall != 'kill'),]
# datw3CPU.clean$syscall <- factor(datw3CPU.clean$syscall) #refactor

#for datw3 all

table(datw3[which(datw3$syscall =='mmap'),]$y)
table(datw3[which(datw3$syscall =='mprotect'),]$y)
table(datw3[which(datw3$syscall =='munmap'),]$y)
table(datw3[which(datw3$syscall =='mremap'),]$y)
table(datw3[which(datw3$syscall =='read'),]$y)
table(datw3[which(datw3$syscall =='pread64'),]$y)
table(datw3[which(datw3$syscall =='recvfrom'),]$y)
table(datw3[which(datw3$syscall =='rt_sigaction'),]$y)
table(datw3[which(datw3$syscall =='rt_sigprocmask'),]$y)
table(datw3[which(datw3$syscall =='rt_sigpending'),]$y)
table(datw3[which(datw3$syscall =='sendto'),]$y)
table(datw3[which(datw3$syscall =='write'),]$y)
table(datw3[which(datw3$syscall =='pwrite64'),]$y)
table(datw3[which(datw3$syscall =='kill'),]$y)
table(datw3[which(datw3$syscall =='access'),]$y)
table(datw3[which(datw3$syscall =='mq_notify'),]$y)
table(datw3[which(datw3$syscall =='setuid'),]$y)
table(datw3[which(datw3$syscall =='mq_notify'),]$y)
table(datw3[which(datw3$syscall =='mlock'),]$y)
table(datw3[which(datw3$syscall =='mlockall'),]$y)
table(datw3[which(datw3$syscall =='munlock'),]$y)
table(datw3[which(datw3$syscall =='munlockall'),]$y)

#we remove mmap, kill for being suspect to complete separation
# table(datw3[which(datw3$syscall =='mmap'),]$y)
# table(datw3[which(datw3$syscall =='kill'),]$y)
# 
datw3.clean <- datw3[which(datw3$syscall != 'mq_notify'),]
datw3.clean <- datw3.clean[which(datw3.clean$syscall != 'mremap'),]
datw3.clean <- datw3.clean[which(datw3.clean$syscall != 'pread64'),]
datw3.clean <- datw3.clean[which(datw3.clean$syscall != 'pwrite64'),]
datw3.clean <- datw3.clean[which(datw3.clean$syscall != 'kill'),]
datw3.clean <- datw3.clean[which(datw3.clean$syscall != 'rt_sigpending'),]
datw3.clean <- datw3.clean[which(datw3.clean$syscall != 'setuid'),]
datw3.clean$syscall <- factor(datw3.clean$syscall) #refactor
datw3 <- datw3.clean


#remove non good strategies
#none to remove

