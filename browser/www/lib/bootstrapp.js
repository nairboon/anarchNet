var an = {};
// recoursively load the depnedencies, after "onload" of the pervious
an_load_js("../../../third-party/mootools-core.js",
function() {
    an_load_js("../../../third-party/mootools-more.js",
        function() {
            an_load_js("../../../lib/lib.js",function(){});
    });
});

function an_load_js(fn,cb) {
    var fp = document.createElement('script');
    fp.setAttribute("type","application/javascript");
    fp.setAttribute("src",fn);
    fp.addEventListener("load",cb);
    document.getElementsByTagName("head")[0].appendChild(fp);
}