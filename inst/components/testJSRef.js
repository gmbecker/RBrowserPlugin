function go() {

    var R;
	try {
		netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
    	        const cid = "@R/Revaluator;1";
		R = Components.classes[cid].createInstance();
		R = R.QueryInterface(Components.interfaces.REvaluator);
	} catch (err) {
		alert( " failed to start R " + err);
		return;
	}

    R.init(["R"], 1);
    R.library("RFirefox");

/* We want the conversion of an JavaScript reference to 
   try to create an instance of the corresponding R class if that is defined.
   So we need to find
 */

//    R.eval("print(.Call(\"R_getInterfaces\"))");
//    alert("Content-Type: " + document.contentType);
    R.call1("showIDocument", document);
    R.call1("jsObject", document);
    R.call1("showIDocument", document.getElementById('go'));
    R.call1("print", document.getElementById('go'));

    R.call1("jsEval", document); //, "alert(\"Hi there\");");
}

