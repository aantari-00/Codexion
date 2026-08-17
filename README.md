*This project has been created as part of the 42 curriculum by halaroub*

---

Description

Codexion is a multithreading simulation project that implements a circular seating arrangement of coders sharing USB dongles to access a Quantum Compiler. Each coder must alternately compile, debug, and refactor code, requiring two dongles simultaneously to compile. The simulation demonstrates fundamental concurrency concepts including thread synchronization, resource arbitration, and deadlock prevention.

The goal of this project is to simulate a real-time system where coders (threads) compete for limited resources (dongles) while respecting scheduling policies (FIFO or EDF) and avoiding burnout. A coder burns out if they fail to start compiling within a specified time window (time_to_burnout) since their last compilation or the start of simulation.

The simulation stops either when a coder burns out or when all coders have successfully compiled the required number of times (number_of_compiles_required).

---

Instructions

Compilation

To compile the project, run:

```bash
make
```

This will generate the executable codexion using the flags -Wall -Wextra -Werror -pthread.

Available Makefile rules:

· make all - Compile the project
· make clean - Remove object files
· make fclean - Remove object files and executable
· make re - Recompile from scratch

Execution

Run the program with the following mandatory arguments:

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

Arguments:

· number_of_coders: Number of coders and number of dongles
· time_to_burnout: Time in milliseconds before a coder burns out if not compiling
· time_to_compile: Time in milliseconds required to compile
· time_to_debug: Time in milliseconds spent debugging
· time_to_refactor: Time in milliseconds spent refactoring
· number_of_compiles_required: Number of compilations required per coder to end simulation successfully
· dongle_cooldown: Time in milliseconds a dongle is unavailable after release
· scheduler: Scheduling policy: fifo or edf

Example:

```bash
./codexion 5 5000 1000 500 500 3 2000 edf
```

Output Format

The program outputs timestamped logs for each state change:

```
<timestamp> <coder_id> has taken a dongle
<timestamp> <coder_id> is compiling
<timestamp> <coder_id> is debugging
<timestamp> <coder_id> is refactoring
<timestamp> <coder_id> burned out
```

---

Blocking Cases Handled

Deadlock Prevention (Coffman's Conditions)

Rather than juggling per-dongle mutexes with a lock-ordering scheme, this
implementation uses a single mutex (sim->lock) to protect the dongles,
the coders' state and the scheduling queue together. A coder never holds
one dongle while waiting for the other: wait_for_turn() (scheduler.c)
grants both dongles atomically, under that one lock, or none at all. This
sidesteps Coffman's conditions instead of juggling them individually:

1. Mutual Exclusion: still present and necessary — a dongle's state
   (holder, available_time) can only be read or changed while holding
   sim->lock.
2. Hold and Wait: eliminated. A coder registers a request, waits on a
   condition variable, and only ever comes out of that wait already
   holding both dongles (see can_take_dongles() in dongles.c). There is
   no intermediate state where it holds one and waits for the other.
3. No Preemption: not needed, since Hold and Wait cannot occur.
4. Circular Wait: not needed either, for the same reason.

Since a program can only deadlock if all four conditions hold at once,
breaking condition 2 alone is enough to rule deadlock out entirely.

Starvation Prevention

A binary min-heap (heap.c) is used as the scheduler's priority queue:

· fifo: coders are ordered by req_time (when they started waiting), so
  the coder who has been waiting longest is always served first.
· edf: coders are ordered by deadline (last_compile_start +
  time_to_burnout), so whoever is closest to burning out is served
  first. Ties are broken by req_time.

Only the coder at the top of the heap is ever granted dongles, so
requests are always served in a well-defined priority order and a
newer, lower-priority request can never overtake an older one.

Cooldown Handling

When a coder releases its dongles (release_turn() in scheduler.c), each
dongle's available_time is set to the current time plus
dongle_cooldown. can_take_dongles() checks both holder == -1 and
now >= available_time, so a dongle cannot be re-taken, by anyone, until
its cooldown has fully elapsed.

Precise Burnout Detection

A dedicated monitor thread (monitor.c) checks every coder roughly every
millisecond (a 1ms usleep between checks). A coder has burned out once
now - last_compile_start >= time_to_burnout. last_compile_start is the
moment the coder's last compile began (not ended), matching the
subject's definition of burnout. Once a burnout is found, the monitor
prints it and sets the stop flag immediately, so the message appears
well within the 10ms tolerance required by the subject (measured at a
few milliseconds in practice, see test/run_tests.sh for how to
reproduce a burnout run).

Log Serialization

print_status() (utils.c) locks a dedicated print_lock mutex before
computing the timestamp and calling printf, and unlocks it right after.
Since every log line goes through this same function and mutex, two
messages can never interleave on the same line.

---

Thread Synchronization Mechanisms

Mutexes (pthread_mutex_t)

Two mutexes are used:

· sim->lock: the main lock. It protects the dongles' state (holder,
  available_time), each coder's state (last_compile_start,
  compiles_done, finished) and the scheduler's priority queue, all
  together. Using one lock for all of this shared state keeps the
  locking easy to reason about: a coder is granted both dongles in a
  single atomic step (see "Hold and Wait" above), and there is no risk
  of forgetting to lock one piece of state while touching another.
  Used in wait_for_turn(), release_turn() and monitor_routine().
· sim->print_lock: a separate, dedicated mutex used only by
  print_status() to serialize console output. Keeping it separate from
  sim->lock means a coder does not need to hold the main lock while
  printing (printf can be comparatively slow), so other coders are
  never blocked from taking dongles just because someone else is
  writing a log line.

Condition Variables (pthread_cond_t)

sim->wake is the single condition variable coders wait on while they
do not yet have their dongles:

```c
// wait_for_turn(), scheduler.c
pthread_mutex_lock(&sim->lock);
heap_push(&sim->queue, req, sim->args.scheduler);
while (!sim->stop && !is_coders_turn(sim, coder, get_time_in_ms()))
    pthread_cond_wait(&sim->wake, &sim->lock);
// ... coder now holds both dongles, or the simulation is stopping
pthread_mutex_unlock(&sim->lock);
```

Every time dongles are released (release_turn()) or the simulation
stops (monitor_routine()), pthread_cond_broadcast() wakes every waiting
coder so each can re-check, under the lock, whether it is now its turn.

Race Condition Prevention Examples

Example 1: granting dongles atomically

```c
// wait_for_turn(), scheduler.c — still holding sim->lock here
heap_pop(&sim->queue, sim->args.scheduler);
take_dongles(coder);
coder->last_compile_start = get_time_in_ms();
```

Because the heap pop, the dongle state change and the
last_compile_start update all happen while sim->lock is held, no other
thread (including the monitor) can ever observe a half-updated state:
a coder is either "still waiting" or "holding both dongles with an
up-to-date last_compile_start", never something in between.

Example 2: comparing EDF deadlines safely

```c
// heap.c
static int is_higher_priority(t_request *a, t_request *b, int scheduler)
{
    if (scheduler == SCHEDULER_EDF && a->deadline != b->deadline)
        return (a->deadline < b->deadline);
    return (a->req_time < b->req_time);
}
```

Each request's deadline is computed once, under sim->lock, when the
coder starts waiting (get_deadline() in scheduler.c), and stored in the
heap entry. Comparisons made later while sifting the heap read that
stored value, so they never race with a coder's last_compile_start
changing concurrently.

Thread-Safe Communication Between Coders and Monitor

The monitor thread (monitor.c) and the coder threads only ever talk to
each other through state guarded by sim->lock:

```c
// monitor_routine() holds sim->lock while checking every coder
static int coder_burned_out(t_coder *coder, long now)
{
    return (!coder->finished
        && now - coder->last_compile_start >= coder->sim->args.time_to_burnout);
}
```

```c
// monitor_routine(), still under sim->lock
if (should_stop)
    sim->stop = 1;
pthread_cond_broadcast(&sim->wake);
```

Setting stop and broadcasting happen together, under the same lock
that coder threads check in their wait loop, so a coder can never miss
a stop signal or read a stale value of last_compile_start.

---

Resources

Video Tutorials

Process VS Thread (Part 1)
https://youtu.be/v8AgQ0xhM6I?si=BJJUUHeZZwvuoXGh

Process VS Thread (Part 2)
https://youtu.be/YIaMwZmWMGo?si=9SGtouXz14vaZVdE

Synchronous/Asynchronous/Multithreading
https://youtu.be/Vxhp6_WF2Is?si=yjHHeWw6O2cxOnqJ

Articles & Documentation

Understanding Multi-threading Basics
https://medium.com/@anton.baksheiev/understanding-the-fundamentals-of-multi-threading-a-beginners-guide-b6585844a538

Race Conditions in Operating Systems
https://www.geeksforgeeks.org/operating-systems/race-condition-in-operating-systems/

What is a Mutex?
https://stackoverflow.com/questions/34524/what-is-a-mutex

Condition Variables Tutorial
https://www.cs.tufts.edu/comp/21/notes/condition_variables/index.html

Deadlock, Starvation and Livelock
https://www.geeksforgeeks.org/operating-systems/deadlock-starvation-and-livelock/

AI Assistance

AI (Claude) was used throughout this project, specifically for:

· Reading and summarizing the subject PDF to make sure every requirement
  (arguments, log format, burnout timing, scheduler behaviour, Makefile
  rules) was accounted for.
· Designing the synchronization strategy: a single lock guarding the
  dongles, the coders' state and the scheduling queue, chosen to
  eliminate the "hold and wait" Coffman condition instead of juggling
  per-dongle mutexes with a lock-ordering scheme.
· Implementing the binary min-heap priority queue (heap.c) used for
  fifo/edf scheduling, and the coder/monitor/scheduler logic built on
  top of it.
· Writing the smoke tests in test/run_tests.sh and checking the result
  with ThreadSanitizer and AddressSanitizer/UBSan to look for data
  races, memory leaks and undefined behaviour.
· Writing this README.

Every generated part was reviewed line by line and re-checked against
the subject before being kept, and re-tested after each change with the
sanitizer builds described above.
