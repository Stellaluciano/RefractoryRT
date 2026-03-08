#include "refractoryrt/metrics.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace refractoryrt {

double percentile(std::vector<double> values, double p) {
  if (values.empty()) {
    return 0.0;
  }
  if (p <= 0.0) {
    return *std::min_element(values.begin(), values.end());
  }
  if (p >= 100.0) {
    return *std::max_element(values.begin(), values.end());
  }

  std::sort(values.begin(), values.end());
  const double idx = (p / 100.0) * (static_cast<double>(values.size() - 1));
  const auto lo = static_cast<std::size_t>(std::floor(idx));
  const auto hi = static_cast<std::size_t>(std::ceil(idx));
  if (lo == hi) {
    return values[lo];
  }

  const double frac = idx - static_cast<double>(lo);
  return values[lo] + frac * (values[hi] - values[lo]);
}

SummaryMetrics compute_metrics(const SimulationResult& result) {
  SummaryMetrics summary;
  if (result.records.empty() || result.end_time <= result.start_time) {
    return summary;
  }

  std::vector<double> latencies;
  latencies.reserve(result.records.size());

  std::unordered_map<OpType, double> op_wait_sum;
  std::unordered_map<OpType, int> op_counts;

  for (const auto& record : result.records) {
    const double latency = record.finish_time - record.task.arrival_time;
    const double wait = record.start_time - record.task.arrival_time;

    latencies.push_back(latency);
    op_wait_sum[record.task.op_type] += wait;
    op_counts[record.task.op_type] += 1;
  }

  const double total = std::accumulate(latencies.begin(), latencies.end(), 0.0);
  summary.average_latency = total / static_cast<double>(latencies.size());
  summary.p50_latency = percentile(latencies, 50.0);
  summary.p95_latency = percentile(latencies, 95.0);
  summary.p99_latency = percentile(latencies, 99.0);
  summary.max_latency = *std::max_element(latencies.begin(), latencies.end());

  const double duration = result.end_time - result.start_time;
  summary.throughput = static_cast<double>(result.records.size()) / duration;

  double variance_acc = 0.0;
  for (double x : latencies) {
    const double d = x - summary.average_latency;
    variance_acc += d * d;
  }
  summary.latency_stddev = std::sqrt(variance_acc / static_cast<double>(latencies.size()));

  for (auto op : {OpType::ATTENTION, OpType::KV_CACHE, OpType::LAYERNORM}) {
    const int count = op_counts[op];
    if (count == 0) {
      summary.per_op_avg_wait[op] = 0.0;
    } else {
      summary.per_op_avg_wait[op] = op_wait_sum[op] / static_cast<double>(count);
    }
  }

  std::vector<double> per_op_values;
  for (auto op : {OpType::ATTENTION, OpType::KV_CACHE, OpType::LAYERNORM}) {
    per_op_values.push_back(summary.per_op_avg_wait[op] + 1e-6);
  }

  double sum = 0.0;
  double sum_sq = 0.0;
  for (double v : per_op_values) {
    sum += v;
    sum_sq += v * v;
  }
  const double n = static_cast<double>(per_op_values.size());
  summary.fairness_index = (sum * sum) / (n * sum_sq);

  return summary;
}

}  // namespace refractoryrt
