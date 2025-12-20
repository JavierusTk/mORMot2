# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mORMot2/src/misc` folder contains specialized binary file format parsers that are too specific for the core framework but not tied to ORM/SOA/MVC features. These are standalone utilities for reading and analyzing file structures.

**Key Characteristic**: Zero dependencies on database, ORM, or networking layers. These are pure binary format parsers.

**When to use**: Cross-platform analysis of Windows executables (PE/COFF), extracting version info, parsing binary file formats. Not for ISO 9660 (incomplete).

## Units Overview

### mormot.misc.pecoff (Production Ready)

**Purpose**: Cross-platform PE (Portable Executable) and COFF file format parser for Windows executables and libraries (.exe, .dll).

**Key Features**:
- Parse PE32 (32-bit) and PE32+ (64-bit) executables
- Extract version information from resources
- Read/parse all PE sections (.text, .data, .reloc, .edata, etc.)
- Digital signature stuffing (hide arbitrary data in executable signatures)
- Cross-platform (works on Linux/macOS to analyze Windows executables)

### mormot.misc.iso (Early Draft - NOT FUNCTIONAL)

**Purpose**: ISO 9660 CD/DVD file system reader (optical disc media format).

**Status**: This unit is just in early draft state - nothing works yet.

**Note**: Do NOT use this unit in production. It only contains type definitions without implementation.

## Quick Patterns

### Extract Version Info

```pascal
uses mormot.misc.pecoff;

var
  info: TDocVariantData;
begin
  info := GetPEFileVersion('C:\Windows\System32\notepad.exe');
  WriteLn(info.ToJSON('', '', jsonHumanReadable));
  WriteLn('Company: ', info.S['CompanyName']);
  WriteLn('Product: ', info.S['ProductName']);
end;
```

### Parse PE Structure

```pascal
var
  pe: TSynPELoader;
begin
  pe := TSynPELoader.Create;
  try
    if pe.LoadFromFile('myapp.exe') then
    begin
      pe.ParseStringFileInfoEntries;
      WriteLn('Architecture: ', pe.ArchitectureName);
      WriteLn('Version: ', pe.FileVersionStr);  // '[major].[minor].[patch].[build]'
      WriteLn('64-bit: ', pe.IsPE64);

      // Get all version info as variant document
      WriteLn('Company: ', pe.StringFileInfoEntries.S['CompanyName']);
    end;
  finally
    pe.Free;
  end;
end;
```

### Digital Signature Stuffing

```pascal
// Hide arbitrary text in executable digital signature
procedure StuffExeCertificate(const MainFile, NewFile: TFileName;
  const Stuff: RawUtf8; UseInternalCertificate: boolean = false);

// Retrieve stuffed text from signature
function FindStuffExeCertificate(const FileName: TFileName): RawUtf8;

// Usage:
StuffExeCertificate('myapp.exe', 'myapp_stuffed.exe', 'License key: ABC123');
WriteLn(FindStuffExeCertificate('myapp_stuffed.exe')); // 'License key: ABC123'
```

### Navigation Methods

```pascal
// TSynPELoader navigation
function GetSectionByName(const AName: RawUtf8): PImageSectionHeader;
function GetSectionByRVA(RVA: cardinal): PImageSectionHeader;
function OffsetFrom(RVA: cardinal): cardinal;

// Properties
property Architecture: TCoffArch;      // caI386, caAmd64, caArm64, etc.
property IsPE64: boolean;
property CoffHeader: PImageFileHeader;
property PE32/PE64: PImageNtHeaders32/64;
property StringFileInfoEntries: TDocVariantData;
```

## AI Guidelines

- ⚠️ **Lifetime management**: Pointers are only valid while `TSynPELoader` is alive. Copy data if persistence is needed.
  ```pascal
  // WRONG - pointer becomes invalid
  function GetVersion: RawUtf8;
  var pe: TSynPELoader;
  begin
    pe := TSynPELoader.Create;
    pe.LoadFromFile('app.exe');
    Result := pe.FileVersionStr; // OK - returns copy
    pe.Free; // Now pe.StringFileInfo is invalid!
  end;
  ```
- ⚠️ **Architecture detection**: Always check `Architecture` property, not just `IsPE64`.
  ```pascal
  case pe.Architecture of
    caI386:   WriteLn('32-bit x86');
    caAmd64:  WriteLn('64-bit x64');
    caArm64:  WriteLn('64-bit ARM');
  end;
  ```
- ⚠️ **Resource parsing**: Must call `ParseResources` before accessing version info.
  ```pascal
  pe.LoadFromFile('app.exe');
  pe.ParseResources; // Required!
  WriteLn(pe.FileVersionStr);
  ```
- ⚠️ **Digital signature stuffing**: Only works with executables that have valid signatures.
  - Use `UseInternalCertificate := true` if OpenSSL not available
  - Stuffed data is hidden in signature, doesn't affect code execution
  - Maximum size depends on certificate structure (typically several KB)
- ⚠️ **Do NOT use mormot.misc.iso**: It's incomplete and non-functional.
- ✅ **Memory management**: Files are memory-mapped via `TMemoryMap` for efficiency. Pointers returned are valid only while the loader object is alive. No automatic memory copies.
- ✅ **Cross-platform**: Parsers work on any platform (Windows/Linux/macOS/FreeBSD) since they only read binary file formats without OS-specific APIs.
- ✅ **Common use cases**:
  - Extract version info from executables programmatically
  - Analyze PE structure (sections, imports, exports, resources)
  - Validate executable signatures
  - Hide metadata/licensing info in signatures without breaking code signing
  - Cross-platform analysis of Windows executables

## Files Organization

```
mormot.misc.pecoff.pas     # PE/COFF parser (production ready)
mormot.misc.iso.pas        # ISO 9660 parser (early draft - NOT functional)
```

**Dependencies**: Minimal dependencies from mORMot core:
- `mormot.core.base` - Basic types, UTF-8, memory mapping
- `mormot.core.os` - File system access
- `mormot.core.unicode` - String conversions (pecoff only)
- `mormot.core.text` - Text utilities (pecoff only)
- `mormot.core.rtti` - Runtime type info (pecoff only)
- `mormot.core.variants` - TDocVariant (pecoff only)

**References**:
- PE/COFF Format: https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
- ISO 9660: ECMA-119 specification

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
