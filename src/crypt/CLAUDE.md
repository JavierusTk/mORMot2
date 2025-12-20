# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

The mORMot2 cryptographic units provide a **high-performance, stand-alone cryptographic library** with optional OpenSSL acceleration. The implementation philosophy prioritizes **native Pascal/assembly performance** over external dependencies.

**Key Design Principles**:
- **Stand-alone by default**: Pure Pascal with x86_64/x86/ARM assembly optimizations (AES-NI, SSE4, AVX2, CLMUL)
- **Faster than OpenSSL**: Native AES-CTR/CFB/OFB/CBC outperforms OpenSSL on x86_64 (except AES-GCM: ~20% slower)
- **OpenSSL optional**: Can delegate RSA/ECC/AES-GCM to OpenSSL 1.1/3.x where OpenSSL is faster
- **Factory-based architecture**: High-level `Rnd()`/`Hash()`/`Sign()`/`Cipher()`/`Asym()`/`Cert()`/`Store()` hide complexity
- **Validated against OpenSSL**: Correctness verified against OpenSSL reference implementation

**When to use**: Default choice for mORMot applications. Only add OpenSSL dependency for RSA/ECC-intensive workloads or AES-GCM.

## Cryptographic Algorithms Quick Ref

| Algorithm | Purpose | Performance | Security | When to Use |
|-----------|---------|-------------|----------|------------|
| **AES-256** | Symmetric encryption | Very fast | Excellent | Default choice for data |
| **ChaCha20** | Symmetric (modern) | Fast | Excellent | Mobile/ARM platforms |
| **SHA-256** | Hashing | Very fast | Excellent | Passwords, integrity checks |
| **SHA-3** | Hashing (newest) | Fast | Excellent | New designs (FIPS standard) |
| **HMAC** | Message authentication | Very fast | Excellent | Verify message integrity |
| **ECC-256** | Asymmetric (modern) | Moderate | Excellent | **Preferred** key exchange |
| **RSA-4096** | Asymmetric (legacy) | Slow | Good | Compatibility only |

**OpenSSL Integration** (Critical for production):

```pascal
// ✅ DO: Use OpenSSL for best performance
{$define USE_OPENSSL}  // Set BEFORE including mormot.core.crypt

uses mormot.core.crypt;

// Automatically uses OpenSSL if available
// Falls back to built-in if not available

// ❌ DON'T: Ignore OpenSSL availability
// Built-in implementation is safe but slower
// On POSIX systems, OpenSSL is nearly mandatory for production
```

Note: Set `USE_OPENSSL` in project defines, not in source code.

## SAD References

**📖 Main Documentation**: [Software Architecture Design - mORMot 2](/mnt/w/mORMot2/DOCS/README.md)

| Task | SAD Chapter(s) | Notes |
|------|----------------|-------|
| Hash/HMAC/PBKDF2 | [Chapter 21: Security](../DOCS/mORMot2-SAD-Chapter-21.md) | AES, SHA-2/SHA-3, factories |
| Symmetric encryption | [Chapter 21: Security](../DOCS/mORMot2-SAD-Chapter-21.md) | AES modes, `Cipher()` factory |
| ECC/RSA signatures | [Chapter 23: Asymmetric Encryption](../DOCS/mORMot2-SAD-Chapter-23.md) | `Asym()` factory, ES256/RS256 |
| X.509 certificates | [Chapter 23: Asymmetric Encryption](../DOCS/mORMot2-SAD-Chapter-23.md) | `Cert()`/`Store()` factories |
| JWT tokens | [Chapter 21: Security](../DOCS/mORMot2-SAD-Chapter-21.md) | TJwtCrypt class |
| OpenSSL integration | [Chapter 21: Security](../DOCS/mORMot2-SAD-Chapter-21.md) | RegisterOpenSsl |
| Architecture overview | [Chapter 21: Security](../DOCS/mORMot2-SAD-Chapter-21.md) | 3-layer design |

## Quick Patterns

### Performance Comparison (Stand-alone vs OpenSSL)

| Algorithm | Stand-alone | OpenSSL | Recommendation |
|-----------|-------------|---------|----------------|
| AES-CTR/CFB/OFB/CBC | **Faster** | Slower | Use stand-alone (default) |
| AES-GCM | ~20% slower | **Faster** | Use OpenSSL for GCM (`RegisterOpenSsl`) |
| SHA-256/SHA-512 | Comparable | Comparable | Use stand-alone (no dependency) |
| RSA/ECC | Slower | **Faster** | Use OpenSSL for production (`RegisterOpenSsl`) |

**Note**: Stand-alone AES-GCM is still faster than OpenSSL 3.0 (which regressed from 1.1).

### Optimization Strategy

```pascal
// Option 1: Default (no dependencies, fastest for most algorithms)
uses mormot.crypt.secure;
var cipher := Cipher('aes-256-ctr', @key[1], true);

// Option 2: OpenSSL for RSA/ECC intensive workloads
uses mormot.crypt.openssl, mormot.crypt.secure;
initialization
  RegisterOpenSsl; // Registers faster OpenSSL to factories

// Option 3: OpenSSL for AES-GCM only
var cipher := Cipher('aes-256-gcm-osl', @key[1], true);
```

### Cross-Platform Assembly Optimizations

| Platform | Features | Conditionals | Validation |
|----------|----------|--------------|------------|
| **x86_64** | AES-NI, CLMUL, SSE4, AVX2 | `USEAESNI64`, `USECLMUL`, `USEGCMAVX` | Full |
| **x86** | AES-NI (4x interleaved), SSE4 | `USEAESNI32`, `USECLMUL` | Full |
| **ARM64** | ARMv8 Crypto Extensions | `USEARMCRYPTO` | Linux/Android only |

**Assembly Files**:
- `mormot.crypt.core.asmx64.inc` - x86_64 optimizations
- `mormot.crypt.core.asmx86.inc` - x86 optimizations

**External Libraries** (platform-specific):
- Windows/Linux x86_64: CRC32C, SHA-512 (`.o`/`.obj` files)
- Windows/Linux x86: SHA-512 (`.o`/`.obj` files)

### Key Files Summary

| File | Purpose | Performance |
|------|---------|-------------|
| `mormot.crypt.core.pas` | AES, SHA, HMAC primitives | Faster than OpenSSL (most) |
| `mormot.crypt.secure.pas` | Factories, auth, key mgmt | N/A (abstraction layer) |
| `mormot.crypt.ecc256r1.pas` | Native ECC secp256r1 | Slower than OpenSSL |
| `mormot.crypt.ecc.pas` | High-level ECC | Uses ecc256r1 |
| `mormot.crypt.rsa.pas` | Native RSA | Slower than OpenSSL |
| `mormot.crypt.x509.pas` | X.509 certificates | N/A (data structures) |
| `mormot.crypt.jwt.pas` | JSON Web Tokens | N/A (protocol layer) |
| `mormot.crypt.openssl.pas` | OpenSSL 1.1/3.x bindings | Faster RSA/ECC/AES-GCM |
| `mormot.crypt.pkcs11.pas` | HSM support (PKCS#11) | Hardware-dependent |
| `mormot.crypt.other.pas` | Deprecated algorithms | Avoid in new code |

## AI Guidelines

> **Critical rules for code generation/modification**

- ⚠️ **Never instantiate low-level classes directly**: Use `Rnd()`/`Hash()`/`Sign()`/`Cipher()`/`Asym()`/`Cert()`/`Store()` factories instead of `TAes.Create()` or `TSha256.Create()`
- ⚠️ **Never use deprecated algorithms for security**: MD5, SHA-1, RC4 only acceptable for legacy compatibility (e.g., digest auth)
- ⚠️ **Never use native RSA/ECC in production without benchmarking**: Consider `RegisterOpenSsl` for performance-critical workloads
- ⚠️ **Windows OpenSSL conditional**: Define `USE_OPENSSL` in project options (Conditional defines) or `mormot.crypt.openssl.pas` compiles as void unit (silent failure)
- ✅ **Use factory pattern for all cryptographic operations**: Simplifies algorithm switching and OpenSSL integration
- ✅ **Validate with OpenSSL**: Framework correctness verified against OpenSSL reference implementation (see `/test`)
- ✅ **Check legal compliance**: Ensure compliance with cryptographic export restrictions in your country (see LICENSE.md)

## Files Organization

```
mormot.crypt.core.pas          # Low-level primitives (AES, SHA, HMAC)
mormot.crypt.core.asmx64.inc   # x86_64 assembly optimizations
mormot.crypt.core.asmx86.inc   # x86 assembly optimizations
mormot.crypt.secure.pas        # High-level factories and abstractions
mormot.crypt.ecc256r1.pas      # Native ECC implementation
mormot.crypt.ecc.pas           # High-level ECC wrapper
mormot.crypt.rsa.pas           # Native RSA implementation
mormot.crypt.x509.pas          # X.509 certificates (RFC 5280)
mormot.crypt.jwt.pas           # JSON Web Tokens (RFC 7797)
mormot.crypt.openssl.pas       # OpenSSL 1.1/3.x bindings
mormot.crypt.pkcs11.pas        # HSM support (PKCS#11)
mormot.crypt.other.pas         # Deprecated/legacy algorithms
```

**Project Files**: `/mnt/w/mORMot2/packages/` (compile via RAD Studio/Lazarus)

## Notes

**OpenSSL Registration Pattern**:
```pascal
uses mormot.crypt.openssl, mormot.crypt.x509;

initialization
  RegisterOpenSsl;  // Registers OpenSSL to factories
  RegisterX509;     // Extends X.509 support (after OpenSSL)
```

**Effect**: `Asym('es256')` → OpenSSL ECC; `Asym('rs256')` → OpenSSL RSA; `Cipher('aes-256-gcm')` → OpenSSL AES-GCM

**Version Compatibility**:
- Delphi 7 to 12.2 Athenes (Windows only for crypto units)
- FPC 3.2.2 fixes branch (3.2.2 stable has variant regression)
- OpenSSL 1.1.x and 3.x supported

**Legal Notice**: **Ensure compliance with cryptographic software export restrictions in your country** (see LICENSE.md).

---

**Last Updated**: 2025-12-20
**mORMot Version**: 2.3+ (trunk)
**Maintained By**: Synopse Informatique - Arnaud Bouchez
