#include "sqldb.h"
namespace db {
using namespace litesql;
ObjectSnapshotRelation::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : snapshot(ObjectSnapshotRelation::Snapshot), object(ObjectSnapshotRelation::Object) {
    switch(rec.size()) {
    case 2:
        snapshot = rec[1];
    case 1:
        object = rec[0];
    }
}
const std::string ObjectSnapshotRelation::table__("__Object_Snapshot_");
const litesql::FieldType ObjectSnapshotRelation::Object("Object1","INTEGER",table__);
const litesql::FieldType ObjectSnapshotRelation::Snapshot("Snapshot2","INTEGER",table__);
void ObjectSnapshotRelation::link(const litesql::Database& db, const db::Object& o0, const db::Snapshot& o1) {
    Record values;
    Split fields;
    fields.push_back(Object.name());
    values.push_back(o0.id);
    fields.push_back(Snapshot.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void ObjectSnapshotRelation::unlink(const litesql::Database& db, const db::Object& o0, const db::Snapshot& o1) {
    db.delete_(table__, (Object == o0.id && Snapshot == o1.id));
}
void ObjectSnapshotRelation::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<ObjectSnapshotRelation::Row> ObjectSnapshotRelation::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Object.fullName());
    sel.result(Snapshot.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<ObjectSnapshotRelation::Row>(db, sel);
}
template <> litesql::DataSource<db::Object> ObjectSnapshotRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Object.fullName());
    sel.where(srcExpr);
    return DataSource<db::Object>(db, db::Object::Id.in(sel) && expr);
}
template <> litesql::DataSource<db::Snapshot> ObjectSnapshotRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Snapshot.fullName());
    sel.where(srcExpr);
    return DataSource<db::Snapshot>(db, db::Snapshot::Id.in(sel) && expr);
}
DiffObjectRelation::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : object(DiffObjectRelation::Object), diff(DiffObjectRelation::Diff) {
    switch(rec.size()) {
    case 2:
        object = rec[1];
    case 1:
        diff = rec[0];
    }
}
const std::string DiffObjectRelation::table__("__Diff_Object_");
const litesql::FieldType DiffObjectRelation::Diff("Diff1","INTEGER",table__);
const litesql::FieldType DiffObjectRelation::Object("Object2","INTEGER",table__);
void DiffObjectRelation::link(const litesql::Database& db, const db::Diff& o0, const db::Object& o1) {
    Record values;
    Split fields;
    fields.push_back(Diff.name());
    values.push_back(o0.id);
    fields.push_back(Object.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void DiffObjectRelation::unlink(const litesql::Database& db, const db::Diff& o0, const db::Object& o1) {
    db.delete_(table__, (Diff == o0.id && Object == o1.id));
}
void DiffObjectRelation::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<DiffObjectRelation::Row> DiffObjectRelation::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Diff.fullName());
    sel.result(Object.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<DiffObjectRelation::Row>(db, sel);
}
template <> litesql::DataSource<db::Diff> DiffObjectRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Diff.fullName());
    sel.where(srcExpr);
    return DataSource<db::Diff>(db, db::Diff::Id.in(sel) && expr);
}
template <> litesql::DataSource<db::Object> DiffObjectRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Object.fullName());
    sel.where(srcExpr);
    return DataSource<db::Object>(db, db::Object::Id.in(sel) && expr);
}
DiffSnapshotRelation::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : snapshot(DiffSnapshotRelation::Snapshot), diff(DiffSnapshotRelation::Diff) {
    switch(rec.size()) {
    case 2:
        snapshot = rec[1];
    case 1:
        diff = rec[0];
    }
}
const std::string DiffSnapshotRelation::table__("__Diff_Snapshot_");
const litesql::FieldType DiffSnapshotRelation::Diff("Diff1","INTEGER",table__);
const litesql::FieldType DiffSnapshotRelation::Snapshot("Snapshot2","INTEGER",table__);
void DiffSnapshotRelation::link(const litesql::Database& db, const db::Diff& o0, const db::Snapshot& o1) {
    Record values;
    Split fields;
    fields.push_back(Diff.name());
    values.push_back(o0.id);
    fields.push_back(Snapshot.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void DiffSnapshotRelation::unlink(const litesql::Database& db, const db::Diff& o0, const db::Snapshot& o1) {
    db.delete_(table__, (Diff == o0.id && Snapshot == o1.id));
}
void DiffSnapshotRelation::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<DiffSnapshotRelation::Row> DiffSnapshotRelation::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Diff.fullName());
    sel.result(Snapshot.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<DiffSnapshotRelation::Row>(db, sel);
}
template <> litesql::DataSource<db::Diff> DiffSnapshotRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Diff.fullName());
    sel.where(srcExpr);
    return DataSource<db::Diff>(db, db::Diff::Id.in(sel) && expr);
}
template <> litesql::DataSource<db::Snapshot> DiffSnapshotRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Snapshot.fullName());
    sel.where(srcExpr);
    return DataSource<db::Snapshot>(db, db::Snapshot::Id.in(sel) && expr);
}
DiffObjIDRelation::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : objID(DiffObjIDRelation::ObjID), diff(DiffObjIDRelation::Diff) {
    switch(rec.size()) {
    case 2:
        objID = rec[1];
    case 1:
        diff = rec[0];
    }
}
const std::string DiffObjIDRelation::table__("__Diff_ObjID_");
const litesql::FieldType DiffObjIDRelation::Diff("Diff1","INTEGER",table__);
const litesql::FieldType DiffObjIDRelation::ObjID("ObjID2","INTEGER",table__);
void DiffObjIDRelation::link(const litesql::Database& db, const db::Diff& o0, const db::ObjID& o1) {
    Record values;
    Split fields;
    fields.push_back(Diff.name());
    values.push_back(o0.id);
    fields.push_back(ObjID.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void DiffObjIDRelation::unlink(const litesql::Database& db, const db::Diff& o0, const db::ObjID& o1) {
    db.delete_(table__, (Diff == o0.id && ObjID == o1.id));
}
void DiffObjIDRelation::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<DiffObjIDRelation::Row> DiffObjIDRelation::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Diff.fullName());
    sel.result(ObjID.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<DiffObjIDRelation::Row>(db, sel);
}
template <> litesql::DataSource<db::Diff> DiffObjIDRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Diff.fullName());
    sel.where(srcExpr);
    return DataSource<db::Diff>(db, db::Diff::Id.in(sel) && expr);
}
template <> litesql::DataSource<db::ObjID> DiffObjIDRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(ObjID.fullName());
    sel.where(srcExpr);
    return DataSource<db::ObjID>(db, db::ObjID::Id.in(sel) && expr);
}
const litesql::FieldType ObjID::Own::Id("id_","INTEGER","ObjID_");
const std::string ObjID::type__("ObjID");
const std::string ObjID::table__("ObjID_");
const std::string ObjID::sequence__("ObjID_seq");
const litesql::FieldType ObjID::Id("id_","INTEGER",table__);
const litesql::FieldType ObjID::Type("type_","TEXT",table__);
const litesql::FieldType ObjID::AnID("anID_","TEXT",table__);
void ObjID::defaults() {
    id = 0;
}
ObjID::ObjID(const litesql::Database& db)
     : litesql::Persistent(db), id(Id), type(Type), anID(AnID) {
    defaults();
}
ObjID::ObjID(const litesql::Database& db, const litesql::Record& rec)
     : litesql::Persistent(db, rec), id(Id), type(Type), anID(AnID) {
    defaults();
    size_t size = (rec.size() > 3) ? 3 : rec.size();
    switch(size) {
    case 3: anID = convert<const std::string&, std::string>(rec[2]);
        anID.setModified(false);
    case 2: type = convert<const std::string&, std::string>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const std::string&, int>(rec[0]);
        id.setModified(false);
    }
}
ObjID::ObjID(const ObjID& obj)
     : litesql::Persistent(obj), id(obj.id), type(obj.type), anID(obj.anID) {
}
const ObjID& ObjID::operator=(const ObjID& obj) {
    if (this != &obj) {
        id = obj.id;
        type = obj.type;
        anID = obj.anID;
    }
    litesql::Persistent::operator=(obj);
    return *this;
}
std::string ObjID::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back(id.name());
    values.push_back(id);
    id.setModified(false);
    fields.push_back(type.name());
    values.push_back(type);
    type.setModified(false);
    fields.push_back(anID.name());
    values.push_back(anID);
    anID.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
}
void ObjID::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    std::string newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void ObjID::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
    updateField(updates, table__, type);
    updateField(updates, table__, anID);
}
void ObjID::addIDUpdates(Updates& updates) {
}
void ObjID::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ftypes.push_back(Id);
    ftypes.push_back(Type);
    ftypes.push_back(AnID);
}
void ObjID::delRecord() {
    deleteFromTable(table__, id);
}
void ObjID::delRelations() {
    DiffObjIDRelation::del(*db, (DiffObjIDRelation::ObjID == id));
}
void ObjID::update() {
    if (!inDatabase) {
        create();
        return;
    }
    Updates updates;
    addUpdates(updates);
    if (id != oldKey) {
        if (!typeIsCorrect()) 
            upcastCopy()->addIDUpdates(updates);
    }
    litesql::Persistent::update(updates);
    oldKey = id;
}
void ObjID::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<ObjID> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool ObjID::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<ObjID> ObjID::upcast() {
    if (type == Object::type__)
        return auto_ptr<ObjID>(new Object(select<Object>(*db, Id == id).one()));
    if (type == Snapshot::type__)
        return auto_ptr<ObjID>(new Snapshot(select<Snapshot>(*db, Id == id).one()));
    if (type == Diff::type__)
        return auto_ptr<ObjID>(new Diff(select<Diff>(*db, Id == id).one()));
    return auto_ptr<ObjID>(new ObjID(*this));
}
std::auto_ptr<ObjID> ObjID::upcastCopy() {
    ObjID* np = NULL;
    if (type == "Object")
        np = new Object(*db);
    if (type == "Snapshot")
        np = new Snapshot(*db);
    if (type == "Diff")
        np = new Diff(*db);
    np->id = id;
    np->type = type;
    np->anID = anID;
    np->inDatabase = inDatabase;
    return auto_ptr<ObjID>(np);
}
std::ostream & operator<<(std::ostream& os, ObjID o) {
    os << "-------------------------------------" << std::endl;
    os << o.id.name() << " = " << o.id << std::endl;
    os << o.type.name() << " = " << o.type << std::endl;
    os << o.anID.name() << " = " << o.anID << std::endl;
    os << "-------------------------------------" << std::endl;
    return os;
}
const litesql::FieldType Object::Own::Id("id_","INTEGER","Object_");
Object::SnapshotsHandle::SnapshotsHandle(const Object& owner)
         : litesql::RelationHandle<Object>(owner) {
}
void Object::SnapshotsHandle::link(const Snapshot& o0) {
    ObjectSnapshotRelation::link(owner->getDatabase(), *owner, o0);
}
void Object::SnapshotsHandle::unlink(const Snapshot& o0) {
    ObjectSnapshotRelation::unlink(owner->getDatabase(), *owner, o0);
}
void Object::SnapshotsHandle::del(const litesql::Expr& expr) {
    ObjectSnapshotRelation::del(owner->getDatabase(), expr && ObjectSnapshotRelation::Object == owner->id);
}
litesql::DataSource<Snapshot> Object::SnapshotsHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return ObjectSnapshotRelation::get<Snapshot>(owner->getDatabase(), expr, (ObjectSnapshotRelation::Object == owner->id) && srcExpr);
}
litesql::DataSource<ObjectSnapshotRelation::Row> Object::SnapshotsHandle::getRows(const litesql::Expr& expr) {
    return ObjectSnapshotRelation::getRows(owner->getDatabase(), expr && (ObjectSnapshotRelation::Object == owner->id));
}
Object::DiffsHandle::DiffsHandle(const Object& owner)
         : litesql::RelationHandle<Object>(owner) {
}
void Object::DiffsHandle::link(const Diff& o0) {
    DiffObjectRelation::link(owner->getDatabase(), o0, *owner);
}
void Object::DiffsHandle::unlink(const Diff& o0) {
    DiffObjectRelation::unlink(owner->getDatabase(), o0, *owner);
}
void Object::DiffsHandle::del(const litesql::Expr& expr) {
    DiffObjectRelation::del(owner->getDatabase(), expr && DiffObjectRelation::Object == owner->id);
}
litesql::DataSource<Diff> Object::DiffsHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return DiffObjectRelation::get<Diff>(owner->getDatabase(), expr, (DiffObjectRelation::Object == owner->id) && srcExpr);
}
litesql::DataSource<DiffObjectRelation::Row> Object::DiffsHandle::getRows(const litesql::Expr& expr) {
    return DiffObjectRelation::getRows(owner->getDatabase(), expr && (DiffObjectRelation::Object == owner->id));
}
const std::string Object::type__("Object");
const std::string Object::table__("Object_");
Object::Object(const litesql::Database& db)
     : ObjID(db) {
}
Object::Object(const litesql::Database& db, const litesql::Record& rec)
     : ObjID(db, rec) {
}
Object::Object(const Object& obj)
     : ObjID(obj) {
}
const Object& Object::operator=(const Object& obj) {
    ObjID::operator=(obj);
    return *this;
}
Object::SnapshotsHandle Object::snapshots() {
    return Object::SnapshotsHandle(*this);
}
Object::DiffsHandle Object::diffs() {
    return Object::DiffsHandle(*this);
}
std::string Object::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back("id_");
    values.push_back(id);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return ObjID::insert(tables, fieldRecs, valueRecs);
}
void Object::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    std::string newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Object::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    ObjID::addUpdates(updates);
}
void Object::addIDUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
}
void Object::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ObjID::getFieldTypes(ftypes);
}
void Object::delRecord() {
    deleteFromTable(table__, id);
    ObjID::delRecord();
}
void Object::delRelations() {
    ObjectSnapshotRelation::del(*db, (ObjectSnapshotRelation::Object == id));
    DiffObjectRelation::del(*db, (DiffObjectRelation::Object == id));
}
void Object::update() {
    if (!inDatabase) {
        create();
        return;
    }
    Updates updates;
    addUpdates(updates);
    if (id != oldKey) {
        if (!typeIsCorrect()) 
            upcastCopy()->addIDUpdates(updates);
        else
            addIDUpdates(updates);
    }
    litesql::Persistent::update(updates);
    oldKey = id;
}
void Object::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Object> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Object::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Object> Object::upcast() {
    return auto_ptr<Object>(new Object(*this));
}
std::auto_ptr<Object> Object::upcastCopy() {
    Object* np = new Object(*this);
    np->inDatabase = inDatabase;
    return auto_ptr<Object>(np);
}
std::ostream & operator<<(std::ostream& os, Object o) {
    os << "-------------------------------------" << std::endl;
    os << o.id.name() << " = " << o.id << std::endl;
    os << o.type.name() << " = " << o.type << std::endl;
    os << o.anID.name() << " = " << o.anID << std::endl;
    os << "-------------------------------------" << std::endl;
    return os;
}
const litesql::FieldType Snapshot::Own::Id("id_","INTEGER","Snapshot_");
Snapshot::DiffsHandle::DiffsHandle(const Snapshot& owner)
         : litesql::RelationHandle<Snapshot>(owner) {
}
void Snapshot::DiffsHandle::link(const Diff& o0) {
    DiffSnapshotRelation::link(owner->getDatabase(), o0, *owner);
}
void Snapshot::DiffsHandle::unlink(const Diff& o0) {
    DiffSnapshotRelation::unlink(owner->getDatabase(), o0, *owner);
}
void Snapshot::DiffsHandle::del(const litesql::Expr& expr) {
    DiffSnapshotRelation::del(owner->getDatabase(), expr && DiffSnapshotRelation::Snapshot == owner->id);
}
litesql::DataSource<Diff> Snapshot::DiffsHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return DiffSnapshotRelation::get<Diff>(owner->getDatabase(), expr, (DiffSnapshotRelation::Snapshot == owner->id) && srcExpr);
}
litesql::DataSource<DiffSnapshotRelation::Row> Snapshot::DiffsHandle::getRows(const litesql::Expr& expr) {
    return DiffSnapshotRelation::getRows(owner->getDatabase(), expr && (DiffSnapshotRelation::Snapshot == owner->id));
}
const std::string Snapshot::type__("Snapshot");
const std::string Snapshot::table__("Snapshot_");
const litesql::FieldType Snapshot::Based("based_","TEXT",table__);
const litesql::FieldType Snapshot::Content("content_","TEXT",table__);
const litesql::FieldType Snapshot::Time("time_","INTEGER",table__);
void Snapshot::defaults() {
    time = 0;
}
Snapshot::Snapshot(const litesql::Database& db)
     : ObjID(db), based(Based), content(Content), time(Time) {
    defaults();
}
Snapshot::Snapshot(const litesql::Database& db, const litesql::Record& rec)
     : ObjID(db, rec), based(Based), content(Content), time(Time) {
    defaults();
    size_t size = (rec.size() > 6) ? 6 : rec.size();
    switch(size) {
    case 6: time = convert<const std::string&, litesql::DateTime>(rec[5]);
        time.setModified(false);
    case 5: content = convert<const std::string&, std::string>(rec[4]);
        content.setModified(false);
    case 4: based = convert<const std::string&, std::string>(rec[3]);
        based.setModified(false);
    }
}
Snapshot::Snapshot(const Snapshot& obj)
     : ObjID(obj), based(obj.based), content(obj.content), time(obj.time) {
}
const Snapshot& Snapshot::operator=(const Snapshot& obj) {
    if (this != &obj) {
        based = obj.based;
        content = obj.content;
        time = obj.time;
    }
    ObjID::operator=(obj);
    return *this;
}
Snapshot::DiffsHandle Snapshot::diffs() {
    return Snapshot::DiffsHandle(*this);
}
std::string Snapshot::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back("id_");
    values.push_back(id);
    fields.push_back(based.name());
    values.push_back(based);
    based.setModified(false);
    fields.push_back(content.name());
    values.push_back(content);
    content.setModified(false);
    fields.push_back(time.name());
    values.push_back(time);
    time.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return ObjID::insert(tables, fieldRecs, valueRecs);
}
void Snapshot::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    std::string newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Snapshot::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, based);
    updateField(updates, table__, content);
    updateField(updates, table__, time);
    ObjID::addUpdates(updates);
}
void Snapshot::addIDUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
}
void Snapshot::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ObjID::getFieldTypes(ftypes);
    ftypes.push_back(Based);
    ftypes.push_back(Content);
    ftypes.push_back(Time);
}
void Snapshot::delRecord() {
    deleteFromTable(table__, id);
    ObjID::delRecord();
}
void Snapshot::delRelations() {
    ObjectSnapshotRelation::del(*db, (ObjectSnapshotRelation::Snapshot == id));
    DiffSnapshotRelation::del(*db, (DiffSnapshotRelation::Snapshot == id));
}
void Snapshot::update() {
    if (!inDatabase) {
        create();
        return;
    }
    Updates updates;
    addUpdates(updates);
    if (id != oldKey) {
        if (!typeIsCorrect()) 
            upcastCopy()->addIDUpdates(updates);
        else
            addIDUpdates(updates);
    }
    litesql::Persistent::update(updates);
    oldKey = id;
}
void Snapshot::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Snapshot> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Snapshot::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Snapshot> Snapshot::upcast() {
    return auto_ptr<Snapshot>(new Snapshot(*this));
}
std::auto_ptr<Snapshot> Snapshot::upcastCopy() {
    Snapshot* np = new Snapshot(*this);
    np->based = based;
    np->content = content;
    np->time = time;
    np->inDatabase = inDatabase;
    return auto_ptr<Snapshot>(np);
}
std::ostream & operator<<(std::ostream& os, Snapshot o) {
    os << "-------------------------------------" << std::endl;
    os << o.id.name() << " = " << o.id << std::endl;
    os << o.type.name() << " = " << o.type << std::endl;
    os << o.anID.name() << " = " << o.anID << std::endl;
    os << o.based.name() << " = " << o.based << std::endl;
    os << o.content.name() << " = " << o.content << std::endl;
    os << o.time.name() << " = " << o.time << std::endl;
    os << "-------------------------------------" << std::endl;
    return os;
}
const litesql::FieldType Diff::Own::Id("id_","INTEGER","Diff_");
Diff::PrevHandle::PrevHandle(const Diff& owner)
         : litesql::RelationHandle<Diff>(owner) {
}
void Diff::PrevHandle::link(const ObjID& o0) {
    DiffObjIDRelation::link(owner->getDatabase(), *owner, o0);
}
void Diff::PrevHandle::unlink(const ObjID& o0) {
    DiffObjIDRelation::unlink(owner->getDatabase(), *owner, o0);
}
void Diff::PrevHandle::del(const litesql::Expr& expr) {
    DiffObjIDRelation::del(owner->getDatabase(), expr && DiffObjIDRelation::Diff == owner->id);
}
litesql::DataSource<ObjID> Diff::PrevHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return DiffObjIDRelation::get<ObjID>(owner->getDatabase(), expr, (DiffObjIDRelation::Diff == owner->id) && srcExpr);
}
litesql::DataSource<DiffObjIDRelation::Row> Diff::PrevHandle::getRows(const litesql::Expr& expr) {
    return DiffObjIDRelation::getRows(owner->getDatabase(), expr && (DiffObjIDRelation::Diff == owner->id));
}
const std::string Diff::type__("Diff");
const std::string Diff::table__("Diff_");
const litesql::FieldType Diff::Snapshot("snapshot_","TEXT",table__);
const litesql::FieldType Diff::Content("content_","TEXT",table__);
const litesql::FieldType Diff::Time("time_","INTEGER",table__);
void Diff::defaults() {
    time = 0;
}
Diff::Diff(const litesql::Database& db)
     : ObjID(db), snapshot(Snapshot), content(Content), time(Time) {
    defaults();
}
Diff::Diff(const litesql::Database& db, const litesql::Record& rec)
     : ObjID(db, rec), snapshot(Snapshot), content(Content), time(Time) {
    defaults();
    size_t size = (rec.size() > 6) ? 6 : rec.size();
    switch(size) {
    case 6: time = convert<const std::string&, litesql::DateTime>(rec[5]);
        time.setModified(false);
    case 5: content = convert<const std::string&, std::string>(rec[4]);
        content.setModified(false);
    case 4: snapshot = convert<const std::string&, std::string>(rec[3]);
        snapshot.setModified(false);
    }
}
Diff::Diff(const Diff& obj)
     : ObjID(obj), snapshot(obj.snapshot), content(obj.content), time(obj.time) {
}
const Diff& Diff::operator=(const Diff& obj) {
    if (this != &obj) {
        snapshot = obj.snapshot;
        content = obj.content;
        time = obj.time;
    }
    ObjID::operator=(obj);
    return *this;
}
Diff::PrevHandle Diff::prev() {
    return Diff::PrevHandle(*this);
}
std::string Diff::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back("id_");
    values.push_back(id);
    fields.push_back(snapshot.name());
    values.push_back(snapshot);
    snapshot.setModified(false);
    fields.push_back(content.name());
    values.push_back(content);
    content.setModified(false);
    fields.push_back(time.name());
    values.push_back(time);
    time.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return ObjID::insert(tables, fieldRecs, valueRecs);
}
void Diff::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    std::string newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Diff::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, snapshot);
    updateField(updates, table__, content);
    updateField(updates, table__, time);
    ObjID::addUpdates(updates);
}
void Diff::addIDUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
}
void Diff::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ObjID::getFieldTypes(ftypes);
    ftypes.push_back(Snapshot);
    ftypes.push_back(Content);
    ftypes.push_back(Time);
}
void Diff::delRecord() {
    deleteFromTable(table__, id);
    ObjID::delRecord();
}
void Diff::delRelations() {
    DiffObjectRelation::del(*db, (DiffObjectRelation::Diff == id));
    DiffSnapshotRelation::del(*db, (DiffSnapshotRelation::Diff == id));
    DiffObjIDRelation::del(*db, (DiffObjIDRelation::Diff == id));
}
void Diff::update() {
    if (!inDatabase) {
        create();
        return;
    }
    Updates updates;
    addUpdates(updates);
    if (id != oldKey) {
        if (!typeIsCorrect()) 
            upcastCopy()->addIDUpdates(updates);
        else
            addIDUpdates(updates);
    }
    litesql::Persistent::update(updates);
    oldKey = id;
}
void Diff::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Diff> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Diff::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Diff> Diff::upcast() {
    return auto_ptr<Diff>(new Diff(*this));
}
std::auto_ptr<Diff> Diff::upcastCopy() {
    Diff* np = new Diff(*this);
    np->snapshot = snapshot;
    np->content = content;
    np->time = time;
    np->inDatabase = inDatabase;
    return auto_ptr<Diff>(np);
}
std::ostream & operator<<(std::ostream& os, Diff o) {
    os << "-------------------------------------" << std::endl;
    os << o.id.name() << " = " << o.id << std::endl;
    os << o.type.name() << " = " << o.type << std::endl;
    os << o.anID.name() << " = " << o.anID << std::endl;
    os << o.snapshot.name() << " = " << o.snapshot << std::endl;
    os << o.content.name() << " = " << o.content << std::endl;
    os << o.time.name() << " = " << o.time << std::endl;
    os << "-------------------------------------" << std::endl;
    return os;
}
SQLDB::SQLDB(std::string backendType, std::string connInfo)
     : litesql::Database(backendType, connInfo) {
    initialize();
}
std::vector<litesql::Database::SchemaItem> SQLDB::getSchema() const {
    vector<Database::SchemaItem> res;
    res.push_back(Database::SchemaItem("schema_","table","CREATE TABLE schema_ (name_ TEXT, type_ TEXT, sql_ TEXT);"));
    if (backend->supportsSequences()) {
        res.push_back(Database::SchemaItem("ObjID_seq","sequence","CREATE SEQUENCE ObjID_seq START 1 INCREMENT 1"));
    }
    res.push_back(Database::SchemaItem("ObjID_","table","CREATE TABLE ObjID_ (id_ " + backend->getRowIDType() + ",type_ TEXT,anID_ TEXT)"));
    res.push_back(Database::SchemaItem("Object_","table","CREATE TABLE Object_ (id_ " + backend->getRowIDType() + ")"));
    res.push_back(Database::SchemaItem("Snapshot_","table","CREATE TABLE Snapshot_ (id_ " + backend->getRowIDType() + ",based_ TEXT,content_ TEXT,time_ INTEGER)"));
    res.push_back(Database::SchemaItem("Diff_","table","CREATE TABLE Diff_ (id_ " + backend->getRowIDType() + ",snapshot_ TEXT,content_ TEXT,time_ INTEGER)"));
    res.push_back(Database::SchemaItem("__Object_Snapshot_","table","CREATE TABLE __Object_Snapshot_ (Object1 INTEGER,Snapshot2 INTEGER)"));
    res.push_back(Database::SchemaItem("__Diff_Object_","table","CREATE TABLE __Diff_Object_ (Diff1 INTEGER,Object2 INTEGER)"));
    res.push_back(Database::SchemaItem("__Diff_Snapshot_","table","CREATE TABLE __Diff_Snapshot_ (Diff1 INTEGER,Snapshot2 INTEGER)"));
    res.push_back(Database::SchemaItem("__Diff_ObjID_","table","CREATE TABLE __Diff_ObjID_ (Diff1 INTEGER,ObjID2 INTEGER)"));
    res.push_back(Database::SchemaItem("__Object_Snapshot_Object1idx","index","CREATE INDEX __Object_Snapshot_Object1idx ON __Object_Snapshot_ (Object1)"));
    res.push_back(Database::SchemaItem("__Object_Snapshot_Snapshot2idx","index","CREATE INDEX __Object_Snapshot_Snapshot2idx ON __Object_Snapshot_ (Snapshot2)"));
    res.push_back(Database::SchemaItem("__Object_Snapshot__all_idx","index","CREATE INDEX __Object_Snapshot__all_idx ON __Object_Snapshot_ (Object1,Snapshot2)"));
    res.push_back(Database::SchemaItem("__Diff_Object_Diff1idx","index","CREATE INDEX __Diff_Object_Diff1idx ON __Diff_Object_ (Diff1)"));
    res.push_back(Database::SchemaItem("__Diff_Object_Object2idx","index","CREATE INDEX __Diff_Object_Object2idx ON __Diff_Object_ (Object2)"));
    res.push_back(Database::SchemaItem("__Diff_Object__all_idx","index","CREATE INDEX __Diff_Object__all_idx ON __Diff_Object_ (Diff1,Object2)"));
    res.push_back(Database::SchemaItem("__Diff_Snapshot_Diff1idx","index","CREATE INDEX __Diff_Snapshot_Diff1idx ON __Diff_Snapshot_ (Diff1)"));
    res.push_back(Database::SchemaItem("__Diff_Snapshot_Snapshot2idx","index","CREATE INDEX __Diff_Snapshot_Snapshot2idx ON __Diff_Snapshot_ (Snapshot2)"));
    res.push_back(Database::SchemaItem("__Diff_Snapshot__all_idx","index","CREATE INDEX __Diff_Snapshot__all_idx ON __Diff_Snapshot_ (Diff1,Snapshot2)"));
    res.push_back(Database::SchemaItem("__Diff_ObjID_Diff1idx","index","CREATE INDEX __Diff_ObjID_Diff1idx ON __Diff_ObjID_ (Diff1)"));
    res.push_back(Database::SchemaItem("__Diff_ObjID_ObjID2idx","index","CREATE INDEX __Diff_ObjID_ObjID2idx ON __Diff_ObjID_ (ObjID2)"));
    res.push_back(Database::SchemaItem("__Diff_ObjID__all_idx","index","CREATE INDEX __Diff_ObjID__all_idx ON __Diff_ObjID_ (Diff1,ObjID2)"));
    return res;
}
void SQLDB::initialize() {
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;
}
}
