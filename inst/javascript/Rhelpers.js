
function runScriptsNPAPI()
{

    
    var embs = window.document.getElementsByTagName('embed');
    var stags = window.document.getElementsByTagName('script');
    var Reng;
    for (var j=0; j < embs.length; j++)
	{
	    if (embs[j].type == 'application/test-r')
		{
		    Reng = embs[j];
		    break;
		}
	}
    if(typeof(Reng) == 'undefined')
	return;
    var mytag;
    var code; 
    var out;
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
    var evt = document.createEvent('Events');
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

setTimeout(runScriptsNPAPI, 200);