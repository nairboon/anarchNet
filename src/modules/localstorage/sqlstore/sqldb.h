#ifndef sqldb_hpp
#define sqldb_hpp
#include "litesql.hpp"
namespace db {
class ObjID;
class Object;
class Snapshot;
class Diff;
class ObjectSnapshotRelation {
public:
    class Row {
    public:
        litesql::Field<int> snapshot;
        litesql::Field<int> object;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const std::string table__;
    static const litesql::FieldType Object;
    static const litesql::FieldType Snapshot;
    static void link(const litesql::Database& db, const db::Object& o0, const db::Snapshot& o1);
    static void unlink(const litesql::Database& db, const db::Object& o0, const db::Snapshot& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<ObjectSnapshotRelation::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class DiffObjectRelation {
public:
    class Row {
    public:
        litesql::Field<int> object;
        litesql::Field<int> diff;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const std::string table__;
    static const litesql::FieldType Diff;
    static const litesql::FieldType Object;
    static void link(const litesql::Database& db, const db::Diff& o0, const db::Object& o1);
    static void unlink(const litesql::Database& db, const db::Diff& o0, const db::Object& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<DiffObjectRelation::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class DiffSnapshotRelation {
public:
    class Row {
    public:
        litesql::Field<int> snapshot;
        litesql::Field<int> diff;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const std::string table__;
    static const litesql::FieldType Diff;
    static const litesql::FieldType Snapshot;
    static void link(const litesql::Database& db, const db::Diff& o0, const db::Snapshot& o1);
    static void unlink(const litesql::Database& db, const db::Diff& o0, const db::Snapshot& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<DiffSnapshotRelation::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class DiffObjIDRelation {
public:
    class Row {
    public:
        litesql::Field<int> objID;
        litesql::Field<int> diff;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const std::string table__;
    static const litesql::FieldType Diff;
    static const litesql::FieldType ObjID;
    static void link(const litesql::Database& db, const db::Diff& o0, const db::ObjID& o1);
    static void unlink(const litesql::Database& db, const db::Diff& o0, const db::ObjID& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<DiffObjIDRelation::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class ObjID : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    static const std::string type__;
    static const std::string table__;
    static const std::string sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<std::string> type;
    static const litesql::FieldType AnID;
    litesql::Field<std::string> anID;
protected:
    void defaults();
public:
    ObjID(const litesql::Database& db);
    ObjID(const litesql::Database& db, const litesql::Record& rec);
    ObjID(const ObjID& obj);
    const ObjID& operator=(const ObjID& obj);
protected:
    std::string insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
    void create();
    virtual void addUpdates(Updates& updates);
    virtual void addIDUpdates(Updates& updates);
public:
    static void getFieldTypes(std::vector<litesql::FieldType>& ftypes);
protected:
    virtual void delRecord();
    virtual void delRelations();
public:
    virtual void update();
    virtual void del();
    virtual bool typeIsCorrect();
    std::auto_ptr<ObjID> upcast();
    std::auto_ptr<ObjID> upcastCopy();
};
std::ostream & operator<<(std::ostream& os, ObjID o);
class Object : public ObjID {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class SnapshotsHandle : public litesql::RelationHandle<Object> {
    public:
        SnapshotsHandle(const Object& owner);
        void link(const Snapshot& o0);
        void unlink(const Snapshot& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Snapshot> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<ObjectSnapshotRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    class DiffsHandle : public litesql::RelationHandle<Object> {
    public:
        DiffsHandle(const Object& owner);
        void link(const Diff& o0);
        void unlink(const Diff& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Diff> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<DiffObjectRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const std::string type__;
    static const std::string table__;
    Object(const litesql::Database& db);
    Object(const litesql::Database& db, const litesql::Record& rec);
    Object(const Object& obj);
    const Object& operator=(const Object& obj);
    Object::SnapshotsHandle snapshots();
    Object::DiffsHandle diffs();
protected:
    std::string insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
    void create();
    virtual void addUpdates(Updates& updates);
    virtual void addIDUpdates(Updates& updates);
public:
    static void getFieldTypes(std::vector<litesql::FieldType>& ftypes);
protected:
    virtual void delRecord();
    virtual void delRelations();
public:
    virtual void update();
    virtual void del();
    virtual bool typeIsCorrect();
    std::auto_ptr<Object> upcast();
    std::auto_ptr<Object> upcastCopy();
};
std::ostream & operator<<(std::ostream& os, Object o);
class Snapshot : public ObjID {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class DiffsHandle : public litesql::RelationHandle<Snapshot> {
    public:
        DiffsHandle(const Snapshot& owner);
        void link(const Diff& o0);
        void unlink(const Diff& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Diff> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<DiffSnapshotRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const std::string type__;
    static const std::string table__;
    static const litesql::FieldType Based;
    litesql::Field<std::string> based;
    static const litesql::FieldType Content;
    litesql::Field<std::string> content;
    static const litesql::FieldType Time;
    litesql::Field<litesql::DateTime> time;
protected:
    void defaults();
public:
    Snapshot(const litesql::Database& db);
    Snapshot(const litesql::Database& db, const litesql::Record& rec);
    Snapshot(const Snapshot& obj);
    const Snapshot& operator=(const Snapshot& obj);
    Snapshot::DiffsHandle diffs();
protected:
    std::string insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
    void create();
    virtual void addUpdates(Updates& updates);
    virtual void addIDUpdates(Updates& updates);
public:
    static void getFieldTypes(std::vector<litesql::FieldType>& ftypes);
protected:
    virtual void delRecord();
    virtual void delRelations();
public:
    virtual void update();
    virtual void del();
    virtual bool typeIsCorrect();
    std::auto_ptr<Snapshot> upcast();
    std::auto_ptr<Snapshot> upcastCopy();
};
std::ostream & operator<<(std::ostream& os, Snapshot o);
class Diff : public ObjID {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class PrevHandle : public litesql::RelationHandle<Diff> {
    public:
        PrevHandle(const Diff& owner);
        void link(const ObjID& o0);
        void unlink(const ObjID& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<ObjID> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<DiffObjIDRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const std::string type__;
    static const std::string table__;
    static const litesql::FieldType Snapshot;
    litesql::Field<std::string> snapshot;
    static const litesql::FieldType Content;
    litesql::Field<std::string> content;
    static const litesql::FieldType Time;
    litesql::Field<litesql::DateTime> time;
protected:
    void defaults();
public:
    Diff(const litesql::Database& db);
    Diff(const litesql::Database& db, const litesql::Record& rec);
    Diff(const Diff& obj);
    const Diff& operator=(const Diff& obj);
    Diff::PrevHandle prev();
protected:
    std::string insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
    void create();
    virtual void addUpdates(Updates& updates);
    virtual void addIDUpdates(Updates& updates);
public:
    static void getFieldTypes(std::vector<litesql::FieldType>& ftypes);
protected:
    virtual void delRecord();
    virtual void delRelations();
public:
    virtual void update();
    virtual void del();
    virtual bool typeIsCorrect();
    std::auto_ptr<Diff> upcast();
    std::auto_ptr<Diff> upcastCopy();
};
std::ostream & operator<<(std::ostream& os, Diff o);
class SQLDB : public litesql::Database {
public:
    SQLDB(std::string backendType, std::string connInfo);
protected:
    virtual std::vector<litesql::Database::SchemaItem> getSchema() const;
    static void initialize();
};
}
#endif
