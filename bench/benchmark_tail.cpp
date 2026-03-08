#include <iomanip>
#include <iostream>

#include "refractoryrt/metrics.h"
#include "refractoryrt/scheduler.h"
#include "refractoryrt/simulator.h"
#include "refractoryrt/workload.h"

using namespace refractoryrt;

void print_tail(const char* name, const SummaryMetrics& m) {
  std::cout << name << " => p95=" << m.p95_latency << " p99=" << m.p99_latency
            << " max=" << m.max_latency << " stddev=" << m.latency_stddev << "\n";
}

int main() {
  const auto workload = generate_tail_stress_workload(1600);
  Simulator simulator;

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "Tail-Stress Benchmark\n";
  print_tail("FIFO", compute_metrics(simulator.run(workload, make_fifo_scheduler())));
  print_tail("PRIORITY", compute_metrics(simulator.run(workload, make_priority_scheduler())));
  print_tail("REFRACTORY", compute_metrics(simulator.run(workload, make_refractory_scheduler())));

  return 0;
}
