#pragma once

#include <vector>

#include "refractoryrt/task.h"

namespace refractoryrt {

std::vector<Task> generate_bursty_workload(int num_tasks, int seed = 1337);
std::vector<Task> generate_mixed_workload(int num_tasks, int seed = 2024);
std::vector<Task> generate_tail_stress_workload(int num_tasks, int seed = 7);

}  // namespace refractoryrt
