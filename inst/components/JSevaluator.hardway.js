/* Constants */

//reference to our interface;
const JSevaluator = Components.interface.JSevaluator;

//reference to nsISupports;
const nsISupports = Components.interface.nsISupports;

//uuid
const CLASS_ID = Components.ID("{1214e30c-6c09-4dc5-bd13-f46f72924fb8}");

//description
const CLASS_NAME = "Javascript Evaluation Interface for use from C code";

//contractid
const CONTRACT_ID = "@www.omegahat.org/jsevaluator;1";

/*Class Definition */

//constructor
function JSevaluator() {
}

//class definition
JSevaluator.prototype = {

    //method definitions

    eval: function(code)
    {
	alert("hithere");
	throw "eval is getting called.";
	var out = eval(code);
	return jsToVariant(out);
    }

    callFun: function(funName, args, numArgs)
    {
	alert("hithere2");
	var jsargs = new array(numArgs);
	var i=0;
	for (i = 0; i < numArgs; i++)
	    {
		jsargs[i] = variantToJS(args[i]);
	    }

	var fn = window[funName];
	if (typeof fn !== 'function')
	    throw "fn does not appear to be a function";
	
	var out = fn.apply(window, jsargs);
	return jsToVariant(out);
    }
    
    QueryInterface: function(aIID)
    {
	if (!aIID.equals(JSevaluator) &&
	    !aIID.equals(nsISupports))
	    throw Components.results.NS_ERROR_NO_INTERFACE;
	return this;
    }


};


//class factory;

var JSevaluatorFactory = {
    createInstance: function(aOuter, aIID)
    {
	if(aOuter != null)
	    throw Components.results.NS_ERROR_NO_AGGREGATION;
	return (new JSevaluator()).QueryInterface(aIID);
    }
};

/***********************************************************
module definition (xpcom registration)
***********************************************************/
var JSevaluatorModule = {
  registerSelf: function(aCompMgr, aFileSpec, aLocation, aType)
  {
    aCompMgr = aCompMgr.
        QueryInterface(Components.interfaces.nsIComponentRegistrar);
    aCompMgr.registerFactoryLocation(CLASS_ID, CLASS_NAME, 
        CONTRACT_ID, aFileSpec, aLocation, aType);
  },

  unregisterSelf: function(aCompMgr, aLocation, aType)
  {
    aCompMgr = aCompMgr.
        QueryInterface(Components.interfaces.nsIComponentRegistrar);
    aCompMgr.unregisterFactoryLocation(CLASS_ID, aLocation);        
  },
  
  getClassObject: function(aCompMgr, aCID, aIID)
  {
    if (!aIID.equals(Components.interfaces.nsIFactory))
      throw Components.results.NS_ERROR_NOT_IMPLEMENTED;

    if (aCID.equals(CLASS_ID))
      return JSevaluatorFactory;

    throw Components.results.NS_ERROR_NO_INTERFACE;
  },

  canUnload: function(aCompMgr) { return true; }
};

/***********************************************************
module initialization

When the application registers the component, this function
is called.
***********************************************************/
function NSGetModule(aCompMgr, aFileSpec) { return JSevaluatorModule; }

