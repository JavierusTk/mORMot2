# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `/mnt/w/mORMot2/src/db` directory contains mORMot 2's **Data Access Layer** - a lightweight, high-performance SQL and NoSQL database abstraction that **deliberately avoids the complexity and overhead** of Delphi's traditional `TDataSet`/`DB.pas` architecture.

**Why This Exists:**
- Direct JSON integration for REST/ORM (no manual field copying)
- Simple, focused API with minimal types (vs 50+ classes in DB.pas)
- Cross-database compatibility without vendor lock-in
- Standalone usage (no dependency on `mormot.orm.*` or `mormot.rest.*`)
- Highest possible performance (statement caching, parameter pooling, thread-safe connections)

**When to Use:** Any project needing SQL/NoSQL access - works with or without mORMot ORM/REST layers.

## Database Provider Hierarchy

```
TSqlDBConnection (abstract - NOT thread-safe)
├─ Direct databases:
│  ├─ SQLite3 (mormot.db.sql.sqlite3)
│  ├─ PostgreSQL (mormot.db.sql.postgres)
│  ├─ Oracle (mormot.db.sql.oracle)
│  ├─ MSSQL (mormot.db.sql.mssql)
│  └─ MySQL (mormot.db.sql.mysql)
│
├─ Abstraction layers:
│  ├─ ZEOS (cross-database - recommended)
│  └─ ODBC (Windows)
│
└─ Use TSqlDBConnectionThreadSafe wrapper for multi-threading
```

## Database Providers Quick Ref

| Provider | Dependency | Use When | Limitation |
|----------|-----------|----------|-----------|
| **sqlite3** | Built-in | Embedded, single-server | No horizontal scaling |
| **postgres** | libpq | Enterprise PostgreSQL | Connection overhead |
| **oracle** | OCI | Enterprise Oracle | Field size 1333 chars |
| **mssql** | Windows/ODBC | Windows enterprise | Field size 4000 chars |
| **zeos** | ZDBC library | Any database, cross-platform | **Recommended** for portability |
| **odbc** | ODBC drivers | Windows only | Windows-specific |

**Thread-Safe Connection** (CRITICAL pattern):

```pascal
// ✅ DO: Use ThreadSafe wrapper for multi-threaded access
Props := TSqlDBPostgresConnectionProperties.Create(...);
Conn := Props.ThreadSafeConnection;  // Returns wrapper
Stmt := Conn.NewStatementPrepared('SELECT * FROM users WHERE id=?', True);

// ❌ DON'T: Use base connection directly in threads
Props := TSqlDBPostgresConnectionProperties.Create(...);
Conn := Props.NewConnection;  // Base connection NOT thread-safe!
// Using in multiple threads = race conditions
```

## SAD References

**📖 Main Documentation**: [Software Architecture Design - mORMot 2](/mnt/w/mORMot2/DOCS/README.md)

| Task | SAD Chapter(s) | Notes |
|------|----------------|-------|
| Database connections | [Chapter 7: SQL Database Access](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-07.md) | TSqlDBConnection, providers |
| SQLite3 integration | [Chapter 8: SQLite3 Database](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-08.md) | Virtual tables, FTS |
| MongoDB/NoSQL | [Chapter 9: External NoSQL Database Access](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-09.md) | BSON, wire protocol |
| Architecture overview | [SAD 7.2](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-07.md) | Core abstractions |
| Provider hierarchy | [SAD 7.3](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-07.md) | Direct, ODBC, ZEOS |
| API reference | [SAD 7.5](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-07.md) | TSqlDBStatement usage |

## Quick Patterns

> **Note**: Only unique content NOT covered in SAD belongs here.

### Database-Specific Behavior

**LIMIT Syntax** (use `AdaptSqlLimitForEngineList()` for auto-conversion):
- Oracle: `WHERE rownum<=N`
- MSSQL: `SELECT TOP(N)`
- MySQL/SQLite/PostgreSQL: `LIMIT N`
- Firebird: `SELECT FIRST N`

**Field Size Limits** (TEXT/VARCHAR):
- Oracle: 1333 chars (4000 bytes ÷ 3 for UTF-8)
- MSSQL/MySQL/MariaDB: 4000 chars
- Firebird: 32760 chars
- PostgreSQL/SQLite: No limit (0)

**Batch Parameters** (max `?` placeholders per query):
- SQLite: 500 (theoretical 999)
- Oracle/MSSQL/MySQL/PostgreSQL: 500
- NexusDB: 100 (empirical)

### Key Differences from DB.pas

| Feature | mORMot DB | TDataSet/DB.pas |
|---------|-----------|-----------------|
| **JSON Support** | Native (`FetchAllAsJson`, `ColumnBlob`) | Manual field iteration |
| **Threading** | Explicit thread-safe wrappers | Not thread-safe |
| **Memory** | Minimal overhead (no component stack) | Heavy (50+ classes) |
| **Delphi CE** | Fully compatible | Restricted |
| **NoSQL** | MongoDB BSON/wire protocol | SQL only |
| **Remote Access** | Built-in HTTP proxy (`mormot.db.proxy.pas`) | Requires DataSnap/etc |

## AI Guidelines

> **Critical rules for code generation/modification**

- ⚠️ **Thread Safety**: Use `TSqlDBConnectionThreadSafe` or `Props.ThreadSafeConnection` for multi-threaded access. Base `TSqlDBConnection` is **NOT thread-safe**.
- ⚠️ **Parameter Binding**: Always use `?` placeholders and `Bind()` methods. Framework auto-converts to engine syntax (`:AA`, `$1`, etc.).
- ⚠️ **Statement Caching**: Use `NewStatementPrepared(..., True)` to enable prepared statement caching (major performance boost).
- ✅ **JSON Integration**: Prefer `FetchAllAsJson()` or `TResultsWriter` over manual field copying.
- ✅ **Provider Selection**: Use `mormot.db.sql.zeos.pas` for cross-database projects (avoids direct driver dependencies).
- ✅ **Logging Control**: Define `SYNDB_SILENCE` to disable statement execution logging.

## Files Organization

```
mormot.db.core.pas          # Core abstractions (TSqlDBConnection, TSqlDBStatement)
mormot.db.sql.pas           # SQL connection factory
mormot.db.raw.*.pas         # Low-level database drivers (SQLite3, PostgreSQL, Oracle, ODBC, OleDB)
mormot.db.sql.*.pas         # High-level wrappers (zeos recommended)
mormot.db.rad.*.pas         # TDataSet bridges (FireDAC, UniDAC, BDE, NexusDB)
mormot.db.rad.ui.*.pas      # UI integration (TVirtualDataSet, TClientDataSet)
mormot.db.nosql.*.pas       # MongoDB (BSON encoding, wire protocol)
mormot.db.proxy.pas         # HTTP-based remote database relay
```

**Project Files**: `/mnt/w/mORMot2/src/db/*.pas`

## Notes

- **Recommended Provider**: `mormot.db.sql.zeos.pas` for cross-database support (uses ZDBC library)
- **MongoDB Version**: Supports MongoDB 5.1+ with OP_MSG/OP_COMPRESSED protocol. Define `MONGO_OLDPROTOCOL` for MongoDB < 3.6.
- **MAX_SQLFIELDS**: Default 64 columns per table (configurable to 128/192/256 via compiler define)

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
