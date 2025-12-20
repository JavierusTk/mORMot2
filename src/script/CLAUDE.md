# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The mORMot scripting layer provides thread-safe JavaScript engine integration for client and server-side scripting. It enables user-defined workflows, business rules, and hot-reloadable logic without recompilation.

**Current Status**: Abstract layer complete (`mormot.script.core`), low-level QuickJS bindings available (`mormot.lib.quickjs`), high-level wrapper in development (`mormot.script.quickjs`).

**Key Philosophy**: Engine-agnostic design with per-thread isolation, automatic pooling, and hot-reload support.

## QuickJS Integration Quick Ref

| Component | Purpose | Thread-Local? |
|-----------|---------|---------------|
| `TQuickJS` | Runtime engine | **YES - one per thread** |
| `JSValue` | Wrapper type | Bound to parent engine |
| `JSContext` | Execution context | Engine-owned |
| `JSObject` | JavaScript object | Bound to context |
| `JSFunction` | Callable reference | Bound to context |

**Thread Affinity Pattern** (CRITICAL - most common mistake):

```pascal
// ✅ DO: Create engine per-thread
var
  engine: TQuickJS;  // Declare at thread scope
begin
  engine := TQuickJS.Create(nil);  // One engine per thread
  try
    engine.Eval('var x = 42;');
    // Use engine in this thread only
  finally
    engine.Free;
  end;
end;

// ❌ DON'T: Share engine across threads (CRASHES)
var
  gEngine: TQuickJS;  // Global shared

procedure Thread1;
begin
  gEngine.Eval('...');  // Race condition!
end;

procedure Thread2;
begin
  gEngine.Eval('...');  // Both access same JSValue → CRASH
end;
```

**JSValue Memory Management** (Memory leaks are easy):

```pascal
// ✅ DO: Release JSValue results
var result: JSValue := engine.Eval('complex_operation()');
try
  // Use result
  WriteLn(engine.Stringify(result));
finally
  engine.FreeValue(result);  // MUST release
end;

// ❌ DON'T: Leave JSValue unreleased (LEAK)
var result: JSValue := engine.Eval('...');
WriteLn(engine.Stringify(result));
// Forgot to FreeValue → JSValue never released
```

## SAD References

**📖 Main Documentation**: [Software Architecture Design - mORMot 2](/mnt/w/mORMot2/DOCS/README.md)

| Task | SAD Chapter(s) | Notes |
|------|----------------|-------|
| Embed JavaScript execution | [Chapter 22.3](../../../DOCS/mORMot2-SAD-Chapter-22.md#223-quickjs-integration) | QuickJS C API, memory management |
| Thread-safe scripting | [Chapter 22.4](../../../DOCS/mORMot2-SAD-Chapter-22.md#224-thread-safe-engine-management) | TThreadSafeManager pooling |
| Hot-reload scripts | [Chapter 22.5](../../../DOCS/mORMot2-SAD-Chapter-22.md#225-hot-reload-pattern) | ContentVersion tracking |
| Remote debugging | [Chapter 22.6](../../../DOCS/mORMot2-SAD-Chapter-22.md#226-remote-debugging) | Firefox DevTools protocol |
| ORM/SOA integration | [Chapter 22.7](../../../DOCS/mORMot2-SAD-Chapter-22.md#227-mormot-integration-patterns) | Expose native functions to JS |
| Architecture overview | [Chapter 22.2](../../../DOCS/mORMot2-SAD-Chapter-22.md#222-architecture-overview) | Two-layer design |

## Quick Patterns

### Hot Reload Pattern
```pascal
// Increment version when scripts change
Manager.ContentVersion := Manager.ContentVersion + 1;

// Framework auto-detects stale engines
Engine := Manager.ThreadSafeEngine;
if Engine.ContentVersion <> Manager.ContentVersion then
begin
  // Engine was recreated - reload scripts
  LoadScripts(Engine);
  Engine.ContentVersion := Manager.ContentVersion;
end;
```

### QuickJS Memory Management
```pascal
// CRITICAL: Every JSValue must be freed
Result := JS_Eval(Context, ...);
if JSValueRaw(Result).IsException then
  HandleError;
JS_FreeValue(Context, Result);  // Required!

// Cleanup order: Values → Context → Runtime
```

## AI Guidelines

> **Critical rules for code generation/modification**

- ⚠️ **Never instantiate engines directly**: Always use `TThreadSafeManager.ThreadSafeEngine` (pooling required)
- ⚠️ **Thread affinity is strict**: One engine per thread, never shared across threads
- ⚠️ **FPU corruption prevention**: Always use `ThreadSafeCall()` for script execution (saves/restores FPU state)
- ⚠️ **QuickJS memory**: Every `JSValue` (except constants) must be freed with `JS_FreeValue()`
- ✅ **Low-level API available**: Use `mormot.lib.quickjs` for direct QuickJS C API access (fully functional)
- ✅ **Hot reload is manual**: Application must increment `Manager.ContentVersion` when scripts change
- ✅ **Debugging uses Firefox protocol**: Not Chrome DevTools (port 6000 default)
- ✅ **Set expiration for production**: `EngineExpireTimeOutMinutes := 240` prevents JS memory leaks (4 hours recommended)

## Files Organization

```
mormot.script.core.pas        # Abstract base classes (TThreadSafeEngine, TThreadSafeManager)
mormot.script.quickjs.pas     # High-level QuickJS wrapper (in development)
mormot.lib.quickjs.pas        # Low-level QuickJS C API bindings (fully functional)
```

**Project Files**: `/mnt/w/mORMot2/packages/`

## Notes

**QuickJS Conditional**: Requires `{$define LIBQUICKJSSTATIC}` in project options. Static libraries in `/mnt/w/mORMot2/static/` (libquickjs.a/obj).

**SpiderMonkey**: Planned for high-performance server scenarios (not yet implemented).

**Implementation Warning**: High-level APIs in SAD Ch 22.7-22.8 (RegisterFunction, custom variants) are conceptual/planned. Current production code must use low-level QuickJS C API directly (see SAD Ch 22.3).

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
