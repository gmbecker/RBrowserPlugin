function jsREventHandler( name, value)
{
    netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
    var out;
    Rev.call1(name, value,  out);
}

function addREventHandler( name, targ, event, arg)
{
    var fun = function() {
	netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
	var out;
	Rev.call1(name, arg,  out);
    }
    targ.addEventListener(event, fun, true);    
}
	

