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