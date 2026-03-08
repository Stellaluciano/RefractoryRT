#pragma once

#include <memory>
#include <vector>

#include "refractoryrt/refractory_policy.h"
#include "refractoryrt/task.h"

namespace refractoryrt {

class Scheduler {
 public:
  virtual ~Scheduler() = default;
  virtual int choose_next(const std::vector<Task>& ready, double now) = 0;
  virtual void on_task_executed(const Task& task, double now) = 0;
  virtual const char* name() const = 0;
};

class FifoScheduler : public Scheduler {
 public:
  int choose_next(const std::vector<Task>& ready, double now) override;
  void on_task_executed(const Task& task, double now) override;
  const char* name() const override;
};

class PriorityScheduler : public Scheduler {
 public:
  explicit PriorityScheduler(double waiting_weight = 0.08);

  int choose_next(const std::vector<Task>& ready, double now) override;
  void on_task_executed(const Task& task, double now) override;
  const char* name() const override;

 private:
  double waiting_weight_;
};

class RefractoryScheduler : public Scheduler {
 public:
  RefractoryScheduler(double waiting_weight = 0.08,
                      double contention_weight = 0.8,
                      RefractoryPolicy policy = RefractoryPolicy());

  int choose_next(const std::vector<Task>& ready, double now) override;
  void on_task_executed(const Task& task, double now) override;
  const char* name() const override;

 private:
  double contention_penalty(OpType op) const;

  double waiting_weight_;
  double contention_weight_;
  RefractoryPolicy policy_;
  OpType last_op_;
  int consecutive_runs_;
};

std::unique_ptr<Scheduler> make_fifo_scheduler();
std::unique_ptr<Scheduler> make_priority_scheduler();
std::unique_ptr<Scheduler> make_refractory_scheduler();

}  // namespace refractoryrt
