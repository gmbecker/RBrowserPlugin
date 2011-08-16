
    try {
	//	console.log("inside try statement");
		netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
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
    netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
    /*
    try {
	//	console.log("inside try statement");
	//	netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
	//const cid = "@R/Revaluator;1";
	//var Rev = Components.classes[cid].createInstance();
	console.log(Rev);
	Rev = Rev.QueryInterface(Components.interfaces.REvaluator);
	//console.log(Rev);
    } catch (err) {
	alert( " failed to start R " + err);
	return;
    }    

    var args = ["R"];
    Rev.init(args, 1);
    Rev.library("RFirefox");
    //stags = document.getElementsByTagName("script");

    //netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
    //Rev = GetRevaluator();
    */
    var stags = document.getElementsByTagName("script");
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
			Rev.eval(mytag.text);
			if(mytag.src)
			    Rev.call1("source", mytag.src)
		}
//mytag.addListener("load", processRTag, false);
    }
}
window.addEventListener("load", runRTags, false);


   

/*
pv.listen(window,"load",function()
{
    for(pv.$={i:0,x:document.getElementsByTagName("script")};
	pv.$.i<pv.$.x.length;pv.$.i++)
	{
	    pv.$.s=pv.$.x[pv.$.i];
	    if(pv.$.s.type=="text/javascript+protovis")
		try{
		    window.eval(pv.parse(pv.$.s.text))
			}
		catch(b){pv.error(b)}}delete pv.$});
pv.Format={};
*/
