"use strict";
/**
 * The frontend module provides methods for interacting with the GUI
 * @module frontend
 */

/**
 * "kernel" manages the apps
 * @class Manager
 * @namespace an
 * @static
 */
an.Manager = (function(){
            this.apps = []; // {name, running}
            this.dock = null;
/*    function refreshMenu() {
        $('appdock_c').empty();
            if(this.dock !== null)
                this.dock.el.destroy();
                
            var buffer ="<ul id='appdock_list'>";
            for (var i = 0; i < this.apps.length;++i)
             buffer += "<li class='appdock'><a href='#' onclick='an.Manager.runApp("+i+");'><img src='apps/active/"+
             this.apps[i].name+ "/logo.png' alt='"+this.apps[i].name+"'/></a></li>\n";  
    
            buffer+="</ul>";
            $("appdock_c").set('html',buffer);
         
            this.dock = new UvumiDock("appdock_list",{position: "top"},"appdock_c");
        }*/
        
        // public functions
    return {
        runApp: function(name) {
          var app = null;

            for(var i=0;i<apps.length;++i)
                if(apps[i].name == name)
                    app = apps[i];
                    
            if(app === null) {
                alert("app not found?!?");
                return;
            }
            if(app.running) {
                app.window.focus();
                return;
            }
            
            app.window = open("launch.html?app="+name);
        },
        addApp: function (name) {
                apps.push({name: name, running: false,window: null});
        },
        refreshMenu: function() {
         $('an_wrapper').empty();
            
            var buffer ="";
            for (var i = 0; i < apps.length;++i)
             buffer += "<div class='an_wrapper_el'><a href='#' onclick=\"an.Manager.runApp('"+apps[i].name+"');\"><img src='apps/active/"+
             apps[i].name+ "/logo.png' alt='"+apps[i].name+"'/><div class='an_wrapper_title'>"+apps[i].name+"</div></a></div>\n";  
    
            $("an_wrapper").set('html',buffer);       
        }
    };
})();


Element.implement({
    append: function (html){
    if($type(html)!=='string') return false;
    var temp=new Element('div');
    temp.set('html',html);
    this.adopt(temp.childNodes);
    return this;

}});

window.addEvent('domready', function() {

    new Request.JSON({url: "list_apps", onSuccess: function(res) {
            res.each(an.Manager.addApp,an.Manager);
                        an.Manager.refreshMenu();

            }}).get();
        
 // playing with mootools xD
    var quoteChanger = function() { this.fx.start(this.quotes[Math.floor(Math.random()*this.quotes.length)]); };
    var qc_context = { quotes: [ 
    "You must not lose faith in humanity. Humanity is an ocean; if a few drops of the ocean are dirty, the ocean"+
     "does not become dirty - Mahatma Gandhi ", 
    "You must be the change you wish to see in the world - Mahatma Gandhi",
    "If we can really understand the problem, the answer will come out of it, because the answer is not separate"+
     "from the problem - Jiddu Krishnamurti",
    "It is no measure of health to be well adjusted to a profoundly sick society - Jiddu Krishnamurti",
    "Freedom is the freedom to say that two plus two make four. If that is granted, all else follows.",
    "The Fight for the Future Begins",
    "Enlightenment is man’s leaving his self-caused immaturity. Immaturity is the incapacity to use one's"+
     "intelligence without the guidance of another - Immanuel Kant",
     "There is ... only a single categorical imperative and it is this: Act only on that maxim through which"+
      "you can at the same time will that it should become a universal law - Immanuel Kant",
      "" 
    ], fx: new Fx.Text('top_quote', {
                duration: 'long',
                transition: Fx.Transitions.Bounce.easeOut
                })
        };
    quoteChanger.periodical(7000, qc_context);        
});
