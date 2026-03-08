#include <iomanip>
#include <iostream>

#include "refractoryrt/metrics.h"
#include "refractoryrt/scheduler.h"
#include "refractoryrt/simulator.h"
#include "refractoryrt/types.h"
#include "refractoryrt/workload.h"

using namespace refractoryrt;

void report(const char* name, const SummaryMetrics& m) {
  std::cout << "Scheduler: " << name << "\n";
  std::cout << std::fixed << std::setprecision(3)
            << "Avg latency: " << m.average_latency << "\n"
            << "P95 latency: " << m.p95_latency << "\n"
            << "P99 latency: " << m.p99_latency << "\n"
            << "Throughput: " << m.throughput << "\n"
            << "Fairness: " << m.fairness_index << "\n"
            << "Per-op wait:\n";
  for (auto op : {OpType::ATTENTION, OpType::KV_CACHE, OpType::LAYERNORM}) {
    std::cout << "  " << to_string(op) << ": " << m.per_op_avg_wait.at(op) << "\n";
  }
  std::cout << "\n";
}

int main() {
  const auto workload = generate_bursty_workload(1200);
  Simulator simulator;

  report("FIFO", compute_metrics(simulator.run(workload, make_fifo_scheduler())));
  report("PRIORITY", compute_metrics(simulator.run(workload, make_priority_scheduler())));
  report("REFRACTORY", compute_metrics(simulator.run(workload, make_refractory_scheduler())));

  return 0;
}
