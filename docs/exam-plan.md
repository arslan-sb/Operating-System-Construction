# OSC Oral Exam — 5-Day Plan (30h)

Exam: ~2026-09-05. Prof: "all three core areas; exercises + lab tasks also relevant;
concepts over memorization; be ready to refer to your OOStuBS implementation."

## Three core areas (from L13)
1. An expedition through the architecture of the x86 PC  (L02, L03, L06, L11, L12)
2. Control flows and their interactions                  (L04, L05, L07, L08, L09, L10)
3. OS concepts in general and in Linux/Windows           (L01, L08, L09, L10, L12)

## Reality check
30h cannot cover 13 lectures + implementing labs 1-7. Optimize for the ORAL EXAM,
not for a complete codebase. Finish Lab 1 (it is ~25% done), get Lab 2 working
(interrupts = highest conceptual payoff), and study labs 3-7 as *designs* you can
draw and explain rather than code you have written.

Hard rule: if coding overruns its slot, STOP and move to the lecture block.
Concepts are graded; unfinished code is not.

---

## Day 1 (6h) — Finish Lab 1 properly

### 1.5h — CGA_Screen (machine/cgascr.{h,cc})
Current bugs to fix:
- `setpos()` only updates x/y; it must move the **hardware cursor** via the CRT
  controller: index port 0x3D4, data port 0x3D5, register 14 = offset high byte,
  register 15 = offset low byte, offset = y*80 + x.
- `getpos()` reads shadow state. Either read back from the CRTC (what the task
  intends) or keep the shadow and be able to justify it.
- `print()` never advances y, never wraps at x==80, ignores '\n', never scrolls,
  never calls setpos() at the end. Printing >80 chars runs past the row; at y==24
  it writes past 0xB8FA0 into memory that is not video RAM.
- No `scrollup()`. Needed: memmove rows 1..24 to 0..23, blank row 24.
- `show()` has no bounds check on x/y.
- `CGA_START` is a non-static member (8 bytes per object). Make it
  `static constexpr` / an enum constant.
- Screen is never cleared at startup.

Constants: 80x25, 2 bytes/cell (even = ASCII, odd = attribute), base 0xB8000.

### 2.5h — The output stack (all three classes are still empty)
- `Stringbuffer` (object/strbuf): `put(char)`, `flush()` pure virtual, internal
  buffer + index. Flush when full or on explicit flush.
- `O_Stream` : public Stringbuffer (object/o_stream): `operator<<` for char,
  unsigned char, char*, short/int/long/long long (signed + unsigned), void*, bool,
  and `O_Stream& (*)(O_Stream&)` for manipulators. Base member (2/8/10/16).
  Manipulators: `endl` (put '\n' + flush), `bin`, `oct`, `dec`, `hex`.
  Number conversion: build digits backwards into a local buffer, then put() them.
- `CGA_Stream` : public O_Stream (device/cgastr): implements `flush()` by calling
  `CGA_Screen::print(buffer, index, attribute)` and resetting the index.
- `main.cc`: define global `CGA_Stream kout;`, construct `Application`, call
  `action()`. Right now main.cc bypasses the whole stack by using CGA_Screen
  directly — that is exactly the architecture question you'll be asked.
- `Application::action()` (user/appl.cc): print something using `kout << ... << endl`.

### 2h — Understand what you just wrote
Be able to answer:
- Why buffer at all? What is a reasonable buffer size, and why?
- Why is `flush()` pure virtual? What would break without it?
- Draw the class diagram: Stringbuffer -> O_Stream -> CGA_Stream, and CGA_Screen
  as the machine-level class. Which layer knows about hardware?
- Why does `operator<<` return `O_Stream&`?
- How does a manipulator work (function pointer passed to an overloaded operator)?
- Why `-ffreestanding -nostdlib -fno-rtti -fno-exceptions -fno-threadsafe-statics
  -mno-red-zone -mno-sse` in the Makefile? Each flag has a real kernel reason.
  (red zone: interrupt handlers would clobber it; SSE: no FPU state saving;
  no-rtti/exceptions: need runtime support that doesn't exist.)
- References vs pointers, virtual functions and the vtable, why a class with
  virtual methods costs 8 extra bytes.

---

## Day 2 (6h) — Interrupts: hardware + software

### 3h — L03 (Interrupts – Hardware) + L04 (Interrupts – Software)
Must be able to draw, from memory, the full path:
device -> PIC 8259A (master/slave, IRQ line, cascade on IRQ2) -> INTR pin ->
CPU fetches vector -> IDT entry (interrupt gate vs trap gate) -> pushes
SS:RSP/RFLAGS/CS:RIP (+error code) -> assembler stub in startup.asm ->
`guardian(slot)` -> `Plugbox::report(slot)` -> `Gate::trigger()`.
Also: PIC vs APIC/IO-APIC, masking (IMR), EOI, edge vs level triggered,
exceptions vs traps vs interrupts, why the IDT needs 256 entries.

### 2.5h — Implement Lab 2
- `keyctrl.cc`: `key_hit()` (poll status port 0x64 for outb, but only for the
  keyboard, not aux; read 0x60; feed `key_decoded()`), `set_led()`, `set_repeat_rate()`.
- `Gate` (abstract, virtual `trigger()`), `Plugbox` (array of Gate*, `assign`, `report`).
- `Keyboard` : Gate + CGA_Stream user — `trigger()` reads a key and echoes it.
- `guardian()` calls into the plugbox.
- Unmask IRQ1 in the PIC; `sti`.
Goal: typing in QEMU echoes to screen via interrupt, not polling.

### 0.5h — Redraw the whole interrupt path on blank paper without looking.

---

## Day 3 (6h) — Interrupt synchronization + x86-64 + startup

### 2.5h — L05 (Interrupts – Synchronization)  ★ signature topic of this course
Be able to compare the three approaches and give the failure mode of each:
1. no synchronization -> race on shared data (e.g. Stringbuffer index)
2. hard synchronization -> `cli`/`sti` around critical sections; kills latency
3. **prologue/epilogue model** -> prologue runs with interrupts disabled, does the
   minimum device work; epilogue is queued and runs at a lower level with
   interrupts enabled but mutually exclusive. `Guarded_*` classes, `Secure`,
   `Locker`, the epilogue queue, the "level" concept.
Know why the epilogue queue needs an atomic enqueue, and what a "relay" is.

### 1.5h — L06 (x86-64)
Registers (RAX..R15, RIP, RFLAGS), rings 0-3, real vs protected vs long mode,
segmentation vs paging (and why OOStuBS uses flat segments), System V AMD64 calling
convention: args in RDI RSI RDX RCX R8 R9, return in RAX, callee-saved RBX RBP
R12-R15, 16-byte stack alignment, the red zone.

### 2h — E03 (Traps/Startup) + E04 (Assembler), and read YOUR `startup.asm` line by line
"What happens from power-on until `main()` runs?" is a near-certain question:
BIOS/UEFI -> GRUB (multiboot header) -> 32-bit protected mode -> set up GDT and
page tables -> long mode -> set up stack -> call global constructors -> `main()`.
Also know how `io_port.asm` implements `inb`/`outb`.

---

## Day 4 (6h) — Control flows

### 2h — L07 (Coroutines and Threads)
Be ready to write the context switch on paper: the `toc` struct holds only the
callee-saved registers + rsp; `context_switch` pushes rbx rbp r12-r15, saves rsp
into the old toc, loads rsp from the new toc, pops, `ret` — and the `ret` returns
into the *other* coroutine. Know `toc_settle` (faking an initial stack frame so the
first switch "returns" into the entry function). Coroutine vs thread vs process.

### 1.5h — L08 (Scheduling)
Ready list, Dispatcher (who is running / `go` / `dispatch`), Scheduler
(`ready`, `schedule`, `exit`, `kill`), cooperative vs preemptive, why preemption
needs the timer interrupt, `Guarded_Scheduler` and why `resume` must not be
interrupted. FCFS/RR/priority/MLFQ, and how Linux CFS differs.

### 1.5h — L09 (Thread Synchronization)
Semaphore (P/V, `wait`/`signal`), Waitingroom, Customer, Bell/Bellringer,
why `Guarded_Semaphore`, busy waiting vs blocking, deadlock conditions.

### 1h — Skim E05 (Preemption), E06 (Waiting), E07 (Queue)
Do NOT code these. Learn the *design*: what classes they add and where they plug
into the layering. Being able to say "here is how I would implement it in OOStuBS,
and this is where it hooks into the guardian/scheduler" scores nearly as well.

---

## Day 5 (6h) — Core area 3, big picture, rehearsal

### 1.5h — L10 (IPC), L11 (Buses), L12 (Drivers)
IPC: shared memory vs message passing, synchronous vs asynchronous, mailbox.
Buses: PCI(e) config space, BAR, enumeration, DMA, bus mastering, MMIO vs PMIO.
Drivers: polling vs interrupt-driven vs DMA, top half / bottom half in Linux
(and how that maps onto prologue/epilogue!), device model.

### 1.5h — L01, L02, L13
OS purposes, monolithic vs microkernel, the OOStuBS layer diagram from L13
(application / IPC / process management / interrupt synchronization / interrupt
handling / control-flow abstraction / device access). Redraw it from memory.

### 2h — Mock exam, out loud, with pen and paper
Answer each of these standing up, in ~3 minutes, in English or German:
1. Draw the OOStuBS class structure and explain the layering.
2. What happens when I press a key, from the physical switch to a character on screen?
3. Why do you need a Stringbuffer? Show me your flush().
4. Explain the prologue/epilogue model. Why not just cli/sti?
5. Write a context switch in x86-64 assembler.
6. What is in the IDT? What is the difference between an interrupt gate and a trap gate?
7. How does the CPU know which handler to call? What does the hardware push?
8. How do you address more CGA registers than you have I/O ports? (index/data multiplexing)
9. Two address spaces on the PC — name them and give a device that uses both. (CGA)
10. What does `virtual` cost at runtime? How is it implemented?
11. Why can't you use exceptions/RTTI/the standard library in a kernel?
12. How does a semaphore block a thread, and who wakes it?
13. What makes preemptive scheduling harder than cooperative?
14. From power-on to main(): the whole boot chain.
15. Where in your code is a race condition possible, and how did you prevent it?

### 1h — Reread your own code, every file, end to end
Be able to open any file and explain any line. "Refer to your OOStuBS
implementation" means they may ask you to show them.

---

## Repo hygiene (10 min, do it once)
`build/`, `isofiles/boot/system`, `experiment/`, and the old `main` binary are
build products. Add them to `.gitignore` and `git rm --cached` them so your repo
shows only source when you open it in the exam.
