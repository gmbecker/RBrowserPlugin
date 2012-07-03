
#include "npapi.h"
#include "npfunctions.h"
#include <npruntime.h>
#include <stdio.h>

#ifndef XP_MACOSX
#include <R.h>
#include <Rdefines.h>
#include <Rembedded.h>
#else
#include <R/R.h>
#include <R/Rdefines.h>
#include <R/Rembedded.h>
#endif



class RCallQueue
{ 
 public:
  SEXP requestRCall(SEXP toEval, int *err, NPP inst);

 private:
  uint64_t enterQueue()
  int lock();
  int unlock();

 private:
  int isLocked;
  uint64_t serving;
  uint64_t lastInQueue;
}



