rm(ddat.io)
ddat<-read.csv(file = "~/bear/Res/Res_all.csv", check.names=FALSE,
                header = F, col.names=c('program','syscall','stratNo','st','et','y','thresh','type'), quote = "")
ddat$program <- NULL
ddat$stratNo <- factor(ddat$stratNo)

ddat.pr <- ddat[which(ddat$type == 'processor'),]
ddat.ds <- ddat[which(ddat$type == 'disk'),]
ddat.net <- ddat[which(ddat$type == 'network'),]
ddat.enc <- ddat[which(ddat$type == 'encode'),]

ddat.proc <- rbind(ddat.pr, ddat.enc)
ddat.io <- rbind(ddat.net, ddat.ds)

table(ddat.proc)
table(ddat.io)

ddat.proc$y <- relevel(ddat.proc$y, ref = "N")
ddat.proc$syscall <- relevel(ddat.proc$syscall, ref = "mmap")
ddat.proc$stratNo <- relevel(ddat.proc$stratNo, ref = "2")

ddat.io$y <- relevel(ddat.io$y, ref = "N")
ddat.io$syscall <- relevel(ddat.io$syscall, ref = "mremap")
ddat.io$stratNo <- relevel(ddat.io$stratNo, ref = "2")
# ddat.io <- ddat.io[which(ddat.io$syscall != 'mmap'),]

# table(ddat.pr)
# table(ddat.ds)
# table(ddat.net)
# table(ddat.enc)
# 
# table(ddat.pr$y)
# table(ddat.ds$y)
# table(ddat.net$y)
# table(ddat.enc$y)
# 
# ddat.pr$y <- relevel(ddat.pr$y, ref = "N")
# ddat.pr$syscall <- relevel(ddat.pr$syscall, ref = "mmap")
# ddat.pr$stratNo <- relevel(ddat.pr$stratNo, ref = "2")
# 
# ddat.ds$y <- relevel(ddat.ds$y, ref = "N")
# ddat.ds$syscall <- relevel(ddat.ds$syscall, ref = "mmap")
# ddat.ds$stratNo <- relevel(ddat.ds$stratNo, ref = "2")
# 
# ddat.net$y <- relevel(ddat.net$y, ref = "N")
# ddat.net$syscall <- relevel(ddat.net$syscall, ref = "mmap")
# ddat.net$stratNo <- relevel(ddat.net$stratNo, ref = "2")
# 
# ddat.enc$y <- relevel(ddat.enc$y, ref = "N")
# ddat.enc$syscall <- relevel(ddat.enc$syscall, ref = "mmap")
# ddat.enc$stratNo <- relevel(ddat.enc$stratNo, ref = "2")

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

sink("~/bear/Res/Types/ResProc_lr1.txt")
summary(lr1.proc)
confusionMatrix(lr1.proc)
varImp(lr1.proc)
exp(confint(lr1.proc$finalModel))
exp(lr1.proc$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResProc_lr2.txt")
summary(lr2.proc)
confusionMatrix(lr2.proc)
varImp(lr2.proc)
exp(confint(lr2.proc$finalModel))
exp(lr2.proc$finalModel$coefficients)
sink()

library(xlsx)
cos1.proc <- exp(lr1.proc$finalModel$coefficients)
write.xlsx(cos1.proc, "~/bear/Res/Types/coefs1_Proc.xlsx")
cos2.proc <- exp(lr2.proc$finalModel$coefficients)
write.xlsx(cos2.proc, "~/bear/Res/Types/coefs2_Proc.xlsx")



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

sink("~/bear/Res/Types/ResIo_lr1.txt")
summary(lr1.io)
confusionMatrix(lr1.io)
varImp(lr1.io)
exp(confint(lr1.io$finalModel))
exp(lr1.io$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResIo_lr2.txt")
summary(lr2.io)
confusionMatrix(lr2.io)
varImp(lr2.io)
exp(confint(lr2.io$finalModel))
exp(lr2.io$finalModel$coefficients)
sink()

library(xlsx)
cos1.io <- exp(lr1.io$finalModel$coefficients)
write.xlsx(cos1.io, "~/bear/Res/Types/coefs1_Io.xlsx")
cos2.io <- exp(lr2.io$finalModel$coefficients)
write.xlsx(cos2.io, "~/bear/Res/Types/coefs2_Io.xlsx")

# ###############################


library(caret)
set.seed(5207) #set a for ddata splitting
trainIndex.pr <- createDataPartition(ddat.pr$y, p = .8,
                                  list = FALSE,
                                  times = 1)
training.pr <- ddat.pr[trainIndex.pr,]
test.pr <- ddat.pr[-trainIndex.pr,]
Control.pr <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)
set.seed(2741)
lr1.pr <- train(y ~ stratNo  , data = training.pr, method = "glm", trControl = Control.pr )
lr2.pr <- train(y ~ syscall  , data = training.pr, method = "glm", trControl = Control.pr )

sink("~/bear/Res/Types/ResPr_lr1.txt")
summary(lr1.pr)
confusionMatrix(lr1.pr)
varImp(lr1.pr)
exp(confint(lr1.pr$finalModel))
exp(lr1.pr$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResPr_lr2.txt")
summary(lr2.pr)
confusionMatrix(lr2.pr)
varImp(lr2.pr)
exp(confint(lr2.pr$finalModel))
exp(lr2.pr$finalModel$coefficients)
sink()

library(xlsx)
cos1.pr <- exp(lr1.pr$finalModel$coefficients)
write.xlsx(cos1.pr, "~/bear/Res/Types/coefs1_Pr.xlsx")
cos2.pr <- exp(lr2.pr$finalModel$coefficients)
write.xlsx(cos2.pr, "~/bear/Res/Types/coefs2_Pr.xlsx")

#########
trainIndex.ds <- createDataPartition(ddat.ds$y, p = .8,
                                     list = FALSE,
                                     times = 1)
training.ds <- ddat.ds[trainIndex.ds,]
test.ds <- ddat.ds[-trainIndex.ds,]
Control.ds <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)
set.seed(2741)
lr1.ds <- train(y ~ stratNo  , data = training.ds, method = "glm", trControl = Control.ds )
lr2.ds <- train(y ~ syscall  , data = training.ds, method = "glm", trControl = Control.ds )

sink("~/bear/Res/Types/ResDs_lr1.txt")
summary(lr1.ds)
confusionMatrix(lr1.ds)
varImp(lr1.ds)
exp(confint(lr1.ds$finalModel))
exp(lr1.ds$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResPr_lr2.txt")
summary(lr2.ds)
confusionMatrix(lr2.ds)
varImp(lr2.ds)
exp(confint(lr2.ds$finalModel))
exp(lr2.ds$finalModel$coefficients)
sink()

library(xlsx)
cos1.ds <- exp(lr1.ds$finalModel$coefficients)
write.xlsx(cos1.ds, "~/bear/Res/Types/coefs1_Ds.xlsx")
cos2.ds <- exp(lr2.ds$finalModel$coefficients)
write.xlsx(cos2.ds, "~/bear/Res/Types/coefs2_Ds.xlsx")

#######
trainIndex.net <- createDataPartition(ddat.net$y, p = .8,
                                     list = FALSE,
                                     times = 1)
training.net <- ddat.net[trainIndex.net,]
test.net <- ddat.net[-trainIndex.net,]
Control.net <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)
set.seed(2741)
lr1.net <- train(y ~ stratNo  , data = training.net, method = "glm", trControl = Control.net )
lr2.net <- train(y ~ syscall  , data = training.net, method = "glm", trControl = Control.net )

sink("~/bear/Res/Types/ResNet_lr1.txt")
summary(lr1.net)
confusionMatrix(lr1.net)
varImp(lr1.net)
exp(confint(lr1.net$finalModel))
exp(lr1.net$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResNet_lr2.txt")
summary(lr2.net)
confusionMatrix(lr2.net)
varImp(lr2.net)
exp(confint(lr2.net$finalModel))
exp(lr2.net$finalModel$coefficients)
sink()

library(xlsx)
cos1.net <- exp(lr1.net$finalModel$coefficients)
write.xlsx(cos1.net, "~/bear/Res/Types/coefs1_Net.xlsx")
cos2.net <- exp(lr2.net$finalModel$coefficients)
write.xlsx(cos2.net, "~/bear/Res/Types/coefs2_Net.xlsx")

# ######
trainIndex.enc <- createDataPartition(ddat.enc$y, p = .8,
                                     list = FALSE,
                                     times = 1)
training.enc <- ddat.enc[trainIndex.enc,]
test.enc <- ddat.enc[-trainIndex.enc,]
Control.enc <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)
set.seed(2741)
lr1.enc <- train(y ~ stratNo  , data = training.enc, method = "glm", trControl = Control.enc )
lr2.enc <- train(y ~ syscall  , data = training.enc, method = "glm", trControl = Control.enc )

sink("~/bear/Res/Types/ResEnc_lr1.txt")
summary(lr1.enc)
confusionMatrix(lr1.enc)
varImp(lr1.enc)
exp(confint(lr1.enc$finalModel))
exp(lr1.enc$finalModel$coefficients)
sink() 

sink("~/bear/Res/Types/ResEnc_lr2.txt")
summary(lr2.enc)
confusionMatrix(lr2.enc)
varImp(lr2.enc)
exp(confint(lr2.enc$finalModel))
exp(lr2.enc$finalModel$coefficients)
sink()

library(xlsx)
cos1.enc <- exp(lr1.enc$finalModel$coefficients)
write.xlsx(cos1.enc, "~/bear/Res/Types/coefs1_Enc.xlsx")
cos2.enc <- exp(lr2.enc$finalModel$coefficients)
write.xlsx(cos2.enc, "~/bear/Res/Types/coefs2_Enc.xlsx")
