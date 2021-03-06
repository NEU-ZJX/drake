#include "drake/automotive/maliput/base/manual_phase_provider.h"

#include <string>

#include "drake/common/drake_throw.h"

namespace drake {
namespace maliput {

using api::rules::PhaseProvider;

void ManualPhaseProvider::AddPhaseRing(
    const api::rules::PhaseRing::Id& id,
    const api::rules::Phase::Id& initial_phase) {
  auto result = phases_.emplace(id, initial_phase);
  if (!result.second) {
     throw std::logic_error(
        "Attempted to add multiple phase rings with id " + id.string());
  }
}


void ManualPhaseProvider::SetPhase(
    const api::rules::PhaseRing::Id& id,
    const api::rules::Phase::Id& phase) {
  phases_.at(id) = phase;
}

optional<PhaseProvider::Result> ManualPhaseProvider::DoGetPhase(
    const api::rules::PhaseRing::Id& id) const {
  auto it = phases_.find(id);
  if (it == phases_.end()) {
    return nullopt;
  }
  // TODO(liang.fok) Add support for "next phase" and "duration until", #9993.
  return Result{it->second, nullopt};
}

}  // namespace maliput
}  // namespace drake
