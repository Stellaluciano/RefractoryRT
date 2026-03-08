#include "refractoryrt/refractory_policy.h"

#include <algorithm>
#include <cmath>

namespace refractoryrt {

RefractoryPolicy::RefractoryPolicy(double window,
                                   double decay_rate,
                                   double burst_step,
                                   double max_penalty,
                                   double starvation_threshold,
                                   double bypass_scale)
    : window_(window),
      decay_rate_(decay_rate),
      burst_step_(burst_step),
      max_penalty_(max_penalty),
      starvation_threshold_(starvation_threshold),
      bypass_scale_(bypass_scale) {}

void RefractoryPolicy::update_state_to(OpType op, double now) {
  auto& st = state_[op];
  const double dt = std::max(0.0, now - st.last_run_time);
  st.penalty *= std::exp(-decay_rate_ * dt);

  while (!st.recent_execution_times.empty() &&
         (now - st.recent_execution_times.front()) > window_) {
    st.recent_execution_times.pop_front();
  }
  st.recent_run_count = static_cast<int>(st.recent_execution_times.size());
  st.last_run_time = now;
}

void RefractoryPolicy::decay_to(double now) {
  for (auto& kv : state_) {
    update_state_to(kv.first, now);
  }
}

double RefractoryPolicy::get_penalty(OpType op, double now, double waiting_time) const {
  const auto it = state_.find(op);
  if (it == state_.end()) {
    return 0.0;
  }

  const auto& st = it->second;
  const double dt = std::max(0.0, now - st.last_run_time);
  double penalty = st.penalty * std::exp(-decay_rate_ * dt);

  if (should_bypass(waiting_time)) {
    penalty *= bypass_scale_;
  }

  return penalty;
}

void RefractoryPolicy::on_execute(OpType op, double now) {
  update_state_to(op, now);
  auto& st = state_[op];

  st.recent_execution_times.push_back(now);
  while (!st.recent_execution_times.empty() &&
         (now - st.recent_execution_times.front()) > window_) {
    st.recent_execution_times.pop_front();
  }

  st.recent_run_count = static_cast<int>(st.recent_execution_times.size());
  st.penalty = std::min(max_penalty_, st.penalty + burst_step_ * st.recent_run_count);
}

bool RefractoryPolicy::should_bypass(double waiting_time) const {
  return waiting_time >= starvation_threshold_;
}

}  // namespace refractoryrt
