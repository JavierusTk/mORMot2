# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This directory contains conceptual documentation for **Domain-Driven Design (DDD)** patterns as implemented using the mORMot framework. While this folder itself has minimal implementation, mORMot's existing ORM, SOA, and REST layers fully support DDD architectural patterns.

**📖 Comprehensive DDD Documentation**: See [SAD Chapter 24: Domain-Driven Design](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-24.md) for complete coverage including:
- Value Objects, Entities, and Aggregates
- Repository Pattern with `IRestOrm`
- Domain Services and Application Services
- Unit of Work with `TRestBatch`
- Event-Driven Design patterns
- Clean Architecture layer mapping
- Testing DDD code

## Domain-Driven-Design vs Kingdom-Driven-Design

**DDD (Domain-Driven-Design)**: A set of paranoid architecture and design rules for writing highly-maintainable applications with strict layer separation, ubiquitous language, and bounded contexts.

**KDD (Kingdom-Driven-Design)**: mORMot's own less paranoid set of design rules, leveraging framework abstractions like ORM and SOA. Achieves similar maintainability goals with less boilerplate through framework conventions.

## Directory Status

This folder currently contains:
- ✅ README.md with conceptual overview
- ✅ Patterns documented in SAD Chapter 24
- ❌ No Pascal source files (`.pas`) - patterns implemented in other modules

## Typical DDD Architecture with mORMot

```
Domain Layer (Pure Pascal)
  └─> TOrm entities with business logic methods
  └─> Value objects as records or classes

Application Layer (Services)
  └─> IInvokable interfaces (src/soa)
  └─> Application service implementations

Infrastructure Layer
  └─> IRestOrm repositories (src/orm)
  └─> TSqlDBConnection (src/db)

Presentation Layer
  └─> TRestHttpServer (src/rest)
  └─> MVC views (src/core/mormot.core.mvc.pas)
```

## Domain-Driven Design Layers

```
Application Layer
  ├─ Use Cases (orchestrate domain operations)
  └─ DTOs (data transfer between layers)
      ↓
Domain Layer (HEART OF THE DESIGN)
  ├─ Aggregates (bounded contexts)
  ├─ Value Objects (immutable, no identity)
  ├─ Domain Services (cross-aggregate logic)
  └─ Repositories (aggregate persistence interface)
      ↓
Infrastructure Layer
  ├─ ORM Implementation (TRestOrm, database)
  ├─ External APIs (HTTP clients, messaging)
  └─ Persistence (file storage, caching)
```

## DDD Patterns Quick Ref

| Pattern | Purpose | Mutable? | Example |
|---------|---------|----------|---------|
| **Aggregate** | Consistency boundary | Yes | Order with line items |
| **Value Object** | Domain concept without ID | No | Money, EmailAddress |
| **Domain Service** | Cross-aggregate logic | No | PaymentProcessor |
| **Repository** | Aggregate persistence | N/A | IOrderRepository |
| **Entity** | Has identity, mutable | Yes | Customer, Product |
| **Factory** | Complex object creation | N/A | OrderFactory |

**Repository Pattern** (DO/DON'T):

```pascal
// ✅ DO: Inject repository interface
type
  IOrderRepository = interface
    function GetOrder(ID: TID): TOrder;
    procedure SaveOrder(const Order: TOrder);
  end;

type
  TOrderService = class(TInjectableObjectRest)
  private
    fOrderRepo: IOrderRepository;
  public
    constructor Create(const aRepo: IOrderRepository);
    procedure ProcessOrder(const Order: TOrder);
  end;

// ❌ DON'T: Access ORM directly in domain logic
type
  TOrderService = class(TInjectableObjectRest)
  public
    procedure ProcessOrder(const OrderID: TID);
  end;

procedure TOrderService.ProcessOrder(const OrderID: TID);
begin
  Server.Orm.Retrieve(OrderID, fOrder);  // WRONG - mixes concerns
end;
```

**Note**: This folder is meta-documentation only - see SAD Chapter 24 for complete patterns.

## Key DDD Patterns in mORMot

| Pattern | mORMot Implementation | Location |
|---------|----------------------|----------|
| **Repository** | `IRestOrm` interface | `src/orm` |
| **Entity** | `TOrm` classes with ID | `src/orm` |
| **Value Object** | Records or classes without ID | User code |
| **Aggregate Root** | `TOrm` with composition via `TRecordReference` | `src/orm` |
| **Domain Service** | Regular Pascal unit with business logic | User code |
| **Application Service** | `IInvokable` interface | `src/soa` |
| **Unit of Work** | `TRestBatch` for atomic operations | `src/orm` |
| **Event Sourcing** | JSON document storage in `TOrm` | User code |
| **CQRS** | Separate read/write models via ORM | User code |

## Related Documentation

- [ORM Layer](../orm/CLAUDE.md) - Domain entities and repositories
- [SOA Layer](../soa/CLAUDE.md) - Application services
- [REST Layer](../rest/CLAUDE.md) - HTTP/API infrastructure
- [Core Interfaces](../core/CLAUDE.md) - IoC/DI support via `mormot.core.interfaces.pas`

## Note for AI Assistants

**Critical Rule**: Use the existing framework components rather than creating new abstractions in this folder. The mORMot architecture already aligns well with DDD principles through its ORM, SOA, and REST layers.

**Anti-Pattern**: Creating custom "Repository" classes that wrap `IRestOrm`. Use `IRestOrm` directly - it's already the Repository pattern.

**Best Practice**: Keep domain logic in `TOrm` methods, application orchestration in `IInvokable` services, and infrastructure in `IRestOrm` implementations.

---

**Last Updated:** 2025-12-20
**mORMot Version:** 2.x
**License:** MPL/GPL/LGPL
