/*
 * AppController.j
 * NewApplication
 *
 * Created by You on April 9, 2010.
 * Copyright 2010, Your Company All rights reserved.
 */

@import <Foundation/CPObject.j>
//@import "ChatViewController.j"

SearchRoomToolbarItemIdentifier = "Search Room Item Identifier";
JoinRoomToolbarItemIdentifier = "Join Room Item Identifier";

@implementation AppController : CPObject
{
		@outlet CPWindow					theWindow;
		@outlet CPTextField	   	  inputbox;
    @outlet CPScrollView		  messages;
    @outlet CPTableView	      rooms;
		@outlet CPWindow					roomListWindow;
		@outlet CPTextField	   	  roomToJoin;
		@outlet CPTableView	      roomList;
		@outlet CPSearchField			roomSearch;
}

- (void)awakeFromCib
{
  // Create a new toolbar instance, and attach it to our document window 
    var toolbar = [[[CPToolbar alloc] initWithIdentifier: @"MainToolbar"] autorelease];
    
   // [toolbar setDisplayMode: CPToolbarDisplayModeIconOnly];
    
    // We are the delegate
    [toolbar setDelegate: self];
    
    // Attach the toolbar to the document window 
    [theWindow setToolbar: toolbar];

}
- (CPToolbarItem) toolbar: (CPToolbar)toolbar itemForItemIdentifier: (CPString) itemIdent willBeInsertedIntoToolbar:(BOOL) willBeInserted {
    // Required delegate method:  Given an item identifier, this method returns an item 
    // The toolbar will use this method to obtain toolbar items that can be displayed in the customization sheet, or in the toolbar itself 
    var toolbarItem = nil;
    
    if ([itemIdent isEqual: SearchRoomToolbarItemIdentifier]) {
        toolbarItem = [[[CPToolbarItem alloc] initWithItemIdentifier: itemIdent] autorelease];
	[toolbarItem setLabel: @"Search"];
	[toolbarItem setImage: [[CPImage alloc] initWithContentsOfFile: @"Resources/SearchTabMenu.png" size: CPSizeMake(30,30)]];
	[toolbarItem setTarget: self];
	[toolbarItem setAction: @selector(searchRooms)];
	[toolbarItem setToolTip: @"Search rooms "];
	}
	else if([itemIdent isEqual: JoinRoomToolbarItemIdentifier]) {
			toolbarItem = [[[CPToolbarItem alloc] initWithItemIdentifier: itemIdent] autorelease];
			[toolbarItem setLabel: @"Join"];
			[toolbarItem setImage: [[CPImage alloc] initWithContentsOfFile: @"Resources/JoinTabMenu.png" size: CPSizeMake(30,30)]];
			[toolbarItem setTarget: self];
			[toolbarItem setAction: @selector(test:)];
	}
	 else {
	// itemIdent refered to a toolbar item that is not provide or supported by us or cocoa 
	// Returning nil will inform the toolbar this kind of item is not supported 
	toolbarItem = nil;
    }
    return toolbarItem;

}
- (CPArray) toolbarDefaultItemIdentifiers: (CPToolbar) toolbar {
    // Required delegate method:  Returns the ordered list of items to be shown in the toolbar by default    
    // If during the toolbar's initialization, no overriding values are found in the user defaults, or if the
    // user chooses to revert to the default items this set will be used 
    return [CPArray arrayWithObjects: 	SearchRoomToolbarItemIdentifier, JoinRoomToolbarItemIdentifier,
					CPToolbarFlexibleSpaceItemIdentifier, CPToolbarSpaceItemIdentifier, CPToolbarSeparatorItemIdentifier, nil];
}
- (CPArray) toolbarAllowedItemIdentifiers: (CPToolbar) toolbar {
    // Required delegate method:  Returns the list of all allowed items by identifier.  By default, the toolbar 
    // does not assume any items are allowed, even the separator.  So, every allowed item must be explicitly listed   
    // The set of allowed items is used to construct the customization palette 
    return [CPArray arrayWithObjects: 	SearchRoomToolbarItemIdentifier,JoinRoomToolbarItemIdentifier,
					CPToolbarFlexibleSpaceItemIdentifier, CPToolbarSpaceItemIdentifier, CPToolbarSeparatorItemIdentifier, nil];
}
- (void) searchRooms
{
		[roomListWindow orderFront:self];
		[roomListWindow center];
}
- (@action)sendMessage:(id)sender
{
	alert("bal" + [inputbox stringValue]);
}

-(@action)switchRoom:(id)sender
{
	alert("switch");
}

-(@action)joinRoom:(id)sender
{
	
}

- (void)applicationDidFinishLaunching:(CPNotification)aNotification
{
    [theWindow setFullBridge:YES];

/*var cib = [[CPCib alloc] initWithContentsOfURL: [[CPBundle mainBundle] pathForResource:@"MainMenu.cib"]];
[cib instantiateCibWithExternalNameTable:[CPDictionary dictionaryWithObject:self forKey:CPCibOwner]];
[window orderFront:self];*/

  /*  var theWindow = [[CPWindow alloc] initWithContentRect:CGRectMakeZero() styleMask:CPBorderlessBridgeWindowMask],
        contentView = [theWindow contentView];

    var label = [[CPTextField alloc] initWithFrame:CGRectMakeZero()];

    [label setStringValue:@"Hello World!"];
    [label setFont:[CPFont boldSystemFontOfSize:24.0]];

    [label sizeToFit];

    [label setAutoresizingMask:CPViewMinXMargin | CPViewMaxXMargin | CPViewMinYMargin | CPViewMaxYMargin];
   // [label setCenter:[contentView center]];

    //[contentView addSubview:label];

    //[theWindow orderFront:self];

    var theWindow2 = [[CPWindow alloc] initWithContentRect:CGRectMake(100,50,200,300) styleMask:CPTitledWindowMask | CPClosableWindowMask | CPMiniaturizableWindowMask | CPResizableWindowMask];
    [[theWindow2 contentView] addSubview:label];
        //contentView = [theWindow contentView];
        
         [label setCenter:[[theWindow2 contentView] center]];
         
        [theWindow2 setShowsResizeIndicator:YES] ;
[theWindow2 setTitle:@"My Window Title"] ;
            [theWindow2 orderFront:self];

        */
}

@end
