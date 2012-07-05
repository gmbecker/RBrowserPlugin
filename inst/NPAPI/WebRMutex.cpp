#include "WebR.h"

void RCallQueue::lock()
{
  this->isLocked = 1;
}

void RCallQueue::unlock()
{
  this->isLocked = 0;
}

SEXP
RCallQueue::requestRCall(SEXP toeval, SEXP env, int *err, NPP inst)
{
  uint64_t spot = 0;
  spot = this->enterQueue();
  
  this->waitInQueue(spot);
  
  if(inst)
    {
      //update PluginInstance and JS global R variables
      makeRGlobals(inst);
    }
  
  SEXP ans, call;

  PROTECT(call = toeval);  
  Rf_PrintValue(call);
  Rf_PrintValue(env);
  
  PROTECT( ans = R_tryEval(toeval, env, err));
  if(err)
    ans = R_NilValue;
  else
    {
      if(TYPEOF(ans) == PROMSXP)
	ans = R_tryEval(ans, env, err);
      //Unlock R and reset queue if this was the last request.
    } 

  this->advanceQueue(spot);
  UNPROTECT(1);
  return ans;
  }

SEXP RCallQueue::requestRLookup(const char *name)
{
  uint64_t spot = 0;
  spot = this->enterQueue();
  
  this->waitInQueue(spot);
  
  SEXP ans;
  int err = 0;
  ans = Rf_findVar( Rf_install(name), R_GlobalEnv);
  if(TYPEOF(ans) == PROMSXP)
    ans = R_tryEval(ans, R_GlobalEnv, &err);
  
  this->advanceQueue(spot);
  
  return ans;
}

void RCallQueue::waitInQueue(uint64_t spot)
{
  sigset_t mask, oldmask;
  //If R is already busy, wait until our turn comes
  if(this->isLocked)
    { 
      //http://www.gnu.org/software/libc/manual/html_node/Sigsuspend.html#Sigsuspend
      fprintf(stderr, "\nR is in use. Waiting in Queue spot %ld", spot);fflush(stderr);
      sigemptyset (&mask);
      sigaddset (&mask, SIGUSR1);
      
      sigprocmask(SIG_BLOCK, &mask, &oldmask);
      //wait until our turn comes up
      while(this->serving != spot)
	sigsuspend(&oldmask);
     
      sigprocmask(SIG_UNBLOCK, &oldmask, NULL);
    }
  else
    {
      //if R wasn't busy before, lock it to indicate it is busy now
      this->lock();
      this->serving = spot;
    }
}

void RCallQueue::advanceQueue(uint64_t spot)
{
  
  fprintf(stderr, "\nDone serving spot %ld. LastInQueue:%ld", spot, this->lastInQueue);fflush(stderr);
  if(this->lastInQueue == spot)
    {
      this->serving = 1;
      this->lastInQueue = 0;
      this->unlock();
    } else {
    //serve the next request;
    this->serving = spot + 1;
    raise(SIGUSR1);
  }
}

uint64_t RCallQueue::enterQueue()
{
  this->lastInQueue++;
  return this->lastInQueue;
}

void makeRGlobals(NPP inst)
{

  SEXP klass, ans, ptr;
  SEXP klass2, ans2, ptr2;
  SEXP klass3, ans3, ptr3;
	
  PROTECT( klass = MAKE_CLASS( "PluginInstance" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( ptr = R_MakeExternalPtr( inst,
                                    Rf_install( "NPP" ),
                                    R_NilValue));


  PROTECT( klass2 = MAKE_CLASS( "NPNFunctionsRef" ) );
  PROTECT( ans2 = NEW( klass2 ) );
  PROTECT( ptr2 = R_MakeExternalPtr( myNPNFuncs,
                                     Rf_install("NPNFuncs"),
                                     R_NilValue));
  SET_SLOT( ans2, Rf_install("ref"), ptr2);
  SET_SLOT( ans, Rf_install("funcs"), ans2);
  //finalizer here if needed
  
  SET_SLOT(ans, Rf_install("ref"), ptr);
  Rf_defineVar(Rf_install("PluginInstance"), ans, R_GlobalEnv);

  NPObject *domwin = NULL;
  NPError res;
  res = myNPNFuncs->getvalue(inst, NPNVWindowNPObject , &domwin);
  NPVariant *winvar =  (NPVariant *) myNPNFuncs->memalloc(sizeof(NPVariant));
  OBJECT_TO_NPVARIANT(domwin, *winvar);
  PROTECT( klass3 = MAKE_CLASS( "NPVariantRef" ) );
  PROTECT( ans3 = NEW( klass2 ) );
  PROTECT( ptr3 = R_MakeExternalPtr( winvar,
                                     Rf_install("DomWindow"),
                                     R_NilValue));
  SET_SLOT( ans3, Rf_install("ref"), ptr3);
  Rf_defineVar(Rf_install("JS"), ans3, R_GlobalEnv);
  UNPROTECT(9);


}
