#include <cassert>
#include <vector>

#include "refractoryrt/scheduler.h"

using namespace refractoryrt;

int main() {
  std::vector<Task> ready = {
      Task{1, OpType::ATTENTION, 0.0, 3.0, 4.0},
      Task{2, OpType::KV_CACHE, 0.1, 2.0, 3.2},
      Task{3, OpType::LAYERNORM, 0.2, 1.0, 2.0},
  };

  auto fifo = make_fifo_scheduler();
  auto priority = make_priority_scheduler();
  auto refractory = make_refractory_scheduler();

  const int a = fifo->choose_next(ready, 1.0);
  const int b = priority->choose_next(ready, 1.0);
  const int c = refractory->choose_next(ready, 1.0);

  assert(a >= 0 && a < static_cast<int>(ready.size()));
  assert(b >= 0 && b < static_cast<int>(ready.size()));
  assert(c >= 0 && c < static_cast<int>(ready.size()));

  refractory->on_task_executed(ready[c], 1.0);
  const int d = refractory->choose_next(ready, 1.5);
  assert(d >= 0 && d < static_cast<int>(ready.size()));

  return 0;
}
