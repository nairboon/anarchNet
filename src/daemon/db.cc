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

#include <cstdlib>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "db.h"
#include "diff_match_patch.h"
#include <boost/foreach.hpp>
#include "crypto.h"
#include "logger.h"
#include "db_manager.h"


using std::string;

namespace an
{
  
  namespace db {
    bool needs_ObjID::create_random_id() {
      std::string stime = boost::posix_time::to_iso_string(time);
      std::stringstream rndinp;
      rndinp << stime << content << random();
      id = ObjID(crypto::toHex(crypto::Hash(rndinp.str())));
      return true;
    }
    bool needs_ObjID::create_content_id() {
      id = ObjID(crypto::toHex(crypto::Hash(content)));
      return true;
    }
    
    bool Object::create(String inp)
    {
      id = ObjID(crypto::toHex(crypto::Hash("OBJECT"+crypto::Hash(inp))));
      return create(inp,id);
    }
    
    bool Object::create(String inp,const ObjID& custom_id)
    {
        diff_match_patch<String> dmp;
    std::string initialDiff = dmp.patch_toText(dmp.patch_make("", inp));
    
    LOG(INFO) << "oc.patch: " << initialDiff;
     db::DiffPtr diff(new db::Diff(initialDiff,""));
      diffs.push_back(diff);

      master = diff->id;
      id = custom_id;
      head = inp;
      return DBManager::instance().create_object(shared_from_this());
    }
    bool Object::create_from_diffs()
    {
      return false;
    }
    
    bool Object::save() {
      //return DBManager::instance().save_object(id,shared_from_this());
      if(!DBManager::instance().delete_object(id))
	return false;
      
     if(!DBManager::instance().create_object(shared_from_this()))
	return false;
      
      return true;
    }
    
    bool Object::remove() {
      return DBManager::instance().delete_object(id);
    }
    
    bool Object::load(const ObjID& id)
    {
      return DBManager::instance().get_object_head(id,shared_from_this());
    }
    
    String Object::get() {
     /* db::SnapshotPtr ss = snapshots.back();
      String res = ss->content;
      diff_match_patch<String> dmp;
      LOG(INFO) << "loading obj with diffs: " << ss->diffs.size() << " : " << diffs.size();
      
      BOOST_FOREACH (an::db::DiffPtr diff, diffs) {
	LOG(INFO) << "diff: " << diff->content;
	std::pair<String, std::vector<bool> > out =
	dmp.patch_apply(dmp.patch_fromText(diff->content), res);
	res = out.first;
      }
      LOG(INFO) << "before: " << ss->content << " after: " << res;
      return res;*/
     return head;
    }
    String Object::get(const ObjID& revision) {
      return "";
    }
  }
}
