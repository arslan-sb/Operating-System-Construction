# OOStuBS

My implementation of **OOStuBS**, the teaching operating system for the
*Operating-System Construction* course (OSC, SS 2026) at TU Dresden.

OOStuBS is a small 64-bit kernel for the x86 PC, written in C++ and x86
assembler, built up over seven lab tasks. It boots from a GRUB image, runs
without any C library, and talks to the hardware directly.

## Building and running

```bash
make            # compile and link, then build build/bootdisk.iso
make qemu       # build and boot the image in QEMU
make clean      # remove build artifacts
```

Debugging:

```bash
make qemu-gdb   # start QEMU and wait for a debugger to attach
make gdb        # attach gdb    (also: make ddd, make cgdb)
make qemu-smp   # boot with 2 CPUs
```

To write the image to a USB stick and boot it on real hardware, set
`DRIVE_HD` in the Makefile to the correct device and run `make bootdisk`.

**Careful:** `DRIVE_HD` defaults to `/dev/sdz` on purpose. Pointing it at the
wrong device destroys the data on that device.

### Requirements

`g++`, `nasm`, `xorriso`, `grub-mkrescue` (i386-pc target) and
`qemu-system-x86_64`.

## How the code is organised

The source tree mirrors the layering of the system: each directory may only
depend on the ones below it.

| Directory | Contains | Knows about |
|---|---|---|
| `user/` | the application | nothing below `device/` |
| `device/` | device drivers | both halves — this is where they meet |
| `object/` | data structures, device independent | nothing hardware-specific |
| `guard/` | interrupt synchronisation | — |
| `machine/` | hardware access | the bare metal |

Plus `startup.asm` (boot, IDT, PIC setup, global constructors), `sections`
(linker script) and `main.cc` (entry point, global objects).

### The output path

The chain from an application's `<<` down to the video memory:

```
    Stringbuffer            CGA_Screen        object / machine
         |                       |
     O_Stream                    |            formatting, number bases
           \                    /
            \                  /
               CGA_Stream                     device
```

- **`CGA_Screen`** (`machine/`) — the hardware. CGA uses *both* PC address
  spaces: the video memory is mapped at `0xB8000` and reached with an ordinary
  pointer, while the CRT controller's registers live in the I/O address space
  and are reached with `in`/`out` through the index port `0x3D4` and the data
  port `0x3D5`.
- **`Stringbuffer`** (`object/`) — collects characters so that a whole line is
  handed to the device at once instead of one port access per character.
  `flush()` is pure virtual, which is what keeps the class device independent.
- **`O_Stream`** (`object/`) — the `<<` operators, number conversion in base
  2/8/10/16, and the manipulators `endl`, `bin`, `oct`, `dec`, `hex`.
- **`CGA_Stream`** (`device/`) — inherits from *both* `O_Stream` and
  `CGA_Screen`. It is the only class that knows the device-independent and the
  device-dependent half, and therefore the only one that can implement
  `flush()`.

Global objects such as `kout` are constructed before `main()` runs: the linker
script collects their constructors into `.init_array`, and `_init` in
`startup.asm` walks that section.

## Progress

| Lab | Topic | Status |
|---|---|---|
| 1 | C++, CGA output | `CGA_Screen` ✅ · `Stringbuffer` ✅ · `O_Stream` 🚧 · `CGA_Stream` ⬜ |
| 2 | Keyboard, interrupts | on branch `lab02-keyboard-interrupts` |
| 3 | Traps, startup | ⬜ |
| 4 | Assembler | ⬜ |
| 5 | Preemption | ⬜ |
| 6 | Waiting | ⬜ |
| 7 | Queue | ⬜ |

## Branches

- **`lab01-io-functionality`** — current work (default branch)
- **`lab02-keyboard-interrupts`** — interrupt-driven keyboard, parked until
  lab 1 is finished

## Notes

`docs/` holds my own working notes:

- `lab1-walkthrough.md` — each class of lab 1, with the reasoning behind it
- `exam-plan.md` — study plan and rehearsal questions for the oral exam

## Course

*Operating-System Construction*, Prof. Horst Schirmeier, TU Dresden.
Template code from the course's own repository; the implementations in
`machine/`, `object/`, `device/`, `guard/` and `user/` are mine.
