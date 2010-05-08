
var ChatApp = new Class({
            Extends: an.App,
            initialize: function() {
                this.parent("chat");
                this.currentRoom = null;
                this.rooms = [];
                this.messages = new Hash(); // roomname: array(messages)
                this.username = "foobar";
                this.roomList;
            this.addEvent("onRun", function() {
            
              var win =  this.newWindow({
                    title		: 'Chat',
                    overflow	: 'scrollbar',
                    contentURL:  this.dir+'mainwindow.html'
                    });
                    
                win.buildToolbar({
                    skin: "abc",
                    openOnRollover: true,
                    closeOnRollout: true,
                    menu: [{
                        text : 'Search Channel',
                        options : { 'class' : 'ui-dd-floor' },
                        },
                        {text : 'Create Channel',
                        action : function() {  
                                    this.newDialog('create_channel.html',{title: "create Channel"});             
                                        }.bind(this)
                        }
                        ]
                });
            });
        },
        addRoom: function(name) {
            this.rooms.push(name);
            this.messages.set(name,[]);
        },
        addMessage: function(msg) {
            $('chat_messages').append("<li><strong>"+msg.name+":</strong> "+msg.message+"<div style='float:right;'>" +
                                    msg.time + "</div></li>\n");
        },
        sendMessage: function(msg) {
            var message = {name: this.username, message: msg,time: $time()};
            this.addMessage(message);
            this.messages.get(this.currentRoom).push(message);
            //send to dht...
        },
        buildRoomList: function() {
            var items = [];
            for (var n in this.rooms) {
                var item = {title: n, desc: this.messages.get(n).length + " Messages"};
                items.push(item);
            }
            var graphite = new UI.ListView({
				//skin : 'BlueTouch',
				width : 260,
				height : 400,
				resizable:true,
				styles : {
					backgroundColor : '#e8e8e8'
				}, items: items
			}).inject($('left'));
        }
    });
        
an.Manager.registerApp(new ChatApp());
