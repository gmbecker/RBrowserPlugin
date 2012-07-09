#include "WebR.h"

typedef struct rCall {
  RCallQueue *queue;
  SEXP toeval;
  SEXP env;
  int *err;
  NPP inst;
  SEXP _ret;
} rcall_t;

typedef struct rLookup {
  RCallQueue *queue;
  const char *name;
  SEXP _ret;
} rlookup_t;


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
  pthread_t thr;
  rcall_t *argsin = (rcall_t *) malloc(sizeof(rcall_t));
  argsin->queue = this;
  argsin->toeval = toeval;
  argsin->env = env;
  argsin->err = err;
  argsin->inst = inst;
  /*
void * argsin =  malloc(sizeof(SEXP)*2 + sizeof(int*) + sizeof(NPP));
  int *curpos =   (int *) argsin;
  *(SEXP *)curpos =  toeval;
  curpos = curpos + sizeof(SEXP);
  *(SEXP *)curpos = env;
  curpos = curpos + sizeof(SEXP);
  *(int **) curpos = err;
  curpos = curpos + sizeof(int*);
  *(NPP *) curpos = inst; 
  curpos = curpos + sizeof(NPP);
  *(RCallQueue**) curpos = this;
  */
  (SEXP) pthread_create(&thr, &rThreadAttrs, &doRCall, (void*)argsin);

//XXX pretty sure this is going to cause the same problem as not having threads...
  pthread_join(thr, NULL);
  SEXP ans = argsin->_ret;
  free(argsin);
  return ans;
}

void* doRCall(void * in)
{

  rcall_t *callin = reinterpret_cast<rcall_t*>(in);
  
  RCallQueue *queue = reinterpret_cast<RCallQueue*>(callin->queue);
  SEXP toeval = reinterpret_cast<SEXP>(callin->toeval);
  SEXP env = reinterpret_cast<SEXP>(callin->env);
  int *err = reinterpret_cast<int*>(callin->err);
  NPP inst = reinterpret_cast<NPP>(callin->inst);
  uint64_t spot = 0;
  spot = queue->enterQueue();
  queue->waitInQueue(spot);
  
  if(inst)
    {
      //update PluginInstance and JS global R variables
      makeRGlobals(inst);
    }
  
  SEXP ans, call;

  PROTECT(call = toeval);  
  Rf_PrintValue(call);
    
  PROTECT( ans = R_tryEval(toeval, env, err));
  if(*err)
    ans = R_NilValue;
  else
    {
      if(TYPEOF(ans) == PROMSXP)
	ans = R_tryEval(ans, env, err);
      //Unlock R and reset queue if this was the last request.
    } 

  queue->advanceQueue(spot);
  callin->_ret = ans;
  UNPROTECT(1);
  return ans;
  }

SEXP RCallQueue::requestRLookup(const char *name)
{
  pthread_t thr;

  rlookup_t *argsin = (rlookup_t *) malloc(sizeof(rlookup_t));

  argsin->queue = this;
  argsin->name = name;
			     
  (SEXP) pthread_create(&thr, NULL, &doRLookup, (void*) argsin);
  
  //XXX pretty sure this is going to cause the same problem as not having threads...
  pthread_join(thr, NULL);
  SEXP ans = argsin->_ret;
  free(argsin);
return ans;
}  


void* doRLookup(void *in)
{
  uint64_t spot = 0;

  rlookup_t *argsin = reinterpret_cast<rlookup_t*>(in);
  RCallQueue *queue = reinterpret_cast<RCallQueue *>(argsin->queue);
  const char *name = reinterpret_cast<const char *>(argsin->name);
  spot = queue->enterQueue();
  
  queue->waitInQueue(spot);
  
  SEXP ans;
  int err = 0;
  ans = Rf_findVar( Rf_install(name), R_GlobalEnv);
  if(TYPEOF(ans) == PROMSXP)
    ans = R_tryEval(ans, R_GlobalEnv, &err);
  
  queue->advanceQueue(spot);
  //R_PreserveObject(ans); //XXX This is never getting released!!!!!
  argsin->_ret = ans;
  return ans;
}

void RCallQueue::waitInQueue(uint64_t spot)
{
  //sigset_t mask, oldmask;
  //If R is already busy, wait until our turn comes
  //if(this->isLocked)
  // { 
      //http://www.gnu.org/software/libc/manual/html_node/Sigsuspend.html#Sigsuspend
  fprintf(stderr, "\nR is in use. Waiting in Queue spot %ld, currently serving spot %ld", spot, this->serving);fflush(stderr);
      /*
      sigemptyset (&mask);
      sigaddset (&mask, SIGUSR1);
      
      sigprocmask(SIG_BLOCK, &mask, &oldmask);
      //wait until our turn comes up
      while(this->serving != spot)
	sigsuspend(&oldmask);
     
      sigprocmask(SIG_UNBLOCK, &oldmask, NULL);
      */
      pthread_mutex_lock(&rMutex);
      while(this->serving != spot)
	{
	  pthread_mutex_unlock(&rMutex);
	  pthread_mutex_lock(&rMutex);
	}
      /*
    }
  else
    {
      //if R wasn't busy before, lock it to indicate it is busy now
      this->lock();
      this->serving = spot;
    }
      */
}

void RCallQueue::advanceQueue(uint64_t spot)
{
  
  fprintf(stderr, "\nDone serving spot %ld. LastInQueue:%ld", spot, this->lastInQueue);fflush(stderr);
  if(this->lastInQueue == spot)
    {
      fprintf(stderr, "....Clearing Queue");fflush(stderr);
      this->serving = 1;
      this->lastInQueue = 0;
      this->unlock();
    } else {
    //serve the next request;
    this->serving = spot + 1;
    //raise(SIGUSR1);
    

  }
  fprintf(stderr, "\nUnlocking queue to serve %ld", this->serving); fflush(stderr);
  pthread_mutex_unlock(&rMutex);
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

void RCallQueue::init()
{
  this->serving = 1;
  

}
