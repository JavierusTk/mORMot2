{
  This file is a part of the Open Source Synopse mORMot framework 2,
  licensed under a MPL/GPL/LGPL three license - see LICENSE.md

  POSIX Sockets API calls via the libc, as used by mormot.net.sock.pas
}

uses
  baseunix,
  termio,
  sockets,
  {$ifdef OSLINUXANDROID}
  linux, // for epoll
  {$endif OSLINUXANDROID}
  initc; // link clib='c' for our socket purpose

{$ifdef OSBSDDARWIN}
  {$define USEPOLL} // epoll API is Linux-specific -> force use poll() on BSD
{$endif OSBSDDARWIN}


{ ******** Sockets Type Definitions }

const
  NI_MAXHOST = 1025;
  NI_MAXSERV = 32;

  NO_ERROR = 0;
  WSATRY_AGAIN = ESysEAGAIN;
  WSAEADDRNOTAVAIL = ESysEADDRNOTAVAIL;
  WSAECONNABORTED = ESysECONNABORTED;
  WSAETIMEDOUT = ESysETIMEDOUT;
  WSAEMFILE = ESysEMFILE;
  WSAECONNREFUSED = ESysECONNREFUSED;

  {$ifdef OSFREEBSD}
  NI_NUMERICHOST = 2;
  NI_NUMERICSERV = 8;
  {$else}
  NI_NUMERICHOST = 1;
  NI_NUMERICSERV = 2;
  {$endif OSFREEBSD}

  FIONREAD = termio.FIONREAD;
  FIONBIO  = termio.FIONBIO;

  {$ifdef OSBSDDARWIN}

    {$ifndef OSOPENBSD}
    {$ifdef OSDARWIN}
    SO_NOSIGPIPE = $1022;
    {$else}
    SO_NOSIGPIPE = $800;
    {$endif OSDARWIN}
    {$endif OSOPENBSD}

    {$ifndef OSOPENBSD}
    // Works under MAC OS X and FreeBSD, but is undocumented, so FPC doesn't include it
    MSG_NOSIGNAL  = $20000;  // Do not generate SIGPIPE.
    {$else}
    MSG_NOSIGNAL  = $400;
    {$endif OSOPENBSD}
  {$else}
    {$ifdef SUNOS}
    MSG_NOSIGNAL = $20000;  // Do not generate SIGPIPE.
    {$else}
    MSG_NOSIGNAL = sockets.MSG_NOSIGNAL; // Do not generate SIGPIPE
    {$endif SUNOS}

  {$endif OSBSDDARWIN}

  _ST: array[TNetLayer] of integer = (
    SOCK_STREAM,
    SOCK_DGRAM,
    SOCK_STREAM);

  _IP: array[TNetLayer] of integer = (
    IPPROTO_TCP,
    IPPROTO_UDP,
    0);

{$packrecords C}

type
  // sockets.pp forced longint for Kylix compatibility
  TSocket = PtrInt;

  THostEnt = record
    h_name: PAnsiChar;
    h_aliases: PPAnsiChar;
    h_addrtype: integer;
    h_length: socklen_t;
    case integer of
       0: (
          h_addr_list: PPAnsiChar);
       1: (
          h_addr: ^pin_addr);
       2: (
          h_addr6: ^pin6_addr);
  end;
  PHostEnt = ^THostEnt;

const
  // poll() flag when there is data to read
  POLLIN       = $001;
  // poll() flag when there is urgent data to read
  POLLPRI      = $002;
  // poll() flag when writing now will not block
  POLLOUT      = $004;
  // poll() flag error condition (always implicitly polled for)
  POLLERR      = $008;
  // poll() flag hung up (always implicitly polled for)
  POLLHUP      = $010;
  // poll() flag invalid polling request (always implicitly polled for)
  POLLNVAL     = $020;
  // poll() flag when normal data may be read
  POLLRDNORM   = $040;
  // poll() flag when priority data may be read
  POLLRDBAND   = $080;
  // poll() flag when writing now will not block
  POLLWRNORM   = $100;
  // poll() flag when priority data may be written
  POLLWRBAND   = $200;
  // poll() flag extension for Linux
  POLLMSG      = $400;

type
  /// polling request data structure for poll()
  TPollFD = record
    /// file descriptor to poll
    fd: integer;
    /// types of events poller cares about
    // - mainly POLLIN and/or POLLOUT
    events: Smallint;
    /// types of events that actually occurred
    // - caller could just reset revents := 0 to reuse the structure
    revents: Smallint;
  end;
  PPollFD = ^TPollFD;


{ ******** Sockets API calls via the libc }

function gethostbyname(name: PAnsiChar): PHostEnt; cdecl;
  external clib name 'gethostbyname';
function socket(af, struct, protocol: integer): TSocket; cdecl;
  external clib name 'socket';
function setsockopt(s: TSocket; level, optname: integer;
   optval: pointer; optlen: integer): integer; cdecl;
  external clib name 'setsockopt';
function ioctlsocket(s: TSocket; cmd: cardinal; arg: PCardinal): integer; cdecl;
  external clib name 'ioctl';
function shutdown(s: TSocket; how: integer): integer; cdecl;
  external clib name 'shutdown';
function closesocket(s: TSocket): integer; cdecl;
  external clib name 'close';
function getnameinfo(addr: PSockAddr; namelen: tsocklen; host: PAnsiChar;
   hostlen: tsocklen; serv: PAnsiChar; servlen: tsocklen; flags: integer): integer; cdecl;
  external clib name 'getnameinfo';
function bind(s: TSocket; addr: PSockAddr; namelen: tsocklen): integer; cdecl;
  external clib name 'bind';
function listen(s: TSocket; backlog: integer): integer; cdecl;
  external clib name 'listen';
function accept(s: TSocket; addr: PSockAddr; var addrlen: tsocklen): integer; cdecl;
  external clib name 'accept';
function connect(s: TSocket; name: PSockAddr; namelen: tsocklen): integer; cdecl;
  external clib name 'connect';
function select(nfds: integer; readfds, writefds, exceptfds: PFDSet;
   timeout: PTimeVal): integer; cdecl;
  external clib name 'select';
function recv(s: TSocket; Buf: Pointer; len: size_t; flags: integer): ssize_t; cdecl;
  external clib name 'recv';
function recvfrom(s: TSocket; Buf: Pointer; len: size_t; flags: integer;
   from: PSockAddr; fromlen: Pinteger): ssize_t; cdecl;
  external clib name 'recvfrom';
function send(s: TSocket; Buf: Pointer; len: size_t; flags: integer): ssize_t; cdecl;
  external clib name 'send';
function sendto(s: TSocket; Buf: Pointer; len: size_t; flags: integer;
   addrto: PSockAddr; tolen: integer): ssize_t; cdecl;
  external clib name 'sendto';
function getpeername(s: TSocket; name: PSockAddr; var namelen: tsocklen): integer; cdecl;
  external clib name 'getpeername';

{$ifdef OSDARWIN}
// circumvent linking issue on Mac OSX
function poll(fds: PPollFD; nfds, timeout: integer): integer; inline;
begin
  result := fppoll(pointer(fds), nfds, timeout);
end;
{$else}
function poll(fds: PPollFD; nfds, timeout: integer): integer; cdecl;
  external clib name 'poll';
{$endif OSDARWIN}

function sockerrno: integer; inline;
begin
  result := cerrno; // from libc
end;


{ ******** TNetSocket Cross-Platform Wrapper }

{ TNetAddr }

function TNetAddr.SetFrom(const address, addrport: RawUtf8;
  layer: TNetLayer): TNetResult;
var
  h: PHostEnt;
  ip4: sockaddr absolute Addr;
begin
  FillCharFast(Addr, SizeOf(Addr), 0);
  result := nrNotFound;
  if address = '' then
    exit;
  // handle domain socket name
  if layer = nlUnix then
  begin
    if length(address) >= SizeOf(psockaddr_un(@Addr)^.sun_path) then
      // avoid buffer overflow
      result := nrFatalError
    else
      with psockaddr_un(@Addr)^ do
      begin
        sun_family := AF_UNIX;
        MoveFast(pointer(address)^, sun_path, length(address));
        result := nrOK;
      end;
    exit;
  end;
  // check supplied port
  ip4.sin_port := htons(GetCardinal(pointer(addrport)));
  if (ip4.sin_port = 0) and
     (addrport <> '0') then // explicit '0' to get ephemeral port
    exit;
  // handle simple cases
  result := nrOk;
  if (address = cLocalhost) or
     (address = c6Localhost) or
     PropNameEquals(address, 'localhost') then
  begin
    ip4.sin_family := AF_INET;
    PCardinal(@ip4.sin_addr)^ := cLocalhost32; // 127.0.0.1
    exit;
  end
  else if (address = cAnyHost) or
          (address = c6AnyHost) then
  begin
    ip4.sin_family := AF_INET; // 0.0.0.0
    exit;
  end;
  // resolve name or IP (getaddrinfo is more recent, but addrinfo is not fixed)
  h := gethostbyname(pointer(address));
  if h = nil then
    result := nrNotFound
  else
  begin
    ip4.sin_family := h^.h_addrtype;
    case h^.h_addrtype of
      AF_INET:
         ip4.sin_addr := h^.h_addr^^;
      AF_INET6:
         psockaddr6(@Addr)^.sin6_addr := h^.h_addr6^^;
      else
        result := nrNotImplemented;
    end;
  end;
end;


{ TNetSocketWrap }

procedure SetTimeVal(ms: PtrUInt; out tv: TTimeVal);
var
  d: PtrUInt;
begin
  if ms = 0 then
  begin
    tv.tv_sec := 0;
    tv.tv_usec := 0;
  end
  else
  begin
    d := ms div 1000;
    tv.tv_sec := d;
    tv.tv_usec := (ms - (d * 1000)) * 1000;
  end;
end;

procedure TNetSocketWrap.SetSendTimeout(ms: integer);
var
  tv: TTimeVal; // POSIX uses a timeval (not Windows)
begin
  SetTimeVal(ms, tv);
  SetOpt(SOL_SOCKET, SO_SNDTIMEO, @tv, SizeOf(tv));
end;

procedure TNetSocketWrap.SetReceiveTimeout(ms: integer);
var
  tv: TTimeVal;
begin
  SetTimeVal(ms, tv);
  SetOpt(SOL_SOCKET, SO_RCVTIMEO, @tv, SizeOf(tv));
end;

procedure TNetSocketWrap.SetLinger(linger: integer);
var
  v: TLinger;
begin
  v.l_linger := linger;
  v.l_onoff := ord(linger >= 0);
  SetOpt(SOL_SOCKET, SO_LINGER, @v, SizeOf(v));
  if linger > 0 then
  begin
    v.l_onoff := ord(true);
    {$ifdef OSBSDDARWIN}
    SetOpt(SOL_SOCKET, SO_REUSEPORT, @v.l_onoff, SizeOf(v.l_onoff));
    {$ifndef OSOPENBSD}
    SetOpt(SOL_SOCKET, SO_NOSIGPIPE, @v.l_onoff, SizeOf(v.l_onoff));
    {$endif OSOPENBSD}
    {$else}
    SetOpt(SOL_SOCKET, SO_REUSEADDR, @v.l_onoff, SizeOf(v.l_onoff));
    {$endif OSBSDDARWIN}
  end;
end;

function TNetSocketWrap.WaitFor(ms: integer; scope: TNetEvents): TNetEvents;
var
  res: integer;
  p: TPollFD; // select() limits process to 1024 sockets in POSIX -> use poll()
  // https://moythreads.com/wordpress/2009/12/22/select-system-call-limitation
begin
  result := [neError];
  if @self = nil then
    exit;
  p.fd := TSocket(@self);
  p.events := 0;
  if neRead in scope then
    p.events := POLLIN or POLLPRI;
  if neWrite in scope then
    p.events := p.events or POLLOUT;
  p.revents := 0;
  res := poll(@p, 1, ms);
  if res < 0 then
  begin
    res := sockerrno;
    if res = WSATRY_AGAIN then
      // timeout, not a true error
      result := [];
    exit;
  end;
  result := [];
  if p.revents and (POLLIN or POLLPRI) <> 0 then
    include(result, neRead);
  if p.revents and POLLOUT <> 0 then
    include(result, neWrite);
  if p.revents and POLLHUP <> 0 then
    include(result, neClosed);
end;


{ ******************** Efficient Multiple Sockets Polling }

{$ifdef USEPOLL}

type
  /// socket polling via the POSIX poll API
  // - direct call of the Linux/POSIX poll() API
  // - note: Windows WSAPoll() API is actually slower than plain Select()
  TPollSocketPoll = class(TPollSocketAbstract)
  protected
    fFD: array of TPollFD; // fd=-1 for ignored fields
    fTags: array of TPollSocketTag;
    fFDCount: integer;
    procedure FDVacuum;
  public
    constructor Create; override;
    function Subscribe(socket: TNetSocket; events: TPollSocketEvents;
      tag: TPollSocketTag): boolean; override;
    function Unsubscribe(socket: TNetSocket): boolean; override;
    function WaitForModified(var results: TPollSocketResults;
      timeoutMS: integer): boolean; override;
  end;

constructor TPollSocketPoll.Create;
begin
  inherited Create;
  fMaxSockets := 20000;
end;

function TPollSocketPoll.Subscribe(socket: TNetSocket; events: TPollSocketEvents;
  tag: TPollSocketTag): boolean;
var
  i, new: PtrInt;
  n, e, fd, s: integer;
begin
  result := false;
  if (self = nil) or
     (socket = nil) or
     (byte(events) = 0) or
     (fCount = fMaxSockets) then
    exit;
  if pseRead in events then
    e := POLLIN or POLLPRI
  else
    e := 0;
  if pseWrite in events then
    e := e or POLLOUT;
  s := PtrUInt(socket);
  new := -1;
  if fFDCount = fCount then
  begin
    // no void entry
    for i := 0 to fFDCount - 1 do
      if fFD[i].fd = s then
        // already subscribed
        exit;
  end
  else
    for i := 0 to fFDCount - 1 do
    begin
      fd := fFD[i].fd;
      if fd = s then
        // already subscribed
        exit;
      if fd < 0 then
        // found a void entry
        new := i;
    end;
  if new < 0 then
  begin
    if fFDCount = length(fFD) then
    begin
      // add new entry to the array
      n := NextGrow(fFDCount);
      if n > fMaxSockets then
        n := fMaxSockets;
      SetLength(fFD, n);
      SetLength(fTags, n);
    end;
    new := fFDCount;
    inc(fFDCount);
  end;
  fTags[new] := tag;
  with fFD[new] do
  begin
    fd := TSocket(socket);
    events := e;
    revents := 0;
  end;
  inc(fCount);
  result := true;
end;

procedure TPollSocketPoll.FDVacuum;
var
  n, i: PtrInt;
begin
  n := 0;
  for i := 0 to fFDCount - 1 do
    if fFD[i].fd > 0 then
    begin
      if i <> n then
      begin
        fFD[n] := fFD[i];
        fTags[n] := fTags[i];
      end;
      inc(n);
    end;
  fFDCount := n;
end;

function TPollSocketPoll.Unsubscribe(socket: TNetSocket): boolean;
var
  i: PtrInt;
  s: integer;
begin
  s := PtrUInt(socket);
  for i := 0 to fFDCount - 1 do
    if fFD[i].fd = s then
    begin
      with fFD[i] do
      begin
        revents := 0; // quickly ignored in WaitForModified()
        fd := -1; // mark entry as void and reusable
      end;
      dec(fCount);
      if fCount <= fFDCount shr 1 then
        FDVacuum; // avoid too many void entries
      result := true;
      exit;
    end;
  result := false;
end;

function TPollSocketPoll.WaitForModified(var results: TPollSocketResults;
  timeoutMS: integer): boolean;
var
  e: TPollSocketEvents;
  i, d: PtrInt;
  ev: integer;
begin
  result := false; // error
  if (self = nil) or
     (fCount = 0) then
    exit;
  results.Count := poll(pointer(fFD), fFDCount, timeoutMS);
  if results.Count <= 0 then
    exit;
  if length(results.Events) < results.Count then
    SetLength(results.Events, fCount);
  d := 0;
  for i := 0 to fFDCount - 1 do
  begin
    ev := fFD[i].revents;
    if (ev <> 0) and
       (fFD[i].fd > 0) then
    begin
      if ev = POLLIN then
        byte(e) := byte([pseRead]) // optimized for the most common case
      else
      begin
        byte(e) := 0;
        if ev and (POLLIN or POLLPRI) <> 0 then
          include(e, pseRead);
        if ev and POLLOUT <> 0 then
          include(e, pseWrite);
        if ev and POLLERR <> 0 then
          include(e, pseError);
        if ev and POLLHUP <> 0 then
          include(e, pseClosed);
      end;
      with results.Events[d] do
      begin
        events := e;
        tag := fTags[i];
      end;
      inc(d);
      fFD[i].revents := 0; // reset result flags for reuse
    end;
  end;
  if d <> results.Count then // paranoid
    raise ENetSock.CreateFmt('TPollSocketPoll: %d<>%d', [results.Count, d]);
  result := true;
end;

function PollSocketClass: TPollSocketClass;
begin
  PollSocketClass := TPollSocketPoll;
end;

{$else}

type
  /// socket polling via Linux epoll optimized API
  // - not available under Windows or BSD/Darwin
  // - direct call of the epoll API in level-triggered (LT) mode
  TPollSocketEpoll = class(TPollSocketAbstract)
  protected
    fEPFD: integer;
    fResults: array of EPoll_Event;
  public
    constructor Create; override;
    destructor Destroy; override;
    // directly calls epoll's EPOLL_CTL_ADD control interface
    function Subscribe(socket: TNetSocket; events: TPollSocketEvents;
      tag: TPollSocketTag): boolean; override;
    // directly calls epoll's EPOLL_CTL_DEL control interface
    function Unsubscribe(socket: TNetSocket): boolean; override;
    // directly calls epool_wait() function
    function WaitForModified(var results: TPollSocketResults;
      timeoutMS: integer): boolean; override;
    /// read-only access to the low-level epoll_create file descriptor
    property EPFD: integer
      read fEPFD;
  end;

constructor TPollSocketEpoll.Create;
begin
  inherited Create;
  fMaxSockets := 20000;
  fEPFD := epoll_create(fMaxSockets);
  SetLength(fResults, fMaxSockets);
end;

destructor TPollSocketEpoll.Destroy;
begin
  fpClose(fEPFD);
  inherited;
end;

function TPollSocketEpoll.Subscribe(socket: TNetSocket; events: TPollSocketEvents;
  tag: TPollSocketTag): boolean;
var
  e: EPoll_Event;
begin
  result := false;
  if (self = nil) or
     (socket = nil) or
     (PtrUInt(socket) = PtrUInt(fEPFD)) or
     (byte(events) = 0) or
     (fCount = fMaxSockets) then
    exit;
  e.data.ptr := pointer(tag); // don't use data.u64
  if pseRead in events then
    if pseWrite in events then
      e.events := EPOLLIN or EPOLLPRI or EPOLLOUT
    else
      e.events := EPOLLIN or EPOLLPRI
  else if pseWrite in events then
    e.events := EPOLLOUT
  else
    e.events := 0;
  // EPOLLERR and EPOLLHUP are always implicitly tracked
  result := epoll_ctl(fEPFD, EPOLL_CTL_ADD, TSocket(socket), @e) = NO_ERROR;
  if result then
    inc(fCount);
end;

function TPollSocketEpoll.Unsubscribe(socket: TNetSocket): boolean;
var
  e: EPoll_Event; // should be there even if not used
begin
  if (self = nil) or
     (socket = nil) or
     (PtrUInt(socket) = PtrUInt(fEPFD)) then
    result := false
  else
  begin
    result := epoll_ctl(fEPFD, EPOLL_CTL_DEL, TSocket(socket), @e) = NO_ERROR;
    if result then
      dec(fCount);
  end;
end;

function TPollSocketEpoll.WaitForModified(var results: TPollSocketResults;
  timeoutMS: integer): boolean;
var
  e: TPollSocketEvents;
  i: PtrInt;
  ev: integer;
  t: TPollSocketTag;
begin
  result := false; // error
  if (self = nil) or
     (fCount = 0) then
    exit;
  results.Count := epoll_wait(fEPFD, pointer(fResults), fMaxSockets, timeoutMS);
  if results.Count <= 0 then
    exit;
  if length(results.Events) < results.Count then
    SetLength(results.Events, fCount);
  for i := 0 to results.Count - 1 do
  begin
    with fResults[i] do
    begin
      ev := events;
      t := TPollSocketTag(data.ptr);
    end;
    if ev = EPOLLIN then
      byte(e) := byte([pseRead]) // optimized for the most common case
    else
    begin
      byte(e) := 0;
      if ev and (EPOLLIN or EPOLLPRI) <> 0 then
        include(e, pseRead);
      if ev and EPOLLOUT <> 0 then
        include(e, pseWrite);
      if ev and EPOLLERR <> 0 then
        include(e, pseError);
      if ev and EPOLLHUP <> 0 then
        include(e, pseClosed);
    end;
    with results.Events[i] do
    begin
      events := e;
      tag := t;
    end;
  end;
  result := true;
end;

function PollSocketClass: TPollSocketClass;
begin
  PollSocketClass := TPollSocketEpoll;
end;

{$endif USEPOLL}



procedure InitializeUnit;
begin
  SocketAPIVersion := OSVersionShort +
    {$ifdef USEPOLL} ' poll' {$else} ' epoll' {$endif USEPOLL};
end;

procedure FinalizeUnit;
begin
end;

