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
  //  pthread_t thr;
  rcall_t *argsin = (rcall_t *) malloc(sizeof(rcall_t));
  argsin->queue = this;
  argsin->toeval = toeval;
  argsin->env = env;
  argsin->err = err;
  argsin->inst = inst;
  
  //(SEXP) pthread_create(&thr, &rThreadAttrs, &doRCall, (void*)argsin);

//XXX pretty sure this is going to cause the same problem as not having threads...
  //pthread_join(thr, NULL);
  
  doRCall((void*) argsin);
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
  int32_t spot = 0;
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
    {
      
      ans = R_NilValue;
    }
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
  //pthread_t thr;

  rlookup_t *argsin = (rlookup_t *) malloc(sizeof(rlookup_t));

  argsin->queue = this;
  argsin->name = name;
 
  //pthread_create(&thr, NULL, &doRLookup, (void*) argsin);
  
  //XXX pretty sure this is going to cause the same problem as not having threads...
  //pthread_join(thr, NULL);
  doRLookup((void*) argsin);
  SEXP ans = argsin->_ret;
  free(argsin);
return ans;
}  


void* doRLookup(void *in)
{
  int32_t spot = 0;

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

void RCallQueue::waitInQueue(int32_t spot)
{
      //http://www.gnu.org/software/libc/manual/html_node/Sigsuspend.html#Sigsuspend
  if(this->serving == spot)
    return;

  //pthread_mutex_lock(&queueMutex);
  //pthread_cond_wait(&queueAdvance, &queueMutex);
  //fprintf(stderr, "\nQueue advance detected. spot: %d serving: %d\n", spot, this->serving);
  //  while(this->serving !=spot)
  // {
      //pthread_cond_wait(&queueAdvance, &queueMutex);
  // }
  // pthread_mutex_unlock(&queueMutex);
  /*
  //////

      int val = pthread_mutex_lock(&rMutex);
      fprintf(stderr, "\nInitial call to lock returned %d", val);fflush(stderr);
      while(this->serving != spot)
	{
	  pthread_mutex_unlock(&rMutex);
	  pthread_mutex_lock(&rMutex);
	}
  */
}

void RCallQueue::advanceQueue(int32_t spot)
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
  //  pthread_mutex_unlock(&rMutex);
  /*
  pthread_mutex_lock(&queueMutex);
  pthread_cond_broadcast(&queueAdvance);
  pthread_mutex_unlock(&queueMutex);
  */
}

int32_t RCallQueue::enterQueue()
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
  /*
  NPObject *domwin = NULL;
  NPError res;
  res = myNPNFuncs->getvalue(inst, NPNVWindowNPObject , &domwin);
  NPVariant *winvar =  (NPVariant *) myNPNFuncs->memalloc(sizeof(NPVariant));
  OBJECT_TO_NPVARIANT(domwin, *winvar);
  */
  NPVariant *winvar =  (NPVariant *) myNPNFuncs->memalloc(sizeof(NPVariant));
  getWindowVariant(inst, winvar);
  PROTECT( klass3 = MAKE_CLASS( "JSValueRef" ) );
  PROTECT( ans3 = NEW( klass3 ) );
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

void ThrowRError(NPObject *obj, NPNetscapeFuncs *funcs)
{ 
  int err;
      SEXP errcall, errsxp;
      PROTECT(errcall = allocVector(LANGSXP, 1));
      SETCAR(errcall, Rf_install("geterrmessage"));
      PROTECT(errsxp = R_tryEval(errcall, R_GlobalEnv, &err));
      int len = strlen(CHAR(STRING_ELT(errsxp, 0)));
      NPUTF8 *errmsg = (NPUTF8 *) malloc(len*sizeof(char));
      strcpy(errmsg, CHAR(STRING_ELT(errsxp, 0)));
      fprintf(stderr, "attemtping to throw JS exception. Err msg: %s", errmsg);fflush(stderr);
      funcs->setexception(obj, errmsg);
      UNPROTECT(2);

}
