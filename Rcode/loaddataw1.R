# Last modified 5/4/2016.
# 
# Overview: This loads the datw1a and displays some results that I use to manually 
# correct before running the logistic regressions.
#
# Note that some of the code is to be used for my own understanding. 
rm(datw1,datw2,datw3,test,trainIndex,training,Control,cos,cos1,cos2,lr,lr2,lr1)
# Load datw1a
datw1<-read.csv(file = "/Users/ruimin/Dropbox/bear/Res/Resw1.csv", check.names=FALSE,
                header = F, col.names=c('program','syscall','stratNo','st','et','y'), quote = "")
datw1$program <- NULL
datw1$stratNo <- factor(datw1$stratNo)

datw1$y <- relevel(datw1$y, ref = "N")
datw1$syscall <- relevel(datw1$syscall, ref = "mmap")
datw1$stratNo <- relevel(datw1$stratNo, ref = "2")



# Label the strategy number as a categorical variable.
# datw1IO$stratNo <- factor(datw1IO$stratNo)

# datw1CPU<-read.csv(file = "/Users/AndrewLee/Documents/School/SeniorS1/Oliveira/Ruimin/newdatw1a/format_datw1a/datw1a10_cpu.csv", check.names=FALSE,
#                  header = F, col.names=c('y','program','syscall','stratNo'), quote = "")
# datw1CPU$stratNo <- factor(datw1CPU$stratNo)

# Create a new vector that helps identify which type of program it is
# datw1IO$progType <- rep("io",length(datw1IO$y))
# datw1CPU$progType <- rep("cpu",length(datw1CPU$y))

#concatonate
# datw1 <- rbind(datw1IO, datw1CPU);

#make results binary
# datw1$progType <- factor(datw1$progType)

#load correct package
library(plyr)
# redefine variables for simpilfication in the model
# datw1$y<-revalue(datw1$y, c("abort" = "crash"))
# datw1IO$y<-revalue(datw1IO$y, c("abort" = "crash"))
# datw1CPU$y<-revalue(datw1CPU$y, c("abort" = "crash"))
# datw1$stratNo<-revalue(datw1$stratNo, c("3" = "2"))
# datw1IO$stratNo<-revalue(datw1IO$stratNo, c("3" = "2"))
# datw1CPU$stratNo<-revalue(datw1CPU$stratNo, c("3" = "2"))

# sanity check to make sure I did everything correctly
# levels(datw1$y)

#datw1IO.clean <- datw1IO[which(datw1IO$syscall != 'lstat'),]
#datw1IO.clean <- datw1IO.clean[which(datw1IO.clean$syscall != 'mmap'),]
#datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'wait4'),]
#datw1IO.clean$syscall <- factor(datw1IO.clean$syscall) #refactor

table(datw1[which(datw1$stratNo =='1'),]$y)
table(datw1[which(datw1$stratNo =='2'),]$y)
table(datw1[which(datw1$stratNo =='3'),]$y)
table(datw1[which(datw1$stratNo =='4'),]$y)
table(datw1[which(datw1$stratNo =='5'),]$y)
table(datw1[which(datw1$stratNo =='6'),]$y)
table(datw1[which(datw1$stratNo =='7'),]$y)
table(datw1[which(datw1$stratNo =='8'),]$y)
table(datw1[which(datw1$stratNo =='9'),]$y)
table(datw1[which(datw1$stratNo =='10'),]$y)

datw1.clean <- datw1[which(datw1$stratNo != '0'),]
# datw1.clean <- datw1.clean[which(datw1.clean$stratNo != '7'),]
datw1.clean <- datw1.clean[which(datw1.clean$stratNo != '9'),]
datw1.clean <- datw1.clean[which(datw1.clean$stratNo != '10'),]
datw1.clean$stratNo <- factor(datw1.clean$stratNo) #refactor
datw1 <- datw1.clean
#for datw1 all

table(datw1[which(datw1$syscall =='mmap'),]$y)
table(datw1[which(datw1$syscall =='mprotect'),]$y)
table(datw1[which(datw1$syscall =='munmap'),]$y)
table(datw1[which(datw1$syscall =='mremap'),]$y)
table(datw1[which(datw1$syscall =='read'),]$y)
table(datw1[which(datw1$syscall =='pread64'),]$y)
table(datw1[which(datw1$syscall =='recvfrom'),]$y)
table(datw1[which(datw1$syscall =='rt_sigaction'),]$y)
table(datw1[which(datw1$syscall =='rt_sigprocmask'),]$y)
table(datw1[which(datw1$syscall =='rt_sigpending'),]$y)
table(datw1[which(datw1$syscall =='sendto'),]$y)
table(datw1[which(datw1$syscall =='write'),]$y)
table(datw1[which(datw1$syscall =='pwrite64'),]$y)
table(datw1[which(datw1$syscall =='kill'),]$y)
table(datw1[which(datw1$syscall =='access'),]$y)
table(datw1[which(datw1$syscall =='mq_notify'),]$y)
table(datw1[which(datw1$syscall =='setuid'),]$y)
table(datw1[which(datw1$syscall =='mlock'),]$y)
table(datw1[which(datw1$syscall =='mlockall'),]$y)
table(datw1[which(datw1$syscall =='munlock'),]$y)
table(datw1[which(datw1$syscall =='munlockall'),]$y)

datw1.clean <- datw1[which(datw1$syscall != 'mq_notify'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'munmap'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'mremap'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'pread64'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'pwrite64'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'recvfrom'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'sendto'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'kill'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'rt_sigpending'),]
datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'setuid'),]
datw1 <- datw1.clean

# datw1.clean <- datw1[which(datw1$syscall != 'munmap'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'mremap'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'pread64'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'pwrite64'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'recvfrom'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'rt_sigpending'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'sendto'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'kill'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'mlockall'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'munlock'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'munlockall'),]
# datw1.clean$syscall <- factor(datw1.clean$syscall) #refactor
# datw1 <- datw1.clean

#we remove mmap, kill for being suspect to complete separation
# table(datw1[which(datw1$syscall =='mmap'),]$y)
# table(datw1[which(datw1$syscall =='kill'),]$y)
# 
# datw1.clean <- datw1[which(datw1$syscall != 'kill'),]
# datw1.clean <- datw1.clean[which(datw1.clean$syscall != 'mmap'),]
# datw1.clean$syscall <- factor(datw1.clean$syscall) #refactor


#remove non good strategies
#none to remove