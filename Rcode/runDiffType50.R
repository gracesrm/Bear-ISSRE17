ddat<-read.csv(file = "~/bear/Res/Res_all.csv", check.names=FALSE,
               header = F, col.names=c('program','syscall','stratNo','st','et','y','thresh','type'), quote = "")
ddat$program <- NULL
ddat$stratNo <- factor(ddat$stratNo)

ddat.pr <- ddat[which(ddat$type == 'processor'),]
ddat.ds <- ddat[which(ddat$type == 'disk'),]
ddat.net <- ddat[which(ddat$type == 'network'),]
ddat.enc <- ddat[which(ddat$type == 'encode'),]

ddat.proc <- rbind(ddat.pr, ddat.enc)
ddat.proc <- ddat.proc[which(ddat.proc$thresh == '50'),]
ddat.io <- rbind(ddat.net, ddat.ds)
ddat.io <- ddat.io[which(ddat.io$thresh == '50'),]

# table(ddat.proc)
# table(ddat.io)

ddat.proc$y <- relevel(ddat.proc$y, ref = "N")
ddat.proc$syscall <- relevel(ddat.proc$syscall, ref = "mmap")
ddat.proc$stratNo <- relevel(ddat.proc$stratNo, ref = "2")

ddat.io$y <- relevel(ddat.io$y, ref = "N")
ddat.io$syscall <- relevel(ddat.io$syscall, ref = "mmap")
ddat.io$stratNo <- relevel(ddat.io$stratNo, ref = "2")


library(plyr)

# Start modeling
library(caret)
set.seed(5207) #set a for ddata splitting
trainIndex.proc <- createDataPartition(ddat.proc$y, p = .8,
                                       list = FALSE,
                                       times = 1)
training.proc <- ddat.proc[trainIndex.proc,]
test.proc <- ddat.proc[-trainIndex.proc,]
Control.proc <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)
set.seed(2741)
lr1.proc <- train(y ~ stratNo  , data = training.proc, method = "glm", trControl = Control.proc )
lr2.proc <- train(y ~ syscall  , data = training.proc, method = "glm", trControl = Control.proc )

sink("~/bear/Res/Types/ResProc50_lr1.txt")
summary(lr1.proc)
confusionMatrix(lr1.proc)
varImp(lr1.proc)
exp(confint(lr1.proc$finalModel))
exp(lr1.proc$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResProc50_lr2.txt")
summary(lr2.proc)
confusionMatrix(lr2.proc)
varImp(lr2.proc)
exp(confint(lr2.proc$finalModel))
exp(lr2.proc$finalModel$coefficients)
sink()

library(xlsx)
cos1.proc <- exp(lr1.proc$finalModel$coefficients)
write.xlsx(cos1.proc, "~/bear/Res/Types/coefs1_Proc50.xlsx")
cos2.proc <- exp(lr2.proc$finalModel$coefficients)
write.xlsx(cos2.proc, "~/bear/Res/Types/coefs2_Proc50.xlsx")



set.seed(5207) #set a for ddata splitting
trainIndex.io <- createDataPartition(ddat.io$y, p = .8,
                                     list = FALSE,
                                     times = 1)
training.io <- ddat.io[trainIndex.io,]
test.io <- ddat.io[-trainIndex.io,]
Control.io <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)
set.seed(2741)
lr1.io <- train(y ~ stratNo, data = training.io, method = "glm", trControl = Control.io )
lr2.io <- train(y ~ syscall, data = training.io, method = "glm", trControl = Control.io )

sink("~/bear/Res/Types/ResIo50_lr1.txt")
summary(lr1.io)
confusionMatrix(lr1.io)
varImp(lr1.io)
exp(confint(lr1.io$finalModel))
exp(lr1.io$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResIo50_lr2.txt")
summary(lr2.io)
confusionMatrix(lr2.io)
varImp(lr2.io)
exp(confint(lr2.io$finalModel))
exp(lr2.io$finalModel$coefficients)
sink()

library(xlsx)
cos1.io <- exp(lr1.io$finalModel$coefficients)
write.xlsx(cos1.io, "~/bear/Res/Types/coefs1_Io50.xlsx")
cos2.io <- exp(lr2.io$finalModel$coefficients)
write.xlsx(cos2.io, "~/bear/Res/Types/coefs2_Io50.xlsx")

# ###############################
