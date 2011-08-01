Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");


/***********************************************************
class definition
***********************************************************/

//class constructor
function JSevaluatorImpl() {
// If you only need to access your component from Javascript, uncomment the following line:
//this.wrappedJSObject = this;
}

// class definition
JSevaluatorImpl.prototype = {

  // properties required for XPCOM registration:
  classDescription: "A Component To Execute Javascript From C Code",
  classID:          Components.ID("{1214e30c-6c09-4dc5-bd13-f46f72924fb8}"),
  contractID:       "@www.omegahat.org/jsevaluator;1",

  _xpcom_categores: [],
  /*
  // [optional] an array of categories to register this component in.
   _xpcom_categories: [{

    // Each object in the array specifies the parameters to pass to
    // nsICategoryManager.addCategoryEntry(). 'true' is passed for both
    // aPersist and aReplace params.
    category: "computing",

    // optional, defaults to the object's classDescription
    entry: "RFirefox",

    // optional, defaults to the object's contractID (unless 'service' is specified)
    //value: "...",

    // optional, defaults to false. When set to true, and only if 'value' is not
    // specified, the concatenation of the string "service," and the object's contractID
    // is passed as aValue parameter of addCategoryEntry.
     service: true
  }],
  */
  // QueryInterface implementation, e.g. using the generateQI helper (remove argument if skipped steps above)
  QueryInterface: XPCOMUtils.generateQI([Components.interfaces.JSevaluator, 
					 Components.interfaces.nsISupports]),

  // ...component implementation...
  // define the function we want to expose in our interface
    //method definitions

  eval: function(code)
  {
      //http://stackoverflow.com/questions/3247748/using-eval-call-in-firefox-xpcom-component
      var wm = Components.classes["@mozilla.org/appshell/window-mediator;1"].getService(Components.interfaces.nsIWindowMediator);
      var recentWindow = wm.getMostRecentWindow("navigator:browser");

      return eval(recentWindow, code);
      
      /*
      var out = eval(code);
      //return jsToVariant(out)
      return out;*/
      //return 53;
  },
  
  callFun: function(funName, args, numArgs)
  {
      /*
      var jsargs = new array(numArgs);
	var i=0;
      for (i = 0; i < numArgs; i++)
	  {
	      jsargs[i] = variantToJS(args[i]);
	  }
      */
      var fn = window[funName];
      if (typeof fn !== 'function')
	    throw "fn does not appear to be a function";
      
      var out = fn.apply(window, args);
      //return jsToVariant(out);
      return out;
  },
  
  assign: function(varname, val)
  {
      window[varname] = val;
      return TRUE;
  },
  get: function(varname)
  {
      return window[varname];
  }, 
};
var components = [JSevaluatorImpl];

function NSGetModule(compMgr, fileSpec) {
   return XPCOMUtils.generateModule(components);
 }

/*


/ Constants /

//reference to our interface;
const JSevaluator = Components.interface.JSevaluator;

//reference to nsISupports;
const nsISupports = Components.interface.nsISupports;

//uuid
const CLASS_ID = Components.ID("{1214e30c-6c09-4dc5-bd13-f46f72924fb8}");

//description
const CLASS_NAME = "Javascript Evaluation Interface for use from C code";

//contractid
const CONTRACT_ID = "RFirefox@www.omegahat.org/jsevaluator;1";

/*Class Definition /

//constructor
function JSevaluator() {
}

//class definition
JSevaluator.prototype = {

    //method definitions

    eval: function(code)
    {
	var out = eval(code)
	return jsToVariant(out)
    }

    callFun: function(funName, args, numArgs)
    {
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

*/