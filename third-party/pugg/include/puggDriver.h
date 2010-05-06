/**
* @file
* @author  Tunc Bahcecioglu <http://www.tuncbahcecioglu.com>
* @version 1.0
*
* @section LICENSE
*
* GNU LESSER GENERAL PUBLIC LICENSE
* Version 3, 29 June 2007
*
* <http://www.gnu.org/copyleft/lesser.html>
*/

#ifndef FILE_puggDriver_h__
#define FILE_puggDriver_h__

#include <string>

namespace pugg
{
	/**
			Base class for all drivers.
		This driver has only one virtual function called getName.
		Host program must inherit this class and a virtual function that creates the related class
		For an example see ...

	 */
	class Driver
	{
	public:
		/**
			Returns the driver's name.
		 As pugg holds drivers in std::map structure this string functions as a key.
		 This key helps the host program distinguish between diffrent drivers.
		 */
		virtual std::string getName() = 0;
		
		virtual ~Driver() { };
	};
}

#endif // FILE_puggDriver_h__