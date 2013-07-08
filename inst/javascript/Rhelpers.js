function runScriptsNPAPI()
{
    var embs, stags, Reng, mytag, code, out, evt;
    embs = window.document.getElementsByTagName('embed');
    stags = window.document.getElementsByTagName('script');
    for (var j=0; j < embs.length; j++)
	{
	    if (embs[j].type == 'application/test-r' || embs[j].type == 'application/r')
		{
		    Reng = embs[j];
		    break;
		}
	}
    if(typeof(Reng) == 'undefined')
	return;
    //alert('stags length:'+ stags.length);
    for(var i=0; i < stags.length; i++)
	{
	    mytag = stags[i];
	    if (mytag.type == 'text/R')
		{
		    //if(mytag.text != ' ')
			out = Reng.eval(mytag.text);
			if(mytag.src)
			    out = Reng.source( mytag.src)
		}
	}
    window.R = Reng;
    evt = document.createEvent('Events');
    evt.initEvent('rcoderun', true, false);
    document.dispatchEvent(evt);
    
}

function namedArgsCall(name, argvals)
{
    R[name](args(argvals));
}

function makeFun(obj)
{
    //    var Reng;
    function fun()
    {
	var args = Array.prototype.slice.call(arguments);
	//	alert('here I am');
	window.R.listCall(obj, args);
    }
    return fun;
}
//window.addEventListener('load', doAttach, true, true);


function args(obj)
{
    obj.namedArgsForR = true;
    if(typeof obj.convertRet == 'undefined' || obj.convertRet == 'default' || obj.convertRet == 1)
	obj.convertRet = 1; //default
    else if (obj.convertRet == 'reference' || obj.convertRet == 0)
	obj.convertRet = 0; //force reference
    else if (obj.convertRet == 'copy' || obj.convertRet == 2)
	obj.convertRet = 2; //force copy
    else if ((obj.convertRet == 'custom' || obj.convertRet ==  3) && obj.convertFun != 'undefined')
	obj.convertRet = 3;
    else
	throw 'Unrecognized value for convertRet argument:' + obj.convertRet;
    return obj;
}

window.emptyArg = {emptyRArg:true};

function create0(obj)
{
   var ret =  new obj();
   return ret;
}

function create1(obj, param)
{
   var ret =  new obj(param);
   return ret;
}

function create2(obj, param1, param2)
{
    var ret = new obj(param1, param2);
    return ret;
}

function create3(obj, param1, param2, param3)
{
    var ret = new obj(param1, param2, param3);
    return ret;
}

function create4(obj, param1, param2, param3, param4)
{
    var ret = new obj(param1, param2, param3, param4);
    return ret;
}

window.emptyArg={emptyRArg:true};
window.standard = 0;
window.reference = 1;
window.copy=2;

function convertRet(x)
{
    return {behavior:x, _convertRet:true};
}

function offsets(e)
{
    var ret = new Array(2);
    if(typeof( e.offsetX) != 'undefined')
        ret = [e.offsetX, e.offsetY];
    else  
        ret =  [e.pageX - e.target.offsetLeft, e.pageY - e.target.offsetTop];
    return ret;
}

function makeHandler(target, etype, rfun)
{
    /*
    // when adding functions using prototype, "this" is the
    // object which the new function was called on 
    var callingFunction = this;
    
    // pass the desired scope object as the first arg
    var scope = arguments[0];
    
    // create a new arguments array with the first arg removed 
    var otherArgs = [];
    for(var i = 1; i < arguments.length; i++){ 
	otherArgs.push(arguments[i]);
    }
    
    // return a function remembering to include the event 
    return function(e) {
	// Add the event object to the arguments array
	otherArgs.push(e || window.event);
	// Array is in the wrong order so flip it
	otherArgs.reverse();
	
	// Now use apply to set scope and arguments
	callingFunction.apply(scope, otherArgs);
    }
    */
    target.addEventListener(etype, function(e) {
//	var args = [];
//	args.push(e);
//	rfun(args[0]);
	var ecpy = e;
	rfun(ecpy);
    });
    return;
}


setTimeout(runScriptsNPAPI, 200);