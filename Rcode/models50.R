# Last modified 5/4/2016 by Andrew Lee
# Modifications include comments and suggestions to Ruimin.

# Overview: The code below uses an R package called "caret" to perform a generalized linear model ("glm"). While I used
# caret, an alternatie method (and probably simplier to use) would have been to just call glm(y ~ x, data = data) function
# directly and evaluate the results that way.
# 
# GLM's are used for logistic regressions.
# 
# 


# The line commented out below is an example of me using glm for the logistic regression.
#fit <- glm(y ~ syscall + stratNo+ progType, data =dat.clean, family = binomial(link = "logit"))

# This loads the package "caret"
library(caret)

# Caret requires the use of a training set and a test set b/c of machine learning. The code below
# creates a training and test set for full dataset IO + CPU
set.seed(5207) #set a for data splitting

# trainIndex <- createDataPartition(dat.clean$y, p = .8,
#                                   list = FALSE,
#                                   times = 1)
# training <- dat.clean[trainIndex,]
# test <- dat.clean[-trainIndex,]
trainIndex <- createDataPartition(dat50$y, p = .8,
                                  list = FALSE,
                                  times = 1)
training <- dat50[trainIndex,]
test <- dat50[-trainIndex,]

# Doing the same thing except for IO dataset now
# set.seed(8931)
# trainIndexIO <- createDataPartition(datIO.clean$y, p = .8,
#                                     list = FALSE,
#                                     times = 1)
# trainingIO <- datIO.clean[trainIndexIO,]
# testIO <- datIO.clean[-trainIndexIO,]

# Same thing but for CPU dataset
# set.seed(1512)
# trainIndexCPU <- createDataPartition(datCPU.clean$y, p = .8,
#                                     list = FALSE,
#                                     times = 1)
# trainingCPU <- datCPU.clean[trainIndexCPU,]
# testCPU <- datCPU.clean[-trainIndexCPU,]

# A special function necessary for the caret package. Here we define the sampling method
# to be cross validation with 10 folds with 5 repetitions. 
Control <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)

# set.seed here actually does nothing
set.seed(2741)
# Instead of using the glm function (see line 17), I use the caret function "train" which runs multiple glm models
# all with different sample sizes
lr <- train(y ~ syscall + stratNo  , data = training, method = "glm", trControl = Control )
lr1 <- train(y ~ stratNo  , data = training, method = "glm", trControl = Control )
lr2 <- train(y ~ syscall  , data = training, method = "glm", trControl = Control )
# Data changes
# set.seed(3522)
# lrIO <- train(y ~ syscall + stratNo  , data = trainingIO, method = "glm", trControl = Control )
# 
# set.seed(8133)
# lrCPU <- train(y ~ syscall + stratNo  , data = trainingCPU, method = "glm", trControl = Control )

######
# Now that the models are done running, I computed and analyzed the results.

 sink("~/bear/Res/Res50_lr.txt")
 summary(lr)
 confusionMatrix(lr)
 varImp(lr)
 exp(confint(lr$finalModel))
 exp(lr$finalModel$coefficients)
 sink()
 
 sink("~/bear/Res/Res50_lr1.txt")
 summary(lr1)
 confusionMatrix(lr1)
 varImp(lr1)
 exp(confint(lr1$finalModel))
 exp(lr1$finalModel$coefficients)
 sink() 
 
 sink("~/bear/Res/Res50_lr2.txt")
 summary(lr2)
 confusionMatrix(lr2)
 varImp(lr2)
 exp(confint(lr2$finalModel))
 exp(lr2$finalModel$coefficients)
 sink()
 
 library(xlsx)
 cos <- exp(lr$finalModel$coefficients)
 write.xlsx(cos, "~/bear/Res/coefs_50.xlsx")
 
 library(xlsx)
 cos1 <- exp(lr1$finalModel$coefficients)
 write.xlsx(cos1, "~/bear/Res/coefs1_50.xlsx")
 cos2 <- exp(lr2$finalModel$coefficients)
 write.xlsx(cos2, "~/bear/Res/coefs2_50.xlsx")
 
# sink() outputs the console data into a text file
sink("~/bear/Res/Res50_lr1.txt")
# Summary of the variables. I wanted to see which variables are significant, not significant.
summary(lr1)
# Confusion matrix tells me how well the predictions were.
confusionMatrix(lr1)
# Function in caret that tells me what variables are important and how important with respect to 
# the other variables. Type "?varImp" for manual
varImp(lr1)
# This creates the odds ratios confdience intervals.
exp(confint(lr1$finalModel))
# This creates a single odds ratio.
exp(lr1$finalModel$coefficients)
# Stops sink function
sink()

#glmnet <- train(y ~ progType + syscall + stratNo  , data = training, method = "glmnet", trControl = Control )

#rf <- train(y ~ progType + syscall + stratNo  , data = training, method = "rf", trControl = Control )
