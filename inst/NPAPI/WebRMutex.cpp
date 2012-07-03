
void RCallQueue::lock()
{
  this->isLocked = 1;
}

void RCallQueue::unlock()
{
  this->isLocked = 0;
}

SEXP
RCallQueue::requestRCall(SEXP toeval, int *err, NPP inst)
{
  sigset_t mask, oldmask;

  uint64_t spot = 0;
  //If R is already busy, get in line and wait until our turn comes
  if(this->isLocked)
    {
      spot = this->enterQueue();
      
      //http://www.gnu.org/software/libc/manual/html_node/Sigsuspend.html#Sigsuspend
      sigemptyset (&mask);
      sigaddset (&mask, SIGUSR1);
      
      sigprocmask(SIG_BLOCK, &mask, &oldmask);
      while(this->serving != spot)
	sigsuspend(&oldmask);
	
      sigprocmask(SIG_UNBLOCK, &oldmask);

    }
  else
    {
      //if R wasn't busy before, lock it to indicate it is busy now
      this->lock();
    }
  //indicate we are now serving this request
  this->serving = spot;
  SEXP ans;
  PROTECT(ans = R_tryEval(toeval, R_GlobalEnv, err));
  
  //Unlock R and reset queue if this was the last request.
  if(this->lastInQueue == spot)
    {
      this->serving = 0;
      this->lastInQueue = 0;
      this->unlock();
    } else {
    //serve the next request;
    this->serving = spot + 1;
    raise(SIGUSR1);
  }
  UNPROTECT(1);
  return ans;
}

uint64_t RCallQueue::enterQueue()
{
  this->lastInQueue++;
  return this->lastInQueue;
}
