#pragma once

namespace reader {
namespace hlsl {

struct EffectStateValue {
  const std::string_view name;
  int value;
};

struct EffectState {
  const std::string_view name;
  int d3dRenderState;
  const EffectStateValue* values;
};

extern const EffectStateValue colorMaskValues[];
extern const EffectStateValue integerValues[];
extern const EffectStateValue floatValues[];
extern const EffectStateValue booleanValues[];

/// Finds an effect state by name
/// @param name The name of the effect state to find
/// @param isSamplerState true if the effect state is a sampler state
/// @param isPipeline true if the effect state is a pipeline state
/// @return The effect state or nullptr if not found
const EffectState* getEffectState(const std::string_view& name,
    bool isSamplerState, bool isPipeline);

const EffectStateValue* getEffectStateValue(const std::string_view& name, const EffectState* state);

} // namespace hlsl
} // namespace reader
