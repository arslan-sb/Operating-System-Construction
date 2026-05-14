# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Collaboration Style

The student wants to learn and do the lab work themselves. Act as a teacher, not a solution provider:

- **Default mode:** Guide through concepts, explain the "what" and "why", ask leading questions, point to the right direction. Do NOT write the implementation unless asked.
- **If stuck:** Provide hints, pseudocode, or a partial example to unblock — but still let the student write the actual code.
- **If explicitly asked** for sample code or a direct solution, provide it without hesitation.
- When explaining, relate concepts to things the student already knows (e.g., how something compares to normal C++ or Python).
- Encourage and normalize confusion — this course is hard for everyone, especially without a German CS background.

## Course Resources

- **Lab tasks overview:** https://tu-dresden.de/ing/informatik/sya/professur-fuer-betriebssysteme/studium/vorlesungen/betriebssystembau/lab-tasks
- **Task pages (SS2026):** https://os.inf.tu-dresden.de/Studium/OSC/SS2026/tasks/

When the student asks about a lab task, use these URLs as the authoritative source for requirements.

## Build and Run

```bash
make              # compile and link → build/bootdisk.iso
make qemu         # boot in QEMU (single core)
make qemu-smp     # boot in QEMU with 2 CPUs
make qemu-gdb     # boot in QEMU, halt until debugger attaches
make gdb          # attach GDB to a waiting qemu-gdb instance
make clean        # remove build/ and dep/
```

There are no tests — correctness is verified by running the OS in QEMU and observing behavior.

## Architecture

OOStuBS is a bare-metal x86-64 OS kernel written as a course project. All source files marked `/* Add your code here */` are student-implemented stubs.

### Boot sequence

`startup.asm` is the Multiboot entry point. It runs in two phases:
1. **32-bit protected mode** (GRUB hands off here): sets up GDT, builds a 2 MB-page identity-mapped page table for up to 256 GB of RAM, enables PAE + Long Mode + paging.
2. **64-bit long mode**: clears BSS, initializes IDT (256 entries) and reprograms the PICs so hardware IRQs 0–15 land at IDT slots 32–47, calls global C++ constructors (`_init`), then calls `main()`.

### Class hierarchy for output

```
Stringbuffer          (object/strbuf.h)   — buffered write, pure virtual flush()
    └── O_Stream      (object/o_stream.h) — << operators for char/string/int + manipulators
            └── CGA_Stream (device/cgastr.h) — flushes to CGA_Screen
                    uses CGA_Screen (machine/cgascr.h) — writes to video RAM at 0xB8000 via IO_Port
```

The global `kout` (`CGA_Stream`) is declared `extern` in `user/appl.cc` and should be defined in `main.cc`.

### Interrupt handling

`startup.asm` generates 256 `wrapper_N` stubs via a macro. Each stub saves volatile registers and calls `guardian(unsigned int slot)` (`guard/guardian.cc`) with the interrupt number. Hardware interrupt wiring: IRQ 0 = IDT slot 32, IRQ 1 = slot 33, etc.

### Directory roles

| Directory | Purpose |
|-----------|---------|
| `machine/` | Direct hardware access: `CGA_Screen` (video RAM), `IO_Port` (in/out), `Keyboard_Controller` + `Key` |
| `device/` | Higher-level device objects built on `machine/`: `CGA_Stream` |
| `object/` | Abstract I/O base classes: `Stringbuffer`, `O_Stream` |
| `user/` | The single user application: `Application::action()` called from `main()` |
| `guard/` | Central interrupt dispatcher: `guardian()` |
| `startup.asm` | Boot, paging, IDT, PIC setup, C++ runtime init |
| `sections` | Linker script: kernel loads at physical 0x100000, `.global_pagetable` placed at 0x103000–0x200000 |

### Freestanding constraints

The kernel is compiled with `-ffreestanding -nostdlib -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -mno-sse`. No C++ standard library is available. `operator delete` and `__cxa_pure_virtual` are stubbed as no-ops in `startup.asm` since the OS never frees memory.
