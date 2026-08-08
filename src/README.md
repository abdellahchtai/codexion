*This project has been created as part of the 42 curriculum by abchtaib.*

# Codexion

**Master the race for resources before the deadline masters you.**

---

## Description

Codexion is a multithreaded simulation inspired by the classic *Dining Philosophers* problem, reframed around a team of coders competing for a limited pool of shared **dongles**. Each coder needs **two specific dongles at once** (its left and right neighbor in a ring) in order to compile. Once it has both, it compiles, debugs, and refactors before releasing the dongles back into the pool — where they enter a cooldown period before becoming available again.

Every coder is racing against its own deadline: if too much time passes without successfully compiling, that coder **burns out** and the simulation ends. The goal of the project is to correctly synchronize a pool of concurrent threads (one per coder, plus a monitor thread) so that:

- No two coders ever hold the same dongle at the same time (mutual exclusion).
- No deadlock ever occurs, regardless of how many coders or dongles are involved.
- No coder starves indefinitely while others keep compiling.
- Burnout is detected precisely, and the simulation stops cleanly and immediately when it happens.
- All console output remains readable and correctly ordered under heavy thread contention.

The project supports two scheduling strategies for resolving contention when multiple coders want the same dongle: **FIFO** (first come, first served) and **EDF** (Earliest Deadline First, i.e. whoever is closest to burning out is served first).

---

## Instructions

### Compilation

```bash
make
```

This produces the executable `codexion` (or `a.out`, depending on your Makefile target name) at the project root. The Makefile also supports the standard 42 targets:

```bash
make        # build the project
make clean  # remove object files
make fclean # remove object files and the binary
make re     # fclean + make
```

### Execution

```bash
./codexion <nb_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <nb_of_compiles_required> <dongle_cooldown> <scheduler>
```

| Argument                    | Description                                                              |
|-----------------------------|---------------------------------------------------------------------------|
| `nb_of_coders`               | Number of coder threads (and dongles) to simulate                        |
| `time_to_burnout`            | Milliseconds a coder can go without compiling before burning out          |
| `time_to_compile`            | Milliseconds spent compiling once both dongles are acquired               |
| `time_to_debug`              | Milliseconds spent debugging after compiling                              |
| `time_to_refactor`           | Milliseconds spent refactoring after debugging                           |
| `nb_of_compiles_required`    | Number of successful compile cycles before a coder finishes normally      |
| `dongle_cooldown`            | Milliseconds a dongle must "rest" after release before it can be reused   |
| `scheduler`                  | Contention resolution strategy: `fifo` or `edf`                          |

Example:

```bash
./codexion 4 2000 90 60 60 10 600 edf
```

This runs 4 coders, each needing to complete 10 compiles, with a 2000ms burnout window, using Earliest Deadline First scheduling to resolve dongle contention.

**Choosing a feasible `time_to_burnout`:** because each dongle is shared between exactly two neighbors and must go through a compile + cooldown cycle before being reused, there is a hard lower bound below which burnout is mathematically unavoidable regardless of scheduling fairness:

```
even number :minimum_time_to_burnout ≈ 2 × (time_to_compile + dongle_cooldown + marge for execution)
odd  number: minimum_time_to_burnout ≈ 3 × (time_to_compile + dongle_cooldown + marge for execution)
```

Values at or below this bound will legitimately trigger burnout — that reflects the simulation correctly reporting an infeasible deadline, not a synchronization bug.

---

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)

Deadlock requires four conditions to hold simultaneously: mutual exclusion, hold-and-wait, no preemption, and circular wait. Rather than allowing all four and detecting/recovering from deadlock, this project **breaks the conditions that can be broken safely**:

- **Consistent lock ordering.** Every coder that needs two dongles always locks the same pair of dongles in the same relative order (`first_dg` before `second_dg`) as every other coder sharing that dongle pair. Two threads can never be found holding one lock while waiting on a lock the other already holds in reverse order — the classic circular-wait scenario is structurally impossible.
- **Atomic acquisition intent via a shared request queue.** A coder registers its intent to acquire both dongles by pushing a request into each dongle's priority queue (min-heap) before attempting to claim either one. A coder only actually claims (marks `available = 0`) a dongle once it holds the locks on *both* dongles simultaneously and confirms it is at the front of both queues. This avoids a coder holding one dongle indefinitely while blocked waiting for the second (partial hold-and-wait), since acquisition of both dongles is checked and committed atomically under both locks.
- **Bounded waiting instead of busy-holding.** If a coder cannot acquire both dongles, it releases both mutexes immediately and waits on a condition variable rather than holding either lock while sleeping — eliminating any scenario where a sleeping thread blocks progress for others.

### Starvation prevention

With `time_to_burnout` values close to the theoretical minimum, a naive scheduler can let one coder win every tie indefinitely while its neighbors starve. Two mechanisms prevent this:

- **Staggered thread start.** Even-numbered coders briefly delay their very first dongle request relative to odd-numbered coders. On a ring topology, this guarantees the very first round of contention splits cleanly along parity lines (odd-id coders vs. even-id coders) instead of producing a simultaneous tie that a single tiebreaker would resolve the same way every round.
- **Deadline-driven fairness (EDF) with a stable tiebreaker.** After the first round, each coder's next deadline (`last_compile + time_to_burnout`) is computed from its own most recent compile time, so a coder that just won naturally gets a *later* deadline than one that is still waiting — guaranteeing that whoever has been waiting longest is prioritized on the next round. Ties that do occur (e.g. under FIFO scheduling, or a genuine simultaneous deadline) are resolved by a strictly monotonic global sequence counter, so ordering is always deterministic and never favors the same coder indefinitely.

### Cooldown handling

A dongle is not immediately reusable after release: `release_dongles` stamps it with `available_at = now + dongle_cooldown` and sets `available = 0` conceptually until that time passes. The acquisition check (`dongles_ready`) verifies both `available` and `now >= available_at` under the dongle's lock before allowing a claim, so a dongle can never be reacquired before its cooldown has fully elapsed, even under heavy contention from multiple waiting coders.

### Precise burnout detection

A dedicated monitor thread (`burnout_checker`) periodically inspects each coder's time since its last successful compile against `time_to_burnout`. As soon as any coder exceeds this window, the monitor:

1. Sets a shared `burnout_flag` under mutex protection.
2. Logs the burnout event with the exact simulation timestamp.
3. Broadcasts on every dongle's condition variable so that any coder currently blocked in `pthread_cond_timedwait` wakes up immediately rather than waiting out its poll timeout.

Every coder thread checks the burnout flag both before attempting to acquire dongles and at each stage of its compile/debug/refactor cycle, ensuring the simulation halts promptly and consistently across all threads once burnout is detected.

### Log serialization

All console output goes through a single logging function protected by a dedicated mutex, ensuring that concurrent `printf` calls from multiple coder threads (and the monitor thread) never interleave mid-line. Combined with reading the simulation clock and formatting the message while holding the same lock, this guarantees that each log line is atomic and that lines appear in an order consistent with the events that produced them.

---

## Thread synchronization mechanisms

| Primitive | Protects | Purpose |
|---|---|---|
| `pthread_mutex_t lock` (per dongle) | A dongle's `available`, `available_at`, and its request heap | Ensures only one thread at a time can inspect or mutate a given dongle's state |
| `pthread_mutex_t mutex_last_compile` (per coder) | A coder's `last_compile` timestamp | Prevents a race between a coder updating its own deadline and the scheduler reading it to compute request keys |
| `pthread_mutex_t mtx_fifo` (global) | The monotonic sequence/FIFO counter | Guarantees every request receives a unique, strictly increasing tiebreak value, even when multiple coders push requests concurrently |
| `pthread_mutex_t finished_mutex` (global) | `finished_coders` counter | Prevents a lost-update race when multiple coder threads finish and increment the shared completion counter simultaneously |
| `pthread_mutex_t burnout_flag_mutex` (global) | `burnout_flag` | Ensures the flag is read/written atomically between the monitor thread (writer) and all coder threads (readers) |
| `pthread_cond_t cond` (per dongle) | Coordinates waiting/waking around a dongle's availability | Lets a coder thread sleep efficiently instead of busy-spinning while waiting for a dongle, and wakes it promptly when the dongle is released or a burnout occurs |
| `pthread_mutex_t log_mutex` (global) | Console output | Serializes all `printf` calls so log lines are never interleaved or corrupted |

### How race conditions are prevented

A coder never inspects a dongle's `available` flag or its request queue without holding that dongle's mutex, and it always acquires the mutexes for **both** of its dongles before checking or committing to a claim — preventing a scenario where the dongle's state changes between the check and the claim (check-then-act race). Similarly, `last_compile` is only ever read or written while holding `mutex_last_compile`, so a coder computing its next deadline for a new request never observes a half-written timestamp.

### Thread-safe communication between coders and the monitor

The monitor thread and coder threads never communicate through unsynchronized shared memory. All coordination happens through:

- **The `burnout_flag`**, written once by the monitor under `burnout_flag_mutex` and polled by coders under the same lock before and during every stage of their cycle.
- **Condition variable broadcasts.** When the monitor detects burnout, it broadcasts on every dongle's condition variable (after acquiring that dongle's lock), which immediately wakes any coder thread currently parked in `pthread_cond_timedwait` — rather than making it wait out its polling timeout before noticing the simulation has ended.
- **The `finished_coders` counter**, incremented by each coder under `finished_mutex` as it completes its required number of compiles, which the monitor consults to determine whether the simulation has finished normally (all coders done) rather than via burnout.

This design means a coder thread never needs to poll shared state without a lock, and the monitor never needs to directly manipulate a coder's internal state — all interaction is mediated by mutex-protected flags and condition variable signaling, keeping the synchronization surface small and easy to reason about.

---

## Resources

### Classic references

- POSIX Threads Programming — [https://hpc-tutorials.llnl.gov/posix/](https://hpc-tutorials.llnl.gov/posix/)
- `man pthread_mutex_lock`, `man pthread_cond_wait`, `man pthread_cond_timedwait`
- E. G. Coffman, M. Elphick, A. Shoshani, *"System Deadlocks"*, ACM Computing Surveys, 1971 — origin of the four Coffman conditions for deadlock
- E. W. Dijkstra, *"Hierarchical ordering of sequential processes"* (the Dining Philosophers problem)
- C. L. Liu, J. W. Layland, *"Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment"*, 1973 — foundational paper on Earliest Deadline First (EDF) scheduling
- The Little Book of Semaphores, Allen B. Downey — free online resource covering classic synchronization problems and their solutions

### AI usage

-IA was used to find good tests for the project to see if it's correctly implemented.