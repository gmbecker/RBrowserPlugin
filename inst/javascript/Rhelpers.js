
function jsREventHandler( name, value)
{

    
    //build event to pass to chrome.
    var evt = document.createEvent("Events");
    evt.initEvent("RFirefoxCall", true, false);
    //Add our data
    var RFirefoxData = document.getElementById("RFirefoxCallData");
    RFirefoxData.setAttribute("FunName", name);
    RFirefoxData.setAttribute("FunValue", value);
    evt.RFunName = name;
    evt.RFunValue = value;
    RFirefoxData.dispatchEvent(evt);
    
    //    Rev.call1(name, value,  out);
}
/*
function addREventHandler( name, targ, event, arg)
{
    var fun = function() {
	var out;
	Rev.call1(name, arg,  out);
    }
    targ.addEventListener(event, fun, true);    
}
*/


