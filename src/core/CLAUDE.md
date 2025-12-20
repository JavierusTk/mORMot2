# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mORMot2/src/core` folder contains the foundational units of the mORMot 2 framework - cross-platform, cross-compiler "bricks" that provide low-level text, JSON, binary processing, RTTI abstraction, OS/platform abstraction, memory management, compression, threading, and logging.

All higher-level features (ORM, SOA, database access) build on these 24 core units. This is **performance-critical code** used in production by thousands of applications - every optimization matters.

**When to use**: Any feature requiring cross-compiler compatibility (FPC/Delphi), high-performance processing (JSON, binary), or OS abstraction (threading, files, console).

## Dependency Chain (Critical)

Units have **strict layered dependencies** - lower layers never depend on higher ones:

```
mormot.core.base        ← Foundation: RawUtf8, TDynArray, SynLZ (zero deps)
  → mormot.core.os      ← OS abstraction: TSynLocker, file/console APIs
    → mormot.core.unicode ← Charset conversions: Utf8ToString, WinAnsi
      → mormot.core.text  ← Text parsing: CSV, formatting, numbers
        → mormot.core.datetime ← ISO-8601, TTimeLog, TUnixTime
          → mormot.core.rtti   ← Cross-compiler RTTI: TRttiCustom, Rtti.*
            → mormot.core.buffers ← Compression, Base64, streams
              → mormot.core.data   ← TDynArray wrappers, binary serialization
                → mormot.core.json ← JSON (900MB/s): TJsonWriter, TDocVariant
                  → [higher: variants, collections, log, threads, interfaces, test]
```

## Core Units Quick Reference

| Unit | Purpose | Key Classes/Functions |
|------|---------|----------------------|
| `mormot.core.base` | RTL types, zero dependencies | `RawUtf8`, `PtrInt`, `TDynArray`, SynLZ compression |
| `mormot.core.os` | OS abstraction layer | `TSynLocker`, `TSynEvent`, file/console/process APIs |
| `mormot.core.rtti` | Cross-compiler RTTI | `TRttiCustom`, `PRttiInfo`, `Rtti.RegisterType()` |
| `mormot.core.json` | High-perf JSON (900MB/s) | `TJsonWriter`, `GetJsonField()`, `TDocVariant` |
| `mormot.core.data` | Dynamic arrays + binary | `TDynArrayHashed`, `BinarySave/Load()`, `TRawUtf8List` |
| `mormot.core.text` | Text parsing/formatting | `FormatUtf8()`, CSV, number conversions |
| `mormot.core.variants` | Variant extensions | `TDocVariant`, `IDocList`, `IDocDict` |
| `mormot.core.log` | Production logging | `TSynLog`, `ISynLog` (RAII), async file writes |
| `mormot.core.threads` | Threading utilities | `TSynBackgroundThread`, `TSynParallelProcess` |

**RTTI Pattern** (avoid `TypInfo.pas` directly):
```pascal
// ✅ DO: Use mormot.core.rtti (cross-compiler safe)
var rt: TRttiCustom;
rt := Rtti.RegisterType(TypeInfo(TMyRecord));  // Cached, thread-safe

// ❌ DON'T: Direct TypInfo.pas (FPC/Delphi incompatible)
var info: PTypeInfo;  // Different layouts per compiler
```

## SAD References

**📖 Main Documentation**: [Software Architecture Design - mORMot 2](/mnt/w/mORMot2/DOCS/README.md)

| Task | SAD Chapter(s) | Notes |
|------|----------------|-------|
| Understanding unit layering | [Chapter 3: Framework Architecture](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-03.md) | Dependency order critical |
| Working with dynamic arrays | [Chapter 4.5: Data Structures](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-04.md) | TDynArray, TRawUtf8List |
| JSON parsing/generation | [Chapter 4.4: JSON Processing](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-04.md) | TJsonWriter, GetJsonField |
| RTTI abstraction usage | [Chapter 4.3: RTTI](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-04.md) | TRttiCustom cache |
| Threading utilities | [Chapter 4.8: Threading](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-04.md) | Thread pools, locks |
| Production logging | [Chapter 4.7: Logging](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-04.md) | TSynLog, ISynLog |
| Cross-platform file I/O | [Chapter 4.2: OS Abstraction](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-04.md) | mormot.core.os |
| Configuration defines | [Chapter 3.4: Build Configuration](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-03.md) | mormot.defines.inc |

## Quick Patterns

### Performance-Critical Design (NOT in SAD)

- **No memory allocation in hot paths**: JSON parsing uses `PUtf8Char` pointers into input buffer
- **Aggressive inlining**: Most functions marked `inline` for zero-call overhead
- **CPU-specific asm**: x86_64/i386 optimizations in `mormot.core.base.asm*.inc`
- **Lookup tables**: See `TJsonTokens`, `TJsonCharSet` for branch-less parsing

### FPC Memory Managers (NOT in SAD)

```pascal
// Default: RTL MM (single-threaded optimized)
// Include in program .dpr via {$I mormot.uses.inc}

// FPC_X64MM: Multi-threaded MM (x86_64 only)
//   - FastMM4-based algorithms
//   - Lockless bins for tiny blocks (<=128/256 bytes)
//   - Use FPCMM_SERVER mode for multi-threaded servers

// FPC_LIBCMM: System malloc/free wrapper
```

### Configuration Defines (Quick Ref)

| Define | Purpose | When to Use |
|--------|---------|-------------|
| `PUREMORMOT2` | No mORMot 1.18 compatibility | New projects |
| `FPC_X64MM` | Custom x64 memory manager | FPC multi-threaded servers |
| `FPCMM_BOOST` / `FPCMM_SERVER` | MM threading modes | Tune for workload |
| `NEWRTTINOTUSED` | Exclude Delphi 2010+ enhanced RTTI | Smaller EXE |
| `NOPATCHVMT` / `NOPATCHRTL` | Disable runtime patches | Compatibility issues |

**Note**: Set in project options, not in unit source.

## AI Guidelines

- ⚠️ **Dependency Order**: Never break layer hierarchy (see diagram above). Adding circular dependency breaks compilation.
- ⚠️ **Cross-Compiler**: Code must work on **both FPC and Delphi** (Delphi 7-12.2, FPC 3.2+). Use `.inc` files for compiler-specific code, not `{$ifdef}` clutter.
- ⚠️ **RTTI Abstraction**: Never use `TypInfo.pas` directly. Use `mormot.core.rtti` types (`PRttiInfo`, `TRttiCustom`, `Rtti.*` methods) for cross-compiler compatibility.
- ⚠️ **Performance**: This is library code used by thousands. Profile before optimizing, but every cycle counts in hot paths.
- ✅ **Testing**: Changes to core affect all framework users. Regression tests in `/mnt/w/mORMot2/test/test.core.*.pas` are mandatory.
- ✅ **OS Abstraction**: Always prefer `mormot.core.os` functions over direct RTL calls (`SysUtils`, `Windows`, `Linux`, `Unix`) for portability.
- ✅ **Asm Optimization**: Always provide Pascal fallback. Asm is optimization, not requirement. Test on multiple CPUs.

## Files Organization

```
mormot.core.*.pas          # Main unit implementations (24 units)
mormot.core.*.inc          # Compiler/platform-specific code
  .rtti.fpc.inc / .rtti.delphi.inc   # RTTI abstraction
  .os.posix.inc / .os.windows.inc    # OS abstraction
  .base.asmx64.inc / .base.asmx86.inc # CPU optimizations
mormot.defines.inc         # Global conditional defines
mormot.uses.inc            # Memory manager selection (FPC)
```

**Project Files**: `/mnt/w/mORMot2/test` (tests), `/mnt/w/mORMot2/packages` (packages)
**Note**: Core units are library code only (no executables here).

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
