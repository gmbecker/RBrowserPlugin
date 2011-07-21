
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
        obj.init(args, 1);

	//    alert("JS-R object: " + obj);
    alert("R version: " + obj.version);

//    var lib = obj.call1("library", "RFirefox");
    var lib = obj.library("RFirefox");
//    alert("loaded RFirefox " + lib);

   var rn = obj.eval("1:10");
   alert("1:10 " + rn + " " + rn.length);

  rn = obj.call1("rnorm", 10);
  alert("rnorm(10) " + rn + " " + rn.length);

  if(TestDBIface) {
    var ev = obj.eval("print(1:10)");
    ev = obj.eval("x <- rnorm(100)");
    ev = obj.eval("rnorm(x)");
    alert("rnorm(x) -> " + ev);

    var rx;
    rx = obj.exists("x");
    alert(" x is defined in R? " + rx + " y defined ? " + obj.exists("y"));

    rx = obj.get("x");
    alert("value of x: " + rx);

    rx = obj.assign("y", true);
    alert("assigned y true?" + rx +  ",  value of y: " + obj.get("y"));

    rx = obj.remove("y");
    alert("removed y. Status of removal " + rx);

    rx = obj.exists("y");
    alert("y exists: " + rx);
    obj.eval("print(objects())");

}

    obj.call1("print", document);

    ev = obj.eval("rnorm");
    alert("rnorm -> " + ev);


           /* Calls with different data types */
//    obj.cal1("foo", document.addEventListener);
	var res = obj.call1("foo", 4);

	var res1 = obj.call1("foo", 14);
	var res2 = obj.call1("foo", 14.2);
        var res3 = obj.call1("foo", true);
        var res4 = obj.call1("foo", "1.3");

        var robj = obj.call1("foo", obj);
    alert("called foo(obj) " + robj);



        var res5 = obj.call1("foo", [1, 2.0, 3]);
        res5 = obj.call1("foo", [1, 2.2, 3]);
        res5 = obj.call1("foo", [true, false, false, true]);
        res5 = obj.call1("foo", [obj, document, window]);

    var h = {foo: 1, bar: "abc"};
    obj.call1("foo", h);

    alert('Performing\n foo(4). Returned ' + res + ';\n foo(14) ' + res1 + ';\n foo(14.2) ' + res2
       + ';\n foo(true) ' + res3  + ";\n foo('1.3')" + res4 
        + ';\n foo(obj) ' + robj + 
        ";\n foo([obj,document, window]) " + res5);

//       obj.call1("", 14);
//    alert('failing at ' + obj.call1("", 14));
}

