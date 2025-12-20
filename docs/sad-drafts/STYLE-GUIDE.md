# mORMot2 SAD Documentation Style Guide

**Purpose**: Ensure consistency across all Software Architecture & Design documentation chapters.

**Extracted from**: Chapters 1-3 of existing mORMot2-SAD-Chapter-*.md files

---

## File Organization

### File Naming Convention
```
mORMot2-SAD-Chapter-NN.md
```
- NN: Zero-padded chapter number (01, 02, 03, etc.)
- Use kebab-case for multi-word sections: `mORMot2-SAD-Foreword.md`

### UTF-8 BOM
- **REQUIRED**: All .md files must use UTF-8 encoding with BOM
- Run `ensure-bom` after creating/editing files

---

## Header Hierarchy

### Level 1: Chapter Title
```markdown
# N. Chapter Title

*Tagline in italics*
```
- **Format**: `# N. Title` (N = chapter number)
- Always followed by an italicized tagline/motto
- Examples:
  - `# 1. mORMot2 Overview` → `*Meet the mORMot*`
  - `# 2. Architecture Principles` → `*Adopt a mORMot*`

### Level 2: Major Sections
```markdown
## N.M. Section Title
```
- **Format**: `## N.M. Title` (N.M = chapter.section numbering)
- Examples: `## 1.1. Client-Server ORM/SOA Framework`

### Level 3: Subsections
```markdown
### Subsection Title
```
- **Format**: `### Title` (no numbering)
- Used for detailed breakdowns within major sections

### Level 4+: Not Used
- Only three levels of headers in SAD documentation

---

## Code Blocks

### Language Tags

**Pascal code** - Use `pascal` language tag:
````markdown
```pascal
uses
  mormot.core.base,
  mormot.core.json;

type
  TOrmCustomer = class(TOrm)
  published
    property Name: RawUtf8 read fName write fName;
  end;
```
````

**JSON data** - Use `json` language tag:
````markdown
```json
{
  "ID": 1234,
  "UserName": "John Smith"
}
```
````

**Bash/shell commands** - Use `bash` language tag:
````markdown
```bash
git clone https://github.com/synopse/mORMot2.git
```
````

**Plain text/diagrams** - No language tag (ASCII art):
````markdown
```
┌────────────────────────────────────┐
│         Architecture               │
└────────────────────────────────────┘
```
````

### Indentation in Code
- **2 spaces** for Pascal code indentation (matches mORMot2 conventions)
- **Consistent alignment** for multi-line declarations

---

## ASCII Diagrams

### Box Drawing Characters
**Consistent character usage**:
```
┌─┬─┐  └─┴─┘  ├─┼─┤  ║  │  ─  ═
└─┴─┘  ┌─┬─┐  ▼  ▲  ►  ◄  •
```

### Standard Layout Patterns

**Architecture layers (vertical stack)**:
```
┌─────────────────────────────────────────┐
│  Layer N: Description                   │
│  ┌──────┬──────┬──────┐                 │
│  │ unit │ unit │ unit │                 │
│  └──────┴──────┴──────┘                 │
└─────────────────────────────────────────┘
                │
┌─────────────────────────────────────────┐
│  Layer N-1: Description                 │
└─────────────────────────────────────────┘
```

**Client-Server communication (horizontal flow)**:
```
┌─────────┐    REST/JSON    ┌─────────┐
│ Client  │ ───────────────► │ Server  │
└─────────┘                  └─────────┘
```

**Process flows**:
```
Customer ──► BackLog ──► Design ──► Dev ──► Software
```

### Diagram Formatting
- **Width**: Keep diagrams within 72-80 characters (narrow) or up to ~120 for complex ones
- **Spacing**: Use consistent internal spacing for readability
- **Alignment**: Align boxes and arrows for visual clarity
- **Place inside code blocks** (no language tag)

---

## Emphasis & Formatting

### Bold (`**text**`)
**Usage**:
- Key terms on first introduction: `**ORM**`, `**SOA**`
- Important keywords: `**REQUIRED**`, `**mORMot 2**`
- Feature names: `**Async HTTP Server**`

### Italic (`*text*`)
**Usage**:
- Chapter taglines: `*Meet the mORMot*`
- Quotes from external sources
- mORMot framework references: `*mORMot 2*`, `*Delphi*`, `*Free Pascal*`
- Emphasis for contrast: `*convention over configuration*`

### Inline Code (`` `text` ``)
**Usage**:
- Type names: `` `TOrm` ``, `` `RawUtf8` ``
- Unit names: `` `mormot.core.base` ``
- Function names: `` `Add()` ``, `` `CreateMissingTables` ``
- File names: `` `SynCommons.pas` ``
- Short code snippets: `` `Server.Orm.Add(Customer)` ``

---

## Lists

### Bullet Lists
```markdown
- **Category**: Description or details
- **Another category**: More details
```
- Use `-` for bullets (not `*` or `+`)
- Bold the category/term if listing features/concepts
- Use hanging indents for multi-line items

### Numbered Lists
```markdown
1. First step
2. Second step
3. Third step
```
- Use for sequential steps or ordered information
- Use Arabic numerals (`1.`, `2.`, not Roman)

---

## Tables

### Standard Table Format
```markdown
| Column 1 | Column 2 | Column 3 |
|----------|----------|----------|
| Data     | Data     | Data     |
| Data     | Data     | Data     |
```

### Table Usage Patterns

**Type mappings** (2 columns):
```markdown
| mORMot 1 | mORMot 2 |
|----------|----------|
| `TSQLRecord` | `TOrm` |
```

**Feature descriptions** (3 columns):
```markdown
| Unit | Purpose | Key Types |
|------|---------|-----------|
| `mormot.core.base` | Foundation types | `RawUtf8`, `PtrInt` |
```

**Comparison tables**:
```markdown
| Feature | SQL | NoSQL |
|---------|-----|-------|
| Schema | Fixed schema | Schema-less |
```

### Table Styling
- **Header row**: Plain text (no bold unless emphasizing specific columns)
- **Code references**: Use backticks for types, units, functions
- **Alignment**: Left-align text columns, use pipes for clarity

---

## Notes & Callouts

### Blockquote Notes
```markdown
> **Note**: The `src/ddd/` folder contains conceptual documentation only.
```
- Use `>` blockquote syntax
- Start with `**Note**:`, `**Warning**:`, or `**Important**:`
- Keep concise (1-2 sentences typically)

### Inline Notes
```markdown
**Why**: Windows executables use Windows path resolver, not WSL path resolver.
```
- Use bold keyword followed by explanation
- Common patterns: `**Why**:`, `**Example**:`, `**Key Principle**:`

---

## Cross-References

### Chapter References
```markdown
*Next Chapter: Architecture Principles*

See Chapter 24 for details.

(see Chapter 2)
```
- Italicize "Next Chapter" text
- Use chapter number for internal references
- Parenthetical when inline: `(see Chapter 2)`

### Navigation Footer
**REQUIRED at end of every chapter**:
```markdown
---

## Navigation

| Previous | Index | Next |
|----------|-------|------|
| [Chapter 1: mORMot 2 Overview](mORMot2-SAD-Chapter-01.md) | [Index](mORMot2-SAD-Index.md) | [Chapter 2: Architecture Principles](mORMot2-SAD-Chapter-02.md) |
```
- Horizontal rule before navigation section
- Three-column table with Previous/Index/Next links
- Always link to Index in middle column
- Use descriptive link text: `[Chapter N: Title](filename.md)`

---

## Section Separators

### Horizontal Rules
```markdown
---
```
- Use `---` (three hyphens, not `***` or `___`)
- Place before major section transitions
- Always before "Next Chapter" and Navigation sections
- Use sparingly within chapter body (only for major breaks)

---

## Special Conventions

### FAQ Sections
```markdown
### Question as heading?

Answer paragraph(s).
```
- Use `###` level heading for each question
- Question phrased naturally (with `?`)
- Answer immediately follows (no "**Answer:**" prefix)

### Version References
```markdown
**mORMot 2** vs **mORMot 1**
```
- Bold framework version names
- Capitalize version numbers

### Unit References in Text
```markdown
All units follow the pattern `mormot.<layer>.<feature>.pas`
```
- Use inline code for unit names
- Include `.pas` extension when referencing file
- Omit extension when discussing unit namespace conceptually

### Code Comments
```pascal
// Create ORM model with our class
Model := TOrmModel.Create([TOrmSample]);
```
- Use `//` single-line comments (not `{ }` or `(* *)`)
- Place comment **above** the code it explains
- Keep comments concise and explanatory

---

## Tone & Language

### Writing Style
- **Active voice**: "mORMot provides..." (not "is provided by")
- **Present tense**: "The framework implements..." (not "implemented")
- **Concise**: Avoid redundancy, favor clarity
- **Objective**: Technical documentation, not marketing

### Terminology Consistency
- **mORMot 2** (not "mORMot2", "Mormot2", "mormot 2")
- **Object-Relational Mapping** → ORM (expand on first use, abbreviate after)
- **REST** / **JSON** / **HTTP** - all caps for protocols/formats
- **Delphi** / **Free Pascal** - proper case for language names

---

## Checklist for New Chapters

- [ ] File named `mORMot2-SAD-Chapter-NN.md`
- [ ] UTF-8 with BOM encoding
- [ ] Chapter title: `# N. Title`
- [ ] Italicized tagline after title
- [ ] Section numbering: `## N.M. Section`
- [ ] Code blocks use `pascal`, `json`, `bash`, or no tag (diagrams)
- [ ] ASCII diagrams aligned and readable
- [ ] Tables formatted consistently
- [ ] Cross-references use correct format
- [ ] Navigation footer present at end
- [ ] Horizontal rules placed correctly (`---`)
- [ ] No level 4+ headers
- [ ] Terminology consistent with existing chapters
- [ ] Run `ensure-bom` on file after creation/editing

---

**Document Status**: v1.0 (extracted from Chapters 1-3)
**Last Updated**: 2024-12-20
