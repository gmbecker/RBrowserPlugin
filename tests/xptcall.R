library(RFirefox)
library(RAutoGenRunTime)
if(FALSE) {
dyn.load("RFirefox.so")
source("../R/classes.R")
source("../R/variant.R")
source("../R/nsXPTType.R")
source("../R/nsXPTCVariantFlags.R")
source("../R/xptcall.R")
}

obj = .Call("R_new_InvokeTestTarget")

arg1 = variant(2L, T_I32)
arg2 = variant(5L, T_I32)
arg3 = variant(list(), T_I32, PTR_IS_DATA)

.xpcomCall(obj, 3L, .args = list(arg1, arg2, arg3), inout = c(TRUE, TRUE, FALSE))

#.xpcomCall(obj, 3L, list(2L, 5L), c(TRUE, TRUE, FALSE), types, 3L)

