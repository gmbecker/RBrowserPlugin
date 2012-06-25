function createDataElement(evt)
{
    var RFirefoxData = document.createElement("RFirefoxDataElement");
    RFirefoxData.setAttribute("RFunName", "");
    RFirefoxData.setAttribute("RFunValue", "");
    RFirefoxData.setAttribute("ReturnValue", "");
    RFirefoxData.setAttribute("SEXPAddr", "");
    RFirefoxData.setAttribute("id", "RFirefoxCallData");
    document.documentElement.appendChild(RFirefoxData);

}

document.addEventListener("load", createDataElement, false, true);


//function jsREventHandler( name, value, singleArg)
function callRFunction( name, args, singleArg)
{

    //default value
    singleArg = typeof singleArg != "undefined" ? singleArg : "false";
    if(typeof args == "undefined")
	args = [];
    //build event to pass to chrome.
    var evt = document.createEvent("Events");
    evt.initEvent("RFirefoxCall", true, false);
    //Add our data
    
    var RFirefoxData = document.getElementById("RFirefoxCallData");
    if(!RFirefoxData)
	{
	    createDataElement(5);
	    RFirefoxData = document.getElementById("RFirefoxCallData");
	}
    RFirefoxData.setAttribute("RFunName", name);
    RFirefoxData.setAttribute("RFunValue", JSON.stringify(args));
    RFirefoxData.setAttribute("RsingleArg", singleArg);
    RFirefoxData.dispatchEvent(evt);
    return JSON.parse(RFirefoxData.getAttribute("RResult"));
    
    //    Rev.call1(name, value,  out);
}


function invokeRCallTest()
{
    evt = document.createEvents("Events");
    evt.initEvent("RCallTest", true, false);
    document.dispatchEvent(evt);
}



function createHandlerClosure(name, arg)
{
    var fun = function() {

	var out;
	//Rev.call1(name, arg, out);
	out = Rev.call(name, arg);
    }
    return fun;
}

function addREventHandler(name, targ, event, arg)
{
  
    targ.addEventListener(event, function(e)
			  {
			      callRFunction(name, arg);
			  }, false, true);
}

function runScriptsNPAPI()
{
    //    alert("in runScriptsNPAPI");
    
    var embs = window.document.getElementsByTagName("embed");
    var stags = window.document.getElementsByTagName("script");
    var Reng;
    for (var j=0; j < embs.length; j++)
	{
	    if (embs[j].type == "application/test-r")
		{
		    Reng = embs[j];
		    break;
		}
	}
    if(typeof(Reng) == "undefined")
	return;
    var mytag;
    var code; 
    var out;
    //alert("stags length:"+ stags.length);
    for(var i=0; i < stags.length; i++)
	{
	    mytag = stags[i];
	    if (mytag.type == "text/R")
		{
		    //if(mytag.text != " ")
			out = Reng.eval(mytag.text);
			if(mytag.src)
			    out = Reng.source( mytag.src)
		}
	}
    var evt = document.createEvent("Events");
    evt.initEvent("rcoderun", true, false);
    document.dispatchEvent(evt);
}

function doAttach()
{
    var embs = window.document.getElementsByTagName("embed");
    var Reng;
    for (var j=0; j < embs.length; j++)
	{
	    if (embs[j].type == "application/test-r")
		{
		    Reng = embs[j];
		    break;
		}
	}
    window.R = Reng; 

    runScriptsNPAPI();

}

function makeFun(obj)
{
    //    var Reng;
    function fun()
    {
	var args = Array.prototype.slice.call(arguments);
	//	alert("here I am");
	window.R.listCall(obj, args);
    }
    return fun;
}

function isArray(obj)
{
    var ret = false;
    if( obj.prototype.toString.call( someVar ) === '[oberject Array]' ) {
	ret = true;
    }
    return ret;
}

window.addEventListener("load", doAttach, true, true);