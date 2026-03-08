#include <iomanip>
#include <iostream>

#include "refractoryrt/metrics.h"
#include "refractoryrt/scheduler.h"
#include "refractoryrt/simulator.h"
#include "refractoryrt/types.h"
#include "refractoryrt/workload.h"

using namespace refractoryrt;

int main() {
  const auto workload = generate_mixed_workload(1400);
  Simulator simulator;

  const auto fifo = compute_metrics(simulator.run(workload, make_fifo_scheduler()));
  const auto priority = compute_metrics(simulator.run(workload, make_priority_scheduler()));
  const auto refractory = compute_metrics(simulator.run(workload, make_refractory_scheduler()));

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "Mixed Workload Comparison\n";
  std::cout << "Scheduler       AvgLatency  Fairness  Throughput\n";
  std::cout << "FIFO            " << fifo.average_latency << "      " << fifo.fairness_index
            << "     " << fifo.throughput << "\n";
  std::cout << "PRIORITY        " << priority.average_latency << "      " << priority.fairness_index
            << "     " << priority.throughput << "\n";
  std::cout << "REFRACTORY      " << refractory.average_latency << "      "
            << refractory.fairness_index << "     " << refractory.throughput << "\n";

  return 0;
}
