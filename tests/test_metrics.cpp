#include <cassert>
#include <vector>

#include "refractoryrt/metrics.h"

using namespace refractoryrt;

int main() {
  std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
  assert(percentile(data, 50.0) == 3.0);
  assert(percentile(data, 0.0) == 1.0);
  assert(percentile(data, 100.0) == 5.0);

  SimulationResult result;
  result.start_time = 0.0;
  result.end_time = 10.0;
  result.records.push_back({Task{1, OpType::ATTENTION, 0.0, 1.0, 1.0}, 0.0, 2.0});
  result.records.push_back({Task{2, OpType::KV_CACHE, 1.0, 1.0, 1.0}, 2.0, 4.0});
  result.records.push_back({Task{3, OpType::LAYERNORM, 2.0, 1.0, 1.0}, 4.0, 5.0});

  const auto m = compute_metrics(result);
  assert(m.average_latency > 0.0);
  assert(m.p95_latency >= m.p50_latency);
  assert(m.max_latency >= m.p99_latency);
  assert(m.throughput > 0.0);

  return 0;
}
