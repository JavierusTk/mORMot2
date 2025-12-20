# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mORMot2/src/orm` folder implements the RESTful ORM (Object-Relational Mapping) layer - a persistence-agnostic data access framework that works with SQLite3, external SQL databases, MongoDB (ODM), or in-memory storage.

**Key Design Principle**: The ORM is decoupled from REST transport (`mormot.rest.core`) and can be used as a pure persistence layer.

**When to use**: Database-backed applications needing clean interface-based data access without tight coupling to specific storage backends.

## Dependency Chain (TOrm Hierarchy)

```
IRestOrm (interface - what to use)
├─ TRestOrmClient (client-side)
└─ TRestOrmServer (server-side)
    ├─ TRestOrmServerDB (SQLite3)
    └─ TRestStorageExternal (external SQL)

TOrm (persistent class)
├─ TOrmModel (schema definition - table order matters!)
├─ TOrmTable (query results - read-only)
└─ TRestBatch (bulk operations - high-performance)
```

## ORM Quick Reference

| Class | Purpose | Use When |
|-------|---------|----------|
| `TOrm` | Base persistent class | Define your entities |
| `TOrmModel` | Data model definition | Configure schema (table order CRITICAL) |
| `IRestOrm` | Universal ORM interface | Code against this, not TRestOrm |
| `TOrmTable` | Query results (read-only) | Iterate Orm.ExecuteList() results |
| `TRestBatch` | High-perf bulk insert/update | >100 records, single roundtrip |
| `TRecordReference` | Typed foreign key | Define relationships |
| `TModTime` / `TCreateTime` | Auto-timestamps | Track creation/modification |
| `TSessionUserID` | Auto-filled by server | Link records to users |

**Typed TID Pattern** (DO/DON'T):

```pascal
// ✅ DO: Use typed IDs for referential integrity
type
  TOrmClientID = type TID;  // Links to TOrmClient
  TOrmClientToBeDeletedID = type TID;  // CASCADE delete

TOrmOrder = class(TOrm)
published
  property Client: TOrmClientID;  // Auto-resets when TOrmClient deleted
  property Owner: TOrmClientToBeDeletedID;  // Order deleted with owner
end;

// ❌ DON'T: Use raw TID fields (no referential integrity)
TOrmOrder = class(TOrm)
published
  property ClientID: TID;  // Just a number - no safety
end;
```

## SAD References

**📖 Main Documentation**: [Software Architecture Design - mORMot 2](/mnt/w/mORMot2/DOCS/README.md)

| Task | SAD Chapter(s) | Notes |
|------|----------------|-------|
| Understanding TOrm classes | [Chapter 5: ORM](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-05.md) | Base class, field types |
| Defining database schema | [Chapter 5.3: TOrmModel](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-05.md) | Model creation |
| CRUD operations | [Chapter 5.2: IRestOrm](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-05.md) | Retrieve, Add, Update, Delete |
| Batch operations | [Chapter 5.5: TRestBatch](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-05.md) | High-performance bulk ops |
| Filtering and validation | [Chapter 6.1: Filtering](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-06.md) | WHERE clauses, validators |
| Virtual tables | [Chapter 6.2: Virtual Tables](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-06.md) | JSON/Binary/External storage |
| Choosing storage backend | [Chapter 6.3: Storage Backends](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-06.md) | SQLite3, External, MongoDB |
| Caching strategies | [Chapter 6.4: Caching](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-06.md) | TOrmCache |

## Quick Patterns

### Storage Engine Selection Guide

| Backend | Use When | Pros | Cons |
|---------|----------|------|------|
| **TRestOrmServerDB** (SQLite3) | Embedded, single-server | Fast, ACID, SQL joins, FTS | No horizontal scaling |
| **TRestStorageExternal** | Enterprise SQL DB | Scalable, existing infra | Connection overhead |
| **TRestStorageMongoDB** | Document store, sharding | Flexible schema, horizontal scaling | No SQL joins |
| **TRestStorageInMemory** | Caching, temp data | Fastest, JSON/binary persistence | RAM-limited, no ACID |

### Static Tables (Server-Side)

**Two distinct types** (commonly confused):

```pascal
// fStaticData[] - Pure in-memory (NOT visible in SQL joins)
Server.StaticDataAdd(TOrmSettings, TRestStorageInMemory);

// fStaticVirtualTable[] - Virtual tables (VISIBLE in SQL joins)
Model.VirtualTableRegister(TOrmSettings, TOrmVirtualTableJson);
Server.CreateMissingTables;
```

**Rule**: Use `fStaticVirtualTable[]` if you need SQL JOINs with other tables.

## AI Guidelines

- ⚠️ **TOrmModel table order**: Never reorder tables after deployment - `TRecordReference` encodes table index in high bits. Changing order breaks existing references.
- ⚠️ **Virtual table registration timing**: Must call `Model.VirtualTableRegister()` BEFORE `TRestOrmServer.Create`. Registration after server creation silently fails.
- ⚠️ **Batch ownership**: When using `Batch.Add(Obj, true)`, the batch owns the object. Do NOT free manually or you'll cause double-free.
- ⚠️ **Static table limitations**: `fStaticData[]` tables are NOT visible in SQL joins. Use `fStaticVirtualTable[]` if you need joins.
- ⚠️ **TModTime fields**: Only updated by server operations (`Add`, `Update`). Direct SQL updates won't trigger auto-update.
- ✅ **IRestOrm interface**: Always declare variables as `IRestOrm` (interface), not `TRestOrm` (implementation class). Use interface-based design.
- ✅ **Thread safety**: `TOrmModel` is read-only after creation (thread-safe). `TOrm` instances are NOT thread-safe (each thread needs own instances). `TRestBatch` is NOT thread-safe (use one per thread).

## Files Organization

```
mormot.orm.base.pas        # Low-level types, TOrmWriter, TOrmPropInfo
mormot.orm.core.pas        # TOrm, IRestOrm, TOrmModel, TOrmTable (195 KB)
mormot.orm.rest.pas        # TRestOrm base implementation
mormot.orm.client.pas      # TRestOrmClient, TRestOrmClientUri
mormot.orm.server.pas      # TRestOrmServer, static tables
mormot.orm.storage.pas     # TRestStorage, TRestStorageInMemory
mormot.orm.sqlite3.pas     # TRestOrmServerDB (SQLite3 native)
mormot.orm.sql.pas         # TRestStorageExternal (via mormot.db.sql)
mormot.orm.mongodb.pas     # TRestStorageMongoDB (NoSQL/ODM)
```

**Project Files**: `/mnt/w/mORMot2/test/test.orm.*.pas` (comprehensive test suite)

## Notes

**Logging**: Enable SQL statement logging via `Orm.LogFamily.Level := LOG_VERBOSE;` for debugging.

**JSON inspection**: Use `TOrm.GetJSONValues()` to debug serialization issues.

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
