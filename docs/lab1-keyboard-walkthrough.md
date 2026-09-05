# Lab 1, Part 4 — Keyboard_Controller + test application

Everything here goes into `machine/keyctrl.cc`. The header and the whole
decoding machinery (`key_decoded()`, `get_ascii_code()`, the scan-code tables)
are already given — you only fill in four things:

1. the constructor's initialiser list
2. `key_hit()`
3. `set_led()`
4. `set_repeat_rate()`

The test application needs `kout`, so finish `O_Stream` and `CGA_Stream` first.

---

## How the keyboard hardware works

Two I/O ports, and which one you get depends on whether you read or write:

| Port | Read | Write |
|---|---|---|
| `0x60` data | output buffer — scan codes and replies | input buffer — bytes *to* the keyboard |
| `0x64` control | status register | commands to the *controller* |

Three status bits matter (already defined in `keyctrl.h`):

| Bit | Name | Meaning |
|---|---|---|
| `0x01` | `outb` | output buffer full — **a byte is waiting for you** |
| `0x02` | `inpb` | input buffer full — the controller has **not yet consumed** your last write |
| `0x20` | `auxb` | the waiting byte came from the **PS/2 mouse**, not the keyboard |

The rule that follows from this: **never read `0x60` before `outb` is set**, and
**never write to `0x60` while `inpb` is set**.

---

## 1. The constructor — a one-line bug that costs you hours

The given constructor calls `set_led(..., false)` three times. Your `set_led`
will do `leds &= ~led` — on a member that has never been initialised.

C++ does **not** zero your members. `leds` starts as whatever was on the stack
or in the BSS. So fix the initialiser list:

```cpp
Keyboard_Controller::Keyboard_Controller()
    : code(0), prefix(0), leds(0), ctrl_port(0x64), data_port(0x60)
{
    set_led(led::caps_lock,   false);
    set_led(led::scroll_lock, false);
    set_led(led::num_lock,    false);

    set_repeat_rate(0, 0);      // maximum speed, minimal delay
}
```

Members are initialised in **declaration order**, not in the order you write
them in the list. `keyctrl.h` declares `code, prefix, gather, leds, ctrl_port,
data_port`, so that is the order used above. Write them in a different order
and g++ warns.

---

## 2. key_hit()

The task says: *"Waits until a code is provided in the keyboard controller
buffer and then fetches it."* So it blocks.

```cpp
Key Keyboard_Controller::key_hit()
{
    Key invalid;    // a default-constructed Key has scan == 0, i.e. invalid

    // Wait for a byte that is actually there (outb) and actually came from
    // the keyboard rather than the PS/2 mouse (auxb clear).
    int status;
    do {
        status = ctrl_port.inb();
    } while (!(status & outb) || (status & auxb));

    code = data_port.inb();

    // One key press can span several bytes: a prefix (0xe0/0xe1), the make
    // code, later the break code. key_decoded() returns true only when a
    // complete key is available; until then there is nothing to report.
    if (key_decoded())
        return gather;

    return invalid;
}
```

**Why does the blocking version still work in task 2?** Because there it is
called from the interrupt handler, and an interrupt only fires *once a byte has
arrived* — so the wait loop exits on its first test. The same function serves
both the polling and the interrupt-driven design.

---

## 3. set_led() — where the task-2 bugs come from

The professor's warning is about one specific mistake: **not consuming the
acknowledgement**.

The keyboard answers *every* byte you send it with `0xfa` (`kbd_reply::ack`).
That byte lands in the same output buffer that scan codes come from. If you
send a command and walk away without reading the ACK, the next `key_hit()`
reads `0xfa` and feeds it to `key_decoded()` as if it were a scan code.

In task 1 you may not even notice. In task 2 you get a phantom keypress right
after boot and spend an evening blaming your interrupt handling.

```cpp
void Keyboard_Controller::set_led(char led, bool on)
{
    // Wait until the controller has digested the previous byte.
    while (ctrl_port.inb() & inpb)
        ;
    data_port.outb(kbd_cmd::set_led);

    // Consume the acknowledgement.
    unsigned char reply;
    do {
        while (!(ctrl_port.inb() & outb))
            ;
        reply = data_port.inb();
    } while (reply != kbd_reply::ack);

    // The keyboard wants the state of ALL THREE LEDs in one byte, so we have
    // to remember them; 'leds' is that shadow copy.
    if (on)
        leds |= led;
    else
        leds &= ~led;

    // Second byte: the parameter. Same handshake again.
    while (ctrl_port.inb() & inpb)
        ;
    data_port.outb(leds);

    do {
        while (!(ctrl_port.inb() & outb))
            ;
        reply = data_port.inb();
    } while (reply != kbd_reply::ack);
}
```

The LED bits are already in the header: `scroll_lock = 1`, `num_lock = 2`,
`caps_lock = 4` — i.e. bits 0, 1 and 2 of the parameter byte.

You will often see the shorter idiom `while (data_port.inb() != kbd_reply::ack);`
It usually works, but it reads the data port without checking `outb` first, so
it can consume a stale byte. The version above is the one you can defend.

---

## 4. set_repeat_rate()

Same two-byte handshake, different command and a packed parameter byte.

The parameter for command `0xf3` ("set typematic rate/delay") is:

```
  bit   7   6   5   4   3   2   1   0
      +---+-------+-------------------+
      | 0 | delay |    repeat rate    |
      +---+-------+-------------------+
```

- bits 0–4: **speed**, 0 = fastest (~30 Hz) … 31 = slowest (~2 Hz)
- bits 5–6: **delay**, 0 = 250 ms … 3 = 1000 ms
- bit 7: must be 0

```cpp
void Keyboard_Controller::set_repeat_rate(int speed, int delay)
{
    while (ctrl_port.inb() & inpb)
        ;
    data_port.outb(kbd_cmd::set_speed);

    unsigned char reply;
    do {
        while (!(ctrl_port.inb() & outb))
            ;
        reply = data_port.inb();
    } while (reply != kbd_reply::ack);

    while (ctrl_port.inb() & inpb)
        ;
    data_port.outb(((delay & 3) << 5) | (speed & 31));

    do {
        while (!(ctrl_port.inb() & outb))
            ;
        reply = data_port.inb();
    } while (reply != kbd_reply::ack);
}
```

The masks `& 3` and `& 31` matter: they keep an out-of-range argument from
spilling into the neighbouring bit field — or into bit 7, which must stay 0.

---

## 5. The final test application

`user/appl.cc`. The task asks for the number bases, 64-bit signed and unsigned
values, a pointer, a character, and then a keyboard polling loop.

```cpp
#include "user/appl.h"
#include "device/cgastr.h"
#include "machine/keyctrl.h"

extern CGA_Stream kout;

void Application::action()
{
    kout << "OOStuBS -- Task 1" << endl;
    kout << "=================" << endl << endl;

    // --- number bases -----------------------------------------------------
    int value = 42;
    kout << "42 dec:  " << dec << value << endl;
    kout << "42 hex:  " << hex << value << endl;
    kout << "42 oct:  " << oct << value << endl;
    kout << "42 bin:  " << bin << value << endl << dec << endl;

    // --- 64-bit values ----------------------------------------------------
    long long          smin = -9223372036854775807LL - 1;   // LLONG_MIN
    unsigned long long umax = 18446744073709551615ULL;      // ULLONG_MAX

    kout << "int64  min: " << smin << endl;
    kout << "uint64 max: " << umax << endl;
    kout << "uint64 max: " << hex << umax << " (hex)" << endl << dec;

    // sign handling: '-' in base 10, bit pattern in every other base
    kout << "-1 dec:     " << -1 << endl;
    kout << "-1 hex:     " << hex << -1 << endl << dec << endl;

    // --- pointers and characters ------------------------------------------
    kout << "&value:     " << (void *)&value << endl;
    kout << "*(&value):  " << *(&value) << endl;
    kout << "char:       " << 'X' << endl;
    kout << "bool:       " << true << endl << endl;

    // --- keyboard ---------------------------------------------------------
    Keyboard_Controller keyboard;

    kout << "type something:" << endl << endl;

    while (true) {
        Key key = keyboard.key_hit();

        if (key.valid())            // false for modifier-only presses
            kout << (char)key.ascii() << flush;
    }
}
```

Two things about the loop:

- **`key.valid()` must be checked.** Pressing SHIFT alone produces no character;
  `key_hit()` returns an invalid Key and `ascii()` would be meaningless.
- **You must flush.** `kout` buffers 80 characters, so without a flush your
  typing would only appear once a line filled up. If you have not written a
  `flush` manipulator, call `kout.flush()` instead.

### Optional: proving the LEDs work

Since `set_led` is the risky function, make it visible — it is also a nice
thing to show during the hand-in:

```cpp
        if (key.scancode() == Key::scan::caps_lock)
            keyboard.set_led(Keyboard_Controller::led::caps_lock, ...);
```

Simpler: `set_repeat_rate(31, 3)` in the constructor instead of `(0, 0)`, boot,
and hold a key down — the repeat should be visibly slow to start and slow to
repeat. Then set it back to `(0, 0)`. If nothing changes, your handshake is
broken, and you have found the bug *now* rather than in task 2.

---

## Questions to rehearse

1. Which two I/O ports does the keyboard use, and what does each do on read
   versus on write?
2. What do `outb`, `inpb` and `auxb` tell you, and what breaks if you ignore
   each one?
3. Why must you read the acknowledgement byte? What exactly goes wrong if you
   do not?
4. Why can one key press produce several bytes? What are the prefixes for?
5. What is a break code, and why does this implementation care about it only
   for SHIFT, CTRL and ALT?
6. `key_hit()` blocks in task 1. Why is that not a problem in task 2?
7. Why does `Keyboard_Controller` need the `leds` member at all?
