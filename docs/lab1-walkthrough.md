# Lab 1 Walkthrough — CGA Output

Work through the five pieces in this order. Each one only needs the piece
before it, so you can build and test after every step.

```
   main.cc          kout << "hello" << endl
      |
   CGA_Stream       flush()  <-- the only class that knows both halves
      |      \
  O_Stream    CGA_Screen     formatting  /  hardware
      |
 Stringbuffer                put(), buffering
```

Build and run at any point with:

    make && make qemu

---

## Piece 1 — CGA_Screen (`machine/cgascr.h`, `machine/cgascr.cc`)

The hardware layer. Nothing above it touches the video card.

### The two address spaces (this is the exam point)

A PC device is reached in two different ways, and **CGA uses both**:

- **Memory address space** — the video memory is mapped at `0xB8000`, so you
  reach it with an ordinary pointer. Two bytes per character cell: the even
  address holds the ASCII code, the odd address holds the attribute byte.
- **I/O address space** — the CRT controller's registers are *not* in memory.
  They need the `in`/`out` instructions, wrapped for you in class `IO_Port`.

### Header

```cpp
#include "machine/io_port.h"

class CGA_Screen {
public:
    CGA_Screen(const CGA_Screen &copy) = delete;
    CGA_Screen& operator=(const CGA_Screen&) = delete;

    enum { COLUMNS = 80, ROWS = 25 };
    enum { STD_ATTR = 0x0f };          // bright white on black

    CGA_Screen();

    void show(int x, int y, char c, unsigned char attrib = STD_ATTR);
    void setpos(int x, int y);
    void getpos(int &x, int &y) const;
    void print(char *text, int length, unsigned char attrib = STD_ATTR);
    void clear(unsigned char attrib = STD_ATTR);

private:
    static char * const CGA_START;     // 0xb8000

    const IO_Port index_port;          // 0x3d4
    const IO_Port data_port;           // 0x3d5

    enum { CURSOR_HIGH = 14, CURSOR_LOW = 15 };

    void scrollup(unsigned char attrib);
};
```

`CGA_START` is `static` on purpose. If you make it a normal member (as the
crash-course snippet does), **every CGA_Screen object carries its own copy of
the same constant pointer** — 8 wasted bytes per instance.

### show()

```cpp
char * const CGA_Screen::CGA_START = (char *)0xb8000;

void CGA_Screen::show(int x, int y, char c, unsigned char attrib)
{
    if (x < 0 || x >= COLUMNS || y < 0 || y >= ROWS)
        return;

    char *pos = CGA_START + 2 * (y * COLUMNS + x);
    *pos     = c;
    *(pos+1) = attrib;
}
```

The slide asks *"what's missing here?"* about the crash-course version. Two
things: **the attribute byte** (without it the character inherits whatever
attribute was left in that cell — often 0, i.e. black on black, invisible),
and **the bounds check** (C++ does no array bounds checking, so an out-of-range
y silently corrupts memory past the video RAM).

### setpos() / getpos() — index/data multiplexing

The CRT controller has far more registers than it has I/O ports. The solution
is **multiplexing**: write the register *number* to the index port `0x3d4`,
then read or write that register's *value* through the data port `0x3d5`.

The cursor position is a 16-bit offset (`y * 80 + x`) split across two 8-bit
registers: 14 = high byte, 15 = low byte.

```cpp
void CGA_Screen::setpos(int x, int y)
{
    if (x < 0 || x >= COLUMNS || y < 0 || y >= ROWS)
        return;

    int offset = y * COLUMNS + x;

    index_port.outb(CURSOR_HIGH);
    data_port.outb(offset >> 8);
    index_port.outb(CURSOR_LOW);
    data_port.outb(offset & 0xff);
}

void CGA_Screen::getpos(int &x, int &y) const
{
    index_port.outb(CURSOR_HIGH);
    int offset = data_port.inb() << 8;
    index_port.outb(CURSOR_LOW);
    offset |= data_port.inb();

    x = offset % COLUMNS;
    y = offset / COLUMNS;
}
```

**A design choice you must be ready to defend.** There is no `int x, y;` member
in this class. The hardware cursor *is* the position. Two copies of the same
state can drift apart; one cannot. The cost is two `inb`s per query, and
`print()` pays that exactly once per call.

If the examiner pushes ("the slide says *change internal state*"), the honest
answer is: *both work; I chose the one that cannot desynchronise.* Knowing why
you chose it is what they are testing.

### print() — the part that was broken

```cpp
void CGA_Screen::print(char *text, int length, unsigned char attrib)
{
    int x, y;
    getpos(x, y);

    for (int i = 0; i < length; i++) {
        if (text[i] == '\n') {
            x = 0;
            y++;
        } else {
            show(x, y, text[i], attrib);
            x++;
        }

        if (x >= COLUMNS) {        // line full -> wrap
            x = 0;
            y++;
        }
        if (y >= ROWS) {           // bottom reached -> make room
            scrollup(attrib);
            y = ROWS - 1;
        }
    }

    setpos(x, y);
}
```

Four things happen here that a naive loop misses: `\n` handling, wrapping at
column 80, scrolling at row 25, and writing the cursor back at the end.

### scrollup()

Slide 34 asks *"Arrived at screen bottom? Scrolling! (How?)"*. The answer:
**copy rows 1..24 one row up, then blank row 24.** There is no `memmove()` —
this is a freestanding environment with no C library.

```cpp
void CGA_Screen::scrollup(unsigned char attrib)
{
    char *dst = CGA_START;
    char *src = CGA_START + 2 * COLUMNS;
    char * const end = CGA_START + 2 * COLUMNS * ROWS;

    while (src < end)
        *dst++ = *src++;

    while (dst < end) {            // blank the duplicated last row
        *dst++ = ' ';
        *dst++ = attrib;
    }
}
```

### clear() and the constructor

```cpp
CGA_Screen::CGA_Screen() : index_port(0x3d4), data_port(0x3d5)
{
    clear();
}

void CGA_Screen::clear(unsigned char attrib)
{
    char *pos = CGA_START;
    char * const end = CGA_START + 2 * COLUMNS * ROWS;

    while (pos < end) {
        *pos++ = ' ';
        *pos++ = attrib;
    }

    setpos(0, 0);
}
```

---

## Piece 2 — Stringbuffer (`object/strbuf.h`, `object/strbuf.cc`)

### Why buffer at all?

Because **one I/O access per character is expensive**. `print()` costs two port
reads for `getpos` plus two writes for `setpos` — paying that per character
would be absurd. Collect characters, hand over a whole line at once.

### Why 80?

One CGA line. Bigger buys nothing, because `print()` can only put 80 characters
on a row anyway. Smaller means more device accesses than necessary.

### Header

```cpp
class Stringbuffer {
public:
    Stringbuffer(const Stringbuffer &copy) = delete;
    Stringbuffer& operator=(const Stringbuffer&) = delete;

    virtual void flush() = 0;

protected:
    Stringbuffer();
    void put(char c);

    enum { BUFFER_SIZE = 80 };
    char buffer[BUFFER_SIZE];
    int  pos;
};
```

### Why is flush() pure virtual?

**Because Stringbuffer is device independent.** It knows how to *collect*
characters, but has no idea where they should end up — screen, serial port,
network. The derived class supplies that knowledge. Making it pure virtual also
makes Stringbuffer abstract, so nobody can accidentally instantiate a buffer
that has nowhere to send its contents.

### Implementation

```cpp
Stringbuffer::Stringbuffer() : pos(0) {}

void Stringbuffer::put(char c)
{
    buffer[pos++] = c;

    if (pos == BUFFER_SIZE)
        flush();
}
```

**A classic C++ trap worth knowing.** You must *not* call `flush()` from the
Stringbuffer constructor. During a base-class constructor the vtable still
points at `Stringbuffer`, so the call would land on the pure virtual method —
not on the derived class's override. The object is not fully built yet.

`buffer` and `pos` are `protected`, not `private`, because `flush()` lives in
the derived class and has to read what we collected.

---

## Piece 3 — O_Stream (`object/o_stream.h`, `object/o_stream.cc`)

Inherits from Stringbuffer and adds the formatting. **It stays abstract** — it
does not implement `flush()`, because it is just as device independent as its
base class.

### Header

```cpp
#include "object/strbuf.h"

class O_Stream : public Stringbuffer {
public:
    O_Stream(const O_Stream &copy) = delete;
    O_Stream& operator=(const O_Stream&) = delete;

    O_Stream();

    int base;                       // 2, 8, 10 or 16

    O_Stream& operator<<(char c);
    O_Stream& operator<<(unsigned char c);
    O_Stream& operator<<(const char *string);
    O_Stream& operator<<(bool b);

    O_Stream& operator<<(short value);
    O_Stream& operator<<(unsigned short value);
    O_Stream& operator<<(int value);
    O_Stream& operator<<(unsigned int value);
    O_Stream& operator<<(long value);
    O_Stream& operator<<(unsigned long value);
    O_Stream& operator<<(long long value);
    O_Stream& operator<<(unsigned long long value);

    O_Stream& operator<<(void *pointer);

    O_Stream& operator<<(O_Stream& (*fkt)(O_Stream&));

private:
    void put_unsigned(unsigned long long value);
    void put_signed(long long value);
};

O_Stream& endl(O_Stream& os);
O_Stream& bin(O_Stream& os);
O_Stream& oct(O_Stream& os);
O_Stream& dec(O_Stream& os);
O_Stream& hex(O_Stream& os);
```

**Why does every operator return `O_Stream&`?** That is what makes the calls
chainable. `kout << "x = " << 42 << endl` is really
`((kout.operator<<("x = ")).operator<<(42)).operator<<(endl)` — each call has
to hand the stream back so the next one has something to work on. A reference,
not a copy, because a stream is not copyable (and copying the buffer would be
nonsense).

### The simple overloads

```cpp
O_Stream::O_Stream() : base(10) {}

O_Stream& O_Stream::operator<<(char c)
{
    put(c);
    return *this;
}

O_Stream& O_Stream::operator<<(unsigned char c)
{
    return *this << (char)c;
}

O_Stream& O_Stream::operator<<(const char *string)
{
    while (*string)                 // no strlen() -- freestanding
        put(*string++);
    return *this;
}

O_Stream& O_Stream::operator<<(bool b)
{
    return *this << (b ? "true" : "false");
}
```

### Number conversion

```cpp
void O_Stream::put_unsigned(unsigned long long value)
{
    char digits[64];                // worst case: base 2, 64 bits
    int  n = 0;

    do {
        unsigned int d = (unsigned int)(value % (unsigned)base);
        digits[n++] = (d < 10) ? ('0' + d) : ('a' + d - 10);
        value /= (unsigned)base;
    } while (value != 0);

    while (n > 0)
        put(digits[--n]);
}
```

Two details worth being able to point at:

- **Why a local array and a reverse loop?** Repeated division produces the
  digits *least significant first*, but they must be printed most significant
  first. So collect, then emit backwards.
- **Why `do/while` and not `while`?** So that the value `0` still produces one
  digit. A plain `while (value != 0)` would print nothing at all for zero.

### Signed numbers — two real traps

```cpp
void O_Stream::put_signed(long long value)
{
    if (value < 0) {
        put('-');
        put_unsigned(~(unsigned long long)value + 1);
    } else {
        put_unsigned((unsigned long long)value);
    }
}

O_Stream& O_Stream::operator<<(int value)
{
    if (base == 10)
        put_signed(value);
    else
        put_unsigned((unsigned int)value);
    return *this;
}
```

(The `short`, `long` and `long long` overloads follow the identical pattern,
each casting to *its own* unsigned type.)

**Trap 1 — the width.** A leading `-` only makes sense in base 10. In hex you
want the bit pattern. But casting `(int)-1` straight to `unsigned long long`
sign-extends it and prints `ffffffffffffffff`. Casting to the *same-width*
`unsigned int` first gives the correct `ffffffff`.

**Trap 2 — the most negative value.** Writing `-value` is undefined behaviour
for `INT_MIN`, because `+2147483648` does not fit in an `int`. Negating in
unsigned arithmetic (`~x + 1`, the definition of two's complement) is always
well defined. This is why `min int` prints correctly as `-2147483648`.

### Pointers

```cpp
O_Stream& O_Stream::operator<<(void *pointer)
{
    int old_base = base;
    base = 16;
    *this << "0x";
    put_unsigned((unsigned long long)pointer);
    base = old_base;
    return *this;
}
```

### Manipulators — how `kout << hex` works

This is a favourite question. **`hex` is not a value. It is a function.**

`O_Stream` declares an `operator<<` that takes a *pointer to a function* with
the signature `O_Stream& (*)(O_Stream&)`. When you write `kout << hex`, the
compiler picks that overload, passes it the address of `hex`, and the operator
simply calls it:

```cpp
O_Stream& O_Stream::operator<<(O_Stream& (*fkt)(O_Stream&))
{
    return fkt(*this);
}

O_Stream& endl(O_Stream& os) { os << '\n'; os.flush(); return os; }
O_Stream& bin(O_Stream& os)  { os.base = 2;  return os; }
O_Stream& oct(O_Stream& os)  { os.base = 8;  return os; }
O_Stream& dec(O_Stream& os)  { os.base = 10; return os; }
O_Stream& hex(O_Stream& os)  { os.base = 16; return os; }
```

Note `endl` does two things: inserts the newline **and** flushes. That is why
output appears immediately at the end of a line instead of waiting for the
buffer to fill up. `base` is public so these plain functions can reach it.

---

## Piece 4 — CGA_Stream (`device/cgastr.h`, `device/cgastr.cc`)

```cpp
#include "object/o_stream.h"
#include "machine/cgascr.h"

class CGA_Stream : public O_Stream, public CGA_Screen {
public:
    CGA_Stream(CGA_Stream &copy) = delete;
    CGA_Stream& operator=(const CGA_Stream&) = delete;

    CGA_Stream();
    void flush() override;

    unsigned char attribute;
};
```

```cpp
CGA_Stream::CGA_Stream() : attribute(CGA_Screen::STD_ATTR) {}

void CGA_Stream::flush()
{
    CGA_Screen::print(buffer, pos, attribute);
    pos = 0;
}
```

**This is the whole point of the exercise, and the most likely question.**

`CGA_Stream` uses **multiple inheritance**: the buffering and formatting come
from `O_Stream`, the hardware access from `CGA_Screen`. It is the single place
in the system where the device-*independent* half of the output path meets the
device-*dependent* half — which is exactly why it is the only class that can
implement `flush()`. `buffer` and `pos` come from `Stringbuffer` (through
`O_Stream`); `print()` comes from `CGA_Screen`. Only a class that inherits from
both can put them together in one line.

This is the same reason the C++ crash course spends a slide on multiple
inheritance. Be ready to draw the diagram:

```
      Stringbuffer            CGA_Screen        (object / machine)
           |                       |
       O_Stream                    |
             \                    /
              \                  /
                 CGA_Stream                     (device)
```

---

## Piece 5 — main.cc and Application

```cpp
#include "device/cgastr.h"
#include "user/appl.h"

CGA_Stream kout;
Application application;

int main()
{
    application.action();
    return 0;
}
```

**Why does the constructor of `kout` run at all?** Because global constructors
are executed before `main()`. Look at `startup.asm:231`:

    call   _init   ; call constructors of global objects
    call   main    ; call the OS kernel's C / C++ part

`_init` walks the `.init_array` section (set up by the linker script
`sections`) and calls every entry. That is what clears the screen and
initialises the `IO_Port` members before anything prints. Without it, the
`index_port`/`data_port` members would hold garbage.

One C++ detail in `user/appl.h`: declaring the deleted copy constructor
**suppresses the implicit default constructor**, so you have to ask for it back:

```cpp
Application() = default;
```

Otherwise `Application application;` does not compile.

---

## Testing it

```
make && make qemu
```

Your `Application::action()` should exercise each feature, so you can point at
the screen during the exam:

- the four bases, via the manipulators
- `-1` in decimal and in hex (proves the width handling: `ffffffff`, not
  `ffffffffffffffff`)
- `INT_MIN` (proves the unsigned-negation path)
- `bool`, `char`, a pointer
- a string longer than 80 characters (proves wrapping)
- more than 25 lines (proves scrolling)

---

## The eight questions to rehearse

1. CGA uses two address spaces — name them and say which part of the card is
   in which.
2. The crash-course snippet writes `*pos = 'Q'` and nothing else. What is
   missing, and what goes wrong without it?
3. There are more CRT controller registers than I/O ports. How is that solved?
4. How do you scroll the screen?
5. Why does `Stringbuffer` exist at all? What size did you choose and why?
6. Why is `flush()` pure virtual, and why can only `CGA_Stream` implement it?
7. How does `kout << hex` work, given that `hex` is not a value?
8. Why does `(int)-1` print as `ffffffff` and not `ffffffffffffffff`?
