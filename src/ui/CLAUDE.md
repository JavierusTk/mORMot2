# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The `mORMot2/src/ui` folder contains **User Interface-related units** for VCL/LCL applications on Windows. These units provide cross-platform compatibility abstractions, custom visual components, ORM grid integration, GDI+ graphics support, and a complete PDF generation and reporting engine.

**Key Characteristic**: Most units are **Windows-only** or have limited cross-platform support. Units gracefully degrade to no-ops on POSIX systems.

**When to use**: Building Delphi/FPC GUI applications with ORM integration, PDF generation, or custom reporting. Not suitable for server-side or headless applications.

## Architecture

### Unit Organization

```
mormot.ui.core       → Foundation (LCL/VCL compatibility, UI helpers)
     ↓
mormot.ui.controls   → Custom components (hints, labeled edits, JSON persistence)
mormot.ui.grid.orm   → ORM → Grid binding (TOrmTableToGrid)
mormot.ui.gdiplus    → GDI+ wrapper (TSynPicture: GIF/PNG/TIFF/JPG)
mormot.ui.pdf        → PDF engine (TPdfDocument, TPdfPage)
mormot.ui.report     → Report engine (TGdiPages: preview + PDF export)
```

### Platform Support Matrix

| Unit                | Windows (Delphi) | Windows (FPC) | POSIX (FPC) |
|---------------------|------------------|---------------|-------------|
| mormot.ui.core      | ✅ Full          | ✅ Full       | ⚠️ Limited  |
| mormot.ui.controls  | ✅ Full          | ✅ Full       | ⚠️ Limited  |
| mormot.ui.grid.orm  | ✅ Full          | ✅ Full       | ⚠️ Limited  |
| mormot.ui.gdiplus   | ✅ Full          | ✅ Full       | ❌ No-op    |
| mormot.ui.pdf       | ✅ Full          | ✅ Full       | ❌ No-op    |
| mormot.ui.report    | ✅ Full          | ❌ No-op      | ❌ No-op    |

**POSIX Behavior**: Units compile but provide minimal/no functionality (graceful degradation)

## Quick Patterns

### VCL/LCL Cross-Compatibility

**Problem**: Free Pascal's LCL uses different type names and message constants than Delphi's VCL.

**Solution** (`mormot.ui.core`):
```pascal
{$ifdef FPC}
type
  TWMTimer = TLMTimer;
const
  WM_TIMER = LM_TIMER;
{$endif}
```

**FPC-specific MetaFile Support**:
- LCL lacks `TMetaFile`/`TMetaFileCanvas` on Windows
- Minimal WinAPI wrapper provided in `mormot.ui.core` (non-portable)

### ORM Table → Grid Binding

**Usage**:
```pascal
// Create association (Grid takes ownership of Table)
TOrmTableToGrid.Create(MyDrawGrid, MyOrmTable);

// Grid automatically:
// - Displays column headers from TOrmTable field names
// - Renders rows with proper Unicode handling
// - Sorts on column click
// - Provides incremental search (type to find)
// - Hides ID column by default
```

**Customization**:
- `OnValueText`: Override cell content dynamically
- `OnHintText`: Custom tooltips for truncated text
- `OnDrawCellBackground`: Custom cell rendering
- `MarkAllowed`: Enable checkbox column for row selection

### PDF Generation

**Simple API**:
```pascal
var
  PDF: TPdfDocument;
begin
  PDF := TPdfDocument.Create;
  try
    PDF.Info.Author := 'mORMot';
    with PDF.AddPage do begin
      SetFontAndSize(fnHelvetica, 12);
      TextOut(100, 700, 'Hello World');
    end;
    PDF.SaveToFile('output.pdf');
  finally
    PDF.Free;
  end;
end;
```

**Advanced (GDI integration)**:
```pascal
var
  PDFGDI: TPdfDocumentGdi;
begin
  PDFGDI := TPdfDocumentGdi.Create;
  try
    with PDFGDI.AddPage do begin
      Canvas.Font.Name := 'Arial';
      Canvas.TextOut(50, 50, 'Rendered via TCanvas');
    end;
    PDFGDI.SaveToFile('output.pdf');
  finally
    PDFGDI.Free;
  end;
end;
```

**PDF Features**:
- PDF 1.3-1.7 support, PDF/A compliance (1A/1B, 2A/2B, 3A/3B)
- Encryption (RC4)
- Uniscribe support (Hebrew/Arabic/Asian text)
- Conditionals: `USE_PDFSECURITY`, `USE_UNISCRIBE`, `USE_SYNGDIPLUS`, `USE_METAFILE`

### Report Engine (`TGdiPages`)

**Typical Usage**:
```pascal
var
  Pages: TGdiPages;
begin
  Pages := TGdiPages.Create(Self);
  try
    Pages.BeginDoc;
    Pages.AddColumn(100, taLeft);
    Pages.AddColumn(200, taRight);
    Pages.DrawText('Column 1');
    Pages.DrawText('Column 2', [], True); // True = next line
    Pages.EndDoc;
    Pages.ShowPreview; // Or Pages.ExportPDF('report.pdf')
  finally
    Pages.Free;
  end;
end;
```

**Features**:
- Multi-column layouts with alignment
- Headers/footers with page numbering (use `PAGENUMBER` constant)
- Embedded images/metafiles
- Zoom modes: percent, page fit, page width
- Print with preview dialog

**Status**: Not compatible with FPC/LCL (extensive VCL dependencies)

### PDF Engine Customization

**Compile-time flags** (in project options):
- `NO_USE_PDFSECURITY`: Disable encryption (removes `mormot.crypt.core` dependency)
- `NO_USE_UNISCRIBE`: Disable Uniscribe (saves ~100KB, breaks Hebrew/Arabic)
- `NO_USE_SYNGDIPLUS`: Use default `jpeg` unit instead of GDI+ (limits image format support)
- `NO_USE_METAFILE`: Disable `TMetaFile` support (breaks `TPdfDocumentGdi`)

**Example** (lightweight PDF builds):
```pascal
// Add to project defines:
{$define NO_USE_UNISCRIBE}
{$define NO_USE_METAFILE}
```

## AI Guidelines

- ⚠️ **Platform assumptions**: Don't assume VCL on FPC. Use `{$ifdef FPC}` / `{$ifndef FPC}` instead of `{$ifdef VCL}`.
- ⚠️ **POSIX limitations**: POSIX units compile but don't work. Check for `{$ifdef OSPOSIX}` before assuming functionality.
- ⚠️ **Report engine is Delphi-only**: `TGdiPages` won't work on FPC due to extensive VCL dependencies.
- ⚠️ **GDI+ requires runtime DLL**: `gdiplus.dll` must be available (Windows XP+).
- ⚠️ **MetaFile on FPC is minimal**: Only basic WinAPI wrapper, not full cross-platform `TMetaFile`.
- ✅ **Preserve cross-platform abstractions**: Test changes on both Delphi and FPC if possible.
- ✅ **Check conditional compilation**: Ensure `{$ifdef OSPOSIX}` blocks remain no-ops.
- ✅ **Respect Windows-only units**: Don't add POSIX code to `mormot.ui.pdf`/`mormot.ui.report` (architectural limitation).
- ✅ **Unicode correctness**: All text should be UTF-8 aware (use `RawUtf8` internally, convert to `string` for VCL).
- ✅ **Adding custom controls**: Add to `mormot.ui.controls.pas`, implement both VCL and LCL compatibility paths, register in `procedure Register` for IDE support.

## Files Organization

```
mormot.ui.core.pas         # Foundation (LCL/VCL compatibility, UI helpers)
mormot.ui.controls.pas     # Custom components (hints, labeled edits, JSON persistence)
mormot.ui.grid.orm.pas     # ORM → Grid binding (TOrmTableToGrid)
mormot.ui.gdiplus.pas      # GDI+ wrapper (TSynPicture)
mormot.ui.pdf.pas          # PDF engine (TPdfDocument, TPdfDocumentGdi)
mormot.ui.report.pas       # Report engine (TGdiPages) - Delphi only
```

**Dependencies**:
- **Internal**: `mormot.core.*`, `mormot.db.core`, `mormot.orm.*`, `mormot.rest.client`, `mormot.lib.z`, `mormot.lib.gdiplus`, `mormot.lib.uniscribe`, `mormot.crypt.*`
- **External**: VCL/LCL (`Graphics`, `Controls`, `Forms`, `Grids`, `ExtCtrls`, `Themes`), WinAPI (`Windows`, `Messages`, `ActiveX`, `Winspool`, `Printers`)

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
