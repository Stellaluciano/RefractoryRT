#pragma once

#include <unordered_map>

#include "refractoryrt/operator_state.h"
#include "refractoryrt/types.h"

namespace refractoryrt {

class RefractoryPolicy {
 public:
  RefractoryPolicy(double window = 8.0,
                   double decay_rate = 0.30,
                   double burst_step = 1.2,
                   double max_penalty = 12.0,
                   double starvation_threshold = 20.0,
                   double bypass_scale = 0.15);

  double get_penalty(OpType op, double now, double waiting_time = 0.0) const;
  void on_execute(OpType op, double now);
  void decay_to(double now);
  bool should_bypass(double waiting_time) const;

 private:
  void update_state_to(OpType op, double now);

  double window_;
  double decay_rate_;
  double burst_step_;
  double max_penalty_;
  double starvation_threshold_;
  double bypass_scale_;

  std::unordered_map<OpType, OperatorState> state_;
};

}  // namespace refractoryrt
