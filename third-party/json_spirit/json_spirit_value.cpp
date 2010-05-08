/* Copyright (c) 2007 John W Wilkinson

   This source code can be used for any purpose as long as
   this comment is retained. */

// json spirit version 2.00

#include <iostream>
#include <boost/bind.hpp>
#include "json_spirit.h"
#include "json_spirit_value.h"

namespace json_spirit
{
	bool same_name( const Pair& pair, const std::string& name )
{
	return pair.name_ == name;
}

	const Value& find_value( const Object& obj, const std::string& name )
{
	Object::const_iterator i = find_if( obj.begin(), obj.end(), bind( same_name, _1, boost::ref( name ) ) );
	
	if( i == obj.end() ) return Value::null;
	
	return i->value_;
}

	int find_int( const Object& obj, const std::string& name )
{
	return find_value( obj, name ).get_int();
}

	std::string find_str( const Object& obj, const std::string& name )
{
	return find_value( obj, name ).get_str();
}
}