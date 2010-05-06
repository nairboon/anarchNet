
var ChatApp = new Class({
            Extends: an.App,
            initialize: function() {
                this.parent("chat");
                this.currentRoom = null;
                this.rooms = [];
                this.messages = [];
                this.username = "foobar";
            this.addEvent("onRun", function() {
            
              var win =  this.newWindow({
                    title		: 'Chat',
                    overflow	: 'scrollbar',
                    contentURL:  this.dir+'mainwindow.html'
                    });
            });
        },
        addRoom: function(name) {
            this.rooms.push(name);
            this.messages.push([]);
        },
        addMessage: function(msg) {
            $('chat_messages').append("<li><strong>"+msg.name+":</strong> "+msg.message+"<div style='float:right;'>" +
                                    msg.time + "</div></li>\n");
        },
        sendMessage: function(msg) {
            var message = {name: this.username, message: msg,time: $time()};
            this.addMessage(message);
            this.messages[this.currentRoom].push(message);
            //send to dht...
        }

    });
        
an.Manager.registerApp(new ChatApp());
