#include "WebR.h"

void RCallQueue::lock()
{
  this->isLocked = 1;
}

void RCallQueue::unlock()
{
  this->isLocked = 0;
}

SEXP RCallQueue::requestRCall(SEXP toeval, SEXP env, int *err, NPP inst)
{
  SEXP ans;
  //This seems silly...
  fprintf(stderr, "in RCallQueue stub requestRCall\n");fflush(stderr);
  PROTECT(ans = R_tryEval(toeval, env, err));
  UNPROTECT(1);
  return ans;
}

SEXP RCallQueue::requestRLookup(const char *name)
{
  fprintf(stderr, "in RCallQueue stub requestRLookup\n");fflush(stderr);
  SEXP ans;
  //this seems silly...
  PROTECT(ans = Rf_findVar(Rf_install(name), R_GlobalEnv));
  UNPROTECT(1);
  return ans;
}  

void RCallQueue::waitInQueue(int32_t spot)
{
 
}

void RCallQueue::advanceQueue(int32_t spot)
{
  
    

}

int32_t RCallQueue::enterQueue()
{
}

void RCallQueue::init()
{

}

