#include "WebR.h"

#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>
//#include <Rdevices.h>
#include <Rinternals.h>

extern "C"{
SEXP
R_NPAPI_Remove_All(SEXP plug, SEXP mylist)
{
  int n = LENGTH(mylist);
  SEXP tmp;
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  PROTECT(tmp = R_NilValue);
  NPVariant *obj;
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  bool success; 
  for(int i=0; i< n; i++)
    {
      obj = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( VECTOR_ELT(mylist, i) , Rf_install( "ref" ) ) );
      success = funcs->invoke(inst, obj->value.objectValue, funcs->getstringidentifier("remove"), NULL, 0, ret);
      if(!success)
	{
	  Rf_error("Call to remove failed.");
	  return(ScalarLogical(false));
	}
    }
  funcs->memfree(ret);
  return(ScalarLogical(true));
}






#if 0

static Rboolean raphaelDeviceDriver(DevDesc *dev);
  SEXP do_devRaphael() {
    DevDesc *dev = NULL;
    GEDevDesc *dd;
    R_CheckDeviceAvailable();
    if (!(dev = (DevDesc *) calloc(1, sizeof(DevDesc))))
      return R_NilValue;
    dev->displayList = R_NilValue;
    if (!raphaelDeviceDriver(dev)) {
      free(dev);
      error("unable to start Raphael device");
    }
    gsetVar(install(".Device"), mkString("Raphael"), R_NilValue);
    dd = GEcreateDevDesc(dev);
    Rf_addDevice((DevDesc*) dd);
    GEinitDisplayList(dd);
    return R_NilValue;
  }

static Rboolean raphaelDeviceDriver(DevDesc *dev) {
  dev->deviceSpecific = NULL;
  /*
   * Device functions
   */
  dev->open = raph_Open;
  dev->close = raph_Close;
  dev->activate = raph_Activate;
  dev->deactivate = raph_Deactivate;
  dev->size = raph_Size;
  dev->newPage = raph_NewPage;
  dev->clip = raph_Clip;
  dev->strWidth = raph_StrWidth;
  dev->text = raph_Text;
  dev->rect = raph_Rect;
  dev->circle = raph_Circle;
  dev->line = raph_Line;
  dev->polyline = raph_Polyline;
  dev->polygon = raph_Polygon;
  dev->locator = raph_Locator;
  dev->mode = raph_Mode;
  dev->hold = raph_Hold;
  dev->metricInfo = raph_MetricInfo;
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
  if(!raph_Open(dev)) {
    return FALSE;
  }
  /*
   * Device physical characteristics
   */
  //XXX these need to be changed!
  dev->left = 0;
  dev->right = 1000;
  dev->bottom = 0;
  dev->top = 1000;
  dev->cra[0] = 10;
  dev->cra[1] = 10;
  dev->xCharOffset = 0.4900;
  dev->yCharOffset = 0.3333;
  dev->yLineBias = 0.1;
  dev->ipr[0] = 1.0/72;
  dev->ipr[1] = 1.0/72;
  /*
   * Device capabilities
   */
  dev->canResizePlot= FALSE;
  dev->canChangeFont= FALSE;
  dev->canRotateText= TRUE;
  dev->canResizeText= TRUE;
  dev->canClip = TRUE;
  dev->canHAdj = 2;
  dev->canChangeGamma = FALSE;
  dev->displayListOn = TRUE;
  dev->newDevStruct = 1;
  return TRUE;
}

static void raph_Circle(double x, double y, double r,
			R_GE_gcontext *gc,
			DevDesc *dev) {

  SEXP pap, env, plug;
  SEXP *spec = (SEXP *)dev->deviceSpecific;
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);
  PROTECT(pap = Rf_findVarInFrame(mkString("paper"), env));
  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );

  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant args[3];
  NPVariant *ret = funs->memalloc(sizeof(NPVariant));
  DOUBLE_TO_NPVARIANT(x, args[0]);
  DOUBLE_TO_NPVARIANT(y, args[1]);
  DOUBLE_TO_NPVARIANT(r, args[2]);
  funcs->invoke(inst, paper->value.objectValue, funcs->getstringidententifier("circle"), args, 3, ret);
  //XXX color code here
  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, true, val);
  PROTECT(list = Rf_findVarInFrame(mkString("points"), env));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  return;
}



static void raph_Line(double x1, double y1, double x2, double y2,
		      R_GE_gcontext *gc,
		      DevDesc *dev) {
  SEXP pap, env, plug;
  SEXP *spec = (SEXP *) dev->deviceSpecific;
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);
  PROTECT(pap = Rf_findVarInFrame(mkString("paper"), env));
  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );

  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant arg;
  NPVariant *ret = funs->memalloc(sizeof(NPVariant));
  int len = 1 + 1 + 6*4 + 5; //M,l, 4 doubles, and 5 spaces
  char *dat = (char *) funcs->memalloc(len*sizeof(char));
  sprintf(dat, "M %6f %6f l %6f %6f", x1, x2, x2 - x1, y2 - y1);
  STRINGZ_TO_NPVARIANT(dat, arg);

  funcs->invoke(inst, paper->value.objectValue, funcs->getstringisentifier("path"), args, 1, ret);
  //XXX color code here
  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, true, val);
  PROTECT(list = Rf_findVarInFrame(mkString("lines"), env));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  return;
}
static void raph_Polygon(int n, double *x, double *y,
			 R_GE_gcontext *gc,
			 DevDesc *dev) {
}
static void raph_Polyline(int n, double *x, double *y,
			  R_GE_gcontext *gc,
			  DevDesc *dev) {
}
static void raph_Rect(double x0, double y0, double x1, double y1,
		      R_GE_gcontext *gc,
		      DevDesc *dev) {


  SEXP pap, env, plug;
  SEXP *spec = (SEXP *)dev->deviceSpecific;
  PROTECT(env = (SEXP) spec[0]);
  PROTECT(plug = (SEXP) spec[1]);
  PROTECT(pap = Rf_findVarInFrame(mkString("paper"), env));
  NPVariant *paper = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( pap , Rf_install( "ref" ) ) );

  
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  
  NPVariant args[4];
  NPVariant *ret = funs->memalloc(sizeof(NPVariant));
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
  funcs->invoke(inst, paper->value.objectValue, funcs->getstringisentifier("rect"), args, 4, ret);
  //XXX color code here
  SEXP val, list;
  PROTECT(val = R_NilValue);
  ConvertNPToR(ret, inst, funcs, true, val);
  PROTECT(list = Rf_findVarInFrame(mkString("rects"), env));
  SET_LENGTH(list, LENGTH(list) + 1);
  SET_VECTOR_ELT(list, LENGTH(list) - 1, val);
  UNPROTECT(5);
  return;



}
static void raph_Text(double x, double y, char *str,
		      double rot, double hadj,
		      R_GE_gcontext *gc,
		      DevDesc *dev) {
}
static void raph_NewPage(R_GE_gcontext *gc,
			 DevDesc *dev) {
}






#endif //if 0



};
