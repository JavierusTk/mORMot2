# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mORMot2/src/tools` folder contains command-line utilities built on the mORMot 2 framework. Each tool is a standalone executable targeting specific use cases: cryptography, service management, HTTP downloads, debugging, and code generation.

**Key Characteristic**: These are end-user applications, not library code. Each tool follows a consistent architecture and build pattern.

**When to use**: Reference these when building mORMot-based CLI tools, or use them directly for cryptography (ecc), service management (agl), HTTP downloads (mget), debugging symbols (mab), or OpenAPI code generation (mopenapi).

## Tool Architecture

### Common Structure
All tools follow a consistent pattern:
- **Entry point**: `<tool>.dpr` (console application)
- **Implementation**: Either inline in `.dpr` or separate `mormot.tools.<tool>.pas`
- **Dependencies**: Relative paths to mORMot core units (e.g., `..\..\core\mormot.core.base.pas`)
- **Cross-platform**: Windows (console), Linux, macOS support

### Build Requirements
- **Delphi**: RAD Studio (uses `.dproj` when available)
- **FPC**: Uses `.lpi` project files (Free Pascal Compiler)
- **Compiler directives**: `{$I ..\..\mormot.defines.inc}` and `{$I ..\..\mormot.uses.inc}`

## Quick Patterns

### Tool Reference

#### ecc - Public Key Cryptography Tool
**Location**: `/mnt/w/mORMot2/src/tools/ecc/`
**Implementation**: `mormot.tools.ecc.pas` (1,000+ lines)

**Key Operations**:
- Key management: `new`, `rekey`, `source`, `infopriv`
- Signing: `sign`, `verify` (ECDSA digital signatures)
- Encryption: `crypt`, `decrypt`, `infocrypt` (ECIES encryption)
- Symmetric: `aeadcrypt`, `aeaddecrypt`
- Certificate chains: `chain`, `chainall`
- Password manager: `cheatinit`, `cheat`

**Architecture**:
- Uses `TEccCertificate` for key pair management
- Password-protected private keys with configurable PBKDF2 rounds
- `.synecc` format for encrypted files with embedded metadata
- Certificate chaining produces `.ca` JSON arrays

#### agl (Angelize) - Cross-Platform Services Manager
**Location**: `/mnt/w/mORMot2/src/tools/agl/`
**Implementation**: Uses `mormot.app.agl.pas` and `mormot.app.daemon.pas`

**Key Features**:
- Main service (`TSynAngelize`) manages sub-processes
- JSON configuration files per sub-service
- Leveled startup (dependency rings)
- Auto-restart on crash with backoff
- HTTP/HTTPS health check validation
- Console output redirection
- HTML status page generation

**Commands**:
- `/install`, `/start`, `/stop`, `/uninstall` - Service lifecycle
- `/run`, `/verbose` - Console mode for testing
- `/list` - Current status of all sub-processes
- `/retry` - Force restart of failed service
- `/new` - Create new service config
- `/settings` - Validate JSON configuration

**Architecture**:
- `TSynAngelize` extends `TSynDaemon` (base daemon class)
- Thread-safe process monitoring with watchdog timers
- Exit code handling (fatal vs. retryable errors)
- Graceful shutdown (`WM_QUIT`/`SIGTERM`) with fallback kill

#### mget - HTTP/HTTPS Downloading Tool
**Location**: `/mnt/w/mORMot2/src/tools/mget/`
**Implementation**: `mormot.tools.mget.pas`

**Key Features**:
1. **Resume Downloads**: Uses `RANGE` headers, `.part` extension for incomplete files
2. **Hash Verification**: MD5/SHA1/SHA256 validation (SHA-NI acceleration on x64)
   - Download hash from `<url>.sha256` or specify on command line
3. **Peer-to-Peer Cache**: LAN-based file sharing to reduce WAN bandwidth
   - UDP broadcast discovery (AES-GCM-128 authenticated)
   - Local HTTP cache server with bearer authentication
   - Hash-based storage prevents tampering

**Common Usage**:
```bash
mget https://someuri/some/file.ext                        # Basic download with resume
mget 4544b3...68ba7a@http://someuri/some/file.ext        # With hash verification
mget https://someuri/some/file.ext --hashAlgo sha256     # Download hash first
mget --prompt                                             # Interactive mode
mget --prompt --peer                                      # With PeerCache enabled
```

**Architecture**:
- Built on `THttpClientSocket.WGet()` from `mormot.net.client`
- PeerCache uses `THttpPeerCache` from `mormot.net.server`
- Secret-derived AES-GCM for UDP/HTTP authentication
- IP banning for invalid requests (DOS protection)

#### mab - Debugging Symbol Converter
**Location**: `/mnt/w/mORMot2/src/tools/mab/`
**Implementation**: Inline in `mab.dpr`

**Purpose**: Convert debugging symbols to `.mab` format for stack trace resolution

**Operations**:
- Input: `.map` (Delphi) or DWARF (FPC) debugging info
- Output: `.mab` binary format (mORMot Advanced Binding)
- Can embed `.mab` into executables (`.exe`, `.dll`, `.bpl`, `.ocx`)

**Usage**:
```bash
mab *.map              # Convert all .map files to .mab
mab myapp.exe          # Process myapp.map and embed into myapp.exe
mab somefile.map       # Create somefile.mab
```

**Architecture**:
- Validates `.map` is newer than executable before processing
- Embeds `.mab` as resource or appended data
- Used by mORMot logging for stack trace symbolication

#### mopenapi - OpenAPI/Swagger Code Generator
**Location**: `/mnt/w/mORMot2/src/tools/mopenapi/`
**Implementation**: Uses `mormot.net.openapi.pas`

**Purpose**: Generate Delphi/FPC client `.pas` units from OpenAPI/Swagger `.json` specifications

**Usage**:
```bash
mopenapi --help
mopenapi swagger.json PetStore
mopenapi OpenApiAuth.json /concise
mopenapi test.json --options=DtoNoExample,DtoNoPattern
```

**Architecture**:
- RTTI-based command-line parsing (`TOptions` class)
- `TOpenApiParserOptions` for generation customization
- Outputs type-safe Pascal interfaces and DTOs

### Common Includes Pattern

```pascal
{$I ..\..\mormot.defines.inc}

{$ifdef OSWINDOWS}
  {$apptype console}
  {$R ..\..\mormot.win.default.manifest.res}
{$endif OSWINDOWS}

uses
  {$I ..\..\mormot.uses.inc}
  classes,
  sysutils,
  mormot.core.base         in '..\..\core\mormot.core.base.pas',
  mormot.core.os           in '..\..\core\mormot.core.os.pas',
  // ... more units
```

### Console Application Framework

Built on `mormot.app.console.pas` (`TConsoleApplication` base class):
- Command-line parsing with switches
- Help text generation
- Error handling and exit codes
- RTTI-based option parsing

### Key Architectural Stacks

```
Cryptography Stack (ecc):
mormot.crypt.ecc256r1.pas → mormot.crypt.ecc.pas → mormot.tools.ecc.pas → ecc.dpr

Service Management Stack (agl):
mormot.app.daemon.pas → mormot.app.agl.pas → agl.dpr

HTTP Stack (mget):
mormot.net.client.pas + mormot.net.server.pas → mormot.tools.mget.pas → mget.dpr
```

## AI Guidelines

- ⚠️ **Adding new tools**:
  1. Create subdirectory: `/mnt/w/mORMot2/src/tools/<toolname>/`
  2. Create `<toolname>.dpr` with standard header and relative includes
  3. Implement in separate `.pas` file if complex (e.g., `mormot.tools.<toolname>.pas`)
  4. Update this `README.md` with tool description
  5. Add both Delphi (`.dproj`) and FPC (`.lpi`) project files if needed
- ⚠️ **Logging**: All tools use `mormot.core.log.pas` for structured logging (console output + optional log files).
- ⚠️ **Security considerations**:
  - **ecc**: Private keys encrypted with PBKDF2-derived passwords, configurable iteration rounds (default: 60,000), secure key deletion (memory wiping), certificate expiration validation
  - **mget PeerCache**: Shared secret required, AES-GCM-128 encryption/authentication for UDP/HTTP, IP banning after invalid requests, optional HTTPS for local transfers, cache requires proper filesystem ACLs
  - **agl**: Runs as SYSTEM/root by default, JSON config files should have restricted permissions, watchdog prevents runaway processes
- ⚠️ **Performance notes**: Hash calculation uses SHA-NI hardware acceleration on modern x64 CPUs (mget, ecc), ECC operations use optimized secp256r1 curve implementation, HTTP transfers are streaming (no full file buffering), PeerCache local network transfers avoid WAN bottleneck
- ✅ **Testing**: Each tool should be tested with `<tool> --help` or `/<tool> /help`, basic operations with sample files, error conditions (missing files, invalid input), cross-platform builds (Windows, Linux, macOS).
- ✅ **Integration**: Tools can be tested via `mormot.test.*.pas` units in `/mnt/w/mORMot2/test/`
- ✅ **Cross-platform**: All tools support Windows, Linux, macOS

## Files Organization

```
/mnt/w/mORMot2/src/tools/
├── ecc/
│   ├── ecc.dpr                  # Entry point
│   ├── mormot.tools.ecc.pas     # Implementation (1,000+ lines)
│   └── ecc.dproj / ecc.lpi      # Project files
├── agl/
│   ├── agl.dpr                  # Entry point
│   └── agl.dproj / agl.lpi      # Project files
├── mget/
│   ├── mget.dpr                 # Entry point
│   ├── mormot.tools.mget.pas    # Implementation
│   └── mget.dproj / mget.lpi    # Project files
├── mab/
│   ├── mab.dpr                  # Entry point (inline implementation)
│   └── mab.dproj / mab.lpi      # Project files
└── mopenapi/
    ├── mopenapi.dpr             # Entry point
    └── mopenapi.dproj           # Project file
```

**Dependencies**:
- mORMot 2 core framework (`mormot.core.*`)
- Application framework (`mormot.app.console`, `mormot.app.daemon`)
- Network layer (`mormot.net.client`, `mormot.net.server`)
- Cryptography (`mormot.crypt.*`)

**Related Documentation**: [SAD Chapter 26: Source Code](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-26.md) - Repository structure, static libraries setup, cross-platform compilation guidelines

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
