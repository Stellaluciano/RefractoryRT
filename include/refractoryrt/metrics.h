#pragma once

#include <unordered_map>
#include <vector>

#include "refractoryrt/task.h"
#include "refractoryrt/types.h"

namespace refractoryrt {

struct TaskRunRecord {
  Task task;
  double start_time = 0.0;
  double finish_time = 0.0;
};

struct SimulationResult {
  std::vector<TaskRunRecord> records;
  double start_time = 0.0;
  double end_time = 0.0;
};

struct SummaryMetrics {
  double average_latency = 0.0;
  double p50_latency = 0.0;
  double p95_latency = 0.0;
  double p99_latency = 0.0;
  double max_latency = 0.0;
  double throughput = 0.0;
  double latency_stddev = 0.0;
  double fairness_index = 0.0;
  std::unordered_map<OpType, double> per_op_avg_wait;
};

double percentile(std::vector<double> values, double p);
SummaryMetrics compute_metrics(const SimulationResult& result);

}  // namespace refractoryrt
