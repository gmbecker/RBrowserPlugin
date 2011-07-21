
var TestDBIface = false;

function go() {

	try {
		netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
    	        const cid = "@R/Revaluator;1";
		obj = Components.classes[cid].createInstance();
		obj = obj.QueryInterface(Components.interfaces.REvaluator);
	} catch (err) {
		alert( " failed to start R " + err);
		return;
	}
        var args = ["R"];
	alert("Calling obj.init");
        obj.init(args, 1);

	//alert("JS-R object: " + obj);
	    //alert("R version: " + obj.version);
	    var lib = obj.library("RFirefox");
	    //alert("lib" + lib);
	    var rn = obj.eval("1:10");
	    
	    alert("Evaling R Code:" + rn); 
}

