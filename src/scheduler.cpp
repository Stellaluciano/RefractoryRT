#include "refractoryrt/scheduler.h"

#include <limits>

namespace refractoryrt {

int FifoScheduler::choose_next(const std::vector<Task>& ready, double /*now*/) {
  if (ready.empty()) {
    return -1;
  }

  int best = 0;
  for (int i = 1; i < static_cast<int>(ready.size()); ++i) {
    if (ready[i].arrival_time < ready[best].arrival_time) {
      best = i;
    }
  }
  return best;
}

void FifoScheduler::on_task_executed(const Task&, double) {}

const char* FifoScheduler::name() const { return "FIFO"; }

PriorityScheduler::PriorityScheduler(double waiting_weight)
    : waiting_weight_(waiting_weight) {}

int PriorityScheduler::choose_next(const std::vector<Task>& ready, double now) {
  if (ready.empty()) {
    return -1;
  }

  int best = 0;
  double best_score = -std::numeric_limits<double>::infinity();
  for (int i = 0; i < static_cast<int>(ready.size()); ++i) {
    const double waiting = now - ready[i].arrival_time;
    const double score = ready[i].base_priority + waiting_weight_ * waiting;
    if (score > best_score) {
      best_score = score;
      best = i;
    }
  }
  return best;
}

void PriorityScheduler::on_task_executed(const Task&, double) {}

const char* PriorityScheduler::name() const { return "PRIORITY"; }

RefractoryScheduler::RefractoryScheduler(double waiting_weight,
                                         double contention_weight,
                                         RefractoryPolicy policy)
    : waiting_weight_(waiting_weight),
      contention_weight_(contention_weight),
      policy_(std::move(policy)),
      last_op_(OpType::LAYERNORM),
      consecutive_runs_(0) {}

double RefractoryScheduler::contention_penalty(OpType op) const {
  if (op != last_op_) {
    return 0.0;
  }

  double base = 0.0;
  if (op == OpType::ATTENTION) {
    base = 2.2;
  } else if (op == OpType::KV_CACHE) {
    base = 1.1;
  } else {
    base = 0.35;
  }

  return base * static_cast<double>(consecutive_runs_);
}

int RefractoryScheduler::choose_next(const std::vector<Task>& ready, double now) {
  if (ready.empty()) {
    return -1;
  }

  policy_.decay_to(now);

  int best = 0;
  double best_score = -std::numeric_limits<double>::infinity();

  for (int i = 0; i < static_cast<int>(ready.size()); ++i) {
    const double waiting = now - ready[i].arrival_time;
    const double penalty = policy_.get_penalty(ready[i].op_type, now, waiting);
    const double contention = contention_weight_ * contention_penalty(ready[i].op_type);
    const double score = ready[i].base_priority + waiting_weight_ * waiting - penalty - contention;

    if (score > best_score) {
      best_score = score;
      best = i;
    }
  }

  return best;
}

void RefractoryScheduler::on_task_executed(const Task& task, double now) {
  policy_.on_execute(task.op_type, now);

  if (task.op_type == last_op_) {
    ++consecutive_runs_;
  } else {
    last_op_ = task.op_type;
    consecutive_runs_ = 1;
  }
}

const char* RefractoryScheduler::name() const { return "REFRACTORY"; }

std::unique_ptr<Scheduler> make_fifo_scheduler() {
  return std::make_unique<FifoScheduler>();
}

std::unique_ptr<Scheduler> make_priority_scheduler() {
  return std::make_unique<PriorityScheduler>();
}

std::unique_ptr<Scheduler> make_refractory_scheduler() {
  return std::make_unique<RefractoryScheduler>();
}

}  // namespace refractoryrt
