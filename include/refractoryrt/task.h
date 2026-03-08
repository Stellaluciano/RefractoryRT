#pragma once

#include "refractoryrt/types.h"

namespace refractoryrt {

struct Task {
  int id = 0;
  OpType op_type = OpType::LAYERNORM;
  double arrival_time = 0.0;
  double base_cost = 1.0;
  double base_priority = 0.0;
};

}  // namespace refractoryrt
