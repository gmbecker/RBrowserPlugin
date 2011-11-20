
    try {
	//	console.log("inside try statement");
	//		netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
	const cid = "@R/Revaluator;1";
	var Rev = Components.classes[cid].createInstance();
	//console.log(Rev);
	Rev = Rev.QueryInterface(Components.interfaces.REvaluator);
	//console.log(Rev);
	var args = ["R"];
	Rev.init(args, 1);
	Rev.library("RFirefox"); 
   } catch (err) {
	alert( " failed to start R " + err);
    }    


 

function runRTags()
{
    var stags = content.document.getElementsByTagName("script");
    var mytag;
    var code; 
    var out;
    var numRTags = 0;
    //alert("stags length:"+ stags.length);
    for(var i=0; i < stags.length; i++)
	{
	    
	    mytag = stags[i];
	    if (mytag.type == "text/R")
		{
		    numRTags = numRTags + 1;
		    //if(mytag.text != " ")
			Rev.eval(mytag.text);
			if(mytag.src)
			    Rev.call1("source", mytag.src)
		}
	}

    if(numRTags > 0)
	{
	    //prepare element to pass data between extension and webpage javascript
	    
	    
	    content.document.addEventListener("RFirefoxCall",  ExtjsREventHandler, false, true);
	    
	    
	}

}

//this adds the listener to every page that loads. Probably quicker than checking if the the page has any R script tags first. Need to make sure runRTags doesn't do anything if there aren't any R script tags.
gBrowser.addEventListener("load", runRTags, true, true);

function ExtjsREventHandler( evt)
{
    //alert("In ExtjsREventHandler");
    //var RFirefoxData = content.document.getElementById("RFirefoxCallData");
    var name = evt.target.getAttribute("RFunName");//RFirefoxData.getAttribute("FunName");
    var value = JSON.parse(evt.target.getAttribute("RFunValue"));
    var singleArg = evt.target.getAttribute("RsingleArg");
    var out;
    /*
    if(singleArg == "true")
	{
	    // alert(value);
	out = Rev.call(name, value);
	}
    else
	{
    */
	    // RFirefoxData.getAttribute("FunValue");
	    //  alert("name:" + name + " value:" + value + " target:" + evt.target) ;
	    if(!(value instanceof Array))
		{
		    var tmp = [];
		    tmp[0] = value;
		    value = tmp;
		}
	    out = Rev.listCall(name, value);
	    //	}

    evt.target.setAttribute("RResult", JSON.stringify(out));
    return 0;
}

function addREventHandler( name, targ, event, arg)
{
    alert("hi there");
    var fun = function() {
	var out;
	Rev.call1(name, arg,  out);
    }
    
    //alert(createHandlerClosure);
    //fun = createHandlerClosure(name, arg);
    alert(fun);
    targ.addEventListener(event, fun, false, true);    
}
	

function RCallTest()
{
    var out;
    Rev.call1("RFun", 1, out);
}

gBrowser.addEventListener("RJSCallTest", RCallTest, false, true);
CustomButton = {

1: function () {
	alert("Just testing");	
  },

}


   
