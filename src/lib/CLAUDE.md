# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mORMot2/src/lib` folder contains thin wrappers to external C/stdcall libraries used by the mORMot 2 framework. These are low-level API bindings (Foreign Function Interface) that are then encapsulated in higher-level units throughout the framework.

**Key Principle**: These units provide raw FFI access only. Application code should use the high-level wrapper units instead.

**When to use**: Only when adding new external library bindings or debugging library loading issues. Normal application development uses high-level wrappers in `mormot.core.*`, `mormot.crypt.*`, `mormot.net.*`, etc.

## Architecture Pattern

```
Low-Level (this directory)          High-Level (other src folders)
mormot.lib.z.pas             --->   mormot.core.zip.pas
mormot.lib.openssl11.pas     --->   mormot.crypt.openssl.pas
mormot.lib.curl.pas          --->   mormot.net.client.pas / mormot.net.http.pas
mormot.lib.quickjs.pas       --->   mormot.script.quickjs.pas
mormot.lib.lizard.pas        --->   mormot.core.buffers.pas (TAlgoLizard*)
mormot.lib.gdiplus.pas       --->   mormot.ui.gdiplus.pas
mormot.lib.win7zip.pas       --->   I7zReader/I7zWriter interfaces
```

**Never use `mormot.lib.*` units directly in application code** - they are framework internals.

## Quick Patterns

### Library Linking Strategies

Each library unit supports multiple linking modes via conditionals (defined in `mormot.defines.inc` or unit-specific):

#### zlib (`mormot.lib.z.pas`)
- `ZLIBSTATIC` - Static .o/.obj linking (FPC Win32/Win64, Delphi Win32)
- `ZLIBPAS` - Pure Pascal paszlib (FPC Android, FPC ARM Windows)
- `ZLIBEXT` - Dynamic libz.so (FPC Linux/BSD/Mac)
- `ZLIBRTL` - Delphi RTL's system.zlib.pas (Delphi Win64)
- `LIBDEFLATESTATIC` - Faster libdeflate for in-memory compression (FPC Intel Linux/Win32)

#### OpenSSL (`mormot.lib.openssl11.pas`)
- Dynamic loading by default (libcrypto/libssl .dll/.so/.dylib)
- Supports OpenSSL 1.1.x (deprecated) and 3.x (current)
- **Windows**: SChannel used by default until `OpenSslInitialize()` called
- **POSIX**: Always attempts to load OpenSSL at startup
- **macOS**: System .dylib unstable - use pre-built from https://synopse.info/files/

#### libcurl (`mormot.lib.curl.pas`)
- `LIBCURLSTATIC` - Static libcurl.a (mainly Android)
- Dynamic by default (libcurl.so/.dll)
- `LIBCURLMULTI` - Include advanced "multi session" API

#### QuickJS (`mormot.lib.quickjs.pas`)
- `LIBQUICKJSSTATIC` - Static linking (no external .dll/.so)
- Uses patched https://github.com/c-smile/quickjspp fork
- ES2020 support: modules, async generators, proxies, BigInt

#### Windows-Only Libraries
- `mormot.lib.winhttp.pas` - WinHTTP, WebSockets, http.sys APIs
- `mormot.lib.sspi.pas` - SSPI/SChannel authentication
- `mormot.lib.gdiplus.pas` - GDI+ graphics
- `mormot.lib.win7zip.pas` - 7-Zip DLL wrapper (requires 7z.dll)
- `mormot.lib.uniscribe.pas` - Windows text rendering

#### POSIX-Only Libraries
- `mormot.lib.gssapi.pas` - GSSAPI/Kerberos authentication

### Static Library Files

Static binaries (.o/.obj) are stored in `/mnt/w/mORMot2/static/` and must be downloaded separately:
```bash
# Download and extract to mORMot2/static/
https://synopse.info/files/mormot2static.7z
https://synopse.info/files/mormot2static.tgz
```

The `mormot.lib.static.pas` unit provides:
- Linking constants (`_PREFIX` for symbol names)
- Minimal libc replacement for Windows static linking
- GCC math function wrappers
- FPU exception masking

### Debugging Library Loading

```pascal
// OpenSSL
if not OpenSslIsAvailable then
  raise Exception.Create(OpenSslVersion); // shows error

// libcurl
curl.GlobalInitialize; // raises ECurl if library not found

// QuickJS
{$ifdef LIBQUICKJS}
// code here won't compile if QuickJS unavailable
{$endif}
```

## AI Guidelines

- ⚠️ **Never use lib.* directly**: Application code MUST import high-level wrappers (`mormot.core.zip`, `mormot.crypt.openssl`). These units are framework internals.
- ⚠️ **Platform-specific conditionals**: All library units include `{$I ..\mormot.defines.inc}` which centralizes compiler, OS, CPU, and feature detection. Never override in application code.
- ⚠️ **OpenSSL warnings**:
  - **Windows**: May load obsolete DLLs from system PATH - place correct version in EXE directory
  - **macOS**: System dylibs are unstable - use pre-built binaries
  - **Legal**: Comply with cryptographic software restrictions in your country (see LICENSE.md)
- ⚠️ **7-Zip licensing**: Using `mormot.lib.win7zip.pas` requires documenting use of 7-Zip (LGPL) in your application and linking to www.7-zip.org
- ⚠️ **Memory management**: Most libraries use their own allocators. `OPENSSLUSERTLMM` makes OpenSSL use Pascal RTL memory (risky - OpenSSL leaks).
- ✅ **Adding new libraries**:
  1. Create `mormot.lib.newlib.pas` with raw API declarations
  2. Define linking conditionals (`NEWLIBSTATIC`, `NEWLIBEXT`, etc.)
  3. Add to `mormot.defines.inc` if platform-specific defaults needed
  4. Create high-level wrapper in appropriate `src/` subfolder
  5. Update `src/lib/README.md` with documentation
- ✅ **Cross-compiler compatibility**: These units maintain compatibility with FPC 3.2.x+ and Delphi 7-12.x on Windows, Linux, BSD, macOS (x86/x64/ARM).
- ✅ **Follow existing patterns**: Study `TZLib` record (mormot.lib.z) or `OpenSsl*()` functions (mormot.lib.openssl11) before creating new bindings.

## Files Organization

```
mormot.lib.*.pas           # Raw API bindings (15+ units)
mormot.lib.static.pas      # Static linking support
/mnt/w/mORMot2/static/     # Static binaries (.o/.obj) - download separately
```

**High-Level Wrappers**: See `/mnt/w/mORMot2/src/core/`, `/mnt/w/mORMot2/src/crypt/`, `/mnt/w/mORMot2/src/net/`
**Documentation**: [SAD Chapter 3: Framework Architecture](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-03.md) - Library linking strategies

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
