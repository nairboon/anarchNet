/*
 * AppController.j
 * NewApplication
 *
 * Created by You on April 9, 2010.
 * Copyright 2010, Your Company All rights reserved.
 */

@import <Foundation/CPObject.j>
//@import "ChatViewController.j"


@implementation AppController : CPObject
{
		@outlet CPWindow					theWindow;
		@outlet CPTextField	   	  inputbox;
    @outlet CPScrollView		  messages;
    @outlet CPTableView	      rooms;
}

- (void)awakeFromCib
{
}

- (@action)sendMessage:(id)sender
{
	alert("bal" + [inputbox stringValue]);
}

-(@action)switchRoom:(id)sender
{
	alert("switch");
}
-(@action)ups:(id)s
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
    // Uncomment the following line to turn on the standard menu bar.
    [CPMenu setMenuBarVisible:YES];
}

@end
