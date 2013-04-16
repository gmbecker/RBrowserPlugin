#ifndef JS_INIT_H
#define JS_INIT_H 1
//static const char *JS_INIT_CODE = "function runScriptsNPAPI(){var e=window.document.getElementsByTagName('embed');var t=window.document.getElementsByTagName('script');var n;for(var r=0;r<e.length;r++){if(e[r].type=='application/r'){n=e[r];break}}if(typeof n=='undefined')return;var i;var s;var o;for(var u=0;u<t.length;u++){i=t[u];if(i.type=='text/R'){o=n.eval(i.text);if(i.src)o=n.source(i.src)}}window.R=n;var a=document.createEvent('Events');a.initEvent('rcoderun',true,false);document.dispatchEvent(a)}function namedArgsCall(e,t){R[e](args(t))}function makeFun(e){function t(){var t=Array.prototype.slice.call(arguments);window.R.listCall(e,t)}return t}function args(e){e.namedArgsForR=true;if(typeof e.convertRet=='undefined'||e.convertRet=='default'||e.convertRet==1)e.convertRet=1;else if(e.convertRet=='reference'||e.convertRet==0)e.convertRet=0;else if(e.convertRet=='copy'||e.convertRet==2)e.convertRet=2;else if((e.convertRet=='custom'||e.convertRet==3)&&e.convertFun!='undefined')e.convertRet=3;else throw'Unrecognized value for convertRet argument:'+e.convertRet;return e}function create0(e){var t=new e;return t}function create1(e,t){var n=new e(t);return n}function create2(e,t,n){var r=new e(t,n);return r}function create3(e,t,n,r){var i=new e(t,n,r);return i}function create4(e,t,n,r,i){var s=new e(t,n,r,i);return s}window.emptyArg={emptyRArg:true};runScriptsNPAPI();//setTimeout(runScriptsNPAPI,200)";

static const char *JS_INIT_CODE = "function runScriptsNPAPI(){var e=window.document.getElementsByTagName('embed');var t=window.document.getElementsByTagName('script');var n;for(var r=0;r<e.length;r++){if(e[r].type=='application/test-r'||e[r].type=='application/r'){n=e[r];break}}if(typeof n=='undefined')return;var i;var s;var o;for(var u=0;u<t.length;u++){i=t[u];if(i.type=='text/R'){o=n.eval(i.text);if(i.src)o=n.source(i.src)}}window.R=n;var a=document.createEvent('Events');a.initEvent('rcoderun',true,false);document.dispatchEvent(a)}function namedArgsCall(e,t){R[e](args(t))}function makeFun(e){function t(){var t=Array.prototype.slice.call(arguments);window.R.listCall(e,t)}return t}function args(e){e.namedArgsForR=true;if(typeof e.convertRet=='undefined'||e.convertRet=='default'||e.convertRet==0)e.convertRet=0;else if(e.convertRet=='reference'||e.convertRet==1)e.convertRet=1;else if(e.convertRet=='copy'||e.convertRet==2)e.convertRet=2;else if((e.convertRet=='custom'||e.convertRet==3)&&e.convertFun!='undefined')e.convertRet=3;else throw'Unrecognized value for convertRet argument:'+e.convertRet;return e}function create0(e){var t=new e;return t}function create1(e,t){var n=new e(t);return n}function create2(e,t,n){var r=new e(t,n);return r}function create3(e,t,n,r){var i=new e(t,n,r);return i}function create4(e,t,n,r,i){var s=new e(t,n,r,i);return s}window.emptyArg={emptyRArg:true};setTimeout(runScriptsNPAPI,200)";
#endif //JS_INIT_CODE
