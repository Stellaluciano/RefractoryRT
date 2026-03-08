#include "refractoryrt/simulator.h"

#include <algorithm>

namespace refractoryrt {

Simulator::Simulator(SimulatorConfig config) : config_(config) {}

double Simulator::contention_multiplier(OpType current, OpType previous, int streak) const {
  if (!config_.enable_contention_model || current != previous) {
    return 1.0;
  }

  double alpha = 0.03;
  if (current == OpType::ATTENTION) {
    alpha = 0.12;
  } else if (current == OpType::KV_CACHE) {
    alpha = 0.07;
  }

  return 1.0 + alpha * static_cast<double>(streak);
}

SimulationResult Simulator::run(const std::vector<Task>& workload,
                                std::unique_ptr<Scheduler> scheduler) const {
  SimulationResult result;
  if (workload.empty()) {
    return result;
  }

  std::vector<Task> tasks = workload;
  std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
    if (a.arrival_time == b.arrival_time) {
      return a.id < b.id;
    }
    return a.arrival_time < b.arrival_time;
  });

  result.start_time = tasks.front().arrival_time;
  double now = result.start_time;
  std::size_t next_arrival = 0;
  std::vector<Task> ready;

  OpType last_op = OpType::LAYERNORM;
  int streak = 0;

  while (next_arrival < tasks.size() || !ready.empty()) {
    while (next_arrival < tasks.size() && tasks[next_arrival].arrival_time <= now) {
      ready.push_back(tasks[next_arrival]);
      ++next_arrival;
    }

    if (ready.empty()) {
      now = tasks[next_arrival].arrival_time;
      continue;
    }

    const int selected = scheduler->choose_next(ready, now);
    if (selected < 0 || selected >= static_cast<int>(ready.size())) {
      break;
    }

    Task task = ready[static_cast<std::size_t>(selected)];
    ready.erase(ready.begin() + selected);

    if (task.op_type == last_op) {
      ++streak;
    } else {
      last_op = task.op_type;
      streak = 1;
    }

    const double cost = task.base_cost * contention_multiplier(task.op_type, last_op, streak);
    const double start = now;
    const double finish = now + cost;

    scheduler->on_task_executed(task, now);

    result.records.push_back(TaskRunRecord{task, start, finish});
    now = finish;
  }

  result.end_time = now;
  return result;
}

}  // namespace refractoryrt
