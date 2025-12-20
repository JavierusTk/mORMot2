# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mORMot2/src/rest` directory implements the RESTful-oriented features of the mORMot framework. This is a Client-Server architecture using JSON for data representation, with support for HTTP/HTTPS, WebSockets, and in-process calls.

**Key Philosophy**: mORMot is REST-oriented but not truly RESTful. It's designed for method-based and interface-based services (more RPC-oriented) rather than pure resource-based REST. The framework emphasizes composition over inheritance (TRest.Orm interface, TServiceContainer) following SOLID principles.

**When to use this**:
- Building client-server applications with JSON communication
- Implementing interface-based services (SOA)
- Creating HTTP/WebSocket servers with ORM integration
- RESTful API development with authentication and sessions

## REST Architecture (Composition Pattern)

```
TRest (abstract base)
├─ ORM: IRestOrm interface (object-relational-mapping)
├─ Services: TServiceContainer (SOA functionality)
├─ Run: TRestRunThreads (threading)
└─ REST features (URI routing, auth, sessions)
    ├─ TRestClient (client-side)
    └─ TRestServer (server-side)
        ├─ TRestServerDB (SQLite3 backend)
        ├─ TRestServerFullMemory (in-memory)
        └─ TRestHttpServer (HTTP wrapper)
```

## HTTP Server Modes Quick Reference

| Mode | Threading | Scaling | Best For |
|------|-----------|---------|----------|
| `useHttpApi` | Windows HTTP.SYS | Very High (kernel-mode) | Windows-only high-scale |
| `useHttpAsync` | Event-driven | High (modern async) | **Recommended** for production |
| `useHttpSocket` | Thread-per-connection | Medium (good behind proxy) | Behind reverse proxy |
| `useBidirSocket` | Thread-per-client | Low-medium | WebSockets with fewer clients |
| `useBidirAsync` | Event-driven WebSocket | High | WebSockets at scale |

**Proper Shutdown Sequence** (prevent memory leaks):

```pascal
// ✅ DO: Free in correct order
FreeAndNil(HttpServer);  // Release HTTP wrapper first
FreeAndNil(Server);      // Then REST server
FreeAndNil(Model);       // Finally model

// ❌ DON'T: Wrong order causes leaks/crashes
FreeAndNil(Server);      // HttpServer becomes dangling pointer!
FreeAndNil(HttpServer);  // Can't access HttpServer anymore
```

## SAD References

**📖 Main Documentation**: [Software Architecture Design - mORMot 2](/mnt/w/mORMot2/DOCS/README.md)

| Task | SAD Chapter(s) | Notes |
|------|----------------|-------|
| Understanding REST architecture | [Chapter 10: JSON and RESTful Fundamentals](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-10.md) | JSON serialization, REST concepts |
| Creating servers/clients | [Chapter 11: Client-Server Architecture](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-11.md) | TRestServer, TRestClient, HTTP transport |
| ORM CRUD operations | [Chapter 12: REST ORM Operations](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-12.md) | Retrieve, Add, Update, Delete, Batch |
| User authentication | [Chapter 13: Authentication and Sessions](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-13.md) | Security, sessions, auth schemes |
| Interface-based services | [Chapter 14: Interface-Based Services](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-14.md) | SOA integration with REST |
| Threading models | [Chapter 11: Client-Server Architecture](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-11.md) | Section 11.6: TRestServerAcquireMode |
| URI routing customization | [Chapter 11: Client-Server Architecture](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-11.md) | TRestRouter, custom routing |

## Quick Patterns

### Creating REST Server
```pascal
// 1. Define ORM model
Model := TOrmModel.Create([TOrmUser, TOrmData]);

// 2. Choose storage backend
Server := TRestServerDB.Create(Model, 'data.db3');      // SQLite3
// OR
Server := TRestServerFullMemory.Create(Model);          // In-memory

// 3. Wrap in HTTP server (useHttpAsync for production scaling)
HttpServer := TRestHttpServer.Create('8080', [Server], '+', useHttpAsync);

// 4. Register services (if needed)
Server.ServiceDefine(TMyService, [IMyService], sicShared);

// 5. Start
HttpServer.Start;
```

### Creating REST Client
```pascal
// 1. Choose transport
Client := TRestHttpClientWebsockets.Create(
  'localhost', '8080', TOrmModel.Create([TOrmUser])
);

// 2. Authenticate
if not Client.SetUser('username', 'password') then
  raise Exception.Create('Auth failed');

// 3. Use ORM via interface
Client.Orm.Retrieve(ID, User);

// 4. Use services
Client.Services.Resolve(IMyService, MyService);
```

### HTTP Server Modes (TRestHttpServerUse)
| Mode | Best For | Scaling | Notes |
|------|----------|---------|-------|
| `useHttpAsync` | Production | Excellent | Event-driven, many connections |
| `useHttpSocket` | Behind proxy | Good | Thread-per-connection, HTTP/1.0 |
| `useHttpApi` | Windows only | Excellent | Kernel-mode (HTTP.SYS) |
| `useBidirAsync` | WebSockets | Excellent | Event-driven WS |
| `useBidirSocket` | WebSockets | Good | Thread-per-client WS |

### Threading Models Configuration
```pascal
// Configure execution modes (TRestServerAcquireMode)
Server.AcquireWriteMode := amBackgroundOrmSharedThread; // ORM writes in background
Server.AcquireExecutionMode[execSoaByInterface] := amLocked; // Interface calls mutex-protected

// amUnlocked        - No locking (read-only, thread-safe ops)
// amLocked          - Mutex-protected (default for writes)
// amBackgroundThread - Queued to background thread
// amMainThread      - Queued to main thread (GUI apps)
```

### Proper Shutdown Sequence
```pascal
// CRITICAL: Free in this order to avoid memory leaks
FreeAndNil(HttpServer);  // 1. HTTP wrapper first
FreeAndNil(Server);      // 2. REST server
FreeAndNil(Model);       // 3. Model last
```

## AI Guidelines

> **Critical rules for code generation/modification**

- ⚠️ **Abstract classes**: Never instantiate `TRest` or `TRestServerUriContext` directly (use `TRestServer`/`TRestClient` descendants)
- ⚠️ **ORM access**: Always use `TRest.Orm` interface, not direct inheritance
- ⚠️ **Routing schemes**: Client and server must use matching routing classes (`TRestServerRoutingRest` ↔ `TRestClientRoutingRest`)
- ⚠️ **Memory leaks**: Free `TRestHttpServer` before `TRestServer` before `TOrmModel` (reverse creation order)
- ⚠️ **Authentication timing**: Call `ServiceDefine()` before `CreateMissingTables()`
- ✅ **Production servers**: Use `useHttpAsync` for best scaling (not `useHttpSocket` unless behind proxy)
- ✅ **Security**: Always use `secTLS` in production (not deprecated `secSynShaAes`)
- ✅ **Interface methods**: Require `{$M+}` directive on interface declarations

## Common Issues

### "Unexpected X.UriComputeRoutes" Error
**Cause**: Using abstract `TRestServerUriContext` directly.
**Fix**: Use `TRestServerRoutingRest` or `TRestServerRoutingJsonRpc` as ServicesRouting.

### Services Not Found (404)
**Check**:
1. `ServiceDefine()` called before `CreateMissingTables()`?
2. Correct routing class set via `ServicesRouting`?
3. Router recomputed via `ComputeRoutes()` if changed dynamically?
4. Client and server using matching routing schemes?

### Interface Method Not Called
**Check**:
1. Interface registered with `{$M+}` directive?
2. Method has exactly matching signature on client/server?
3. Authentication successful (`Client.SessionUser <> nil`)?
4. Not blocked by authentication/authorization rules?

### Memory Leaks
**Common mistakes**:
- Not freeing `TRestHttpServer` before `TRestServer`
- Circular references via `IRestOrm` interface (use weak references)
- Services with `sicClientDriven` not freed by client
- Async redirect callbacks not freed before server destruction

## Testing Strategies

### Unit Testing
Use `TRestServerFullMemory` for fast isolated tests:
```pascal
Server := TRestServerFullMemory.Create(Model);
try
  // No HTTP overhead, pure in-memory
  Server.Orm.Add(TestData);
  // ... assertions
finally
  Server.Free;
end;
```

### Integration Testing
Use actual HTTP transport with random port:
```pascal
Server := TRestServerDB.Create(Model, ':memory:'); // SQLite in-memory
HttpServer := TRestHttpServer.Create('0', [Server], '+', useHttpAsync);
ActualPort := HttpServer.Port; // Get assigned port
```

### Load Testing
- Prefer `useHttpAsync` for server mode
- Monitor via `Server.Stats.NotifyOrm/NotifyService`
- Use multiple client instances for concurrency
- Check `ServiceRunningContext.RunningThread` for thread distribution

## Files Organization

```
mormot.rest.core.pas       # TRest foundation (195KB)
mormot.rest.server.pas     # TRestServer, routing, auth (338KB)
mormot.rest.client.pas     # TRestClientUri, auth schemes (128KB)
mormot.rest.http.server.pas # HTTP/WebSocket server transport (72KB)
mormot.rest.http.client.pas # HTTP/WebSocket client transport (47KB)
mormot.rest.memserver.pas  # In-memory standalone server (26KB)
mormot.rest.sqlite3.pas    # SQLite3 database integration (16KB)
mormot.rest.mvc.pas        # Model-View-Controller support (20KB)
```

**Project Files**: `/mnt/w/mORMot2/src/rest/`

## Notes

**mORMot 2 vs mORMot 1**:
- Units renamed: `TSQLRest` → `TRest`, `TSQLRecord` → `TOrm`
- Composition over inheritance: `TRest.Orm` vs. direct `TRestOrm` parent
- Enhanced async/event-driven servers (`useHttpAsync`, `useBidirAsync`)
- Not compatible with mORMot 1 - requires code refactoring

**Platform Support**:
- Windows: All features including HTTP.SYS (`useHttpApi`)
- Linux/BSD/macOS: Socket and async modes only
- Mobile (Delphi): Client-side only

**Dependencies**: Requires `mormot.core.*`, `mormot.orm.*`, `mormot.soa.*`, `mormot.net.*` (for HTTP transport)

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
