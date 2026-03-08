#pragma once

#include <deque>

namespace refractoryrt {

struct OperatorState {
  double last_run_time = -1e30;
  int recent_run_count = 0;
  double penalty = 0.0;
  std::deque<double> recent_execution_times;
};

}  // namespace refractoryrt
