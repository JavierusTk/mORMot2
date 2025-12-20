# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mormot.soa.*` units implement Interface-based Service-Oriented Architecture (SOA) for mORMot 2. This allows defining services as Pascal interfaces that can be consumed remotely via JSON/REST, with automatic client stub generation and bidirectional WebSocket communication.

**When to use**: Business logic layers, DDD services, remote API access, microservices architecture. For low-level endpoints or file uploads, consider method-based services (TRest.ServiceMethodRegister) instead.

**Key advantage**: Type-safe, compiler-checked service contracts with automatic client code generation. Reduces boilerplate and eliminates JSON parsing errors.

## Service Architecture

```
IInvokable (interface - implement your service here)
│
├─ Client-side: TServiceFactoryClient
│  └─ Generates fake implementation (JSON marshalling)
│     └─ Calls over HTTP to server
│
└─ Server-side: TServiceFactoryServer
   └─ Manages real instances
      ├─ Lifecycle control (sicSingle, sicShared, etc.)
      ├─ Authorization rules (Allow, Deny, DenyAll)
      └─ Execution logging (TOrmServiceLog)
```

## Service Instance Patterns (Lifecycle)

| Pattern | Instances | Thread Safe? | Use When |
|---------|-----------|--------------|----------|
| `sicSingle` | New per call | Not required | Stateless, safest |
| `sicShared` | One for all calls | **REQUIRED** | High performance, shared cache |
| `sicClientDriven` | Per client (stateful) | Not required | Client-specific state |
| `sicPerSession` | Per authenticated session | **REQUIRED** | Session-bound data |
| `sicPerUser` | Per user across sessions | **REQUIRED** | User-scoped data |
| `sicPerThread` | Per calling thread | Not required | Thread-local state |

**Dependency Injection Pattern** (CRITICAL for shared services):

```pascal
// ✅ DO: Implement DI for thread-safe shared instances
type
  TCacheService = class(TInjectableObjectRest, ICacheService)
  private
    fLock: TRTLCriticalSection;
    fCache: TSynDictionary;
  public
    constructor Create; override;
    function Get(const Key: RawUtf8): Variant;  // Thread-safe via lock
  end;

// Register as sicShared (one instance, all threads)
Server.ServiceRegister(TCacheService, [TypeInfo(ICacheService)], sicShared);

// ❌ DON'T: Shared instance without thread safety
type
  TBadService = class(TInjectableObjectRest, IBadService)
  public
    fData: TStringList;  // NOT thread-safe!
  end;

// This will crash under concurrent load
Server.ServiceRegister(TBadService, [TypeInfo(IBadService)], sicShared);
```

## SAD References

**Main Documentation**: [Software Architecture Design - mORMot 2](/mnt/w/mORMot2/DOCS/README.md)

| Task | SAD Chapter(s) | Notes |
|------|----------------|-------|
| Define service interfaces | [Chapter 14: Interface-Based Services](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-14.md) | Interface RTTI, contracts |
| Client/Server registration | [Chapter 15: Client-Server Services](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-15.md) | TServiceFactory usage |
| Authorization & logging | [Chapter 15: Client-Server Services](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-15.md) | Per-method security |
| WebSocket callbacks | [Chapter 16: Advanced Service Features](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-16.md) | Bidirectional interfaces |
| Code generation | [Chapter 16: Advanced Service Features](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-16.md) | Mustache templates |
| Architecture overview | [Chapter 14: Interface-Based Services](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-14.md) | Core/Client/Server layers |

## Quick Patterns

> **Note**: Only unique content NOT covered in SAD belongs here.

### Service Instance Implementation Patterns

Choose based on state requirements and performance trade-offs:

| Pattern | Description | Thread Safety | Use Case |
|---------|-------------|---------------|----------|
| `sicSingle` | New instance per call (default) | Not required | Stateless services, safest option |
| `sicShared` | One instance for all calls | **Required** | Fastest, shared caches/resources |
| `sicClientDriven` | Instance tied to client lifetime | Not required | Stateful client sessions |
| `sicPerSession` | One instance per authenticated session | **Required** | User-specific state across calls |
| `sicPerUser` | One instance per user across sessions | **Required** | Cross-session user resources |
| `sicPerGroup` | One instance per user group | **Required** | Group-level shared state |
| `sicPerThread` | One instance per calling thread | Not required | Thread-local resources |

**Key Insight**: `sicSingle` is safest but slowest. `sicShared` is fastest but requires thread-safe implementation. See SAD Ch 14 for lifecycle details.

### Method-Based vs Interface-Based Services

| Aspect | Method-Based | Interface-Based (this folder) |
|--------|--------------|-------------------------------|
| Definition | Class methods on `TRest` | Pascal `interface` types |
| Client code | Manual implementation | Auto-generated stubs |
| Performance | Fastest (direct HTTP access) | Slightly slower (JSON marshalling) |
| Type safety | Low (manual JSON parsing) | High (compiler-checked) |
| WebSockets | Not supported | Built-in bidirectional |
| Best for | Low-level endpoints, file uploads | Business logic, DDD services |

**Recommendation**: Use interface-based services for 95% of use cases. Only drop to method-based for performance-critical endpoints or non-standard HTTP operations.

### Contract Validation Pattern

```pascal
// Server - explicit version control
Server.ServiceRegister(TMyService, [TypeInfo(IMyService)], sicShared)
  .ContractExpected := 'v2.5';

// Client - must match server contract
Client.ServiceRegister([TypeInfo(IMyService)], sicShared, 'v2.5');
```

**Default contract**: MD5 hash of interface methods + parameter types (automatic). Custom contracts provide explicit compatibility control.

### Authorization Pattern

```pascal
Factory := Server.Services.Info(ICalculator) as TServiceFactoryServer;

// Deny all by default (secure by default)
Factory.DenyAll;

// Whitelist specific groups
Factory.Allow(ICalculator, [ADMIN_GROUP_ID, MANAGER_GROUP_ID]);

// Blacklist specific methods for certain groups
Factory.Deny(ICalculator, 'DeleteAll', [MANAGER_GROUP_ID]);
```

States: `idAllowAll` (default) → `idDenyAll` → `idAllowed` → `idDenied`. See SAD Ch 15 for TAuthGroup integration.

### Execution Logging Pattern

```pascal
// Enable automatic call logging to database
Factory.SetServiceLog(RestServer, TOrmServiceLog);

// Logs capture: method name, parameters (JSON), execution time, user/session/IP
```

`TOrmServiceLog` stores statistics for performance tuning and debugging. `TOrmServiceNotifications` extends this for async notification patterns.

## AI Guidelines

> **Critical rules for code generation/modification**

- **Thread Safety**: `sicShared`, `sicPerSession`, `sicPerUser`, `sicPerGroup` require thread-safe implementations (use locks/immutable data)
- **Contract Compatibility**: Client/server contracts must match or connection is rejected (validate after interface changes)
- **Dependency Injection**: Use `TInjectableObjectRest` base class for automatic dependency resolution via Server property
- **WebSockets**: Callback interfaces as method parameters enable bidirectional communication (see SAD Ch 16)
- **Performance**: Prefer `sicShared` for read-heavy workloads, `sicSingle` for write-heavy (no contention)
- **Authorization**: Always use `DenyAll` + `Allow` pattern for security-critical services (whitelist approach)
- **Testing**: Use `TRestServerFullMemory` + `TRestClientUri` for in-memory unit testing (see Testing pattern below)

## Files Organization

```
mormot.soa.core.pas      # Base types, contracts, TServiceFactory abstraction
mormot.soa.client.pas    # Fake interface stubs, JSON marshalling
mormot.soa.server.pas    # Instance management, authorization, logging
mormot.soa.codegen.pas   # Mustache-based code generation, async conversion
```

**Project Files**: `/mnt/w/mORMot2/src/soa/` (4 core units)

**Dependencies**: `mormot.core.interfaces` (RTTI), `mormot.core.json` (serialization), `mormot.orm.*` (logging), `mormot.rest.*` (transport)

## Testing Services

Typical testing approach (no dedicated test infrastructure in this folder):

```pascal
// 1. Create in-memory server
Server := TRestServerFullMemory.Create(Model);

// 2. Register service
Server.ServiceRegister(TServiceCalculator, [TypeInfo(ICalculator)], sicShared);

// 3. Create client
Client := TRestClientUri.Create(Server);

// 4. Register client interface
Client.ServiceRegister([TypeInfo(ICalculator)], sicShared);

// 5. Resolve and call
if Client.Services.Resolve(ICalculator, I) then
  Result := I.Add(10, 20);

// 6. Validate results and database side effects
```

See `/mnt/w/mORMot2/test/` for regression tests.

## Notes

**Key Takeaways**:
1. Contracts prevent version mismatches - always validate client/server compatibility
2. Instance patterns affect scalability - choose based on state requirements
3. Authorization is per-method - fine-grained security control
4. Logging is optional but valuable - enable for performance tuning
5. DI works out of the box - use `TInjectableObjectRest` base class
6. WebSockets enable callbacks - interface parameters become bidirectional channels
7. Code generation reduces boilerplate - Mustache templates generate clients automatically

**Version**: mORMot 2.3+ (trunk)

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
