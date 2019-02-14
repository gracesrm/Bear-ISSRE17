# Last modified 5/4/2016 by Andrew Lee
# Modifications include comments and suggestions to Ruimin.

# Overview: The code below uses an R package called "caret" to perform a generalized linear model ("glm"). While I used
# caret, an alternatie method (and probably simplier to use) would have been to just call glm(y ~ x, datw1a = datw1a) function
# directly and evaluate the results that way.
# 
# GLM's are used for logistic regressions.
# 
# 


# The line commented out below is an example of me using glm for the logistic regression.
#fit <- glm(y ~ syscall + stratNo+ progType, datw1a =datw1.clean, family = binomial(link = "logit"))

# This loads the package "caret"
library(caret)

set.seed(5207) #set a for datw1a splitting

trainIndex <- createDataPartition(datw1$y, p = .8,
                                  list = FALSE,
                                  times = 1)
training <- datw1[trainIndex,]
test <- datw1[-trainIndex,]

Control <- trainControl(method = "repeatedcv", number = 3, repeats = 2, verboseIter = T)

# set.seed here actually does nothing
set.seed(2741)

lr <- train(y ~ syscall + stratNo  , data = training, method = "glm", trControl = Control )
lr1 <- train(y ~ stratNo  , data = training, method = "glm", trControl = Control )
lr2 <- train(y ~ syscall  , data = training, method = "glm", trControl = Control )

 sink("/Users/ruimin/Dropbox/bear/Res/Resw1_lr.txt")
 summary(lr)
 confusionMatrix(lr)
 varImp(lr)
 exp(confint(lr$finalModel))
 exp(lr$finalModel$coefficients)
 sink()
 
 sink("/Users/ruimin/Dropbox/bear/Res/Resw1_lr1.txt")
 summary(lr1)
 confusionMatrix(lr1)
 varImp(lr1)
 exp(confint(lr1$finalModel))
 exp(lr1$finalModel$coefficients)
 sink() 
 
 sink("/Users/ruimin/Dropbox/bear/Res/Resw1_lr2.txt")
 summary(lr2)
 confusionMatrix(lr2)
 varImp(lr2)
 exp(confint(lr2$finalModel))
 exp(lr2$finalModel$coefficients)
 sink()
 
 library(xlsx)
 cos <- exp(lr$finalModel$coefficients)
 write.xlsx(cos, "/Users/ruimin/Dropbox/bear/Res/coefs_w1.xlsx")
 
 library(xlsx)
 cos1 <- exp(lr1$finalModel$coefficients)
 write.xlsx(cos1, "/Users/ruimin/Dropbox/bear/Res/coefs1_w1.xlsx")
 cos2 <- exp(lr2$finalModel$coefficients)
 write.xlsx(cos2, "/Users/ruimin/Dropbox/bear/Res/coefs2_w1.xlsx")
 
# sink() outputs the console datw1a into a text file
# sink("/Users/ruimin/Dropbox/bear/Res/Resw1_lr1.txt")
# Summary of the variables. I wanted to see which variables are significant, not significant.
# summary(lr1)
# Confusion matrix tells me how well the predictions were.
# confusionMatrix(lr1)
# Function in caret that tells me what variables are important and how important with respect to 
# the other variables. Type "?varImp" for manual
# varImp(lr1)
# This creates the odds ratios confdience intervals.
# exp(confint(lr1$finalModel))
# This creates a single odds ratio.
# exp(lr1$finalModel$coefficients)
# Stops sink function
# sink()

#glmnet <- train(y ~ progType + syscall + stratNo  , datw1a = training, method = "glmnet", trControl = Control )

#rf <- train(y ~ progType + syscall + stratNo  , datw1a = training, method = "rf", trControl = Control )
