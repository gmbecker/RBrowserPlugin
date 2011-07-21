dyn.load("../src/RFirefox.so")
library(RAutoGenRunTime)
source("../R/nsXPTType.R")
source("../R/variant.R")

v = variant()
setVariant(v, 3L, T_I32)
print(getVariant(v))
setVariant(v, 101L, T_I32)
print(getVariant(v))



