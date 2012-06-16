/*
 * Copyright (C) 2010, 2011 Port4l
 * https://launchpad.net/anarchNet
 *
 * This file is part of anarchNet.
 *
 * anarchNet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * anarchNet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with anarchNet.  If not, see <http://www.gnu.org/licenses/>.
 */

// Thread-safe lazy singleton template class - http://www.boostcookbook.com/Recipe:/1235044
// written by Port4l, public domain?
#ifndef SRC_DAEMON_SINGLETON_H_
#define SRC_DAEMON_SINGLETON_H_

#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

// Warning: If T's constructor throws, instance() will return a null reference.

namespace an
{
	
 template<class T>	
 class Singleton : private boost::noncopyable {
  public:
    static T& instance()
    {
			boost::call_once(init, flag);
			return *t;
    }
		
    static void init() // never throws
    {
			t.reset(new T());
    }
    
    static void kill()
    {
      t.reset();
    }
  protected:
    ~Singleton() {}
		Singleton() {}
  private:
		static boost::scoped_ptr<T> t;
		static boost::once_flag flag;
	};
}

template<class T> boost::scoped_ptr<T> an::Singleton<T>::t(0);
template<class T> boost::once_flag an::Singleton<T>::flag = BOOST_ONCE_INIT;

#endif  // SRC_DAEMON_SINGLETON_H_
