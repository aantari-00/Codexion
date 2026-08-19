*This project has been created as part of the 42 curriculum by aantari.*

# Codexion

## Description

Codexion is a multithreaded C simulation inspired by the classic dining-philosophers
problem, reframed around coders sharing USB dongles in a co-working space.

`number_of_coders` coders sit around a circular table. Between each pair of neighbouring
coders there is one USB dongle, so there are exactly as many dongles as coders. Each
coder repeatedly cycles through three states:

1. **Compiling** – requires picking up both the dongle on their left and the dongle on
   their right at the same time.
2. **Debugging** – happens after the dongles are put back on the table.
3. **Refactoring** – happens after debugging, then the coder immediately tries to
   compile again.

If a coder does not manage to start compiling within `time_to_burnout` milliseconds of
the beginning of the simulation, or of the start of their last compile, they **burn out**
and the simulation stops. The simulation also stops successfully once every coder has
compiled at least `number_of_compiles_required` times.

The goal of the project is to practice thread synchronization in C: coordinating access
to a shared resource (the dongles) between several threads (the coders) without race
conditions, deadlocks, or starvation, while a separate monitor thread watches for
burnout in near real time.

## Main Features

- One POSIX thread per coder, plus one dedicated monitor thread.
- Dongles are shared between adjacent coders and protected by a mutex; a coder can only
  start compiling once it holds both of its dongles.
- A **dongle cooldown**: once released, a dongle becomes available again only after
  `dongle_cooldown` milliseconds.
- Two selectable arbitration policies for who gets a contested dongle next:
  - `fifo` – requests are served in the order they arrived.
  - `edf` – requests are served in order of earliest deadline
    (`last_compile_start + time_to_burnout`), with request arrival time used as a
    tie-breaker.
- The scheduling queue is a custom binary min-heap (no standard library priority queue
  is used).
- A monitor thread checks every coder roughly every millisecond and reports a burnout
  within 10 ms of it actually happening.
- All state-change logs (`has taken a dongle`, `is compiling`, `is debugging`,
  `is refactoring`, `burned out`) are timestamped in milliseconds since the start of the
  simulation and serialized through a dedicated print mutex so lines never interleave.
- Strict argument parsing/validation: wrong argument count, non-numeric values, or an
  unknown scheduler name are all rejected before the simulation starts.
- Clean shutdown: all mutexes, the condition variable, the heap, and the allocated
  arrays are properly destroyed/freed before the program exits.

## Technologies Used

- **C**, compiled with `cc -Wall -Wextra -Werror -pthread`.
- **POSIX threads** (`pthread_create`, `pthread_join`).
- **Mutexes** (`pthread_mutex_t`) to protect the shared simulation state (dongles,
  scheduling queue, burnout flag) and the console output.
- **A condition variable** (`pthread_cond_t`) used with `pthread_cond_wait` /
  `pthread_cond_broadcast` so coder threads sleep while waiting for a dongle instead of
  busy-looping, and are woken up whenever the state that matters to them might have
  changed.
- **`gettimeofday`** for millisecond timestamps, **`usleep`** for the compile/debug/
  refactor delays and for the monitor's polling interval.
- A hand-written **binary heap** used as the scheduling priority queue.

## Project Structure

```
.
├── Makefile
├── README.md
├── coders/
│   ├── codexion.h      # Shared types (t_sim, t_coder, t_dongle, t_heap, t_args, t_request) and prototypes
│   ├── main.c           # Entry point: parse args, init, spawn/join threads, cleanup
│   ├── parsing.c        # Argument parsing and validation
│   ├── init.c            # Simulation, coder, dongle and heap initialization
│   ├── threads.c        # Creation/joining of the coder threads
│   ├── routine.c        # The coder thread's compile / debug / refactor loop
│   ├── dongles.c        # Checking/taking/releasing a coder's two dongles + cooldown
│   ├── scheduler.c      # Requesting/releasing a turn to compile via the priority queue
│   ├── monitor.c        # Monitor thread: burnout detection and stop condition
│   ├── heap.c            # Priority queue push/peek/pop
│   ├── heap_utils.c    # sift_up/sift_down/is_higher_priority (fifo vs edf ordering)
│   └── utils.c          # Timestamps, serialized logging, sleep helper, stop check
```

## Installation / Setup

Requirements: a POSIX-compliant environment (Linux/macOS) with `cc` and a pthread
implementation available.

```bash
git clone <this-repository-url>
cd Codexion-main
make
```

This produces an executable named `codexion` at the project root. `make clean` removes
the object files, `make fclean` also removes the binary, and `make re` rebuilds from
scratch.

## How to Run

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument                        | Meaning                                                                 |
|----------------------------------|--------------------------------------------------------------------------|
| `number_of_coders`               | Number of coder threads (and number of dongles).                        |
| `time_to_burnout`                 | Max delay (ms) before a coder must start compiling again, or burn out. |
| `time_to_compile`                 | Time (ms) spent compiling.                                              |
| `time_to_debug`                    | Time (ms) spent debugging.                                              |
| `time_to_refactor`                | Time (ms) spent refactoring.                                            |
| `number_of_compiles_required`     | Simulation stops successfully once every coder reaches this count.      |
| `dongle_cooldown`                  | Time (ms) a dongle stays unavailable after being released.              |
| `scheduler`                        | `fifo` or `edf` — how contested dongles are attributed.                 |

All eight arguments are mandatory. `codexion` will refuse to start (and print an error)
if the argument count is wrong, if any of the first seven arguments is not a positive
integer, or if `scheduler` is anything other than `fifo` or `edf`.

### Example

```bash
./codexion 5 3000 200 200 200 4 50 fifo
```

Sample output (format: `timestamp_in_ms coder_id event`):

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
402 2 has taken a dongle
403 2 has taken a dongle
403 2 is compiling
...
```

## How the Main Parts Work

- **`main.c`** parses and validates the arguments, initializes the simulation
  (`init_sim`), spawns the coder threads and the monitor thread, waits for them to
  finish, and frees every resource in `cleanup_sim`.
- **`init.c`** allocates the coders and dongles arrays and the heap, and sets up the
  seating arrangement: coder `i` (0-indexed) is assigned dongle `i` as its left dongle
  and dongle `(i + 1) % n` as its right dongle, so each dongle is shared by exactly two
  neighbours (or, with a single coder, its "left" and "right" dongle are the same slot).
- **`routine.c`** is the loop each coder thread runs: request a turn to compile
  (`wait_for_turn`), log and sleep for `time_to_compile`, release the dongles
  (`release_turn`), then log/sleep through debugging and refactoring before looping
  back.
- **`scheduler.c`** is where a coder registers its request in the shared priority queue
  with its arrival time and its deadline (`last_compile_start + time_to_burnout`), then
  waits on the condition variable until it is both at the front of the queue and able to
  take its two dongles (`dongles.c`). Once it can proceed, it pops itself off the queue,
  takes the dongles, and records the new `last_compile_start`.
- **`dongles.c`** checks whether a coder's two neighbouring dongles are both on the
  table and past their cooldown, marks them as held by the coder, or puts them back on
  the table with a new `available_time` (`now + dongle_cooldown`) when released.
- **`heap.c` / `heap_utils.c`** implement a small binary min-heap used as the
  scheduling queue: `is_higher_priority` compares by deadline first under `edf` (falling
  back to arrival time to break ties), or purely by arrival time under `fifo`.
- **`monitor.c`** runs in its own thread, waking up roughly every millisecond to check
  every coder's `last_compile_start` against `time_to_burnout`. If a coder has gone too
  long without compiling, it logs `burned out` and sets a shared `stop` flag; it also
  broadcasts on the condition variable on every pass so that coder threads waiting for a
  dongle periodically re-check whether they can now proceed (this is what lets the
  compile/cooldown windows be re-evaluated without a timed wait).
- **`utils.c`** centralizes timestamped, mutex-protected logging (`print_status`), a
  helper to read the shared `stop` flag safely, and a sleep helper that wakes up early if
  the simulation has been stopped.

## Blocking Cases Handled

- **Simultaneous dongle acquisition (deadlock avoidance):** a coder never holds just one
  of its two dongles. Both are checked and taken together, under the same mutex-held
  section, so there is no window where a coder holds one dongle and waits forever for
  the other while its neighbour does the opposite (the classic dining-philosophers
  deadlock).
- **Fair arbitration / starvation prevention:** when several coders want the same
  dongle, the shared priority queue decides who goes first, according to the selected
  `scheduler` (arrival order for `fifo`, earliest burnout deadline for `edf`, with
  arrival order as the tie-breaker). This avoids one coder repeatedly winning a dongle
  race while its neighbour starves.
- **Dongle cooldown:** a released dongle records its `available_time`
  (`now + dongle_cooldown`) and cannot be taken again before that time, which is checked
  in `can_take_dongles`.
- **Precise burnout detection:** the monitor thread polls at a ~1 ms interval and
  computes burnout from `last_compile_start`, so a burnout is logged very shortly after
  it actually occurs, in line with the subject's 10 ms tolerance.
- **Serialized logging:** all status lines go through `print_status`, which locks a
  dedicated `print_lock` mutex around the timestamp/format/print, so two coder threads
  (or a coder and the monitor) can never interleave their output on the same line.
- **Clean stop propagation:** once the monitor sets `sim->stop`, it broadcasts on the
  condition variable so any coder thread currently waiting for a dongle wakes up,
  notices the stop flag, and exits its loop instead of blocking forever.

## Thread Synchronization Mechanisms

- **`sim->lock` (`pthread_mutex_t`):** the single mutex protecting all shared
  simulation state — the dongle array, the scheduling heap, each coder's
  `last_compile_start`, and the `stop` flag. Every read or modification of this state
  (in `wait_for_turn`, `release_turn`, `monitor_routine`, `is_stopped`) happens while
  this mutex is held, which is what prevents race conditions between coder threads and
  the monitor.
- **`sim->wake` (`pthread_cond_t`):** coder threads call `pthread_cond_wait(&sim->wake,
  &sim->lock)` while they are not yet allowed to take their dongles (i.e. they are not
  at the head of the queue, or their dongles are not both free and past cooldown). They
  are woken up by `pthread_cond_broadcast` calls made after a coder releases its dongles
  (`release_turn`) or after each monitor pass (`monitor_routine`), which is also how
  cooldown expiry and burnout/stop are picked up without a timed wait.
- **`sim->print_lock` (`pthread_mutex_t`):** a mutex dedicated to `printf` calls in
  `print_status`, keeping log lines from different threads from interleaving.
- **Race-condition example prevented:** without `sim->lock`, two neighbouring coders
  could both observe their shared dongle as free at the same instant and both try to
  take it. Because `can_take_dongles`, `take_dongles` and the heap pop in
  `wait_for_turn` all happen under the same locked section, only one coder can win that
  check-and-take sequence.
- **Coder ↔ monitor communication:** the monitor never touches a coder's thread
  directly; it only reads `last_compile_start`/`finished` and writes `sim->stop` under
  `sim->lock`, and signals waiting coders through `sim->wake`. This keeps all
  cross-thread communication limited to the two shared, mutex-protected structures
  (`t_sim` and `t_coder`) rather than any ad hoc signalling.

## Notes and Limitations

- With `number_of_coders = 1`, the single coder's left and right dongle map to the same
  slot, so it can never simultaneously hold "two" dongles — it will always burn out
  after `time_to_burnout` ms. This matches the subject ("if there is only one coder,
  there should be only one dongle on the table") and is confirmed by the smoke tests in
  `test/run_tests.sh`.
- Timings rely on `gettimeofday` and `usleep`, so exact millisecond precision can vary
  slightly with system load, as acknowledged by the subject's own tolerance for burnout
  logging (within 10 ms).
- The `test/` folder contains a small, ungraded smoke-test script (`run_tests.sh`) used
  during development to sanity-check argument validation and basic simulation behaviour
  under both schedulers; it is not part of the mandatory deliverable.

## Resources

- [POSIX Threads Programming (LLNL tutorial)](https://hpc-tutorials.llnl.gov/posix/)
- [`man pthread_create`, `man pthread_mutex_lock`, `man pthread_cond_wait`](https://man7.org/linux/man-pages/)
- [The Little Book of Semaphores](https://greenteapress.com/wp/semaphores/) — background
  reading on classic synchronization problems such as the dining philosophers.
- [Binary heap (priority queue) — Wikipedia](https://en.wikipedia.org/wiki/Binary_heap)
- [Earliest deadline first scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

### How I Used AI

I used AI mainly as a learning and support tool while working through the
synchronization side of this project, rather than as something that wrote the project
for me. Concretely:

- I asked it to point me to sources and references on POSIX threads, mutexes, and
  condition variables so I could read up on the underlying concepts myself.
- When I got stuck on specific ideas — like why a naive "take dongle A, then dongle B"
  approach can deadlock, or how a condition variable actually avoids busy-waiting — I
  asked the AI to explain those concepts until they made sense to me.
- I asked it for small exercises and practical examples (unrelated to my own code) to
  practice mutexes, condition variables, and priority queues before applying them here.
- I then took what I learned from those explanations and exercises and used it to
  design and write my own implementation, including the dongle acquisition logic, the
  fifo/edf scheduling queue, and the monitor thread — testing and adjusting it myself
  along the way rather than pasting in generated code.

In short, AI was used to understand the "why" behind threading and synchronization and
to get extra practice, while the actual design and code of Codexion were written and
debugged by me.