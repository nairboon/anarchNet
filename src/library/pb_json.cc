/*
 * Copyright (C) 2010, 2011 Remo Hertig (nairboon)
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

//#include "pb_json.h"

//using namespace google::protobuf;
using namespace std;

namespace an
{ /*
std::string JSONInterface::write(const google::protobuf::Message& m)
{
	stringstream out;
	out << "{";
	
	for(int i=0;i<m.GetDescriptor()->field_count();++i) {
		printField(m.GetDescriptor()->field(i),m,out);
		if( (i+1) < m.GetDescriptor()->field_count())
			out << ",";
	}

	out << "}";
	return out.str();
}


void JSONInterface::printField(const FieldDescriptor *fd,
																const google::protobuf::Message&m,stringstream& out)
{
	out << "\"" << fd->name() << "\": ";

	if(fd->is_repeated()) {
		out << "[";
		for(int i=0;i<m.GetReflection()->FieldSize(m,fd);++i) {
			printRepeatedValue(fd,m,out,i);
			if( (i+1) < m.GetReflection()->FieldSize(m,fd))
				out << ",";
		}
		out << "]";
	}
	else {
		printValue(fd,m,out);
	}

}
	
	void JSONInterface::printValue(const google::protobuf::FieldDescriptor *fd,
																	const google::protobuf::Message&m,std::stringstream& out)
	{
		switch(fd->cpp_type()) {
			case FieldDescriptor::CPPTYPE_INT32:
				out << m.GetReflection()->GetInt32(m,fd);
				break;
			case FieldDescriptor::CPPTYPE_INT64:
				out << m.GetReflection()->GetInt64(m,fd);
				break;
			case FieldDescriptor::CPPTYPE_UINT32:
				out << m.GetReflection()->GetUInt32(m,fd);
				break;
			case FieldDescriptor::CPPTYPE_UINT64:
				out << m.GetReflection()->GetUInt64(m,fd);
				break;
			case FieldDescriptor::CPPTYPE_FLOAT:
				out << m.GetReflection()->GetFloat(m,fd);
				break;
			case FieldDescriptor::CPPTYPE_DOUBLE:
				out << m.GetReflection()->GetDouble(m,fd);
				break;
			case FieldDescriptor::CPPTYPE_BOOL:	
				out << m.GetReflection()->GetBool(m,fd);
				break;
			case FieldDescriptor::CPPTYPE_STRING:
				out << "\"" << m.GetReflection()->GetString(m,fd) << "\"";
				break;
			case FieldDescriptor::CPPTYPE_MESSAGE:
				out << write(m.GetReflection()->GetMessage(m,fd));
				break;
			case FieldDescriptor::CPPTYPE_ENUM:
				 out << "\""<< m.GetReflection()->GetEnum(m,fd)->name() <<"\"";
				break;
		}
	}
	
	void JSONInterface::printRepeatedValue(const google::protobuf::FieldDescriptor *fd,
																	const google::protobuf::Message&m,std::stringstream& out,int i)
	{
		switch(fd->cpp_type()) {
			case FieldDescriptor::CPPTYPE_INT32:
				out << m.GetReflection()->GetRepeatedInt32(m,fd,i);
				break;
			case FieldDescriptor::CPPTYPE_INT64:
				out << m.GetReflection()->GetRepeatedInt64(m,fd,i);
				break;
			case FieldDescriptor::CPPTYPE_UINT32:
				out << m.GetReflection()->GetRepeatedUInt32(m,fd,i);
				break;
			case FieldDescriptor::CPPTYPE_UINT64:
				out << m.GetReflection()->GetRepeatedUInt64(m,fd,i);
				break;
			case FieldDescriptor::CPPTYPE_FLOAT:
				out << m.GetReflection()->GetRepeatedFloat(m,fd,i);
				break;
			case FieldDescriptor::CPPTYPE_DOUBLE:
				out << m.GetReflection()->GetRepeatedDouble(m,fd,i);
				break;
			case FieldDescriptor::CPPTYPE_BOOL:	
				out << m.GetReflection()->GetRepeatedBool(m,fd,i);
				break;
			case FieldDescriptor::CPPTYPE_STRING:
				out << "\""<<m.GetReflection()->GetRepeatedString(m,fd,i)<<"\"";
				break;
			case FieldDescriptor::CPPTYPE_MESSAGE:
				out << write(m.GetReflection()->GetRepeatedMessage(m,fd,i));
				break;
			case FieldDescriptor::CPPTYPE_ENUM:
				out << "\""<<m.GetReflection()->GetRepeatedEnum(m,fd,i)->full_name()<<"\"";
				break;
		}
	}
	*/
}