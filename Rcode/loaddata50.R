# Last modified 5/4/2016.
# 
# Overview: This loads the dat50a and displays some results that I use to manually 
# correct before running the logistic regressions.
#
# Note that some of the code is to be used for my own understanding. 


# Load dat50a
dat50<-read.csv(file = "/Users/ruimin/Dropbox/bear/Res/Res50.csv", check.names=FALSE,
                header = F, col.names=c('program','syscall','stratNo','st','et','y'), quote = "")
dat50$program <- NULL
dat50$stratNo <- factor(dat50$stratNo)

library(MASS)
tbl = table(dat50$y, dat50$syscall)
tbl1= table(dat50$y, dat50$stratNo)

chisq.test(tbl) 
chisq.test(tbl1) 

dat50$y <- relevel(dat50$y, ref = "N")
dat50$syscall <- relevel(dat50$syscall, ref = "mmap")
dat50$stratNo <- relevel(dat50$stratNo, ref = "2")

# Label the strategy number as a categorical variable.
# dat50IO$stratNo <- factor(dat50IO$stratNo)

# dat50CPU<-read.csv(file = "/Users/AndrewLee/Documents/School/SeniorS1/Oliveira/Ruimin/newdat50a/format_dat50a/dat50a50_cpu.csv", check.names=FALSE,
#                  header = F, col.names=c('y','program','syscall','stratNo'), quote = "")
# dat50CPU$stratNo <- factor(dat50CPU$stratNo)

# Create a new vector that helps identify which type of program it is
# dat50IO$progType <- rep("io",length(dat50IO$y))
# dat50CPU$progType <- rep("cpu",length(dat50CPU$y))

#concatonate
# dat50 <- rbind(dat50IO, dat50CPU);

#make results binary
# dat50$progType <- factor(dat50$progType)

#load correct package
library(plyr)
# redefine variables for simpilfication in the model
# dat50$y<-revalue(dat50$y, c("abort" = "crash"))
# dat50IO$y<-revalue(dat50IO$y, c("abort" = "crash"))
# dat50CPU$y<-revalue(dat50CPU$y, c("abort" = "crash"))
# dat50$stratNo<-revalue(dat50$stratNo, c("3" = "2"))
# dat50IO$stratNo<-revalue(dat50IO$stratNo, c("3" = "2"))
# dat50CPU$stratNo<-revalue(dat50CPU$stratNo, c("3" = "2"))

# sanity check to make sure I did everything correctly
# levels(dat50$y)

#dat50IO.clean <- dat50IO[which(dat50IO$syscall != 'lstat'),]
#dat50IO.clean <- dat50IO.clean[which(dat50IO.clean$syscall != 'mmap'),]
#dat50.clean <- dat50.clean[which(dat50.clean$syscall != 'wait4'),]
#dat50IO.clean$syscall <- factor(dat50IO.clean$syscall) #refactor

table(dat50[which(dat50$stratNo =='1'),]$y)
table(dat50[which(dat50$stratNo =='2'),]$y)
table(dat50[which(dat50$stratNo =='3'),]$y)
table(dat50[which(dat50$stratNo =='4'),]$y)
table(dat50[which(dat50$stratNo =='5'),]$y)
table(dat50[which(dat50$stratNo =='6'),]$y)
table(dat50[which(dat50$stratNo =='7'),]$y)
table(dat50[which(dat50$stratNo =='8'),]$y)
table(dat50[which(dat50$stratNo =='9'),]$y)
table(dat50[which(dat50$stratNo =='10'),]$y)


# dat50CPU.clean <- dat50CPU.clean[which(dat50CPU.clean$syscall != 'kill'),]
# dat50CPU.clean$syscall <- factor(dat50CPU.clean$syscall) #refactor

#for dat50 all

table(dat50[which(dat50$syscall =='mmap'),]$y)
table(dat50[which(dat50$syscall =='mprotect'),]$y)
table(dat50[which(dat50$syscall =='munmap'),]$y)
table(dat50[which(dat50$syscall =='mremap'),]$y)
table(dat50[which(dat50$syscall =='read'),]$y)
table(dat50[which(dat50$syscall =='pread64'),]$y)
table(dat50[which(dat50$syscall =='recvfrom'),]$y)
table(dat50[which(dat50$syscall =='rt_sigaction'),]$y)
table(dat50[which(dat50$syscall =='rt_sigprocmask'),]$y)
table(dat50[which(dat50$syscall =='rt_sigpending'),]$y)
table(dat50[which(dat50$syscall =='sendto'),]$y)
table(dat50[which(dat50$syscall =='write'),]$y)
table(dat50[which(dat50$syscall =='pwrite64'),]$y)
table(dat50[which(dat50$syscall =='kill'),]$y)
table(dat50[which(dat50$syscall =='access'),]$y)
table(dat50[which(dat50$syscall =='mq_notify'),]$y)
table(dat50[which(dat50$syscall =='setuid'),]$y)
table(dat50[which(dat50$syscall =='mq_notify'),]$y)
table(dat50[which(dat50$syscall =='mlock'),]$y)
table(dat50[which(dat50$syscall =='mlockall'),]$y)
table(dat50[which(dat50$syscall =='munlock'),]$y)
table(dat50[which(dat50$syscall =='munlockall'),]$y)

#we remove mmap, kill for being suspect to complete separation
# table(dat50[which(dat50$syscall =='mmap'),]$y)
# table(dat50[which(dat50$syscall =='kill'),]$y)
# 
# dat50.clean <- dat50[which(dat50$syscall != 'kill'),]
# dat50.clean <- dat50.clean[which(dat50.clean$syscall != 'mmap'),]
# dat50.clean$syscall <- factor(dat50.clean$syscall) #refactor


#remove non good strategies
#none to remove

