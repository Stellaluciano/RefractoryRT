#pragma once

#include <memory>
#include <vector>

#include "refractoryrt/metrics.h"
#include "refractoryrt/scheduler.h"
#include "refractoryrt/task.h"

namespace refractoryrt {

struct SimulatorConfig {
  bool enable_contention_model = true;
};

class Simulator {
 public:
  explicit Simulator(SimulatorConfig config = SimulatorConfig());

  SimulationResult run(const std::vector<Task>& workload,
                       std::unique_ptr<Scheduler> scheduler) const;

 private:
  double contention_multiplier(OpType current, OpType previous, int streak) const;

  SimulatorConfig config_;
};

}  // namespace refractoryrt
