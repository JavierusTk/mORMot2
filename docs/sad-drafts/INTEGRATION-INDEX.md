# mORMot2 SAD Integration Index

**Purpose**: Master index for integrating 22 new documentation sections into existing SAD chapters

**Date**: 2025-12-20

**Status**: Draft - Ready for review and integration

---

## Overview

This document maps all new documentation sections to their target chapters in the mORMot2 Software Architecture & Design (SAD) documentation. It provides:

1. Complete listing of all 22 new sections
2. Suggested insertion points within existing chapters
3. Structure for new Chapter 30
4. Integration workflow recommendations

---

## Summary by Chapter

| Chapter | Existing Sections | New Sections | Total |
|---------|------------------|--------------|-------|
| 04 | TBD | 2 | TBD |
| 08 | TBD | 3 | TBD |
| 11 | TBD | 2 | TBD |
| 16 | TBD | 1 | TBD |
| 20 | TBD | 3 | TBD |
| 21 | TBD | 4 | TBD |
| 22 | TBD | 1 | TBD |
| 24 | TBD | 4 | TBD |
| 26 | TBD | 1 | TBD |
| 30 | 0 (NEW) | 1 | 1 |
| **Total** | **TBD** | **22** | **TBD** |

---

## Chapter 04: Core Framework Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-04.md` (TBD: Verify actual filename)

### New Sections (2)

#### 1. Section 4.X - MVC Framework
- **File**: `04.X-MVC-Framework.md`
- **Title**: *4.X. MVC Framework* - *Model-View-Controller Made Simple*
- **Size**: 607 lines (19,065 bytes)
- **Topics**:
  - TMvcApplication architecture
  - Mustache template engine
  - Session management (TMvcSessionWithCookies)
  - REST server integration
  - Controller/View patterns
- **Suggested Insertion**: After existing core framework sections (likely after 4.6 or similar)
- **Dependencies**: References Chapter 24 (REST/ORM)

#### 2. Section 4.Y - Threading Utilities
- **File**: `04.Y-Threading-Utilities.md`
- **Title**: *4.Y. Threading Utilities* (tagline TBD)
- **Size**: 597 lines (18,475 bytes)
- **Topics**:
  - Threading patterns and utilities
  - Thread-safe operations
  - Synchronization primitives
- **Suggested Insertion**: After 4.X or at end of chapter
- **Dependencies**: Core framework concepts

---

## Chapter 08: Database & Storage Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-08.md` (TBD: Verify actual filename)

### New Sections (3)

#### 3. Section 8.W - 7-Zip Integration
- **File**: `08.W-7Zip-Integration.md`
- **Title**: *8.W. 7-Zip Integration* (tagline TBD)
- **Size**: 617 lines (19,057 bytes)
- **Topics**:
  - 7-Zip compression/decompression
  - Archive handling
  - Integration with mORMot storage
- **Suggested Insertion**: After existing compression sections or at end of chapter

#### 4. Section 8.X - Database Proxy
- **File**: `08.X-Database-Proxy.md`
- **Title**: *8.X. Database Proxy* (tagline TBD)
- **Size**: 1,055 lines (32,623 bytes)
- **Topics**:
  - Database proxy layer architecture
  - Connection pooling
  - Query routing
  - Performance optimization
- **Suggested Insertion**: After core database sections, before advanced topics
- **Dependencies**: Core database concepts

#### 5. Section 8.Z - Async PostgreSQL
- **File**: `08.Z-Async-PostgreSQL.md`
- **Title**: *8.Z. Async PostgreSQL* (tagline TBD)
- **Size**: 754 lines (23,355 bytes)
- **Topics**:
  - Asynchronous PostgreSQL client
  - Non-blocking operations
  - Performance tuning
  - Connection management
- **Suggested Insertion**: After 8.X or at end of chapter
- **Dependencies**: PostgreSQL concepts

---

## Chapter 11: HTTP/Network Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-11.md` (TBD: Verify actual filename)

### New Sections (2)

#### 6. Section 11.X - TUriRouter
- **File**: `11.X-TUriRouter.md`
- **Title**: *11.X. TUriRouter* (tagline TBD)
- **Size**: 601 lines (18,580 bytes)
- **Topics**:
  - URI routing engine
  - Route matching patterns
  - Parameter extraction
  - REST endpoint mapping
- **Suggested Insertion**: After core HTTP server sections
- **Dependencies**: HTTP server concepts

#### 7. Section 11.Y - Socket.IO
- **File**: `11.Y-SocketIO.md`
- **Title**: *11.Y. Socket.IO* (tagline TBD)
- **Size**: 768 lines (23,774 bytes)
- **Topics**:
  - Socket.IO protocol implementation
  - WebSocket transport
  - Real-time bidirectional communication
  - Event handling
- **Suggested Insertion**: After 11.X or at end of chapter
- **Dependencies**: WebSocket concepts

---

## Chapter 16: Service Architecture Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-16.md` (TBD: Verify actual filename)

### New Sections (1)

#### 8. Section 16.X - ServiceContainer
- **File**: `16.X-ServiceContainer.md`
- **Title**: *16.X. ServiceContainer* (tagline TBD)
- **Size**: 515 lines (15,927 bytes)
- **Topics**:
  - Dependency Injection container
  - Service registration and resolution
  - Lifetime management (Singleton, Transient, Scoped)
  - TInjectableObject integration
- **Suggested Insertion**: Before or after DI-related sections
- **Dependencies**: References Chapter 24.13 (TInjectableObject)

---

## Chapter 20: Application Layer Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-20.md` (TBD: Verify actual filename)

### New Sections (3)

#### 9. Section 20.3.1.1 - ICommandLine (Subsection)
- **File**: `20.3.1.1-ICommandLine.md`
- **Title**: *20.3.1.1. ICommandLine* (tagline TBD)
- **Size**: 263 lines (8,144 bytes)
- **Topics**:
  - Command-line interface abstraction
  - Argument parsing
  - Option handling
- **Suggested Insertion**: As subsection under 20.3.1 (or create 20.3.1 if needed)
- **Note**: This is a numbered subsection, not a lettered extension

#### 10. Section 20.X - THttpPeerCache
- **File**: `20.X-THttpPeerCache.md`
- **Title**: *20.X. THttpPeerCache* (tagline TBD)
- **Size**: 933 lines (28,882 bytes)
- **Topics**:
  - HTTP peer-to-peer caching
  - Distributed cache coordination
  - Cache invalidation strategies
  - Performance optimization
- **Suggested Insertion**: After existing HTTP-related sections

#### 11. Section 20.Y - Relay Tunnel
- **File**: `20.Y-Relay-Tunnel.md`
- **Title**: *20.Y. Relay Tunnel* (tagline TBD)
- **Size**: 445 lines (13,780 bytes)
- **Topics**:
  - Relay tunnel protocol
  - NAT traversal
  - Secure tunneling
  - Connection routing
- **Suggested Insertion**: After 20.X or at end of chapter

---

## Chapter 21: Cryptography & Security Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-21.md` (TBD: Verify actual filename)

### New Sections (4)

#### 12. Section 21.2.5 - AES Advanced Modes (Subsection)
- **File**: `21.2.5-AES-Advanced-Modes.md`
- **Title**: *21.2.5. AES Advanced Modes* (tagline TBD)
- **Size**: 732 lines (22,663 bytes)
- **Topics**:
  - AES-GCM, AES-CCM
  - Authenticated encryption modes
  - AEAD (Authenticated Encryption with Associated Data)
  - Performance comparison
- **Suggested Insertion**: As subsection under 21.2 (AES encryption section)
- **Note**: This is a numbered subsection extending existing 21.2

#### 13. Section 21.14 - PKCS#11 HSM
- **File**: `21.14-PKCS11-HSM.md`
- **Title**: *21.14. PKCS#11 HSM* (tagline TBD)
- **Size**: 776 lines (24,002 bytes)
- **Topics**:
  - PKCS#11 interface
  - Hardware Security Module (HSM) integration
  - Key management
  - Cryptographic operations on HSM
- **Suggested Insertion**: After existing numbered sections (likely after 21.13)
- **Note**: This is a numbered section, suggesting it's part of the main sequence

#### 14. Section 21.X - LDAP & Kerberos
- **File**: `21.X-LDAP-Kerberos.md`
- **Title**: *21.X. LDAP & Kerberos* (tagline TBD)
- **Size**: 629 lines (19,476 bytes)
- **Topics**:
  - LDAP authentication
  - Kerberos integration
  - Single Sign-On (SSO)
  - Enterprise authentication
- **Suggested Insertion**: After authentication-related sections

#### 15. Section 21.Y - ACME & Let's Encrypt
- **File**: `21.Y-ACME-LetsEncrypt.md`
- **Title**: *21.Y. ACME & Let's Encrypt* (tagline TBD)
- **Size**: 885 lines (27,396 bytes)
- **Topics**:
  - ACME protocol implementation
  - Let's Encrypt certificate automation
  - Certificate lifecycle management
  - Challenge handling (HTTP-01, DNS-01, TLS-ALPN-01)
- **Suggested Insertion**: After certificate-related sections or at end of chapter

---

## Chapter 22: Scripting Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-22.md` (TBD: Verify actual filename)

### New Sections (1)

#### 16. Section 22 (Replacement/Extension) - QuickJS Advanced
- **File**: `22-QuickJS-Advanced.md`
- **Title**: *22. QuickJS Advanced* (tagline TBD)
- **Size**: 886 lines (27,419 bytes)
- **Topics**:
  - QuickJS engine integration
  - Advanced JavaScript features
  - Pascal-JavaScript interop
  - Performance optimization
- **Suggested Insertion**: Either replace existing Chapter 22 or append as advanced sections
- **Note**: Filename suggests this might be a complete chapter replacement or major extension

---

## Chapter 24: REST/ORM Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-24.md` (TBD: Verify actual filename)

### New Sections (4)

#### 17. Section 24.6 - TOrmMany Advanced (Subsection/Extension)
- **File**: `24.6-TOrmMany-Advanced.md`
- **Title**: *24.6. TOrmMany Advanced* (tagline TBD)
- **Size**: 592 lines (18,309 bytes)
- **Topics**:
  - Many-to-many relationship patterns
  - Advanced TOrmMany usage
  - Junction table optimization
  - Batch operations
- **Suggested Insertion**: Either as subsection under existing 24.6 or replace it
- **Note**: May extend existing section 24.6

#### 18. Section 24.12 - TRestBatch Advanced (Subsection/Extension)
- **File**: `24.12-TRestBatch-Advanced.md`
- **Title**: *24.12. TRestBatch Advanced* (tagline TBD)
- **Size**: 510 lines (15,789 bytes)
- **Topics**:
  - Advanced batch operations
  - Performance optimization
  - Transaction handling
  - Error recovery
- **Suggested Insertion**: Either as subsection under existing 24.12 or replace it

#### 19. Section 24.13 - TInjectableObject
- **File**: `24.13-TInjectableObject.md`
- **Title**: *24.13. TInjectableObject* (tagline TBD)
- **Size**: 683 lines (21,127 bytes)
- **Topics**:
  - Dependency injection framework
  - Constructor injection
  - Property injection
  - Resolver patterns
  - Lifetime management
- **Suggested Insertion**: After existing numbered sections (likely after 24.12)
- **Note**: Numbered section, referenced by 16.X (ServiceContainer)

#### 20. Section 24.15 - TSynValidate
- **File**: `24.15-TSynValidate.md`
- **Title**: *24.15. TSynValidate* (tagline TBD)
- **Size**: 680 lines (21,060 bytes)
- **Topics**:
  - Domain validation framework
  - Built-in validators
  - Custom validation rules
  - Integration with ORM
  - Error handling
- **Suggested Insertion**: After 24.13 or at appropriate validation section

---

## Chapter 26: Tools & Utilities Extensions

**Target Chapter**: `mORMot2-SAD-Chapter-26.md` (TBD: Verify actual filename)

### New Sections (1)

#### 21. Section 26.3.5 - CLI Tools (Subsection)
- **File**: `26.3.5-CLI-Tools.md`
- **Title**: *26.3.5. CLI Tools* (no tagline)
- **Size**: 423 lines (13,094 bytes)
- **Topics**:
  - Command-line tools
  - Utility scripts
  - Development tools
- **Suggested Insertion**: As subsection under 26.3 (Tools section)
- **Note**: This is a numbered subsection extending existing section 26.3

---

## Chapter 30: Specialized Utilities (NEW CHAPTER)

**Target Chapter**: `mORMot2-SAD-Chapter-30.md` (NEW - to be created)

### New Sections (1)

#### 22. Section 30 - Specialized Utilities (Complete Chapter)
- **File**: `30-Specialized-Utilities.md`
- **Title**: *30. Specialized Utilities* - *Analyzing Windows Executables*
- **Size**: 1,162 lines (34,883 bytes)
- **Scope**: Complete new chapter covering PE/COFF parser
- **Topics**:
  - PE/COFF file format parsing
  - Version information extraction
  - Digital signature stuffing (unique feature)
  - Cross-platform executable analysis
  - Architecture detection
  - Resource extraction
  - Security considerations
- **Structure**: 21 numbered sections (30.1 through 30.21)
- **Note**: This is a complete standalone chapter, not an extension

### Chapter 30 Section Outline

```
30. Specialized Utilities
├── 30.1. PE/COFF Parser Overview
├── 30.2. Basic Version Information Extraction
├── 30.3. Detailed PE Analysis with TSynPELoader
├── 30.4. StringFileInfo Resource Parsing
├── 30.5. Low-Level PE Structure Access
├── 30.6. Data Directory Access
├── 30.7. RVA to File Offset Translation
├── 30.8. Digital Signature Stuffing (Unique Feature)
├── 30.9. Stuffing Data into Signed Executables
├── 30.10. Retrieving Stuffed Data
├── 30.11. Signature Stuffing Implementation Details
├── 30.12. Use Cases for Signature Stuffing
├── 30.13. Cross-Platform PE Analysis
├── 30.14. Performance and Memory Considerations
├── 30.15. Error Handling and Validation
├── 30.16. Advanced: Custom Resource Extraction
├── 30.17. Security Considerations
├── 30.18. Real-World Examples
├── 30.19. Best Practices
├── 30.20. Summary
└── 30.21. Next Steps
```

---

## Integration Workflow

### Phase 1: Pre-Integration Verification
1. **Verify existing chapter numbers and filenames**
   - [ ] Confirm actual filenames for Chapters 04, 08, 11, 16, 20, 21, 22, 24, 26
   - [ ] Review existing section numbering in each chapter
   - [ ] Identify actual insertion points for new sections

2. **Review section dependencies**
   - [ ] Check Chapter 24.13 (TInjectableObject) exists before integrating 16.X
   - [ ] Verify Chapter 24 exists before integrating 4.X (MVC Framework)
   - [ ] Review all cross-references in new sections

### Phase 2: Section Numbering Finalization
1. **Determine final section numbers**
   - [ ] Replace placeholder X, Y, Z with actual numbers
   - [ ] Ensure no conflicts with existing sections
   - [ ] Update all cross-references

2. **Resolve subsection placement**
   - [ ] 20.3.1.1 (ICommandLine) - verify parent section exists
   - [ ] 21.2.5 (AES Advanced Modes) - verify section 21.2 exists
   - [ ] 26.3.5 (CLI Tools) - verify section 26.3 exists
   - [ ] 24.6, 24.12 - determine if replacing or extending existing sections

### Phase 3: Content Integration
1. **Add navigation footers** to all 22 files
   - Use format from STYLE-GUIDE.md
   - Link to previous/next chapters appropriately
   - All link to central index

2. **Update master index**
   - [ ] Add all 22 new sections to `mORMot2-SAD-Index.md`
   - [ ] Update chapter TOCs
   - [ ] Add cross-reference entries

3. **Integrate into existing chapters**
   - [ ] Insert sections at determined points
   - [ ] Update chapter navigation footers
   - [ ] Renumber subsequent sections if needed

4. **Create Chapter 30**
   - [ ] Rename `30-Specialized-Utilities.md` to `mORMot2-SAD-Chapter-30.md`
   - [ ] Add navigation footer
   - [ ] Add to master index
   - [ ] Update previous chapter (29) to link to Chapter 30

### Phase 4: Final Review
1. **Consistency checks**
   - [ ] All files use UTF-8 BOM encoding
   - [ ] Header numbering is consistent
   - [ ] Code blocks use correct language tags
   - [ ] ASCII diagrams are properly formatted

2. **Cross-reference validation**
   - [ ] All chapter references are accurate
   - [ ] All section references exist
   - [ ] Navigation links work correctly

3. **Style guide compliance**
   - [ ] Taglines present (add where missing)
   - [ ] Formatting matches existing chapters
   - [ ] Terminology is consistent

---

## Notes for Integration

### Placeholder Section Numbers (X, Y, Z)
The following files use placeholder letters that need to be replaced with actual numbers:

**Chapter 04**:
- 4.X → (TBD based on existing sections)
- 4.Y → (TBD, likely 4.X + 1)

**Chapter 08**:
- 8.W → (TBD)
- 8.X → (TBD)
- 8.Z → (TBD, likely last in sequence)

**Chapter 11**:
- 11.X → (TBD)
- 11.Y → (TBD)

**Chapter 16**:
- 16.X → (TBD)

**Chapter 20**:
- 20.X → (TBD)
- 20.Y → (TBD)

**Chapter 21**:
- 21.X → (TBD)
- 21.Y → (TBD)

### Numbered Sections (May Replace or Extend Existing)
These sections use actual numbers and may conflict with existing sections:

- **21.2.5** - Subsection under 21.2
- **21.14** - May be new or replace existing 21.14
- **24.6** - May replace or extend existing 24.6
- **24.12** - May replace or extend existing 24.12
- **24.13** - Likely new section (referenced by 16.X)
- **24.15** - Likely new section

**Action Required**: Review existing chapters to determine if these sections exist and need replacement or extension.

### Missing Taglines
The following files are missing italicized taglines after the title:

- 04.Y-Threading-Utilities.md
- 08.W-7Zip-Integration.md
- 08.X-Database-Proxy.md
- 08.Z-Async-PostgreSQL.md
- 11.X-TUriRouter.md
- 11.Y-SocketIO.md
- 16.X-ServiceContainer.md
- 20.3.1.1-ICommandLine.md
- 20.X-THttpPeerCache.md
- 20.Y-Relay-Tunnel.md
- 21.2.5-AES-Advanced-Modes.md
- 21.14-PKCS11-HSM.md
- 21.X-LDAP-Kerberos.md
- 21.Y-ACME-LetsEncrypt.md
- 22-QuickJS-Advanced.md
- 24.6-TOrmMany-Advanced.md
- 24.12-TRestBatch-Advanced.md
- 24.13-TInjectableObject.md
- 24.15-TSynValidate.md
- 26.3.5-CLI-Tools.md

**Recommendation**: Add taglines before final integration to match existing SAD chapter style.

### Cross-References to Verify

**16.X (ServiceContainer)** references:
- Chapter 24.13 (TInjectableObject) - ensure this section is integrated first

**4.X (MVC Framework)** references:
- Chapter 24 (REST/ORM Server)

**30 (Specialized Utilities)** references:
- Chapter 21 (Cryptography)
- Chapter 22 (Compression - if applicable)
- Chapter 4 (JSON)

---

## File Size Summary

| Chapter | Files | Total Lines | Total Bytes | Avg Bytes/File |
|---------|-------|-------------|-------------|----------------|
| 04 | 2 | 1,204 | 37,540 | 18,770 |
| 08 | 3 | 2,426 | 75,035 | 25,012 |
| 11 | 2 | 1,369 | 42,354 | 21,177 |
| 16 | 1 | 515 | 15,927 | 15,927 |
| 20 | 3 | 1,641 | 50,806 | 16,935 |
| 21 | 4 | 3,022 | 93,537 | 23,384 |
| 22 | 1 | 886 | 27,419 | 27,419 |
| 24 | 4 | 2,465 | 76,285 | 19,071 |
| 26 | 1 | 423 | 13,094 | 13,094 |
| 30 | 1 | 1,162 | 34,883 | 34,883 |
| **Total** | **22** | **15,113** | **466,880** | **21,222** |

**Note**: Average file size is ~21KB, total documentation adds ~467KB to SAD.

---

## Timeline Estimate

**Assuming sequential integration by one person:**

| Phase | Tasks | Estimated Time |
|-------|-------|----------------|
| Phase 1 | Review existing chapters, verify numbering | 4-6 hours |
| Phase 2 | Finalize section numbers, update cross-refs | 3-4 hours |
| Phase 3 | Add navigation, integrate content | 8-10 hours |
| Phase 4 | Final review, consistency checks | 3-4 hours |
| **Total** | **Complete integration** | **18-24 hours** |

**Recommendation**: Can be parallelized by assigning chapters to different reviewers.

---

## Success Criteria

Integration is complete when:

- [ ] All 22 files have navigation footers
- [ ] All files integrated into target chapters at appropriate locations
- [ ] Chapter 30 created as standalone chapter
- [ ] Master index updated with all new sections
- [ ] All cross-references validated
- [ ] All section numbers finalized (no X, Y, Z placeholders)
- [ ] All files pass UTF-8 BOM check
- [ ] Style guide compliance verified
- [ ] All taglines added
- [ ] Navigation links tested

---

## Contact & Support

**Primary Documentation**: See `STYLE-GUIDE.md` for formatting standards

**Review Notes**: See `REVIEW-NOTES.md` for consistency analysis

**Questions**: Contact documentation maintainer

---

**Document Version**: 1.0
**Last Updated**: 2025-12-20
**Author**: Claude Code Autonomous Worker
**Next Action**: Phase 1 verification of existing chapter structure
