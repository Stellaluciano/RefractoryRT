#pragma once

#include <string>

namespace refractoryrt {

enum class OpType {
  ATTENTION,
  KV_CACHE,
  LAYERNORM,
};

inline std::string to_string(OpType op) {
  switch (op) {
    case OpType::ATTENTION:
      return "ATTENTION";
    case OpType::KV_CACHE:
      return "KV_CACHE";
    case OpType::LAYERNORM:
      return "LAYERNORM";
  }
  return "UNKNOWN";
}

}  // namespace refractoryrt
