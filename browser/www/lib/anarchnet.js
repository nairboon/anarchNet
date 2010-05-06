"use strict";
/* Namespace: Frontend */

/*
    Class: AppInfo
        provides information about the author, designer, license
*/
an.AppInfo = new Class({
    initialize: function() {
        this.author = "Immanuel Goldstein";
        this.logoDesigner = "Foo Bar";
        this.license = "GPL";
    }
});
/*
    Class: App
    
    Arguments: 
        name - (string) Application name
*/
    an.App = new Class({
        Implements: Events,
        initialize: function(name) {
            this.name = name;
            this.isRunning = false;
            this.isInTray = false;
            this.isWindowed = true; // "fullscreen" apps have their own "desktop" render context
            this.contextName = 'appc_' + name;
            this.contextWName = 'appcW_' + name;
            this.context = $(this.contextName);
            this.contextW = $(this.contextWName);
            this.appInfo = new an.AppInfo();
            this.dir = "apps/active/"+name+"/";
            this.menubar = $("mb_"+name);
            this.windows = [];
            
            this._windowsCount = null;
            this._js = [];
            this._css = [];
        },
        loadJavaScript: function(src,param) { this._js.push( Asset.javascript(this.dir + src,param) ); },
        loadCSS: function(src,param) { this._css.push( Asset.css(this.dir + src,param) ); },
        //unloads all resources
        cleanUp: function() { 
            this._js.forEach(function(item) { item.destroy();}); 
            this._js.empty();
            this._css.forEach(function(item) { item.destroy();}); 
            this._css.empty(); 
            },
        tray: function() { this.fireEvent("onTray"); },
        untray: function() { this.fireEvent("onUnTray"); },
        // if there are no windows go tray
        newWindow: function (arg) {            
            this._windowsCount++;
            arg.onCloseComplete = (function() { this._windowsCount--; if(this._windowsCount == 0) this.tray(); }).bind(this);
            var w = new UI.Window(arg);
            this.windows.push(w);
            return w;
        }
        });
        
an.Manager = (function(){
            this.apps = [];
            this.currentApp = null;
            this.dock = null;
            this.menubar = $('desktopNavbar');
            this.context = $('startpageWrapper');
           


                    
            function resetDesktop() {
                $('app_menubars').append(this.currentApp.menubar);
                $('an_nav').append(this.menubar);
                //MochaUI.Desktop.desktopNavBar = this.menubar.get('id');
            }

        function setActiveApp(app) {
                $('app_menubars').append(this.menubar);
                $('an_nav').append(app.menubar);
                //MochaUI.Desktop.desktopNavBar = app.menubar.get('id');
            }
        function refreshMenu() {
        $('appdock_c').empty();
            if(this.dock !== null)
                this.dock.el.destroy();
                
            var buffer ="<ul id='appdock_list'>";
            for (var i = 0; i < this.apps.length;++i)
             buffer += "<li class='appdock'><a href='#' onclick='an.Manager.runApp("+i+");'><img src='apps/active/"+
             this.apps[i].name+ "/logo.png' alt='"+this.apps[i].name+"'/></a></li>\n";  
    
            buffer+="</ul>";
            $("appdock_c").set('html',buffer);
         
            this.dock = new UvumiDock("appdock_list",{position: "top"});
        }
        
        // public functions
    return {
        runApp: function(id) {
              if(!apps[id].isWindowed) {
                if(this.currentApp === null)
                    $j('#app_contexts').append($j('#startpageWrapper'));
                else {
                    this.currentApp.isRunning = false;
                    $j('#app_contexts').append(this.currentApp.contextW);
                }
                this.currentApp = this.apps[id];
                this.currentApp.isRunning = true;
                jQuery('#an_content').append(this.currentApp.contextW);
                MochaUI.Desktop.pageWrapper = $(this.currentApp.contextWName);
                MochaUI.Desktop.page = $(this.currentApp.contextName);
                }
                
                currentApp = apps[id];
                setActiveApp(currentApp);
                
                if(!currentApp.isRunning) {
                    currentApp.fireEvent("onSetup");
                    currentApp.isRunning = true;
                }
                
                currentApp.fireEvent("onRun");
        },
                    // "callback" the apps register themselfe
        registerApp: function(app) {
                apps.push(app);
                refreshMenu();
            },
            
        
        addApp: function (name) {
                $('app_contexts').append("<div id='appcW_"+name+"'></div>");
                $('app_menubars').append("<div id='mb_"+name+"'><ul><li><a class='returnFalse' href=''>Help"+
                                        "</a><ul><li class='divider'><a href='' id='"+name+"_manual'>Manual</a></li>"+
                                                "<li class='divider'><a href='' id='"+name+"_about'>About</a></li>"+
                                        "</ul></ul></li></ul></div>");

                $('appcW_'+name).append("<div id='appc_"+name+"'></div>");

                Asset.javascript("apps/active/"+name+"/"+name+".js");
            },
        getApp: function(name) {
            for(var i=0;i<apps.length;++i)
                if(apps[i].name == name)
                    return apps[i];
            return false;
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

an.windows = (function () {
    return {
        help: function() {
            new UI.Window({
                    id: 'help',
                    title: 'Support',
                    loadMethod: 'ajax',
                    contentURL: 'pages/support.html',
                    width: 320,
                    height: 320,
                    x: 200,
                    y: 90 
                });
        },
    status: function() {
            new UI.Window({
                    id: 'status',
                    title: 'Satus',
                    loadMethod: 'ajax',
                    contentURL: 'pages/status.html',
                    width: 320,
                    height: 320,
                    x: 200,
                    y: 90 
                });
        },
    about: function(){
            new UI.Window({
                id: 'about',
                title: 'About',
                loadMethod: 'ajax',
                contentURL: 'pages/about.html',
                type: 'modal2',
                width: 350,
                height: 195,
                contentBgColor: '#e5e5e5 url(images/logo2.gif) left 3px no-repeat',
                padding: { top: 43, right: 12, bottom: 10, left: 12 },
                scrollbars:  false
            });
        }
    };
})();
window.addEvent('domready', function() {

	var toolbar = new UI.Toolbar({
        zIndex: 1,
        openOnRollover: true,
        closeOnRollout: true,
        menu: [{
            text : 'About',
            options : { 'class' : 'ui-dd-floor' },
            menu : [{
                text : 'Status',
                action : an.windows.status
                    },
                    {
                text : 'Help',
                action : an.windows.help
                    },
                    {
                text : 'About',
                action : an.windows.about
                    }
            ]
        }
    ]
}).inject($('an_nav'));
    
    
    new Request.JSON({url: "list_apps", onSuccess: function(res) {
            res.each(an.Manager.addApp,an.Manager);
            }}).get();
    
  /*  $('an_helpLink').addEvent('click', function(e){
			new Event(e).stop();
			an.windows.help();
		});
    $('an_aboutLink').addEvent('click', function(e){	
			new Event(e).stop();
			an.windows.about();
		});
    $('an_statusLink').addEvent('click', function(e){	
			new Event(e).stop();
			an.windows.status();
	});*/
        
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

function _unimplemented(name) { alert("unimplemented method: "+name); }

// mocha UI stuff

	// Deactivate menu header links
	$$('a.returnFalse').each(function(el){
		el.addEvent('click', function(e){
			new Event(e).stop();
		});
	});
    
window.addEvent('unload', function(){
	if (MochaUI) MochaUI.garbageCleanUp();
});