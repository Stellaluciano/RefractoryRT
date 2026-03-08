#include <iomanip>
#include <iostream>

#include "refractoryrt/metrics.h"
#include "refractoryrt/scheduler.h"
#include "refractoryrt/simulator.h"
#include "refractoryrt/types.h"
#include "refractoryrt/workload.h"

namespace {

void print_summary(const char* name, const refractoryrt::SummaryMetrics& m) {
  using refractoryrt::OpType;
  using refractoryrt::to_string;

  std::cout << "Scheduler: " << name << "\n";
  std::cout << std::fixed << std::setprecision(3);
  std::cout << "  Avg latency: " << m.average_latency << "\n";
  std::cout << "  P95 latency: " << m.p95_latency << "\n";
  std::cout << "  P99 latency: " << m.p99_latency << "\n";
  std::cout << "  Throughput: " << m.throughput << "\n";
  std::cout << "  Fairness:   " << m.fairness_index << "\n";
  std::cout << "  Per-op wait:\n";
  for (auto op : {OpType::ATTENTION, OpType::KV_CACHE, OpType::LAYERNORM}) {
    std::cout << "    " << to_string(op) << ": " << m.per_op_avg_wait.at(op) << "\n";
  }
  std::cout << "\n";
}

}  // namespace

int main() {
  using namespace refractoryrt;

  const auto workload = generate_bursty_workload(300);
  Simulator simulator;

  print_summary("FIFO", compute_metrics(simulator.run(workload, make_fifo_scheduler())));
  print_summary("PRIORITY", compute_metrics(simulator.run(workload, make_priority_scheduler())));
  print_summary("REFRACTORY", compute_metrics(simulator.run(workload, make_refractory_scheduler())));

  return 0;
}
