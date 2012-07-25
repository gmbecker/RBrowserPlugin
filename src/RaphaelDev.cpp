/*
 *  R : A Computer Language for Statistical Data Analysis
 *  Copyright (C) 2004-11  The R Development Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 */
#include "WebR.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Rinternals.h>


#define R_USE_PROTOTYPES 1
#include <R_ext/GraphicsEngine.h>

//#include "grDevices.h"
#include "R_ext/GraphicsDevice.h"
#include <stdlib.h>


#include <R_ext/Boolean.h>
//Found in grDevices.h, we don't seem to actually need anything from that file other than to define this symbol so the library can be loaded...
#ifdef ENABLE_NLS
#include <libintl.h>
#undef _
#define _(String) dgettext ("grDevices", String)
#else
#define _(String) (String)
#endif


void DoColors(NPVariant *el, const pGEcontext gc, NPP inst, NPNetscapeFuncs *funcs);

extern "C"{

  static Rboolean raphaelDeviceDriver(pDevDesc dev, SEXP env, SEXP plug);

  void GEraphaelDevice(SEXP env, SEXP plug)
  {
    pDevDesc dev = NULL;
    pGEDevDesc dd;

    R_GE_checkVersionOrDie(R_GE_version);
    R_CheckDeviceAvailable();
    
    // XXX BEGIN_SUSPECT_INTERRUPTS was causing compiler error of invalid cast between int and Rboolean. I'm pretty sure it has to do with the values of TRUE and or FALSE, which are probably getting defined in one of the headers included in WebR.h?
    // BEGIN_SUSPEND_INTERRUPTS {
      
	if (!(dev = (pDevDesc ) calloc(1, sizeof(DevDesc))))
	  //error(_("unable to start Raphael device"));
	  Rf_error("unable to start Raphael device");
	if (!raphaelDeviceDriver(dev, env, plug)) {
	    free(dev);
	    //error(_("unable to start Raphael device"));
	    Rf_error("unable to start Raphael device");
	    }

	dd = GEcreateDevDesc(dev);
	GEaddDevice2(dd, "Raphael");
      
	// } END_SUSPEND_INTERRUPTS;
 
}
static void Raphael_Circle(double x, double y, double r,
                        const pGEcontext gc,
                        pDevDesc dev) {

  SEXP pap, env, plug;
  SEXP *spec = (SEXP *)dev->deviceSpecific;
  //PROTECT(env = (SEXP) spec[0]);
  //PROTECT(plug = (SEXP) spec[1]);
  //These have R_PreserveObject called on them when the device is created
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);
  // PROTECT(pap = Rf_findVarInFrame(Rf_install("paper"), env));
  //PROTECT(pap = Rf_findVarInFrame(env, Rf_install("paper")));
  PROTECT(pap = (SEXP) spec[2]);

  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );

  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant args[3];
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  DOUBLE_TO_NPVARIANT(x, args[0]);
  DOUBLE_TO_NPVARIANT(y, args[1]);
  DOUBLE_TO_NPVARIANT(r, args[2]);
  funcs->invoke(inst, paper->value.objectValue, funcs->getstringidentifier("circle"), (const NPVariant *) args, 3, ret);
  //XXX color code here
  DoColors(ret, gc, inst, funcs);
  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, CONV_REF, &val);
  PROTECT(list = Rf_findVarInFrame(env, Rf_install("points")));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  //UNPROTECT(3);
  return;

}
static void Raphael_Line(double x1, double y1, double x2, double y2,
                      const pGEcontext gc,
                      pDevDesc dev) {

  SEXP pap, env, plug;
  SEXP *spec = (SEXP *)dev->deviceSpecific;
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);

  PROTECT(pap = Rf_findVarInFrame(env, Rf_install("paper")));
  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );
  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant *arg = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  int len = 1 + 1 + 6*4 + 5 + 1; //M,l, 4 doubles, and 5 spaces (plus null char)
  char dat[100]; 
  
  sprintf(dat, "M %6f %6f l %6f %6f\0", x1, y1, x2 - x1, y2 - y1);
  //  sprintf(dat, "M %6f %6f L %6f %6f\0", x1, x2, x2 - x1, y2 - y1);
  char *dat2 = (char *) funcs->memalloc(strlen(dat)*sizeof(char));
  memcpy(dat2, (const char *) dat, strlen(dat)); 
  STRINGZ_TO_NPVARIANT(dat2, *arg);
  
  funcs->invoke(inst, paper->value.objectValue, funcs->getstringidentifier("path"), (const NPVariant *) arg, 1, ret);
  //XXX color code here
  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, CONV_DEFAULT, &val);
  //PROTECT(list = Rf_findVarInFrame(mkString("lines"), env));
  PROTECT(list = Rf_findVarInFrame(env, Rf_install("lines")));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  return;
}
static void Raphael_Polygon(int n, double *x, double *y,
                         const pGEcontext gc,
                         pDevDesc dev) {
}
static void Raphael_Path(double *x, double *y,
                      int npoly, int *nper,
                      Rboolean winding,
                      const pGEcontext gc,
                      pDevDesc dev) {
}
static void Raphael_Polyline(int n, double *x, double *y,
                          const pGEcontext gc,
                          pDevDesc dev) {


  SEXP pap, env, plug;
  SEXP *spec = (SEXP *)dev->deviceSpecific;
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);

  PROTECT(pap = Rf_findVarInFrame(env, Rf_install("paper")));
  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );
  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant *arg = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  int setsize = 1 + 2 + 2*11;
  int len = n*(setsize) + 1; //M,l, 4 doubles, and 5 spaces (plus null char) M%4.6 %4.6 
  char *dat = (char *) funcs->memalloc(len*sizeof(char));
  sprintf(dat, "M%4.6f %4.6f ", x[0], y[0]);
  int pos = setsize;
  for(int i = 1; i < n; i++)
    {
      sprintf((char*)&dat[strlen(dat)], "L%04.6f %04.6f ", x[i], y[i]);
      pos = pos + setsize;
    }
  memcpy(&dat[strlen(dat)], "\0", 1);
  
  STRINGZ_TO_NPVARIANT(dat, *arg);
  
  funcs->invoke(inst, paper->value.objectValue, funcs->getstringidentifier("path"), (const NPVariant *) arg, 1, ret);
  //XXX color code here
  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, CONV_DEFAULT, &val);
  //PROTECT(list = Rf_findVarInFrame(mkString("lines"), env));
  PROTECT(list = Rf_findVarInFrame(env, Rf_install("polylines")));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  funcs->memfree(dat);
  
  return;
}


static void Raphael_Rect(double x0, double y0, double x1, double y1,
                      const pGEcontext gc,
                      pDevDesc dev) {

  SEXP pap, env, plug;
  SEXP *spec = (SEXP *)dev->deviceSpecific;
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);
  //PROTECT(pap = Rf_findVarInFrame(mkString("paper"), env));
  PROTECT(pap = Rf_findVarInFrame(env, Rf_install("paper")));
  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );

  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant args[4];
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  double tmp;
  if(x0 > x1)
    {
      x0 = tmp;
      x0 = x1;
      x1 = tmp;
    }
  if(y0 > y1)
    {
      y0 = tmp;
      y0 = y1;
      y1 = tmp;
    }
  
  DOUBLE_TO_NPVARIANT(x0, args[0]);
  DOUBLE_TO_NPVARIANT(y0, args[1]);
  DOUBLE_TO_NPVARIANT(x1 - x0, args[2]);
  DOUBLE_TO_NPVARIANT(y1 - y0, args[2]);
  funcs->invoke(inst, paper->value.objectValue, funcs->getstringidentifier("rect"), (const NPVariant *) args, 4, ret);
  //XXX color code here
  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, CONV_DEFAULT, &val);
  //PROTECT(list = Rf_findVarInFrame(mkString("rects"), env));
  PROTECT(list = Rf_findVarInFrame(env, Rf_install("rects")));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  return;
 
}


#define MAX(a, b) (a<b)?b:a

static void Raphael_Text(double x, double y, const char *str,
                      double rot, double hadj,
			 const pGEcontext gc,
                      pDevDesc dev) {
  
  if(R_TRANSPARENT(gc->col))
    return;

  SEXP pap, env, plug;
  SEXP *spec = (SEXP *)dev->deviceSpecific;
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);
  PROTECT(pap  = (SEXP) spec[2]);
  //  PROTECT(pap = Rf_findVarInFrame(env, Rf_install("paper")));
  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );
  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant *args = (NPVariant *) funcs->memalloc(3*sizeof(NPVariant));
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  int len = strlen(str);
  char *dat = (char*) funcs->memalloc((len + 1)*sizeof(char));
  memcpy(dat, str, len);
  memcpy(&dat[len], "\0", 1);
  DOUBLE_TO_NPVARIANT(x, args[0]);
  DOUBLE_TO_NPVARIANT(y, args[1]);
  STRINGZ_TO_NPVARIANT(dat, args[2]);
  
  funcs->invoke(inst, paper->value.objectValue, funcs->getstringidentifier("text"), (const NPVariant *) args, 3, ret);

  int size = (int)((double) MAX(10, gc->ps)*gc->cex);
  NPVariant *args2 = (NPVariant *) funcs->memalloc(2*sizeof(NPVariant));
  //10 = "font-size\0"
  char *dat2 = (char*) funcs->memalloc(10*sizeof(char));
  NPVariant tmp;
  memcpy(dat2, "font-size\0", 10);
  STRINGZ_TO_NPVARIANT(dat2, args2[0]);
  INT32_TO_NPVARIANT(size, args[1]);
  funcs->invoke(inst, ret->value.objectValue, funcs->getstringidentifier("attr"), (const NPVariant *) args2, 2, &tmp);
  //XXX color code here




  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, CONV_REF, &val);
  //PROTECT(list = Rf_findVarInFrame(mkString("lines"), env));
  PROTECT(list = Rf_findVarInFrame(env, Rf_install("texts")));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  funcs->memfree(dat);
  

}
static void Raphael_NewPage(const pGEcontext gc,
                         pDevDesc dev) {

  SEXP *spec = (SEXP *)dev->deviceSpecific;
  SEXP env = ((SEXP*) dev->deviceSpecific)[0];
  
  SEXP call, ptr, ans;
  int error = 0;
  PROTECT(call = ptr = allocVector(LANGSXP, 2));
  SETCAR(call, Rf_install("raphNewPage"));
  ptr = CDR(ptr);
  SETCAR(ptr, env);
  ans = R_tryEval(call, R_GlobalEnv, &error);
  UNPROTECT(1);
  return;
}
static void Raphael_Close(pDevDesc dev) {
  R_ReleaseObject( ( (SEXP*) dev->deviceSpecific )[0] );
  R_ReleaseObject( ( (SEXP*) dev->deviceSpecific )[1] );
  R_ReleaseObject( ( (SEXP*) dev->deviceSpecific )[2] );
 
  free(dev->deviceSpecific);
}
static Rboolean Raphael_Open(pDevDesc dev) {
  return (Rboolean) TRUE;
}
static void Raphael_Clip(double x0, double x1, double y0, double y1,
                      pDevDesc dev) {
}
static void Raphael_MetricInfo(int c, const pGEcontext gc,
                            double* ascent, double* descent,
                            double* width, pDevDesc dev) 
{
#if 0
    Rboolean Unicode = mbcslocale;

    *ascent = 0.0;
    *descent = 0.0;
    *width = 0.0;

    /* dummy, as a test of the headers */
    if (c < 0) { Unicode = TRUE; c = -c; }
    if(Unicode && gc->fontface != 5 && c >= 128) {
	/* Unicode case */ ;
    } else {
	/* single-byte case */ ;
    }
#endif //#if 0
}
static void Raphael_Size(double *left, double *right,
                      double *bottom, double *top,
                      pDevDesc dev) {
    *left = dev->left;
    *right = dev->right;
    *bottom = dev->bottom;
    *top = dev->top;
}
static double Raphael_StrWidth(const char *str,
                            const pGEcontext gc,
                            pDevDesc dev) {
    return 0.0;
}

  static Rboolean raphaelDeviceDriver(pDevDesc dev, SEXP env, SEXP plug) {

    dev->deviceSpecific = Calloc(3, SEXP);
  ((SEXP*)dev->deviceSpecific)[0] = env;
  ((SEXP*)dev->deviceSpecific)[1] = plug;
  R_PreserveObject(( (SEXP*)dev->deviceSpecific )[0]);
  R_PreserveObject(( (SEXP*)dev->deviceSpecific )[1]);
  ((SEXP*)dev->deviceSpecific)[2] = Rf_findVarInFrame(env, Rf_install("paper"));
  R_PreserveObject(( (SEXP*)dev->deviceSpecific )[2]);

 
    //
    //  Device functions
    // 
    
    dev->close = Raphael_Close;
    dev->size = Raphael_Size;
    dev->newPage = Raphael_NewPage;
    dev->clip = Raphael_Clip;
    dev->strWidth = Raphael_StrWidth;
    dev->text = Raphael_Text;
    dev->rect = Raphael_Rect;
    dev->circle = Raphael_Circle;
    dev->line = Raphael_Line;
    dev->polyline = Raphael_Polyline;
    dev->polygon = Raphael_Polygon;
    dev->path = Raphael_Path;
    dev->metricInfo = Raphael_MetricInfo;
    dev->hasTextUTF8 = (Rboolean) FALSE;
    dev->useRotatedTextInContour = (Rboolean) FALSE;
    /*
     * Initial graphical settings
     */
    dev->startfont = 1;
    dev->startps = 10;
    dev->startcol = R_RGB(0, 0, 0);
    dev->startfill = R_TRANWHITE;
    dev->startlty = LTY_SOLID;
    dev->startgamma = 1;
    /*
     * Start device
     */
    if(!Raphael_Open(dev)) {
      return (Rboolean) FALSE;
    }
    /*
     * Device physical characteristics
     */
    dev->left = 0;
    dev->right = 400;
    dev->bottom = 400;
    dev->top = 0;
    dev->cra[0] = 9;
    dev->cra[1] = 12;
    dev->xCharOffset = 0.4900;
    dev->yCharOffset = 0.3333;
    dev->yLineBias = 0.2;
    dev->ipr[0] = 1.0/72;
    dev->ipr[1] = 1.0/72;
    /*
     * Device capabilities
     */
    dev->canClip = (Rboolean) TRUE;
    dev->canHAdj = 2;
    dev->canChangeGamma = (Rboolean) FALSE;
    dev->displayListOn = (Rboolean) FALSE;

    dev->haveTransparency = 1;
    dev->haveTransparentBg = 2;
    return (Rboolean) TRUE;
}


SEXP R_GD_raphaelDevice(SEXP env, SEXP plug)
{
  GEraphaelDevice(env, plug);
  return R_NilValue;
}

};


void DoColors(NPVariant *el, const pGEcontext gc, NPP inst, NPNetscapeFuncs *funcs)
{

  int col = gc->col;
  int fill = gc->fill;
  char *dat = (char *) funcs->memalloc(25*sizeof(char));
  char *nm = (char *) funcs->memalloc(7*sizeof(char));
  //memcpy(&dat[24], "\0", 1);
  NPVariant *args = (NPVariant *) funcs->memalloc(2*sizeof(NPVariant));
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  bool didcolor = false;
  if(!R_TRANSPARENT(col))
    {
      memcpy(nm, "stroke\0",10);
      STRINGZ_TO_NPVARIANT(nm, args[0]);
      //have to get rid of the 6 unused bits (color is only 24 bits)
      sprintf(dat, "rgba(%3d, %3d, %3d, %1.2f)\0", R_RED(col), R_GREEN(col), R_BLUE(col), (double) R_ALPHA(col) / 255.0);
      STRINGZ_TO_NPVARIANT(dat, args[1]);
      funcs->invoke(inst, el->value.objectValue, funcs->getstringidentifier("attr"), args, 2, ret);
      didcolor = true;
      //funcs->releasevariantvalue(ret);
    }
  if(!R_TRANSPARENT(fill))
    {
      memcpy(nm, "fill\0",10);
      STRINGZ_TO_NPVARIANT(nm, args[0]);
      //have to get rid of the 6 unused bits (color is only 24 bits)
      sprintf(dat, "rgba(%3d, %3d, %3d, %1.2f)\0", R_RED(fill), R_GREEN(fill), R_BLUE(fill), (double) R_ALPHA(fill) / 255.0);
      STRINGZ_TO_NPVARIANT(dat, args[1]);
      funcs->invoke(inst, el->value.objectValue, funcs->getstringidentifier("attr"),args, 2, ret);
      //funcs->releasevariantvalue(ret);
      didcolor = true;
    }
  if(didcolor)
    funcs->releasevariantvalue(ret);
  funcs->memfree(ret);
  
  for(int i =0; i < 2; i++)
    funcs->releasevariantvalue(&args[i]);

  funcs->memfree(args);
}

