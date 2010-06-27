var an = {};
var an_basepath = "../../../../";

// recoursively load the depnedencies, after "onload" of the pervious
an_load_js("../../../../third-party/mootools-core.js",
function() {
    an_load_js("../../../../third-party/mootools-more.js",
        function() {
            an_load_js("../../../../lib/lib.js",function(){
                if($defined(AN_BS_LOAD))
                    AN_BS_LOAD.each(function (item) {
                        an_load_js("../src/"+item);
                        });
            });
    });
});

function an_load_js(fn,cb) {
    var fp = document.createElement('script');
    fp.setAttribute("type","application/javascript");
    fp.setAttribute("src",fn);
    fp.addEventListener("load",cb);
    document.getElementsByTagName("head")[0].appendChild(fp);
}

function an_load_css(fn,cb) {
    var fp = document.createElement('link');
    fp.setAttribute("type","text/css");
    fp.setAttribute("rel","stylesheet");
    fp.setAttribute("href",fn);
    fp.addEventListener("load",cb);
    document.getElementsByTagName("head")[0].appendChild(fp);
}

var an_frameworks = { "jQuery":["jquery-1.4.2.min.js","jquery-ui-1.8.2.custom.min.js"] };
var an_frameworks_style = { "jQuery" : {"ui-lightness": "jquery-ui-1.8.2.custom.css"} };

function an_load_framework(name,cb) {
    for(var i=0;i<an_frameworks[name].length;i++)
        an_load_js(an_basepath+"Frameworks/"+name+"/"+an_frameworks[name][i]);
}

function an_load_framework_style(fn,css) {
    var o = an_frameworks_style[fn];
    an_load_framework(fn);
        an_load_css(an_basepath+"Frameworks/"+fn+"/css/"+css+"/"+o[css]);
}