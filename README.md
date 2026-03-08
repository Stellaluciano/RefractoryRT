# RefractoryRT

All rights reserved. Unauthorized copying, modification, distribution, or use of this repository, via any medium, is strictly prohibited without prior written permission.

RefractoryRT is a lightweight C++17 scheduling simulator that models cooldown-aware operator scheduling for AI inference workloads.

## Motivation

Dynamic AI inference workloads often produce burst-induced contention: a hot operator can dominate scheduling, reduce fairness for other operator classes, and worsen tail latency. RefractoryRT studies a practical scheduling response inspired by neural refractory behavior.

## Key idea

RefractoryRT introduces a **refractory scheduler** that applies cooldown penalties to recently executed operator classes:

- **Absolute refractory analogue:** immediate suppression after rapid repeated execution.
- **Relative refractory analogue:** penalty gradually decays over time.
- **Starvation guard:** long-waiting tasks can bypass most cooldown penalty.

Scoring model:

```text
effective_score =
  base_priority
  + waiting_time_weight * waiting_time
  - refractory_penalty
  - optional_contention_penalty
```

This favors urgency and fairness while reducing hot-operator monopolization.

## Repository structure

```text
RefractoryRT/
  CMakeLists.txt
  README.md
  NOTICE
  .gitignore
  include/refractoryrt/
    types.h
    task.h
    operator_state.h
    refractory_policy.h
    scheduler.h
    metrics.h
    simulator.h
    workload.h
  src/
    refractory_policy.cpp
    scheduler.cpp
    metrics.cpp
    simulator.cpp
    workload.cpp
    main.cpp
  bench/
    benchmark_bursty.cpp
    benchmark_mixed.cpp
    benchmark_tail.cpp
  tests/
    test_refractory_policy.cpp
    test_scheduler.cpp
    test_metrics.cpp
```

## Build

```bash
cmake -S . -B build
cmake --build build -j
```

Supported targets:
- `refractoryrt_cli`
- `benchmark_bursty`
- `benchmark_mixed`
- `benchmark_tail`
- `test_refractory_policy`, `test_scheduler`, `test_metrics`

## Example commands

```bash
./build/refractoryrt_cli
./build/benchmark_bursty
./build/benchmark_mixed
./build/benchmark_tail
ctest --test-dir build --output-on-failure
```

## Benchmark scenarios

- **Bursty:** attention-heavy surges with sparse lighter operations.
- **Mixed:** balanced operator mix to inspect average latency and fairness.
- **Tail-stress:** repeated heavy surges to evaluate p95/p99 and maximum latency.

## Expected qualitative findings

Compared to FIFO and static priority, refractory scheduling tends to:
- reduce ATTENTION burst lock-in,
- improve per-operator balance and Jain fairness,
- stabilize p95/p99 latency under synthetic contention windows.

## Limitations

- This is a synthetic simulator, not a production runtime.
- It does not model CUDA kernels, hardware queues, or device-level scheduling.
- It uses single-worker event-driven execution for clarity and speed.

## Future work

- Multi-worker and queue-topology-aware scheduling.
- Path-level refractory control for operator graph segments.
- Integration experiments with real inference runtimes.
