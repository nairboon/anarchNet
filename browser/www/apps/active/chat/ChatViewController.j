@import <Foundation/CPObject.j>

@implementation ChatViewController : CPObject
{
    @outlet CPView						contentView;
		@outlet CPTextField	   	  inputbox;
    @outlet CPScrollView		  messages;
    @outlet CPTableView	      rooms;
}

- (void)awakeFromCib
{
alert("HU");
}

/*- (@action)sendMessage:(id)sender
{
	alert("bal" + [inputbox stringValue]);
}
*/
/*- (@action)switchRoom:(id)sender
{
	alert("bal" + [inputbox stringValue]);
}*/

@end
