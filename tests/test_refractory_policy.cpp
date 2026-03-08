#include <cassert>

#include "refractoryrt/refractory_policy.h"

using namespace refractoryrt;

int main() {
  RefractoryPolicy policy(10.0, 0.5, 1.0, 20.0, 10.0, 0.1);

  const double p0 = policy.get_penalty(OpType::ATTENTION, 0.0);
  policy.on_execute(OpType::ATTENTION, 1.0);
  const double p1 = policy.get_penalty(OpType::ATTENTION, 1.0);
  policy.on_execute(OpType::ATTENTION, 2.0);
  const double p2 = policy.get_penalty(OpType::ATTENTION, 2.0);

  assert(p0 <= p1);
  assert(p2 > p1);

  const double decayed = policy.get_penalty(OpType::ATTENTION, 20.0);
  assert(decayed < p2);

  assert(!policy.should_bypass(5.0));
  assert(policy.should_bypass(12.0));

  return 0;
}
