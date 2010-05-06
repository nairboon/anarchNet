
 /*
	Class: UI.Menu
		Creates a new menu, manages submenus and positionning as well as scrolling thru <UI.Menu.Scroller>
		
	Extends:
		<UI.Element>
	
	Arguments:
		options
			
	Options: 
		- zIndex - (integer) Base z-index for menu element (submenu's z-index will be incremented)
		- contentTag - (string) Tag name for menu elements wrapper
		- itemTag - (string) Tag name for menu elements
		
		- position - (string) Specify where the new menu must be positionned.
			It could be normal (element will be positionned on parent's side),
			over (element will be positionned over the parent element, used for <UI.Select>),
			bottom (element will be positionned on bottom of parent element, used for <UI.Toolbar>)
		
		- scrollToSelected - (boolean) Determine if a menu (specifically a <UI.Select>) should remember last item selected
		- scrollMargin - (integer) Determine remaining margin on top and bottom when a menu is too large to feet in window
		- menu - (array) Array containing menu definition
		
	Example:
		(start code)
		var submenu = new UI.Menu({
			container : this.view.element,
			underlay : this.options.underlay,
			zIndex : 1
		});
		(end)
	
	Implied global: 
		- MooLego - UI,ui,
		- MooTools - $clear, $defined, $empty, $time, Class, Element, Event, Fx, Window
		- Javascript - document
		
	Members:
		Element, Extends, Label, Menu, MenuScroller, Tween, action, 
	    activeItem, addEvent, addEvents, addScrolls, addSubmenu, 
	    addSubmenuArrow, addSubmenuEvents, arrow, bind, body, bottom, build, 
	    buildMenu, canvas, closeMenu, closeOnRollout, component, components, 
	    content, contentTag, controller, currentIndex, default, defaultArrow, 
	    delay, destroy, display, duration, each, element, empty, fireEvent, 
	    getComponentProps, getCoordinates, getElements, getHeight, getScroll, 
	    getSize, getStyle, getWidth, goDown, goUp, height, hide, hideFxDuration, 
	    html, image, inject, itemTag, items, layers, left, listStyle, margin, 
	    menu, menuActionDelay, menuWithAction, mouseUpAction, mousedown, 
	    mouseenter, mouseleave, mouseup, moveRollover, onComplete, onResize, 
	    onScroll, openOnRollover, options, padding, parent, position, props, 
	    push, radius, removeEvents, removeRollover, removeScrolls, 
	    removeSubmenu, right, rollover, rolloverType, scrollMargin, 
	    scrollToSelected, scrolls, selected, separator, set, attach, 
	    setCanvas, setCorners, setPosition, setRollover, setSize, setState, 
	    setStyle, setStyles, show, showDelay, skin, start, stop, styles, 
	    submenu, tag, target, text, time, toInt, top, type, width, wrapper, x, 
	    y, zIndex

	Discussion
		Should use listView
	
*/


UI.Menu = new Class({
	
	Extends: UI.Element,
	
	options: {
		component: 'menu',
		rolloverType: 'menuRollover',
		
		zIndex: 3000,
		contentTag: 'div',
		itemTag: 'div',
		
		position: 'normal',
		scrollToSelected: false,
		scrollMargin: 20,
		menu: []
	},

	/*
	Function: build
		private function
		
		Call UI.Element build, then create a menu wrapper
	
	Return:
		(void)
	
	See also:
		<UI.Element::build>
	*/
	
	build: function(menu) {
		this.parent();
		this.content = new Element(this.options.contentTag,{
			styles: {
				zIndex: 2,
				position: 'relative',
				padding: this.props.components.wrapper.styles.padding,
				margin: 0,
				listStyle: 'none'
			}
		}).inject(this.element);
		
		this.buildMenu();
		
		this.element.setStyles({
			zIndex	: this.options.zIndex
		});
	},
	
	
	
	
	/* 
	Method: buildMenu
		Build the content of the menu or change menu content
	
	Arguments:
		menu - (array) Array containing menu definition
	
	Return:
		this
	 */
	
	buildMenu : function(menu) {
		this.empty();
		var list = (menu) ? menu : this.options.menu;
		var menuItem;
		this.items = [];
		this.currentIndex = 0;
		
		list.each(function(item){
			if (item.text == 'separator') {
				menuItem = new UI.Label({
					skin: this.options.skin,
					tag: this.options.itemTag,
					html: '',
					styles: this.props.components.separator.styles
				}).inject(this.content);
				menuItem.separator = true;
			} else {
				menuItem = new UI.Label({
					skin: this.options.skin,
					tag: this.options.itemTag,
					html: item.text,
					props: UI.skin.getComponentProps(this.skin, 'menuItem'),
					image: item.image
				}).set(item.options);
				
				this.items.push(menuItem);
				
				if (item.action) {
					menuItem.element.addEvent('action', item.action);
				}
				menuItem.inject(this.content);
			}
			this.addSubmenuEvents(item, menuItem);
		},this);
		return this;
	},


	/*
	Function: attach
		private function
		
		Call UI.Element build, then create a menu wrapper
	
	Return:
		(void)
	
	See also:
		<UI.Element::build>
	*/

	attach : function(){
		this.parent();
		if (!this.options.closeMenu) {
			this.addEvent('onCloseMenu', function(e){
				ui.controller.element.closeMenu = $empty;
				this.hide(300);
			}.bind(this));
		} else { 
			this.addEvent('onCloseMenu', function(){
				this.options.closeMenu();
			}.bind(this));
		}
	},
		
	/* 
	Method: addSubmenuEvents
		private function
	
		Attach actions and / or submenu to menu elements
	
	Arguments:
		item - (object) Object containing element properties
		menuItem - (element) Menu item where events will be attached
	
	Return:
		(void)
	 */
	
	addSubmenuEvents: function(item, menuItem){
		
		var self = this;
		
		if(item.menu) {
			menuItem.addEvents({
				'mouseenter': function(){
					if (self.activeItem && self.activeItem.submenu && self.activeItem != menuItem) {
						self.activeItem.submenu.hide();
					}
					if (self.activeItem != menuItem) {
						self.addSubmenu(item, menuItem, 'normal');
					}
					self.moveRollover(menuItem);
				}
			});
			self.addSubmenuArrow(menuItem);
		} else {
			menuItem.addEvents({
				'mouseenter': function(){
					self.removeSubmenu();
					if (menuItem.separator) {
						self.removeRollover();
					}
					else {
						self.moveRollover(menuItem);
					}
				}
			});
		}
		
		menuItem.element.addEvents({
			'mouseleave': function(){
				$clear(self.menuActionDelay);
			},
			'mouseup': function(){
				self.mouseUpAction(menuItem);
			},
			'mousedown': function(e){
				var ev = new Event(e).stop();
				if (!menuItem.separator) {
					self.fireEvent('change');
				}
			}
		});
	},
	
	/* 
	Method: addSubmenu
		private function
		
		Attach a submenu to a menu item if needed
	
	Arguments:
		item - (object) Object containing element properties
		menuItem - (element) Menu item where submenu will be attached
	
	Return:
		(void)
	 */
	
	addSubmenu: function(item, menuItem, position){
		this.menuWithAction = false;
		$clear(this.menuActionDelay);
		this.menuActionDelay = (function(){
			if (!menuItem.submenu) {
				menuItem.submenu = new UI.Menu({
					skin: this.options.skin,
					target: menuItem,
					menu: item.menu,
					closeMenu: this.fireEvent.bind(this, 'closeMenu'),
					openOnRollover: this.options.openOnRollover,
					closeOnRollout: this.options.closeOnRollout,
					position: position,
					zIndex: this.options.component == 'toolbar' ? --this.options.zIndex : ++this.options.zIndex
				}).inject(document.body);
				ui.controller.element.closeMenu = this.fireEvent.bind(this, 'closeMenu');
			} else {
				menuItem.submenu.show(menuItem);
			}
		}.bind(this)).delay(this.props.showDelay);
	},
	
	/* 
	Method: addSubmenuArrow
		private function
		
		Add an arrow on the right side of the element
	
	Arguments:
		menuItem - (element) Menu item where arrow will be attached
	
	Discussion:
		should be drawn on the menuItem, probably
	
	Return:
		(void)
	 */
	
	
	addSubmenuArrow: function(menuItem){
		this.addEvent('addArrows', function(){
			// add arrow
			menuItem.arrow = new UI.Element({
				skin: this.options.skin,
				component: 'element',
				type: 'menuRightArrow',
				styles: {
					height:'10',
					width:'20',
					'padding': 0,
					'position': 'absolute',
					right: 8,
					display: 'block'
				}
			}).inject(menuItem, 'top');
			menuItem.arrow.setStyle('top', (menuItem.element.getHeight() - menuItem.arrow.element.getHeight()) / 2);
			menuItem.element.addEvents({
				'mouseenter': function(){
					menuItem.arrow.setState('over');
				},
				'defaultArrow': function(){
					menuItem.arrow.setState('default');
				}
			});
		});
	},
	
	/* 
	Method: mouseUpAction
		private function
		
		Execute the menu item action and close the menu (as well as submenu if needed)
	
	Arguments:
		menuItem - (element) Menu item with attached action to fire
	
	Return:
		(void)
	 */
	
	mouseUpAction : function(menuItem){
		if ($time() - this.time > 300 && this.rollover) {

			var fx = new Fx.Tween(this.rollover.element, {
				duration: 100,
				onComplete: function(){
					if (this.selected) {
						this.selected.selected = false;
					}
					this.selected = menuItem.element;
					menuItem.element.selected = true;
					this.fireEvent('closeMenu');
					menuItem.element.fireEvent('action');
				}.bind(this)
			}).start('opacity', 0, 1);
		}
	},
	
	/* 
	Method: setRollover
		private function
		
		Create a new rollover element in menu if it doesn't exist
	
	Return:
		(void)
	 */
	
	setRollover: function(){
		if (this.rollover) {
			return;
		}
		this.rollover = new UI.Element({
			skin: this.options.skin,
			type: this.options.rolloverType,
			styles: {
				position: 'absolute',
				zIndex: 1
			}
		}).inject(this.element);
	},
	
	/* 
	Method: moveRollover
		private function
		
		Move the rollover to a new location (menu item)
	
	Arguments:
		menuItem - (element) Rollover will be moved to this menu item position
	
	Return:
		(void)
	 */
	
	moveRollover: function(menuItem){
		var coord = menuItem.getCoordinates(this.element);
		 this.setRollover();
		
		if (this.activeItem) {
			this.activeItem.element.fireEvent('defaultArrow');
			this.activeItem.setState('default');
		}
		
		this.rollover
		.setSize(coord.width, coord.height)
		.setStyles({
			display: 'block',
			top: coord.top,
			left: coord.left
		});
		menuItem.setState('over');

		this.activeItem = menuItem;
	},
	
	/* 
	Method: removeRollover
		private function
	
		Remove the rollover from menu and destroy it
	
	Return:
		(void)
	 */
	
	removeRollover: function(){
		if (this.rollover) {
			this.rollover.destroy();
			delete this.rollover;
			
			this.activeItem.element.fireEvent('defaultArrow');
			this.activeItem.setState('default');
		}
		
		this.activeItem = false;
	},
	
	/* 
	Method: removeSubmenu
		private function
	
		Remove the current submenu as well as submenus if needed
	
	Return:
		(void)
	 */
	
	removeSubmenu: function(){
		if(this.activeItem && this.activeItem.submenu) {
			this.activeItem.element.fireEvent('defaultArrow');
			this.activeItem.submenu.hide(this.props.hideFxDuration);
		}
	},
	
	/* 
	Method: setPosition
		private function
		
		Set the menu position relatively to parent element. Parent could be a menu element or any dom element
	
	Arguments:
		el - (element) Parent element who will define menu position
	
	Return:
		(void)
	 */
	
	setPosition: function(el){
		var elCoordinates 	= el.getCoordinates();
		var menuCoordinates = this.element.getCoordinates();
		this.element.setStyle('height', menuCoordinates.height);
		
		var left,
			top;
		
		if (this.options.position == 'bottom') {
			this.setCorners([0,0,4,4]);
			this.element.setStyles({
				left : elCoordinates.left,
				top : elCoordinates.bottom
			});
			menuCoordinates = this.element.getCoordinates();
			if(menuCoordinates.bottom + this.options.scrollMargin > (Window.getHeight()) || menuCoordinates.top < this.options.scrollMargin) {
				this.addScrolls();
			}
		
		} else if (this.options.position == 'over') {
			this.setCorners([4,4,4,4]);
			var selected = false;
			if (this.options.scrollToSelected) {
				//we set the position to selected element
				this.content.getElements(this.options.itemTag).each(function(menuItem){
					if (menuItem.selected) {
						selected = menuItem;
					}
				});
			}
			top = (!selected) ? 
				elCoordinates.top - this.content.getStyle('paddingTop').toInt() :
				elCoordinates.top - selected.getCoordinates(this.element).top;
			
			this.element.setStyles({
				'top': top + 1,
				'left': elCoordinates.left - 1
			});
			var windowScroll = Window.getScroll();
			menuCoordinates = this.element.getCoordinates();
			menuCoordinates.top -= windowScroll.y;
			menuCoordinates.bottom -= windowScroll.y;
			
			if(menuCoordinates.bottom + this.options.scrollMargin > (Window.getHeight()) || menuCoordinates.top < this.options.scrollMargin) {
				this.addScrolls();
			}

		//default location
		} else {
			var corners = [4,4,4,4];
			
			//determine if menu position is left or right
			if (menuCoordinates.width > (Window.getWidth() - elCoordinates.right)) {
				// menu on left
				left = elCoordinates.left - menuCoordinates.width+2;
				corners[1] = 0;
			} else {
				// menu on right
				left = elCoordinates.right-2;
				corners[0] = 0;
			}
			if (menuCoordinates.height < (Window.getHeight() - elCoordinates.top + Window.getScroll().y)) {
				// menu is under
				top = elCoordinates.top - this.content.getStyle('paddingTop').toInt();
			} else if(menuCoordinates.height < (elCoordinates.top - Window.getScroll().y)) {
				// menu is over
				top = elCoordinates.bottom - menuCoordinates.height + this.content.getStyle('paddingTop').toInt();
				corners = [4,4,corners[1],corners[0]];
			} else {
				// menu is on side
				corners = [4,4,4,4];
				top = elCoordinates.top - this.content.getStyle('paddingTop').toInt();
				this.element.setStyles({
					'top': top,
					'left': left
				});
				this.addScrolls();
			}
			this.setCorners(corners);
			this.element.setStyles({
				'top': top,
				'left': left
			});
		}
	},

	/*
    Method: setCorners
    	private function

		Set corners radius for canvas draw
	
	Return:
		(void)
	  
	Discussion:
		is really needed anymore?
	*/
	
	setCorners: function(corners){
		this.props.layers['default'].radius = corners;
	},
	
	/* 
	Method: addScrolls
		private function
	
		Add scrolls to menu
	
	Return:
		(void)
	*/
	
	addScrolls: function(){
		this.scrolls = new UI.MenuScroller({
			skin: this.options.skin,
			element: this.element,
			content: this.content,
			margin: this.options.scrollMargin,
			props: this.props,
			onScroll: function(){
				this.removeSubmenu();
				this.removeRollover();
			}.bind(this)			,
			onResize: function(){
				var size = this.element.getSize();
				this.setSize(size.x, size.y);
			}.bind(this)
		});
		
		this.addEvent('removeScrolls', function(){
			this.scrolls.removeScrolls();
			this.removeEvents('removeScrolls');
		}.bind(this));
	},
	
	goDown : function(){
		this.moveRollover(this.items[++this.currentIndex]);
	},
	
	goUp : function(){
		this.moveRollover(this.items[--this.currentIndex]);
	},
	
	/* 
	Method: inject
		inject the menu and draw the canvas. Overwrite the inject method of <UI.Element>
	
	Arguments:
		element - (element) Injection target
		target - (string) Determine where to inject.
	
	Return:
		this
	 */
	
	inject: function(element, target){
		this.time = $time();
		this.fireEvent('inject');
		
		this.element.inject(element, target);
		this.setSize();
		
		if (this.options.position != 'over') {
			if (this.options.target) {
				this.setPosition(this.options.target);
			}
			else {
				this.setPosition(element);
			}
			this.setCanvas();
			this.setStyle('visibility', 'visible');
		} else {
			this.setCanvas();
		}
		
		this.fireEvent('addArrows');
		
		if (this.options.closeOnRollout) {
			this.paint.canvas.addEvent('mouseleave', function(){
				this.fireEvent('closeMenu');
			}.bind(this));
		}
		
		return this;
	},
	
	/* 
	Method: show
		Show the menu
	
	Arguments:
		parent - (element) Menu location will be determine relatively to this element
		x - (integer) (optional) new menu width
		y - (integer) (optional) new menu height
	
	Return:
		this
	
	See also:
		<UI.Element::show>
	 */
	
	show : function(parent, x, y) {
		ui.controller.element.closeMenu = this.fireEvent.bind(this, 'closeMenu');
		ui.controller.element.menu = this;
		this.time = $time();
		this.element.setStyle('display', 'block');
		this.setPosition(parent);
		this.setSize(x, y);
		this.parent();
		return this;
	},
	
	/* 
	Method: hide
		Hide the submenu, and clean it (remove rollover, remove scrolls, ...)
	
	Arguments:
		duration - (integer) Fade out duration, in milliseconds
	
	Return:
		this		
	 */
	
	hide: function(duration){
		if (!$defined(duration)) {
			duration = this.props.hideFxDuration;
		}
		
		ui.controller.element.menu = false;
		this.fireEvent('hide');
		this.removeSubmenu();
			
		if (!duration){
			this.setStyle('display', 'none');
			this.removeRollover();
			this.fireEvent('removeScrolls');
		} else {
			var fx = new Fx.Tween(this.element, {
				duration: duration,
				onComplete: function(){
					this.setStyle('display', 'none');
					this.removeRollover();
					this.fireEvent('removeScrolls');
				}.bind(this)
			}).start('opacity', 0);
		}
		
		return this;
	},
	
	/* 
	Method: empty
		Clear menu content
	
	Return:
		this
	 */
	
	empty: function(){
		this.content.empty();
		return this;
	}
	
});