# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

The `mORMot2/src/app` folder contains application framework units for building cross-platform **daemons/services** and **console applications**.

**📖 Complete Documentation**: See [SAD Chapter 20: Application Servers](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-20.md) for comprehensive coverage of daemon/service architecture, command-line handling, Angelize process manager, and deployment patterns.

## Important Notes for Development

**Settings persistence:**
- Auto-saved on change, loaded on startup
- Windows: JSON/INI in exe folder
- Linux: `/etc` for settings, `/var/log` for logs

**Graceful shutdown:**
- `Stop` must handle being called multiple times safely
- Clean up resources in reverse order of initialization

**Cross-platform patterns:**
- Avoid OS-specific code in daemon classes
- Test in both console mode (development) and service mode (production)

**GUI restrictions:**
- Windows services cannot show UI (no MessageBox, ShowMessage, etc.)
- Use logging (`TSynLog`) for all diagnostic output

**Log rotation:**
- Configured via `LogRotateFileCount` (default 2)
- Logs rotate automatically based on size/date

**Folder defaults:**
- Windows: Same folder as executable
- Linux: `/etc` for config, `/var/log` for logs
- Override via `TSynDaemonSettings.WorkFolderName`

**Command parsing:**
- Case-insensitive switch matching
- Supports both `/` (Windows) and `--` (POSIX) prefixes
- Use `ICommandLine.NoPrompt` to detect batch mode

## Application Server Patterns

| Pattern | Use Case | Auto-start? | Logging |
|---------|----------|------------|---------|
| Console app | Development, testing | Manual | stdout/stderr |
| Windows Service | Windows production | Yes (Windows SCM) | EventLog + file |
| Linux Daemon | Linux production | Yes (systemd) | syslog + file |
| Docker container | Cloud deployment | ENTRYPOINT | stdout (captured) |

**Service Installation Pattern** (Windows):

```pascal
// ✅ DO: Use TDDDDaemon for Windows services
uses mormot.app;

// In your service initialization
if TDDDDaemon.IsService then
  // Running as Windows Service - minimal console output
  gApplicationLog := TFileLogger.Create('C:\Logs\myapp.log')
else
  // Running as console - output to console
  gApplicationLog := TConsoleLogger.Create;

// Service auto-restarts on crash if configured properly
// Graceful shutdown via SCM (Service Control Manager)

// ❌ DON'T: Manual service implementation
// Windows services are finicky - use framework helpers
// Manual approach = crashes, hangs, unclean shutdowns
```

**Graceful Shutdown** (let SCM control):

```pascal
// ✅ DO: Respond to service shutdown signals
procedure MyApp.OnServiceStop;
begin
  // Framework calls this when Windows SCM requests stop
  // Close connections, flush logs, clean up
  Server.Shutdown;
  Model.Free;
end;

// ❌ DON'T: Catch SIGTERM manually
// Let the framework handle it - it knows Windows rules
```

## Testing Strategies

**Console mode** for development:
```bash
# Run interactively with verbose logging
./mydaemon --console --verbose

# On Windows
mydaemon.exe /console /verbose
```

**Service mode** for production:
```bash
# Linux
./mydaemon --run      # Run as daemon (background)
./mydaemon --state    # Check status
./mydaemon --kill     # Stop daemon

# Windows
mydaemon.exe /install
net start MyService
mydaemon.exe /state
net stop MyService
mydaemon.exe /uninstall
```

## Examples

See `/mnt/w/mORMot2/ex/ThirdPartyDemos/martin-doyle/05-HttpDaemonORM/` for a complete working example of HTTP microservice as daemon.

## Documentation

**📖 SAD Chapters**:
- [Chapter 20: Application Servers](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-20.md) - Architecture, TSynDaemon, Angelize, deployment
- [Chapter 25: Testing and Logging](/mnt/w/mORMot2/DOCS/mORMot2-SAD-Chapter-25.md) - TSynLog integration patterns

---

**Last Updated:** 2025-12-20
**mORMot Version:** 2.x
**License:** MPL/GPL/LGPL
