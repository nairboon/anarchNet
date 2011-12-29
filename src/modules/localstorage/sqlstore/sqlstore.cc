#include "config_manager.h"
#include "logger.h"
#include "puggKernel.h"
#include "db.h"
#include "sqlstore.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>


namespace fs = boost::filesystem;

using namespace pugg;
using namespace litesql;
using namespace db;

#define PATH "/sqlstore.db"

extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K)
{
	Server<an::plgdrv::LocalStorage>* server = CastToServerType<an::plgdrv::LocalStorage>(K.getServer(PLG_LOCALSTORAGE_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new SqlstoreDriver(),PLG_LOCALSTORAGE_SERVER_VERSION);
}

time_t to_time_t(const boost::posix_time::ptime& pt)
{
  using namespace boost::posix_time;
  static ptime epoch(boost::gregorian::date(1970, 1, 1));
  time_duration diff(pt - epoch);
  return (diff.ticks() / diff.ticks_per_second());
}

bool Sqlstore::initialise() {
	_db_path = an::ConfigManager::instance().datadir()+PATH;
try {
	// check data
	if(!fs::exists(_db_path)) {
		if(!create_db()) {
				LOG(ERROR) << "could not create sqlite db";
				return false;
		}
		LOG(INFO) << "created new sqlitedb";
		return true;
	}
	_db = new SQLDB("sqlite3","database="+_db_path);
	//_db->verbose = true;
	if(_db->needsUpgrade())
	_db->upgrade();
	return true;
}
catch(Except e) {
	LOG(ERROR) << "DBException: " << e;
	return false;
}
	LOG(INFO) << "sqlite store ready";
	return true;
}
boost::shared_ptr<db::Snapshot> Sqlstore::_db_store_snapshot(an::db::SnapshotPtr sn)
{
	boost::shared_ptr<db::Snapshot> ss(new db::Snapshot(*_db));
	ss->based = sn->based;
	ss->anID = sn->id;
	if(sn->_db_id) {
		ss->id = sn->_db_id;
		ss->set_id(sn->_db_id);
		ss->type = sn->_db_type;
		ss->set_inDatabase(true);
	}
	ss->content = sn->content;
	ss->time = to_time_t(sn->time);
	BOOST_FOREACH (an::db::DiffPtr diff, sn->diffs) {
		boost::shared_ptr<db::Diff> d = _db_store_diff(diff);
		ss->diffs().link(*d);
	}
	ss->update();
	return ss;
}

bool Sqlstore::db_store_snapshot(an::db::SnapshotPtr sn) {
	try {
		_db->begin();
		_db_store_snapshot(sn);
		_db->commit();
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

boost::shared_ptr<db::ObjID> Sqlstore::_db_store_id(const an::db::ObjID& id)
{
	boost::shared_ptr<db::ObjID> oi(new db::ObjID(*_db));
	oi->anID = id;
	oi->update();
	return oi;
}

boost::shared_ptr<db::Diff> Sqlstore::_db_store_diff(an::db::DiffPtr ip)
{
	boost::shared_ptr<db::Diff> diff(new db::Diff(*_db));
	diff->snapshot = ip->snapshot;
	diff->anID = ip->id;
	if(ip->_db_id) {
		diff->id = ip->_db_id;
		diff->set_id(ip->_db_id);
		diff->type = ip->_db_type;
		diff->set_inDatabase(true);
	}
	diff->content = ip->content;
	diff->time = to_time_t(ip->time);
	BOOST_FOREACH (an::db::ObjID prev, ip->prev) {
		diff->prev().link(*_db_store_id(prev));
	}
	diff->update();
	return diff;
}

bool Sqlstore::db_store_diff(an::db::DiffPtr diff)
{
	try {
		_db->begin();
		_db_store_diff(diff);
		_db->commit();
		return true;
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::db_remove(const an::db::ObjID& id)
{
	try {
		_db->begin();
		db::ObjID oi = select<db::ObjID>(*_db,db::ObjID::AnID == id).one();
		oi.del();
		_db->commit();
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}
bool Sqlstore::_db_get_snapshot(const an::db::ObjID& id, db::Snapshot& ss, an::db::SnapshotPtr res)
{
	res->id = id;
	res->_db_id = ss.id;
	res->_db_type = ss.type;
	res->based = ss.based.value();
	res->content = ss.content.value();
	res->time = boost::posix_time::from_time_t(ss.time.value().timeStamp());
	std::vector<Diff> prevs = ss.diffs().get().all();
	BOOST_FOREACH (db::Diff prevdiff, prevs) {
		an::db::DiffPtr d(new an::db::Diff());
		_db_get_diff(prevdiff.anID.value(), prevdiff,d);
		res->diffs.push_back(d);
	}
	return true;
}

bool Sqlstore::db_get_snapshot(const an::db::ObjID& id,an::db::SnapshotPtr res)
{
	try {
		db::Snapshot ss = select<Snapshot>(*_db,Snapshot::AnID == id).one();
		return _db_get_snapshot(id,ss,res);
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::_db_get_diff(const an::db::ObjID& id, db::Diff& diff, an::db::DiffPtr res) {
	res->id = id;
	res->_db_id = diff.id;
	res->_db_type = diff.type;
	res->snapshot = diff.snapshot.value();
	res->content = diff.content.value();
	res->time = boost::posix_time::from_time_t(diff.time.value().timeStamp());
	std::vector<ObjID> prevs = diff.prev().get().all();
	BOOST_FOREACH (db::ObjID previd, prevs) {
		res->prev.push_back(previd.anID.value());
	}
	return true;
}

bool Sqlstore::db_get_diff(const an::db::ObjID& id, an::db::DiffPtr res)
{
	try {
		db::Diff diff = select<Diff>(*_db,Diff::AnID == id).one();
		return _db_get_diff(id,diff,res);
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::db_get_obj(const an::db::ObjID& id, an::db::ObjPtr res)
{
	try {
		db::Object obj = select<Object>(*_db,Object::AnID == id).one();
		res->id = id;
		res->_db_id = obj.id;
		res->_db_type = obj.type;

		std::vector<Snapshot> snapshots = obj.snapshots().get().all();

		BOOST_FOREACH (db::Snapshot prevss, snapshots) {
			an::db::SnapshotPtr s(new an::db::Snapshot());
			_db_get_snapshot(prevss.anID.value(), prevss,s);
			res->snapshots.push_back(s);
			BOOST_FOREACH (an::db::DiffPtr ssdiff, s->diffs) {
				res->diffs.push_back(ssdiff);
			}
		}
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::db_store_obj(an::db::ObjPtr obj)
{
	try {
		_db->begin();
		Object dobj(*_db);
		dobj.anID = obj->id;
		if(obj->_db_id) {
			dobj.id = obj->_db_id;
			dobj.set_id(obj->_db_id);
			dobj.type = obj->_db_type;
			dobj.set_inDatabase(true);
		}
		dobj.update();
		BOOST_FOREACH (an::db::SnapshotPtr ss, obj->snapshots) {
			boost::shared_ptr<db::Snapshot> s = _db_store_snapshot(ss);
			dobj.snapshots().link(*s);
		}
		dobj.update();
		_db->commit();
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
	return true;
}

bool Sqlstore::create_db() {
	_db = new SQLDB("sqlite3","database="+_db_path);
	_db->verbose = true;
	_db->create();
	return true;
}

void Sqlstore::shutdown() {
}


bool Sqlstore::kv_put(const std::string& key, const std::string& value) {
	try {
	HT ht(*_db);
	ht.key = key;
	ht.value = value;
	ht.time = to_time_t(boost::posix_time::second_clock::local_time());
	ht.update();
	return true;
}
catch(Except e) {
	LOG(ERROR) << e;
	return false;
}
}

bool Sqlstore::kv_get(const std::string& key,std::string& res) {
	try {
	db::HT ht = select<HT>(*_db, HT::Key == key).one();
	res = ht.value.value();
	return true;
	}
catch(Except e) {
	LOG(ERROR) << e;
	return false;
}
}

bool Sqlstore::kv_remove(const std::string& key) {
	try {
		db::HT ht = select<HT>(*_db, HT::Key == key).one();
		ht.del();
		return true;
	}
	catch(Except e) {
		LOG(ERROR) << e;
		return false;
	}
}
