#include "refractoryrt/workload.h"

#include <random>

namespace refractoryrt {

std::vector<Task> generate_bursty_workload(int num_tasks, int seed) {
  std::mt19937 rng(seed);
  std::uniform_real_distribution<double> jitter(0.0, 0.35);

  std::vector<Task> tasks;
  tasks.reserve(static_cast<std::size_t>(num_tasks));

  double now = 0.0;
  for (int i = 0; i < num_tasks; ++i) {
    Task t;
    t.id = i;

    const int m = i % 10;
    if (m < 7) {
      t.op_type = OpType::ATTENTION;
      t.base_cost = 3.8;
      t.base_priority = 4.5;
    } else if (m < 9) {
      t.op_type = OpType::KV_CACHE;
      t.base_cost = 2.2;
      t.base_priority = 3.2;
    } else {
      t.op_type = OpType::LAYERNORM;
      t.base_cost = 1.0;
      t.base_priority = 2.0;
    }

    if (i % 25 < 18) {
      now += 0.30 + jitter(rng);
    } else {
      now += 1.20 + jitter(rng);
    }

    t.arrival_time = now;
    tasks.push_back(t);
  }

  return tasks;
}

std::vector<Task> generate_mixed_workload(int num_tasks, int seed) {
  std::mt19937 rng(seed);
  std::uniform_real_distribution<double> jitter(0.0, 0.55);
  std::uniform_int_distribution<int> type_pick(0, 2);

  std::vector<Task> tasks;
  tasks.reserve(static_cast<std::size_t>(num_tasks));

  double now = 0.0;
  for (int i = 0; i < num_tasks; ++i) {
    Task t;
    t.id = i;

    const int pick = type_pick(rng);
    if (pick == 0) {
      t.op_type = OpType::ATTENTION;
      t.base_cost = 3.3;
      t.base_priority = 4.0;
    } else if (pick == 1) {
      t.op_type = OpType::KV_CACHE;
      t.base_cost = 2.4;
      t.base_priority = 3.0;
    } else {
      t.op_type = OpType::LAYERNORM;
      t.base_cost = 1.2;
      t.base_priority = 2.4;
    }

    now += 0.65 + jitter(rng);
    t.arrival_time = now;
    tasks.push_back(t);
  }

  return tasks;
}

std::vector<Task> generate_tail_stress_workload(int num_tasks, int seed) {
  std::mt19937 rng(seed);
  std::uniform_real_distribution<double> jitter(0.0, 0.25);

  std::vector<Task> tasks;
  tasks.reserve(static_cast<std::size_t>(num_tasks));

  double now = 0.0;
  for (int i = 0; i < num_tasks; ++i) {
    Task t;
    t.id = i;

    if ((i / 20) % 2 == 0) {
      if (i % 5 != 0) {
        t.op_type = OpType::ATTENTION;
        t.base_cost = 4.0;
        t.base_priority = 4.6;
      } else {
        t.op_type = OpType::KV_CACHE;
        t.base_cost = 2.6;
        t.base_priority = 3.1;
      }
      now += 0.22 + jitter(rng);
    } else {
      if (i % 3 == 0) {
        t.op_type = OpType::LAYERNORM;
        t.base_cost = 1.1;
        t.base_priority = 2.1;
      } else {
        t.op_type = OpType::KV_CACHE;
        t.base_cost = 2.1;
        t.base_priority = 3.0;
      }
      now += 0.90 + jitter(rng);
    }

    t.arrival_time = now;
    tasks.push_back(t);
  }

  return tasks;
}

}  // namespace refractoryrt
