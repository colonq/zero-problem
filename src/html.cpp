#include "html.h"
#include "string_util.h"

#include <sstream>
#include <set>
#include <boost/regex.hpp>

static const char* const html_1[] = {
	R"*(<!DOCTYPE html>)*",
	R"*(<html>)*",
	R"*(<head>)*",
	R"*(	<meta charset="utf-8">)*"
};

static const char* const html_2[] = {
	R"*(	<style type="text/css">)*",
	R"*(		body { margin: 0; padding: 0; background-color: #FAFAFA; font-size: 1vw })*",
	R"*(		#buffer { display: none })*",
	R"*(		.page { page-break-inside: avoid })*",
	R"*(		.info  { width: 50vw; height: 3vw; margin: 1vw 0; font-size: 3vw; font-family: "Roboto", "Tahoma", sans-serif; white-space: pre })*",
	R"*(		.two-canvases { width: 94vw; height: 50vw; padding-top: 1vw; margin: 0 auto; clear: left })*",
	R"*(		.view { width: 45vw; height: 45vw; margin-right: 4vw; float: left })*",
	R"*(		.view:nth-child(3n) { margin-right: 0 })*",
	R"*(		.three-svgs { width: 60vh; height: 32vh; margin: 0 auto; clear: left })*",
	R"*(		.diagram { width: 19.35vh; height: 25.8vh; margin-right: 0.975vh; float: left })*",
	R"*(		.diagram:nth-child(4n) { margin-right: 0 })*",
	R"*(		@media screen {)*",
	R"*(			.three-svgs { display:none; })*",
	R"*(		})*",
	R"*(		@media print {)*",
	R"*(			.info { width: 30vh; height: 1.8vh; margin: 0.5vh 0; font-size: 1.8vh })*",
	R"*(			.two-canvases { display:none; })*",
	R"*(		})*",
	R"*(	</style>)*",
	R"*(	<script>)*"
};

static const char* const html_3[] = {
	R"*(	</script>)*",
	R"*(</head>)*",
	R"*(<body>)*",
	R"*(</body>)*",
	R"*(</html>)*"
};

static const char* const js_load_problems_9[] = {
	R"*(		function y(g){var h=0;return function(){return h<g.length?{done:!1,value:g[h++]}:{done:!0}}}function I(g){var h="undefined"!=typeof Symbol&&Symbol.iterator&&g[Symbol.iterator];return h?h.call(g):{next:y(g)}}var J="function"==typeof Object.defineProperties?Object.defineProperty:function(g,h,k){g!=Array.prototype&&g!=Object.prototype&&(g[h]=k.value)},K="undefined"!=typeof window&&window===this?this:"undefined"!=typeof global&&null!=global?global:this;)*",
	R"*(		function L(g){if(g){for(var h=K,k=["Array","prototype","fill"],l=0;l<k.length-1;l++){var t=k[l];t in h||(h[t]={});h=h[t]}k=k[k.length-1];l=h[k];g=g(l);g!=l&&null!=g&&J(h,k,{configurable:!0,writable:!0,value:g})}}L(function(g){return g?g:function(g,k,l){var h=this.length||0;0>k&&(k=Math.max(0,h+k));if(null==l||l>h)l=h;l=Number(l);0>l&&(l=Math.max(0,h+l));for(k=Number(k||0);k<l;k++)this[k]=g;return this}});)*",
	R"*(		(function(){function g(a){this.g=document.getElementById("s"+a);this.a=new k(a,-1,!1);this.b=2280/9;this.i=this.b/2;this.c=this.b/40;this.f=.5;this.m=9.4}function h(a){var b=document.getElementById("buffer");this.c=b.getContext("2d");this.b=q[a];this.i=b.width;this.w=b.height;this.o=Math.floor(this.i/9.5*.5/2);this.a=Math.floor((this.i-2*this.o)/9);this.A=this.g=9*this.a+2*this.o;this.f=this.o+Math.floor((this.a-1)/2)+.5}function k(a,b,c){c=void 0===c?!0:c;var d=p[!0===c?Math.floor(a/2):Math.floor(a/)*",
	R"*(		3)],e=d.st;this.h=new Int8Array(e.length);for(var f=e.length,m=0;m<f;++m)this.h[m]=e[m];this.labels=new Int16Array(this.h.length);this.a=[];this.H=d.lm;!0===c?this.v=0===a%2?d.va[0]:d.va[1]:(c=d.va[0],e=d.va[1],0<a%3?this.v=0===(a+1)%2?c:e:(this.v=[],0<c.length&&c[0]&&(this.labels[c[0]]=-65),0<e.length&&e[0]&&(this.labels[e[0]]=-66)));this.color=d.cl;this.j=0;a=0>b?this.v.length:b;for(b=0;b<a;++b)this.b()}function l(a,b){var c=document.createElement("div");c.setAttribute("class","info");c.setAttribute("id",)*",
	R"*(		"i"+b);a.appendChild(c)}function t(a,b){var c=document.createElement("canvas");c.setAttribute("class","view");c.setAttribute("id","c"+b);a.appendChild(c)}function z(a,b){var c=document.createElementNS("http://www.w3.org/2000/svg","svg");c.setAttribute("class","diagram");c.setAttribute("id","s"+b);c.setAttribute("xmlns","http://www.w3.org/2000/svg");c.setAttribute("version","1.1");c.setAttribute("viewBox","0 0 2280 3040");a.appendChild(c)}function A(a,b){var c=document.createElement("div");c.setAttribute("class",)*",
	R"*(		"three-svgs");l(c,2*b+1);z(c,3*b);z(c,3*b+1);z(c,3*b+2);a.appendChild(c)}function v(a){for(var b=document.getElementById("c0").getContext("2d"),c=["WWWW","!!!!"],d=0;2>d;++d){b.font="72px "+a+", monospace, serif";var e=b.measureText(c[d]);b.font="72px monospace, serif";if(e.width!=b.measureText(c[d]).width)return!0}return!1}function r(a){return 1===a?"black":"white"}function B(a){return 1===a?"white":"black"}function D(a,b){return"-"===a.charAt(b)?null:a.charCodeAt(b)-96+11*(a.charCodeAt(b+1)-96)})*",
	R"*(		function E(a){for(var b=[],c=a.length,d=0;d<c;d+=2)b.push(D(a,d));return b}function w(a){var b=document.getElementById("buffer"),c=document.getElementById("c"+a);if(c.width!=b.width||c.height!=b.height)c.width=b.width,c.height=b.height;(new h(a)).M();document.getElementById("c"+a).getContext("2d").drawImage(document.getElementById("buffer"),0,0)}function F(){var a=document.getElementById("buffer"),b=document.getElementById("c0"),c=window.devicePixelRatio,d=b.clientWidth;b=b.clientHeight;1<c&&(d=Math.floor(d*)*",
	R"*(		c),b=Math.floor(b*c));if(a.width!=d||a.height!=b)a.width=d,a.height=b;a=2*p.length;for(c=0;c<a;++c)w(c)}function G(a){var b=q[a];return b.j<b.v.length?(q[a]=new k(a,b.j+1),!0):!1}function M(a){document.getElementById("c"+a).addEventListener("wheel",function(b){var c=document.getElementById("c"+a),d=new h(a);if(b.offsetX&&(d=d.g/d.i*c.clientWidth,c=c.clientWidth/40,b.offsetX<c||b.offsetX>d-c))return;0<=b.deltaY?G(a):(c=q[a],0<c.j&&(q[a]=new k(a,c.j-1)));w(a);b.preventDefault()})}function N(a){function b(a,)*",
	R"*(		d){82<(void 0===C[a]?1E4:d-C[a])?(C[a]=d,G(a)?(x[a]=window.requestAnimationFrame(function(c){b(a,c)}),w(a)):x[a]=void 0):x[a]=window.requestAnimationFrame(function(c){b(a,c)})}document.getElementById("c"+a).addEventListener("click",function(){0<q[a].j?(window.cancelAnimationFrame(x[a]),q[a]=new k(a,0),w(a)):b(a,0)})}function O(){var a=void 0;window.addEventListener("resize",function(){clearTimeout(a);a=setTimeout(function(){F()},300)})}var p;window.LoadProblems=function(a){p=a};var H=[-11,-1,1,11],)*",
	R"*(		n=[],q=[];k.prototype.f=function(a){for(var b=this.h,c=a.length,d=0;d<c;++d)b[a[d]]=0};k.prototype.c=function(a){var b=this.h,c=this.color;if(b[a]===c){a=[a];for(var d=new Int8Array(b.length),e=[];0<a.length;){var f=a.pop();d[f]=1;if(0===b[f])return;if(b[f]===c){e.push(f);for(var m=I(H),g=m.next();!g.done;g=m.next())g=f+g.value,d[g]||a.push(g)}}this.f(e)}};k.prototype.b=function(){var a=this.v,b=this.j;if(!(b>=a.length))if(a=a[b],++this.j,this.H=a,null===a||0!==this.h[a])this.color=1==this.color?)*",
	R"*(		2:1;else{this.h[a]=this.color;this.color=1==this.color?2:1;b=I(H);for(var c=b.next();!c.done;c=b.next())this.c(a+c.value);b=this.labels[a];this.labels[a]=this.j;b&&(c=this.a,void 0===c[a]&&(c[a]=[]),c[a].push(b))}};h.prototype.F=function(){var a=this.c;a.clearRect(0,0,this.i,this.w);a.fillStyle="#F9BF6D";a.fillRect(0,0,this.g,this.A)};h.prototype.N=function(){var a=this.c;a.strokeStyle="black";a.lineWidth=this.g/1E3;for(var b=this.f,c=this.a,d=8*c,e=b+8*c,f=1;8>f;++f){var m=b+c*f;a.beginPath();a.moveTo(b,)*",
	R"*(		m);a.lineTo(e,m);a.stroke()}for(f=1;8>f;++f)m=b+c*f,a.beginPath(),a.moveTo(m,b),a.lineTo(m,e),a.stroke();a.beginPath();a.rect(b,b,d,d);a.stroke()};h.prototype.m=function(a,b,c,d){var e=this.c,f=this.a;a=d+f*a;b=d+f*b;e.beginPath();e.arc(a,b,c,0,2*Math.PI,!1);e.fill()};h.prototype.D=function(){this.c.fillStyle="#7C5F36";for(var a=this.f+(this.a/2-this.a/2.3),b=this.b.h,c=this.a/2.1,d=0;9>d;++d)for(var e=0;9>e;++e){var f=11*(d+1)+(e+1);1!==b[f]&&2!==b[f]||this.m(e,d,c,a)}};h.prototype.u=function(a,)*",
	R"*(		b,c,d){this.c.fillStyle=d;this.m(a,b,c,this.f)};h.prototype.L=function(){for(var a=this.b.h,b=this.a/2.1,c=0;9>c;++c)for(var d=0;9>d;++d){var e=11*(c+1)+(d+1);1===a[e]?this.u(d,c,b,"black"):2===a[e]&&this.u(d,c,b,"white")}};h.prototype.C=function(){if(0===this.b.j){var a=this.b.H;if(null!==a){var b=Math.floor(a/11)-1;this.c.fillStyle=r(this.b.color);this.m(a%11-1,b,this.a/10,this.f)}}};h.prototype.B=function(a,b,c){var d=this.c,e=.6*this.a,f=this.f+this.a*(a%11-1),m=this.f+this.a*(Math.floor(a/11)-)*",
	R"*(		1)+e*b.l;d.font=e+"px "+b.name;d.fillStyle=B(this.b.h[a]);d.textAlign="center";d.textBaseline="alphabetic";d.fillText(c,f,m)};h.prototype.K=function(){for(var a=this.b.labels,b=this.b.h,c=a.length,d=0;d<c;++d)if(a[d]&&0!==b[d]){var e=String(a[d]);this.B(d,1>=e.length?n[0]:n[1],e)}};h.prototype.M=function(){this.F();this.D();this.N();this.L();this.C();this.K()};g.prototype.R=function(a,b,c,d,e){var f=document.createElementNS("http://www.w3.org/2000/svg","rect");f.setAttribute("x",a);f.setAttribute("y",)*",
	R"*(		b);f.setAttribute("width",c);f.setAttribute("height",d);f.setAttribute("fill","none");f.setAttribute("stroke","black");f.setAttribute("stroke-width",e);this.g.appendChild(f)};g.prototype.A=function(a,b,c,d,e){var f=document.createElementNS("http://www.w3.org/2000/svg","line");f.setAttribute("x1",a);f.setAttribute("y1",b);f.setAttribute("x2",c);f.setAttribute("y2",d);f.setAttribute("stroke","black");f.setAttribute("stroke-width",e);f.setAttribute("stroke-linecap","butt");this.g.appendChild(f)};g.prototype.N=)*",
	R"*(		function(a,b,c,d,e){var f=document.createElementNS("http://www.w3.org/2000/svg","circle");f.setAttribute("cx",a);f.setAttribute("cy",b);f.setAttribute("r",c);f.setAttribute("fill",e);f.setAttribute("stroke","black");f.setAttribute("stroke-width",d);this.g.appendChild(f)};g.prototype.B=function(){for(var a=this.i,b=this.b,c=this.c,d=a+8*b,e=1;8>e;++e){var f=a+b*e;this.A(a,f,d,f,c)}for(e=1;8>e;++e)f=a+b*e,this.A(f,a,f,d,c);this.R(a,a,d-a,d-a,3*c)};g.prototype.o=function(a,b,c,d){var e=document.createElementNS("http://www.w3.org/2000/svg",)*",
	R"*(		"rect");e.setAttribute("x",a);e.setAttribute("y",b);e.setAttribute("width",c);e.setAttribute("height",d);e.setAttribute("fill","white");this.g.appendChild(e)};g.prototype.w=function(a,b,c){var d=this.i,e=this.b;this.N(d+e*a,d+e*b,e/2-this.c,1.5*this.c,c)};g.prototype.L=function(){for(var a=this.a.h,b=3*this.c,c=2*b,d=this.i,e=this.b,f=0;9>f;++f)for(var m=0;9>m;++m){var g=11*(f+1)+(m+1),h=a[g];if(1===h||2===h){var k=a[g+1];1!==k&&2!==k||this.o(d+e*(m+.5)-b,d+e*f-b,c,c);g=a[g+9+2];1!==g&&2!==g||this.o(d+)*",
	R"*(		e*m-b,d+e*(f+.5)-b,c,c);this.w(m,f,r(h))}}};g.prototype.T=function(a,b,c,d,e){var f=this.i,g=this.b;a=f+g*a;b=f+g*b;c="M"+(a-c*Math.sqrt(3)/2)+" "+(b+c/2)+" L "+a+" "+(b-c)+" L "+(a+c*Math.sqrt(3)/2)+" "+(b+c/2)+" Z";b=document.createElementNS("http://www.w3.org/2000/svg","path");b.setAttribute("d",c);b.setAttribute("fill","none");b.setAttribute("stroke",e);b.setAttribute("stroke-width",d);this.g.appendChild(b)};g.prototype.C=function(){if(0===this.a.j){var a=this.a.H;if(null!==a){var b=2*this.c;)*",
	R"*(		this.T(a%11-1,Math.floor(a/11)-1,this.b/2-this.c/2-b,b,r(this.a.color))}}};g.prototype.S=function(a,b,c,d,e,f){var g=document.createElementNS("http://www.w3.org/2000/svg","text");2<f.length?(g.setAttribute("x",0),g.setAttribute("y",0),g.setAttribute("transform","translate("+a+","+b+")scale(0.7,1)")):(g.setAttribute("x",a),g.setAttribute("y",b));g.setAttribute("font-family",c);g.setAttribute("font-weight","bold");g.setAttribute("font-size",d);g.setAttribute("text-anchor","middle");g.setAttribute("stroke",)*",
	R"*(		"none");g.setAttribute("fill",e);g.textContent=f;this.g.appendChild(g)};g.prototype.u=function(a,b,c,d,e){var f=c.name,g=e.charCodeAt(0),h=48<=g&&58>g,k=this.b*(h?.7:.9),l=this.i,n=this.b;a=n*a;b=n*b;var p=l+a+(h?-this.c/2:0);c=l+b+k*(97<=g&&123>g?c.s:c.l);h||this.o(a,b,n,n);this.S(p,c,f,k,d,e)};g.prototype.K=function(){for(var a=this.a.labels,b=a.length,c=0;c<b;++c){var d=a[c];d&&(d=0<d?String(d):"Z",this.u(c%11-1,Math.floor(c/11)-1,1>=d.length?n[0]:n[1],B(this.a.h[c]),d))}};g.prototype.O=function(a,)*",
	R"*(		b){var c=this.f,d=this.m;a=String(a);7.5<c&&(c=.5,d+=1,this.f=c,this.m=d);10.47<d||(this.w(c,d,r(b)),this.u(c,d,1>=a.length?n[0]:n[1],B(b),a),this.f+=1.1)};g.prototype.U=function(a,b){for(var c=0,d=b.length,e=0;e<d;++e){var f=b.charAt(e);c="("===f?c+a.J:")"===f?c+a.I:c+a.G}return c};g.prototype.F=function(a){var b=this.f,c=this.m;a="("+a+")";var d=n[1],e=this.b,f=e-2*this.c,g=-this.c/2,h=this.i+f*d.l,k=document.createElementNS("http://www.w3.org/2000/svg","text"),l=this.U(d,a)*f/e;b>9-l-.5&&(b=.5,)*",
	R"*(		c+=1,this.f=b,this.m=c);10.47<c||(k.setAttribute("x",b*e+g),k.setAttribute("y",c*e+h),k.setAttribute("font-family",d.name),k.setAttribute("font-size",f),k.setAttribute("stroke","none"),k.setAttribute("fill","black"),k.textContent=a,this.g.appendChild(k),this.f+=l+.4)};g.prototype.P=function(a){var b=this.a.color;b=this.a.j%2?[b,1==b?2:1]:[1==b?2:1,b];for(var c=a.length,d=0;d<c-1;++d){var e=a[d];this.O(e,b[e%2])}this.F(a[c-1])};g.prototype.D=function(){for(var a=this.a.labels,b=this.a.a,c=[],d=b.length,)*",
	R"*(		e=0;e<d;++e){var f=b[e];void 0!==f&&0<f.length&&(c[f[0]]=f.concat(a[e]))}a=I(c);for(b=a.next();!b.done;b=a.next())b=b.value,void 0!==b&&this.P(b)};g.prototype.M=function(){this.B();this.L();this.C();this.K();this.D()};var x=[],C=[];window.addEventListener("DOMContentLoaded",function(){var a=document.createElement("canvas");a.setAttribute("id","buffer");document.body.appendChild(a);a=p.length;for(var b=0;b<a;++b){var c=b,d=document.createElement("div");d.setAttribute("class","two-canvases");l(d,2*)*",
	R"*(		c);t(d,2*c);t(d,2*c+1);document.body.appendChild(d)}for(b=0;b<a;b+=3)c=b,d=document.createElement("div"),d.setAttribute("class","page"),c<a&&A(d,c),c+1<a&&A(d,c+1),c+2<a&&A(d,c+2),document.body.appendChild(d);v("Roboto")&&v("Roboto Condensed")?(n[0]={name:"Roboto",l:.35546875,s:541/2048},n[1]={name:"Roboto Condensed",l:.35546875,s:541/2048,I:643/2048,J:.318359375,G:.49365234375}):v("Verdana")&&v("Tahoma")?(n[0]={name:"Verdana",l:.36328125,s:558.5/2048},n[1]={name:"Tahoma",l:.36328125,s:558.5/2048,)*",
	R"*(		I:.3828125,J:.3828125,G:.5458984375}):(n[0]={name:"sans-serif",l:.36328125,s:558.5/2048},n[1]={name:"sans-serif",l:.36328125,s:558.5/2048,I:.3828125,J:.3828125,G:.5458984375});a=2*p.length;for(b=0;b<a;++b)c=Math.floor(b/2),d=p[c],document.getElementById("i"+b).appendChild(document.createTextNode("#"+(c+1)+"  "+d.wr[0]+"%/"+d.wr[1]+"%"));a=p.length;for(b=0;b<a;++b){c=p[b];for(var e,f=c.st,h=[],u=f.length,r=0;r<u;++r)e=f.charCodeAt(r)-64,d=e%3,h.push(d),e=(e-d)/3,d=e%3,h.push(d),e=(e-d)/3,d=e%3,h.push(d);)*",
	R"*(		d=h;e=[];for(h=f=0;11>h;++h)for(u=0;11>u;++u)0===h||10===h||0===u||10===u?e.push(4):(e.push(d[f]),++f);c.st=e;c.lm=D(c.lm,0);c.cl="B"===c.cl?1:2;c.va[0]=E(c.va[0]);c.va[1]=E(c.va[1])}a=2*p.length;for(b=0;b<a;++b)q.push(new k(b,0));F();a=3*p.length;for(b=0;b<a;++b)(new g(b)).M();a=2*p.length;for(b=0;b<a;++b)M(b);a=2*p.length;for(b=0;b<a;++b)N(b);O()})})();)*"
};

static const char* const js_load_problems_19[] = {
	R"*(		function y(e){var g=0;return function(){return g<e.length?{done:!1,value:e[g++]}:{done:!0}}}function I(e){var g="undefined"!=typeof Symbol&&Symbol.iterator&&e[Symbol.iterator];return g?g.call(e):{next:y(e)}}var J="function"==typeof Object.defineProperties?Object.defineProperty:function(e,g,k){e!=Array.prototype&&e!=Object.prototype&&(e[g]=k.value)},K="undefined"!=typeof window&&window===this?this:"undefined"!=typeof global&&null!=global?global:this;)*",
	R"*(		function L(e){if(e){for(var g=K,k=["Array","prototype","fill"],l=0;l<k.length-1;l++){var t=k[l];t in g||(g[t]={});g=g[t]}k=k[k.length-1];l=g[k];e=e(l);e!=l&&null!=e&&J(g,k,{configurable:!0,writable:!0,value:e})}}L(function(e){return e?e:function(e,k,l){var g=this.length||0;0>k&&(k=Math.max(0,g+k));if(null==l||l>g)l=g;l=Number(l);0>l&&(l=Math.max(0,g+l));for(k=Number(k||0);k<l;k++)this[k]=e;return this}});)*",
	R"*(		(function(){function e(a){this.c=document.getElementById("s"+a);this.a=new k(a,-1,!1);this.b=.5;this.f=19.4}function g(a){var b=document.getElementById("buffer");this.b=b.getContext("2d");this.c=p[a];this.l=b.width;this.I=b.height;this.s=Math.floor(this.l/19.5*.5/2);this.a=Math.floor((this.l-2*this.s)/19);this.J=this.j=19*this.a+2*this.s;this.f=this.s+Math.floor((this.a-1)/2)+.5}function k(a,b,c){c=void 0===c?!0:c;var d=n[!0===c?Math.floor(a/2):Math.floor(a/3)],f=d.st;this.g=new Int8Array(f.length);)*",
	R"*(		for(var h=f.length,r=0;r<h;++r)this.g[r]=f[r];this.labels=new Int16Array(this.g.length);this.a=[];this.H=d.lm;!0===c?this.v=0===a%2?d.va[0]:d.va[1]:(c=d.va[0],f=d.va[1],0<a%3?this.v=0===(a+1)%2?c:f:(this.v=[],0<c.length&&c[0]&&(this.labels[c[0]]=-65),0<f.length&&f[0]&&(this.labels[f[0]]=-66)));this.color=d.cl;this.h=0;a=0>b?this.v.length:b;for(b=0;b<a;++b)this.b()}function l(a,b){var c=document.createElement("div");c.setAttribute("class","info");c.setAttribute("id","i"+b);a.appendChild(c)}function t(a,)*",
	R"*(		b){var c=document.createElement("canvas");c.setAttribute("class","view");c.setAttribute("id","c"+b);a.appendChild(c)}function z(a,b){var c=document.createElementNS("http://www.w3.org/2000/svg","svg");c.setAttribute("class","diagram");c.setAttribute("id","s"+b);c.setAttribute("xmlns","http://www.w3.org/2000/svg");c.setAttribute("version","1.1");c.setAttribute("viewBox","0 0 2280 3040");a.appendChild(c)}function A(a,b){var c=document.createElement("div");c.setAttribute("class","three-svgs");l(c,2*b+)*",
	R"*(		1);z(c,3*b);z(c,3*b+1);z(c,3*b+2);a.appendChild(c)}function v(a){for(var b=document.getElementById("c0").getContext("2d"),c=["WWWW","!!!!"],d=0;2>d;++d){b.font="72px "+a+", monospace, serif";var f=b.measureText(c[d]);b.font="72px monospace, serif";if(f.width!=b.measureText(c[d]).width)return!0}return!1}function q(a){return 1===a?"black":"white"}function B(a){return 1===a?"white":"black"}function D(a,b){return"-"===a.charAt(b)?null:a.charCodeAt(b)-96+21*(a.charCodeAt(b+1)-96)}function E(a){for(var b=)*",
	R"*(		[],c=a.length,d=0;d<c;d+=2)b.push(D(a,d));return b}function w(a){var b=document.getElementById("buffer"),c=document.getElementById("c"+a);if(c.width!=b.width||c.height!=b.height)c.width=b.width,c.height=b.height;(new g(a)).P();document.getElementById("c"+a).getContext("2d").drawImage(document.getElementById("buffer"),0,0)}function F(){var a=document.getElementById("buffer"),b=document.getElementById("c0"),c=window.devicePixelRatio,d=b.clientWidth;b=b.clientHeight;1<c&&(d=Math.floor(d*c),b=Math.floor(b*)*",
	R"*(		c));if(a.width!=d||a.height!=b)a.width=d,a.height=b;a=2*n.length;for(c=0;c<a;++c)w(c)}function G(a){var b=p[a];return b.h<b.v.length?(p[a]=new k(a,b.h+1),!0):!1}function M(a){document.getElementById("c"+a).addEventListener("wheel",function(b){var c=document.getElementById("c"+a),d=new g(a);if(b.offsetX&&(d=d.j/d.l*c.clientWidth,c=c.clientWidth/40,b.offsetX<c||b.offsetX>d-c))return;0<=b.deltaY?G(a):(c=p[a],0<c.h&&(p[a]=new k(a,c.h-1)));w(a);b.preventDefault()})}function N(a){function b(a,d){82<(void 0===)*",
	R"*(		C[a]?1E4:d-C[a])?(C[a]=d,G(a)?(x[a]=window.requestAnimationFrame(function(c){b(a,c)}),w(a)):x[a]=void 0):x[a]=window.requestAnimationFrame(function(c){b(a,c)})}document.getElementById("c"+a).addEventListener("click",function(){0<p[a].h?(window.cancelAnimationFrame(x[a]),p[a]=new k(a,0),w(a)):b(a,0)})}function O(){var a=void 0;window.addEventListener("resize",function(){clearTimeout(a);a=setTimeout(function(){F()},300)})}var n;window.LoadProblems=function(a){n=a};var H=[-21,-1,1,21],m=[],p=[];k.prototype.f=)*",
	R"*(		function(a){for(var b=this.g,c=a.length,d=0;d<c;++d)b[a[d]]=0};k.prototype.c=function(a){var b=this.g,c=this.color;if(b[a]===c){a=[a];for(var d=new Int8Array(b.length),f=[];0<a.length;){var h=a.pop();d[h]=1;if(0===b[h])return;if(b[h]===c){f.push(h);for(var r=I(H),e=r.next();!e.done;e=r.next())e=h+e.value,d[e]||a.push(e)}}this.f(f)}};k.prototype.b=function(){var a=this.v,b=this.h;if(!(b>=a.length))if(a=a[b],++this.h,this.H=a,null===a||0!==this.g[a])this.color=1==this.color?2:1;else{this.g[a]=this.color;)*",
	R"*(		this.color=1==this.color?2:1;b=I(H);for(var c=b.next();!c.done;c=b.next())this.c(a+c.value);b=this.labels[a];this.labels[a]=this.h;b&&(c=this.a,void 0===c[a]&&(c[a]=[]),c[a].push(b))}};g.prototype.F=function(){var a=this.b;a.clearRect(0,0,this.l,this.I);a.fillStyle="#F9BF6D";a.fillRect(0,0,this.j,this.J)};g.prototype.w=function(){var a=this.b;a.strokeStyle="black";a.lineWidth=this.j/1E3;for(var b=this.f,c=this.a,d=18*c,f=b+18*c,h=1;18>h;++h){var e=b+c*h;a.beginPath();a.moveTo(b,e);a.lineTo(f,e);a.stroke()}for(h=)*",
	R"*(		1;18>h;++h)e=b+c*h,a.beginPath(),a.moveTo(e,b),a.lineTo(e,f),a.stroke();a.beginPath();a.rect(b,b,d,d);a.stroke()};g.prototype.o=function(a,b,c,d){var f=this.b,h=this.a;a=d+h*a;b=d+h*b;f.beginPath();f.arc(a,b,c,0,2*Math.PI,!1);f.fill()};g.prototype.D=function(){this.b.fillStyle="#7C5F36";for(var a=this.f+(this.a/2-this.a/2.3),b=this.c.g,c=this.a/2.1,d=0;19>d;++d)for(var f=0;19>f;++f){var h=21*(d+1)+(f+1);1!==b[h]&&2!==b[h]||this.o(f,d,c,a)}};g.prototype.C=function(a,b){var c=this.b,d=this.a,f=this.f;)*",
	R"*(		a=f+d*a;b=f+d*b;c.beginPath();c.arc(a,b,d/12,0,2*Math.PI,!1);c.fill()};g.prototype.N=function(){this.b.fillStyle="black";for(var a=0;3>a;++a)for(var b=0;3>b;++b)this.C(3+6*b,3+6*a)};g.prototype.u=function(a,b,c,d){this.b.fillStyle=d;this.o(a,b,c,this.f)};g.prototype.O=function(){for(var a=this.c.g,b=this.a/2.1,c=0;19>c;++c)for(var d=0;19>d;++d){var f=21*(c+1)+(d+1);1===a[f]?this.u(d,c,b,"black"):2===a[f]&&this.u(d,c,b,"white")}};g.prototype.B=function(){if(0===this.c.h){var a=this.c.H;if(null!==a){var b=)*",
	R"*(		Math.floor(a/21)-1;this.b.fillStyle=q(this.c.color);this.o(a%21-1,b,this.a/10,this.f)}}};g.prototype.A=function(a,b,c){var d=this.b,f=.6*this.a,h=this.f+this.a*(a%21-1),e=this.f+this.a*(Math.floor(a/21)-1)+f*b.i;d.font=f+"px "+b.name;d.fillStyle=B(this.c.g[a]);d.textAlign="center";d.textBaseline="alphabetic";d.fillText(c,h,e)};g.prototype.M=function(){for(var a=this.c.labels,b=this.c.g,c=a.length,d=0;d<c;++d)if(a[d]&&0!==b[d]){var f=String(a[d]);this.A(d,1>=f.length?m[0]:m[1],f)}};g.prototype.P=function(){this.F();)*",
	R"*(		this.D();this.w();this.N();this.O();this.B();this.M()};e.prototype.F=function(){var a=document.createElementNS("http://www.w3.org/2000/svg","rect");a.setAttribute("x",60);a.setAttribute("y",60);a.setAttribute("width",2160);a.setAttribute("height",2160);a.setAttribute("fill","none");a.setAttribute("stroke","black");a.setAttribute("stroke-width",9);this.c.appendChild(a)};e.prototype.u=function(a,b,c,d){var f=document.createElementNS("http://www.w3.org/2000/svg","line");f.setAttribute("x1",a);f.setAttribute("y1",)*",
	R"*(		b);f.setAttribute("x2",c);f.setAttribute("y2",d);f.setAttribute("stroke","black");f.setAttribute("stroke-width",3);f.setAttribute("stroke-linecap","butt");this.c.appendChild(f)};e.prototype.l=function(a,b,c,d,f){var h=document.createElementNS("http://www.w3.org/2000/svg","circle");h.setAttribute("cx",a);h.setAttribute("cy",b);h.setAttribute("r",c);h.setAttribute("fill",f);h.setAttribute("stroke","black");h.setAttribute("stroke-width",d);this.c.appendChild(h)};e.prototype.O=function(){for(var a=1;18>)*",
	R"*(		a;++a){var b=60+120*a;this.u(60,b,2220,b)}for(a=1;18>a;++a)b=60+120*a,this.u(b,60,b,2220);this.F()};e.prototype.w=function(a,b){this.l(60+120*a,60+120*b,12,3,"black")};e.prototype.J=function(){for(var a=0;3>a;++a)for(var b=0;3>b;++b)this.w(3+6*b,3+6*a)};e.prototype.j=function(a,b,c,d){var f=document.createElementNS("http://www.w3.org/2000/svg","rect");f.setAttribute("x",a);f.setAttribute("y",b);f.setAttribute("width",c);f.setAttribute("height",d);f.setAttribute("fill","white");this.c.appendChild(f)};)*",
	R"*(		e.prototype.s=function(a,b,c){this.l(60+120*a,60+120*b,57,4.5,c)};e.prototype.M=function(){for(var a=this.a.g,b=0;19>b;++b)for(var c=0;19>c;++c){var d=21*(b+1)+(c+1),f=a[d];if(1===f||2===f){var h=a[d+1];1!==h&&2!==h||this.j(60+120*(c+.5)-9,60+120*b-9,18,18);d=a[d+19+2];1!==d&&2!==d||this.j(60+120*c-9,60+120*(b+.5)-9,18,18);this.s(c,b,q(f))}}};e.prototype.S=function(a,b,c){a=60+120*a;b=60+120*b;b="M"+(a-52.5*Math.sqrt(3)/2)+" "+(b+26.25)+" L "+a+" "+(b-52.5)+" L "+(a+52.5*Math.sqrt(3)/2)+" "+(b+26.25)+)*",
	R"*(		" Z";a=document.createElementNS("http://www.w3.org/2000/svg","path");a.setAttribute("d",b);a.setAttribute("fill","none");a.setAttribute("stroke",c);a.setAttribute("stroke-width",6);this.c.appendChild(a)};e.prototype.P=function(){if(0===this.a.h){var a=this.a.H;null!==a&&this.S(a%21-1,Math.floor(a/21)-1,q(this.a.color))}};e.prototype.R=function(a,b,c,d,f,h){var e=document.createElementNS("http://www.w3.org/2000/svg","text");2<h.length?(e.setAttribute("x",0),e.setAttribute("y",0),e.setAttribute("transform",)*",
	R"*(		"translate("+a+","+b+")scale(0.7,1)")):(e.setAttribute("x",a),e.setAttribute("y",b));e.setAttribute("font-family",c);e.setAttribute("font-weight","bold");e.setAttribute("font-size",d);e.setAttribute("text-anchor","middle");e.setAttribute("stroke","none");e.setAttribute("fill",f);e.textContent=h;this.c.appendChild(e)};e.prototype.o=function(a,b,c,d,f){var e=c.name,g=f.charCodeAt(0),k=48<=g&&58>g,l=120*(k?.7:.9);a=120*a;b=120*b;var m=60+a+(k?-1.5:0);c=60+b+l*(97<=g&&123>g?c.m:c.i);k||this.j(a,b,120,)*",
	R"*(		120);this.R(m,c,e,l,d,f)};e.prototype.I=function(){for(var a=this.a.labels,b=a.length,c=0;c<b;++c){var d=a[c];d&&(d=0<d?String(d):"Z",this.o(c%21-1,Math.floor(c/21)-1,1>=d.length?m[0]:m[1],B(this.a.g[c]),d))}};e.prototype.C=function(a,b){var c=this.b,d=this.f;a=String(a);17.5<c&&(c=.5,d+=1.2,this.b=c,this.f=d);d>19*1.33-1.5||(this.s(c,d,q(b)),this.o(c,d,1>=a.length?m[0]:m[1],B(b),a),this.b+=1.1)};e.prototype.T=function(a,b){for(var c=0,d=b.length,f=0;f<d;++f){var e=b.charAt(f);c="("===e?c+a.L:")"===)*",
	R"*(		e?c+a.K:c+a.G}return c};e.prototype.B=function(a){var b=this.b,c=this.f;a="("+a+")";var d=m[1],f=60+114*d.i,e=document.createElementNS("http://www.w3.org/2000/svg","text"),g=114*this.T(d,a)/120;b>19-g-.5&&(b=.5,c+=1.2,this.b=b,this.f=c);c>19*1.33-1.5||(e.setAttribute("x",120*b+-1.5),e.setAttribute("y",120*c+f),e.setAttribute("font-family",d.name),e.setAttribute("font-size",114),e.setAttribute("stroke","none"),e.setAttribute("fill","black"),e.textContent=a,this.c.appendChild(e),this.b+=g+.4)};e.prototype.D=)*",
	R"*(		function(a){var b=this.a.color;b=this.a.h%2?[b,1==b?2:1]:[1==b?2:1,b];for(var c=a.length,d=0;d<c-1;++d){var f=a[d];this.C(f,b[f%2])}this.B(a[c-1])};e.prototype.A=function(){for(var a=this.a.labels,b=this.a.a,c=[],d=b.length,f=0;f<d;++f){var e=b[f];void 0!==e&&0<e.length&&(c[e[0]]=e.concat(a[f]))}a=I(c);for(b=a.next();!b.done;b=a.next())b=b.value,void 0!==b&&this.D(b)};e.prototype.N=function(){this.O();this.J();this.M();this.P();this.I();this.A()};var x=[],C=[];window.addEventListener("DOMContentLoaded",)*",
	R"*(		function(){var a=document.createElement("canvas");a.setAttribute("id","buffer");document.body.appendChild(a);a=n.length;for(var b=0;b<a;++b){var c=b,d=document.createElement("div");d.setAttribute("class","two-canvases");l(d,2*c);t(d,2*c);t(d,2*c+1);document.body.appendChild(d)}for(b=0;b<a;b+=3)c=b,d=document.createElement("div"),d.setAttribute("class","page"),c<a&&A(d,c),c+1<a&&A(d,c+1),c+2<a&&A(d,c+2),document.body.appendChild(d);v("Roboto")&&v("Roboto Condensed")?(m[0]={name:"Roboto",i:.35546875,)*",
	R"*(		m:541/2048},m[1]={name:"Roboto Condensed",i:.35546875,m:541/2048,K:643/2048,L:.318359375,G:.49365234375}):v("Verdana")&&v("Tahoma")?(m[0]={name:"Verdana",i:.36328125,m:558.5/2048},m[1]={name:"Tahoma",i:.36328125,m:558.5/2048,K:.3828125,L:.3828125,G:.5458984375}):(m[0]={name:"sans-serif",i:.36328125,m:558.5/2048},m[1]={name:"sans-serif",i:.36328125,m:558.5/2048,K:.3828125,L:.3828125,G:.5458984375});a=2*n.length;for(b=0;b<a;++b)c=Math.floor(b/2),d=n[c],document.getElementById("i"+b).appendChild(document.createTextNode("#"+)*",
	R"*(		(c+1)+"  "+d.wr[0]+"%/"+d.wr[1]+"%"));a=n.length;for(b=0;b<a;++b){c=n[b];for(var f,h=c.st,g=[],u=h.length,q=0;q<u;++q)f=h.charCodeAt(q)-64,d=f%3,g.push(d),f=(f-d)/3,d=f%3,g.push(d),f=(f-d)/3,d=f%3,g.push(d);d=g;f=[];for(g=h=0;21>g;++g)for(u=0;21>u;++u)0===g||20===g||0===u||20===u?f.push(4):(f.push(d[h]),++h);c.st=f;c.lm=D(c.lm,0);c.cl="B"===c.cl?1:2;c.va[0]=E(c.va[0]);c.va[1]=E(c.va[1])}a=2*n.length;for(b=0;b<a;++b)p.push(new k(b,0));F();a=3*n.length;for(b=0;b<a;++b)(new e(b)).N();a=2*n.length;for(b=)*",
	R"*(		0;b<a;++b)M(b);a=2*n.length;for(b=0;b<a;++b)N(b);O()})})();)*"
};

static const boost::regex reBoardSize(R"(\t++sz: (\d+))");
static int GetBoardSize(const std::string& problem)
{
	boost::smatch m;
	if (boost::regex_search(problem, m, reBoardSize)) {
		try {
			auto n = std::stoi(m.str(1));
			return n;
		}
		catch (...) {
			return 0;
		}
	}
	return 0;
}

static const boost::regex reStones(R"(\t++st: '([@A-Z]++)')");
static std::string GetStones(const std::string& problem)
{
	boost::smatch m;
	if (boost::regex_search(problem, m, reStones)) {
		return m.str(1);
	}
	return std::string();
}

std::string GetHtml(const std::string& title, const std::vector<std::string>& problems, int boardSize)
{
	// 問題が一つもなければエラー
	if (problems.empty()) {
		return std::string();
	}

	std::ostringstream o;
	for (const auto& s : html_1) {
		o << s << '\n';
	}
	o << "<title>" << title << "</title>\n";
	for (const auto& s : html_2) {
		o << s << '\n';
	}
	if (boardSize == 9) {
		for (const auto& s : js_load_problems_9) {
			o << s << '\n';
		}
	} else if (boardSize == 19) {
		for (const auto& s : js_load_problems_19) {
			o << s << '\n';
		}
	} else {
		// ボードサイズが9/19以外ならば失敗にする
		return std::string();
	}
	o << "\t\tLoadProblems([";
	std::set<std::string> st;
	bool firstTime = true;
	for (const auto& p : problems) {

		// 引数で指定された碁盤のサイズと異なる問題は出力しない
		if (GetBoardSize(p) != boardSize) {
			continue;
		}

		// 初期配置と手番を確認して既出の問題ならば出力しない
		auto stones = GetStones(p);
		if (!stones.empty()) {
			if (GetProblemColor(p) == Color::BLACK) {
				stones.append("#");
			}
			auto it = st.find(stones);
			if (it == st.end()) {
				if (firstTime) {
					o << "\n";
					firstTime = false;
				} else {
					o << ",\n";
				}
				o << p;
				st.insert(stones);
			}
		}
	}
	o << '\n';

	// 出力できる問題がないので失敗にする
	if (st.empty()) {
		return std::string();
	}

	o << "\t\t]);\n";
	for (const auto& s : html_3) {
		o << s << '\n';
	}
	return o.str();
}

static const boost::regex reHtmlTitle(R"(<title>(.+?)</title>)");

// HTML内からタイトル文字列を抜き出す
std::string GetHtmlTitle(const std::string& html)
{
	boost::smatch m;
	if (boost::regex_search(html, m, reHtmlTitle)) {
		return m.str(1);
	}
	return std::string();
}

static const boost::regex reStartProblems(R"(\t++LoadProblems\(\[.*+)");
static const boost::regex reEndProblems(R"(\t++\]\);.*+)");
static const boost::regex reStart(R"(\t++\{.*+)");
static const boost::regex reEnd(R"(\t++\}.*+)");

// HTML内から問題データ部分を抜き出す
std::vector<std::string> GetProblems(const std::string& html)
{
	std::vector<std::string> problems;
	std::istringstream iss(html);
	std::string line1;
	while (std::getline(iss, line1)) {
		if (boost::regex_match(line1, reStartProblems)) {
			break;
		}
	}
	while (std::getline(iss, line1)) {
		if (boost::regex_match(line1, reEndProblems)) {
			break;
		}
		if (boost::regex_match(line1, reStart)) {
			std::string line2, s;
			while (std::getline(iss, line2)) {
				if (boost::regex_match(line2, reEnd)) {
					if (!s.empty()) {
						problems.push_back(std::string("\t\t{\n") + s + "\t\t}");
					}
					break;
				}
				Trim(line2, '\r');
				s += line2;
				s += '\n';
			}
		}
	}
	return problems;
}

static const boost::regex reColor(R"(\t++cl: '([BW])')");

// 問題の手番を得る
Color GetProblemColor(const std::string& problem)
{
	boost::smatch m;
	if (boost::regex_search(problem, m, reColor)) {
		auto s = m.str(1);
		if (s == "B") {
			return Color::BLACK;
		} else if (s == "W") {
			return Color::WHITE;
		}
	}
	return Color::EMPTY;
}
