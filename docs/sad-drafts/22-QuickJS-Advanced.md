# QuickJS Advanced APIs - Chapter 22 Expansions

*Advanced Memory Management, Class Integration, and Execution Control*

This document provides expanded sections for Chapter 22 (Scripting Engine) covering advanced QuickJS functionality extracted from `mormot.lib.quickjs.pas` and `mormot.script.core.pas`.

---

## 22.3.4. Memory Optimization

### Runtime Memory Configuration

QuickJS provides fine-grained memory control at the runtime level:

```pascal
uses
  mormot.lib.quickjs;

var
  Runtime: JSRuntime;
begin
  Runtime := JS_NewRuntime;
  try
    // Set memory limit (64MB)
    JS_SetMemoryLimit(Runtime, 64 * 1024 * 1024);

    // Set GC threshold (trigger GC when memory exceeds 8MB)
    JS_SetGCThreshold(Runtime, 8 * 1024 * 1024);

    // Create context with custom stack size
    Context := JS_NewContext(Runtime);
    JS_SetMaxStackSize(Context, 512 * 1024);  // 512KB stack
  finally
    JS_FreeContext(Context);
    JS_FreeRuntime(Runtime);
  end;
end;
```

### Custom Memory Allocators

For advanced scenarios, replace QuickJS memory allocator:

```pascal
type
  TCustomAllocator = record
    State: JSMallocState;
    TotalAllocated: PtrUInt;
    AllocationCount: integer;
  end;
  PCustomAllocator = ^TCustomAllocator;

function CustomMalloc(s: PJSMallocState; size: PtrUInt): pointer; cdecl;
var
  Alloc: PCustomAllocator;
begin
  Alloc := PCustomAllocator(s.opaque);
  Inc(Alloc.TotalAllocated, size);
  Inc(Alloc.AllocationCount);
  GetMem(result, size);
end;

procedure CustomFree(s: PJSMallocState; ptr: pointer); cdecl;
var
  Alloc: PCustomAllocator;
begin
  if ptr = nil then
    Exit;
  Alloc := PCustomAllocator(s.opaque);
  Dec(Alloc.AllocationCount);
  FreeMem(ptr);
end;

function CustomRealloc(s: PJSMallocState; ptr: pointer; size: PtrUInt): pointer; cdecl;
begin
  ReallocMem(ptr, size);
  result := ptr;
end;

function CustomMallocUsableSize(ptr: pointer): PtrUInt; cdecl;
begin
  result := MemSize(ptr);
end;

var
  Runtime: JSRuntime;
  MallocFuncs: JSMallocFunctions;
  Allocator: TCustomAllocator;
begin
  // Initialize custom allocator
  FillChar(Allocator, SizeOf(Allocator), 0);
  Allocator.State.opaque := @Allocator;

  // Configure malloc functions
  MallocFuncs.js_malloc := @CustomMalloc;
  MallocFuncs.js_free := @CustomFree;
  MallocFuncs.js_realloc := @CustomRealloc;
  MallocFuncs.js_malloc_usable_size := @CustomMallocUsableSize;

  // Create runtime with custom allocator
  Runtime := JS_NewRuntime2(@MallocFuncs, @Allocator);
  try
    // Runtime now uses custom memory allocator
    Context := JS_NewContext(Runtime);
    // ...
  finally
    JS_FreeContext(Context);
    JS_FreeRuntime(Runtime);
  end;
end;
```

### Memory Usage Monitoring

Track memory consumption in real-time:

```pascal
uses
  mormot.lib.quickjs,
  mormot.core.base;

procedure DumpMemoryUsage(Runtime: JSRuntime);
var
  Usage: JSMemoryUsage;
begin
  JS_ComputeMemoryUsage(Runtime, @Usage);

  ConsoleWrite('Memory Statistics:'#10);
  ConsoleWrite(FormatUtf8('  malloc_size: % bytes'#10, [Usage.malloc_size]));
  ConsoleWrite(FormatUtf8('  malloc_count: %'#10, [Usage.malloc_count]));
  ConsoleWrite(FormatUtf8('  memory_used_size: % bytes'#10, [Usage.memory_used_size]));
  ConsoleWrite(FormatUtf8('  atom_count: %'#10, [Usage.atom_count]));
  ConsoleWrite(FormatUtf8('  str_count: %'#10, [Usage.str_count]));
  ConsoleWrite(FormatUtf8('  obj_count: %'#10, [Usage.obj_count]));
  ConsoleWrite(FormatUtf8('  array_count: %'#10, [Usage.array_count]));
  ConsoleWrite(FormatUtf8('  fast_array_count: %'#10, [Usage.fast_array_count]));
  ConsoleWrite(FormatUtf8('  js_func_count: %'#10, [Usage.js_func_count]));
  ConsoleWrite(FormatUtf8('  js_func_code_size: % bytes'#10, [Usage.js_func_code_size]));
  ConsoleWrite(FormatUtf8('  c_func_count: %'#10, [Usage.c_func_count]));
end;

// Example: Monitor memory during execution
var
  Runtime: JSRuntime;
  Context: JSContext;
  Script: RawUtf8;
  Result: JSValue;
begin
  Runtime := JS_NewRuntime;
  Context := JS_NewContext(Runtime);
  try
    // Before execution
    DumpMemoryUsage(Runtime);

    Script := 'let arr = []; for (let i = 0; i < 10000; i++) arr.push(i);';
    Result := JS_Eval(Context, pointer(Script), length(Script), 'test.js',
                      JS_EVAL_TYPE_GLOBAL);
    JSValue(Result).Free(Context);

    // After execution
    DumpMemoryUsage(Runtime);

    // Force garbage collection
    JS_RunGC(Runtime);

    // After GC
    DumpMemoryUsage(Runtime);
  finally
    JS_FreeContext(Context);
    JS_FreeRuntime(Runtime);
  end;
end;
```

### Garbage Collection Control

```pascal
// Manual garbage collection
JS_RunGC(Runtime);

// Check if object is still alive
if JS_IsLiveObject(Runtime, obj) then
  WriteLn('Object is live');

// Mark values for GC (advanced usage in custom classes)
JS_MarkValue(Runtime, val, @MarkFunc);
```

### Intrinsic Selection for Minimal Footprint

Save memory by selectively enabling JavaScript features:

```pascal
var
  Runtime: JSRuntime;
  Context: JSContext;
begin
  Runtime := JS_NewRuntime;

  // Create raw context (no intrinsics)
  Context := JS_NewContextRaw(Runtime);

  // Add only required intrinsics
  JS_AddIntrinsicBaseObjects(Context);  // Object, Function, Error
  JS_AddIntrinsicDate(Context);         // Date object
  JS_AddIntrinsicJSON(Context);         // JSON.parse/stringify
  JS_AddIntrinsicRegExp(Context);       // Regular expressions
  JS_AddIntrinsicPromise(Context);      // Promises

  // Optional: Advanced features
  // JS_AddIntrinsicProxy(Context);      // Proxy/Reflect
  // JS_AddIntrinsicMapSet(Context);     // Map/Set/WeakMap/WeakSet
  // JS_AddIntrinsicTypedArrays(Context); // Uint8Array, etc.
  // JS_AddIntrinsicBigInt(Context);     // BigInt support

  // Context ready with minimal footprint
end;
```

**Memory Savings:**

| Configuration | Approximate Memory | Use Case |
|---------------|-------------------|----------|
| Full intrinsics (default) | ~600KB | General scripting |
| Base + JSON + Date | ~300KB | Data processing |
| Base only | ~150KB | Minimal embedded |

---

## 22.7.4. Native Class Integration

### Creating Custom JavaScript Classes

Expose Pascal classes to JavaScript with full lifecycle management:

```pascal
type
  // Pascal class to expose
  TCustomer = class
  private
    fID: Int64;
    fName: RawUtf8;
    fBalance: Currency;
  public
    constructor Create(const aName: RawUtf8);
    destructor Destroy; override;
    procedure AddFunds(Amount: Currency);
    function ToString: RawUtf8;
    property ID: Int64 read fID write fID;
    property Name: RawUtf8 read fName write fName;
    property Balance: Currency read fBalance;
  end;

var
  CustomerClassID: JSClassID = 0;

// Finalizer called when JavaScript object is garbage collected
procedure CustomerFinalizer(rt: JSRuntime; val: JSValueRaw); cdecl;
var
  Customer: TCustomer;
begin
  Customer := TCustomer(JS_GetOpaque(val, CustomerClassID));
  if Customer <> nil then
    Customer.Free;
end;

// Constructor function
function JS_CustomerConstructor(ctx: JSContext; this_val: JSValueConst;
  argc: integer; argv: PJSValueConstArr): JSValueRaw; cdecl;
var
  Name: RawUtf8;
  Customer: TCustomer;
  Obj: JSValueRaw;
begin
  // Get name argument
  if argc > 0 then
  begin
    TJSContext(ctx).ToUtf8(JSValue(argv[0]), Name);
    Customer := TCustomer.Create(Name);
  end
  else
    Customer := TCustomer.Create('');

  // Create JavaScript object with class prototype
  Obj := JS_NewObjectProtoClass(ctx, JS_GetClassProto(ctx, CustomerClassID),
                                 CustomerClassID);

  // Attach Pascal object to JS object
  JS_SetOpaque(Obj, Customer);

  result := Obj;
end;

// Getter for 'name' property
function JS_CustomerGetName(ctx: JSContext; this_val: JSValueConst): JSValueRaw; cdecl;
var
  Customer: TCustomer;
begin
  Customer := TCustomer(JS_GetOpaque2(ctx, this_val, CustomerClassID));
  if Customer = nil then
    result := JS_EXCEPTION
  else
    result := TJSContext(ctx).From(Customer.Name).Raw;
end;

// Setter for 'name' property
function JS_CustomerSetName(ctx: JSContext; this_val: JSValueConst;
  val: JSValueConst): JSValueRaw; cdecl;
var
  Customer: TCustomer;
begin
  Customer := TCustomer(JS_GetOpaque2(ctx, this_val, CustomerClassID));
  if Customer = nil then
  begin
    result := JS_EXCEPTION;
    Exit;
  end;

  TJSContext(ctx).ToUtf8(JSValue(val), Customer.fName);
  result := JS_UNDEFINED;
end;

// Method: addFunds(amount)
function JS_CustomerAddFunds(ctx: JSContext; this_val: JSValueConst;
  argc: integer; argv: PJSValueConstArr): JSValueRaw; cdecl;
var
  Customer: TCustomer;
  Amount: double;
begin
  Customer := TCustomer(JS_GetOpaque2(ctx, this_val, CustomerClassID));
  if (Customer = nil) or (argc < 1) then
  begin
    result := JS_EXCEPTION;
    Exit;
  end;

  if JS_ToFloat64(ctx, @Amount, argv[0]) = 0 then
  begin
    Customer.AddFunds(Amount);
    result := JS_UNDEFINED;
  end
  else
    result := JS_EXCEPTION;
end;

// Method: toString()
function JS_CustomerToString(ctx: JSContext; this_val: JSValueConst;
  argc: integer; argv: PJSValueConstArr): JSValueRaw; cdecl;
var
  Customer: TCustomer;
begin
  Customer := TCustomer(JS_GetOpaque2(ctx, this_val, CustomerClassID));
  if Customer = nil then
    result := JS_EXCEPTION
  else
    result := TJSContext(ctx).From(Customer.ToString).Raw;
end;

// Register the class
procedure RegisterCustomerClass(Runtime: JSRuntime; Context: JSContext);
var
  ClassDef: JSClassDef;
  Proto: JSValueRaw;
  Ctor: JSValueRaw;
  Methods: array[0..3] of JSCFunctionListEntry;
begin
  // Create unique class ID
  JS_NewClassID(@CustomerClassID);

  // Define class metadata
  FillChar(ClassDef, SizeOf(ClassDef), 0);
  ClassDef.class_name := 'Customer';
  ClassDef.finalizer := @CustomerFinalizer;

  // Register class
  JS_NewClass(Runtime, CustomerClassID, @ClassDef);

  // Create prototype with methods
  Proto := JS_NewObject(Context);

  Methods[0] := JS_CGETSET_DEF('name', @JS_CustomerGetName, @JS_CustomerSetName);
  Methods[1] := JS_CFUNC_DEF('addFunds', 1, @JS_CustomerAddFunds);
  Methods[2] := JS_CFUNC_DEF('toString', 0, @JS_CustomerToString);
  Methods[3] := JS_PROP_INT32_DEF('type', 1, JS_PROP_CONFIGURABLE);

  JS_SetPropertyFunctionList(Context, Proto, @Methods[0], Length(Methods));
  JS_SetClassProto(Context, CustomerClassID, Proto);

  // Create constructor function
  Ctor := JS_NewCFunction2(Context, @JS_CustomerConstructor, 'Customer',
                           1, JS_CFUNC_constructor, 0);

  // Set constructor.prototype
  JS_SetConstructor(Context, Ctor, Proto);

  // Add to global object
  JS_SetPropertyStr(Context, JS_GetGlobalObject(Context), 'Customer', Ctor);
end;

// Usage example
var
  Runtime: JSRuntime;
  Context: JSContext;
  Script: RawUtf8;
  Result: JSValue;
begin
  Runtime := JS_NewRuntime;
  Context := JS_NewContext(Runtime);
  try
    RegisterCustomerClass(Runtime, Context);

    Script := '''
      let customer = new Customer("Alice");
      customer.addFunds(1000);
      console.log(customer.toString());
    ''';

    Result := JS_Eval(Context, pointer(Script), length(Script),
                      'test.js', JS_EVAL_TYPE_GLOBAL);
    JSValue(Result).Free(Context);
  finally
    JS_FreeContext(Context);
    JS_FreeRuntime(Runtime);
  end;
end;
```

### Property Descriptors and Getters/Setters

Define properties with advanced features:

```pascal
// Define property with getter/setter and flags
function DefineAccessorProperty(ctx: JSContext; obj: JSValueRaw;
  const PropName: RawUtf8; Getter, Setter: JSCFunction): boolean;
var
  Atom: JSAtom;
  GetterFunc, SetterFunc: JSValueRaw;
begin
  Atom := JS_NewAtom(ctx, pointer(PropName));
  try
    if Assigned(Getter) then
      GetterFunc := JS_NewCFunction(ctx, @Getter, 'get', 0)
    else
      GetterFunc := JS_UNDEFINED;

    if Assigned(Setter) then
      SetterFunc := JS_NewCFunction(ctx, @Setter, 'set', 1)
    else
      SetterFunc := JS_UNDEFINED;

    result := JS_DefinePropertyGetSet(ctx, obj, Atom, GetterFunc, SetterFunc,
                JS_PROP_CONFIGURABLE or JS_PROP_ENUMERABLE) >= 0;
  finally
    JS_FreeAtom(ctx, Atom);
  end;
end;

// Define property with custom flags
function DefineDataProperty(ctx: JSContext; obj: JSValueRaw;
  const PropName: RawUtf8; Value: JSValueRaw; Flags: integer): boolean;
var
  Atom: JSAtom;
begin
  Atom := JS_NewAtom(ctx, pointer(PropName));
  try
    result := JS_DefinePropertyValue(ctx, obj, Atom, Value, Flags) >= 0;
  finally
    JS_FreeAtom(ctx, Atom);
  end;
end;

// Example: Read-only property
DefineDataProperty(ctx, obj, 'version',
  TJSContext(ctx).From('1.0.0').Raw,
  JS_PROP_ENUMERABLE);  // Not writable, not configurable
```

### Exotic Objects and Proxies

Create objects with custom behavior:

```pascal
// Enable Proxy support
JS_AddIntrinsicProxy(Context);

// Create proxy that logs all property access
Script := '''
  let handler = {
    get: function(target, prop) {
      console.log("Getting property: " + prop);
      return target[prop];
    },
    set: function(target, prop, value) {
      console.log("Setting property: " + prop + " = " + value);
      target[prop] = value;
      return true;
    }
  };

  let target = { name: "Alice" };
  let proxy = new Proxy(target, handler);

  proxy.name;        // Logs: Getting property: name
  proxy.age = 30;    // Logs: Setting property: age = 30
''';
```

---

## 22.11.3. Custom Memory Management

### Context Lifecycle Management

Proper management of QuickJS contexts with opaque data:

```pascal
type
  TContextData = record
    UserID: integer;
    SessionID: RawUtf8;
    StartTime: TDateTime;
  end;
  PContextData = ^TContextData;

procedure SetupContext(Context: JSContext; UserID: integer;
  const SessionID: RawUtf8);
var
  Data: PContextData;
begin
  New(Data);
  Data.UserID := UserID;
  Data.SessionID := SessionID;
  Data.StartTime := Now;

  // Attach to context
  JS_SetContextOpaque(Context, Data);
end;

procedure CleanupContext(Context: JSContext);
var
  Data: PContextData;
begin
  Data := JS_GetContextOpaque(Context);
  if Data <> nil then
  begin
    Dispose(Data);
    JS_SetContextOpaque(Context, nil);
  end;
end;

// Usage in native function
function JS_GetCurrentUser(ctx: JSContext; this_val: JSValueConst;
  argc: integer; argv: PJSValueConstArr): JSValueRaw; cdecl;
var
  Data: PContextData;
begin
  Data := JS_GetContextOpaque(ctx);
  if Data = nil then
    result := JS_EXCEPTION
  else
    result := JS_NewInt32(ctx, Data.UserID);
end;
```

### Runtime Opaque Data

Store runtime-level data accessible from all contexts:

```pascal
type
  TRuntimeConfig = record
    MaxScriptDuration: integer;
    AllowedAPIs: set of (apiFile, apiNetwork, apiDatabase);
    LogLevel: integer;
  end;
  PRuntimeConfig = ^TRuntimeConfig;

var
  Runtime: JSRuntime;
  Config: TRuntimeConfig;
begin
  Runtime := JS_NewRuntime;

  // Configure runtime
  Config.MaxScriptDuration := 5000;
  Config.AllowedAPIs := [apiDatabase];
  Config.LogLevel := 2;

  JS_SetRuntimeOpaque(Runtime, @Config);

  // Later retrieve from native function
  // Config := PRuntimeConfig(JS_GetRuntimeOpaque(JS_GetRuntime(ctx)));
end;
```

### Safe Runtime Shutdown

Ensure clean shutdown with leak detection:

```pascal
function SafeShutdownRuntime(var Runtime: JSRuntime): RawUtf8;
var
  Usage: JSMemoryUsage;
begin
  result := '';

  if Runtime = nil then
    Exit;

  try
    // Check for leaks before shutdown
    JS_ComputeMemoryUsage(Runtime, @Usage);

    if Usage.obj_count > 0 then
      result := FormatUtf8('Warning: % objects not freed', [Usage.obj_count]);

    // Force GC
    JS_RunGC(Runtime);

    // Re-check
    JS_ComputeMemoryUsage(Runtime, @Usage);
    if Usage.obj_count > 0 then
      result := FormatUtf8('Error: % objects leaked', [Usage.obj_count]);
  finally
    JS_FreeRuntime(Runtime);
    Runtime := nil;
  end;
end;

// Usage
var
  LeakReport: RawUtf8;
begin
  LeakReport := SafeShutdownRuntime(Runtime);
  if LeakReport <> '' then
    WriteLn('Shutdown: ', LeakReport);
end;
```

---

## 22.11.4. Execution Control

### Interrupt Handlers

Implement execution timeouts and cancellation:

```pascal
type
  TInterruptData = record
    StartTime: Int64;
    TimeoutMs: integer;
    Cancelled: boolean;
  end;
  PInterruptData = ^TInterruptData;

// Interrupt callback - return non-zero to stop execution
function InterruptHandler(rt: JSRuntime; opaque: pointer): integer; cdecl;
var
  Data: PInterruptData;
  Elapsed: Int64;
begin
  Data := PInterruptData(opaque);

  // Check cancellation flag
  if Data.Cancelled then
  begin
    result := 1;  // Stop execution
    Exit;
  end;

  // Check timeout
  Elapsed := GetTickCount64 - Data.StartTime;
  if Elapsed > Data.TimeoutMs then
    result := 1   // Timeout exceeded
  else
    result := 0;  // Continue execution
end;

// Execute script with timeout
function EvalWithTimeout(Context: JSContext; const Script: RawUtf8;
  TimeoutMs: integer): JSValue;
var
  Runtime: JSRuntime;
  Data: TInterruptData;
begin
  Runtime := JS_GetRuntime(Context);

  // Setup interrupt handler
  Data.StartTime := GetTickCount64;
  Data.TimeoutMs := TimeoutMs;
  Data.Cancelled := false;

  JS_SetInterruptHandler(Runtime, @InterruptHandler, @Data);
  try
    result := JS_Eval(Context, pointer(Script), length(Script),
                      'script.js', JS_EVAL_TYPE_GLOBAL);

    if JSValue(result).IsException then
    begin
      if Data.Cancelled or ((GetTickCount64 - Data.StartTime) > TimeoutMs) then
        raise ETimeout.Create('Script execution timeout');
    end;
  finally
    JS_SetInterruptHandler(Runtime, nil, nil);
  end;
end;

// Cancellable execution from another thread
var
  InterruptData: TInterruptData;
  ScriptThread: TThread;
begin
  InterruptData.StartTime := GetTickCount64;
  InterruptData.TimeoutMs := 30000;
  InterruptData.Cancelled := false;

  ScriptThread := TThread.CreateAnonymousThread(
    procedure
    var
      Runtime: JSRuntime;
      Context: JSContext;
    begin
      Runtime := JS_NewRuntime;
      Context := JS_NewContext(Runtime);
      try
        JS_SetInterruptHandler(Runtime, @InterruptHandler, @InterruptData);
        // Execute long-running script...
      finally
        JS_FreeContext(Context);
        JS_FreeRuntime(Runtime);
      end;
    end);

  ScriptThread.Start;

  // Later, from UI thread:
  // InterruptData.Cancelled := true;  // Cancels script execution
end;
```

### Stack Size Limits

Prevent stack overflow from recursive scripts:

```pascal
var
  Context: JSContext;
begin
  Context := JS_NewContext(Runtime);

  // Set maximum stack size (256KB)
  JS_SetMaxStackSize(Context, 256 * 1024);

  // This will throw error if exceeded
  Script := '''
    function recursive(n) {
      return recursive(n + 1);  // Stack overflow
    }
    recursive(0);
  ''';

  Result := JS_Eval(Context, pointer(Script), length(Script),
                    'test.js', JS_EVAL_TYPE_GLOBAL);
  // Result.IsException = true (RangeError: Maximum call stack size exceeded)
end;
```

### Async Job Execution

Handle promises and async operations:

```pascal
// Enable promises
JS_AddIntrinsicPromise(Context);

// Execute pending jobs (for async/await)
function ExecutePendingJobs(Runtime: JSRuntime; Context: JSContext): boolean;
var
  PendingContext: JSContext;
  Err: integer;
begin
  result := true;
  while JS_IsJobPending(Runtime) do
  begin
    Err := JS_ExecutePendingJob(Runtime, @PendingContext);
    if Err < 0 then
    begin
      if PendingContext <> nil then
        TJSContext(PendingContext).ErrorDump(true);
      result := false;
      Break;
    end;
  end;
end;

// Example: Execute async script
Script := '''
  async function fetchData() {
    await new Promise(resolve => setTimeout(resolve, 100));
    return { data: "result" };
  }

  fetchData().then(result => console.log(result));
''';

Result := JS_Eval(Context, pointer(Script), length(Script),
                  'async.js', JS_EVAL_TYPE_GLOBAL);
JSValue(Result).Free(Context);

// Process all pending promises
ExecutePendingJobs(Runtime, Context);
```

---

## 22.11.5. Module System

### ES6 Module Loading

Implement module loader for `import`/`export`:

```pascal
type
  TModuleLoader = class
  private
    fModulePath: RawUtf8;
  public
    constructor Create(const BasePath: RawUtf8);
    function NormalizeModuleName(ctx: JSContext;
      const BaseName, ModuleName: RawUtf8): RawUtf8;
    function LoadModule(ctx: JSContext;
      const ModuleName: RawUtf8): JSModuleDef;
  end;

// Module name normalizer
function ModuleNormalize(ctx: JSContext; const module_base_name,
  module_name: PAnsiChar; opaque: pointer): PAnsiChar; cdecl;
var
  Loader: TModuleLoader;
  Normalized: RawUtf8;
begin
  Loader := TModuleLoader(opaque);
  Normalized := Loader.NormalizeModuleName(ctx,
    RawUtf8(module_base_name), RawUtf8(module_name));
  result := js_strdup(ctx, pointer(Normalized));
end;

// Module loader
function ModuleLoad(ctx: JSContext; module_name: PAnsiChar;
  opaque: pointer): JSModuleDef; cdecl;
var
  Loader: TModuleLoader;
begin
  Loader := TModuleLoader(opaque);
  result := Loader.LoadModule(ctx, RawUtf8(module_name));
end;

constructor TModuleLoader.Create(const BasePath: RawUtf8);
begin
  inherited Create;
  fModulePath := IncludeTrailingPathDelimiter(BasePath);
end;

function TModuleLoader.NormalizeModuleName(ctx: JSContext;
  const BaseName, ModuleName: RawUtf8): RawUtf8;
begin
  // Simple resolution: relative to base path
  if (ModuleName <> '') and (ModuleName[1] = '.') then
    result := fModulePath + ExtractFileName(ModuleName)
  else
    result := ModuleName;
end;

function TModuleLoader.LoadModule(ctx: JSContext;
  const ModuleName: RawUtf8): JSModuleDef;
var
  FileName, Source: RawUtf8;
  Func: JSValueRaw;
begin
  FileName := fModulePath + ModuleName;
  Source := StringFromFile(FileName);

  // Compile module
  Func := JS_Eval(ctx, pointer(Source), length(Source),
                  pointer(ModuleName), JS_EVAL_FLAG_MODULE_COMPILE_ONLY);

  if JSValue(Func).IsException then
  begin
    result := nil;
    Exit;
  end;

  // Return module definition
  result := JSModuleDef(JSValue(Func).Ptr);
end;

// Register module loader
var
  Runtime: JSRuntime;
  Context: JSContext;
  Loader: TModuleLoader;
begin
  Runtime := JS_NewRuntime;
  Context := JS_NewContext(Runtime);

  Loader := TModuleLoader.Create('/path/to/modules/');
  try
    JS_SetModuleLoaderFunc(Runtime, @ModuleNormalize, @ModuleLoad, Loader);

    // Now can use ES6 imports
    Script := '''
      import { helper } from './utils.js';
      console.log(helper());
    ''';

    Result := TJSContext(Context).EvalModule(Script, 'main.js');
  finally
    Loader.Free;
  end;
end;
```

### Precompiled Bytecode

Compile scripts to bytecode for faster loading:

```pascal
// Compile to bytecode
function CompileToByteCode(Context: JSContext; const Source: RawUtf8;
  const FileName: RawUtf8): RawByteString;
var
  Func: JSValueRaw;
  ByteCode: PByte;
  Size: PtrUInt;
begin
  // Compile script
  Func := JS_Eval(Context, pointer(Source), length(Source),
                  pointer(FileName), JS_EVAL_FLAG_COMPILE_ONLY);

  if JSValue(Func).IsException then
    raise EJSException.Create(@Context, 'CompileToByteCode');

  try
    // Write to bytecode
    ByteCode := JS_WriteObject(Context, @Size, Func,
                                JS_WRITE_OBJ_BYTECODE);
    if ByteCode = nil then
      raise EJSException.Create(@Context, 'JS_WriteObject');

    try
      SetLength(result, Size);
      Move(ByteCode^, pointer(result)^, Size);
    finally
      js_free(Context, ByteCode);
    end;
  finally
    JS_FreeValue(Context, Func);
  end;
end;

// Load from bytecode
function LoadFromByteCode(Context: JSContext;
  const ByteCode: RawByteString): JSValue;
begin
  result := JS_ReadObject(Context, pointer(ByteCode), length(ByteCode),
                          JS_READ_OBJ_BYTECODE);

  if JSValue(result).IsException then
    raise EJSException.Create(@Context, 'LoadFromByteCode');
end;

// Execute bytecode
function ExecuteByteCode(Context: JSContext; const Func: JSValue): JSValue;
begin
  result := JS_EvalFunction(Context, JSValue(Func).Raw);
end;

// Complete example
var
  Source, FileName: RawUtf8;
  ByteCode: RawByteString;
  Func, Result: JSValue;
begin
  Source := 'function greet(name) { return "Hello, " + name; }';
  FileName := 'greet.js';

  // Compile and save
  ByteCode := CompileToByteCode(Context, Source, FileName);
  FileFromString(ByteCode, 'greet.bin');

  // Later: Load and execute
  ByteCode := StringFromFile('greet.bin');
  Func := LoadFromByteCode(Context, ByteCode);
  Result := ExecuteByteCode(Context, Func);
  JSValue(Result).Free(Context);
end;
```

### C Module Registration

Register native modules that can be imported:

```pascal
// Initialize C module
function InitMyModule(ctx: JSContext; m: JSModuleDef): integer; cdecl;
var
  Methods: array[0..2] of JSCFunctionListEntry;
begin
  // Define module exports
  Methods[0] := JS_CFUNC_DEF('add', 2, @JS_Add);
  Methods[1] := JS_CFUNC_DEF('multiply', 2, @JS_Multiply);
  Methods[2] := JS_PROP_STRING_DEF('version', '1.0', JS_PROP_CONFIGURABLE);

  result := JS_SetModuleExportList(ctx, m, @Methods[0], Length(Methods));
end;

// Register module
procedure RegisterNativeModule(Context: JSContext);
var
  Module: JSModuleDef;
begin
  Module := JS_NewCModule(Context, 'math', @InitMyModule);

  // Declare exports
  JS_AddModuleExport(Context, Module, 'add');
  JS_AddModuleExport(Context, Module, 'multiply');
  JS_AddModuleExport(Context, Module, 'version');
end;

// Usage from JavaScript
Script := '''
  import { add, multiply, version } from 'math';
  console.log(add(5, 3));        // 8
  console.log(multiply(5, 3));   // 15
  console.log(version);          // "1.0"
''';
```

---

## Best Practices Summary

### Memory Management

| Practice | Recommendation |
|----------|----------------|
| **GC Threshold** | Set to ~10% of memory limit for frequent collection |
| **Stack Size** | 256KB-512KB sufficient for most scripts |
| **Custom Allocators** | Use only for specialized scenarios (debugging, sandboxing) |
| **Memory Monitoring** | Call `JS_ComputeMemoryUsage()` after GC in production |

### Class Integration

| Pattern | When to Use |
|---------|-------------|
| **Full Class Binding** | Complex objects with many methods |
| **Property Descriptors** | Read-only/computed properties |
| **Opaque Pointers** | Wrap existing Pascal objects |
| **Finalizers** | Always implement for cleanup |

### Execution Control

| Feature | Use Case |
|---------|----------|
| **Interrupt Handlers** | User scripts, untrusted code |
| **Stack Limits** | Prevent recursive bombs |
| **Job Queue** | Async/await, Promises |
| **Timeouts** | Long-running operations |

### Module System

| Approach | Scenario |
|----------|----------|
| **File-based Modules** | Development, hot reload |
| **Bytecode Caching** | Production, fast startup |
| **C Modules** | Native API exposure |
| **Virtual Modules** | Embedded resources |

---

## Related Documentation

- **Chapter 22.3**: QuickJS Integration (basic usage)
- **Chapter 22.4**: Thread-Safe Engine Management
- **Chapter 22.11**: Best Practices (security, error handling)
- **mormot.lib.quickjs.pas**: Complete API reference
- **mormot.script.core.pas**: Abstract engine framework

---

**Status**: Draft for integration into Chapter 22
**Last Updated**: 2024-12-20
