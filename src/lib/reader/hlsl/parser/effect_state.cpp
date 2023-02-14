#include <string_view>
#include <ctype.h>

#include "../../../util/string_util.h"
#include "effect_state.h"

namespace reader {
namespace hlsl {

const EffectStateValue textureFilteringValues[] = {
  {"None", 0},
  {"Point", 1},
  {"Linear", 2},
  {"Anisotropic", 3},
  {"", 0}
};

const EffectStateValue textureAddressingValues[] = {
  {"Wrap", 1},
  {"Mirror", 2},
  {"Clamp", 3},
  {"Border", 4},
  {"MirrorOnce", 5},
  {"", 0}
};

const EffectStateValue booleanValues[] = {
  {"False", 0},
  {"True", 1},
  {"", 0}
};

const EffectStateValue cullValues[] = {
  {"None", 1},
  {"CW", 2},
  {"CCW", 3},
  {"", 0}
};

const EffectStateValue cmpValues[] = {
  {"Never", 1},
  {"Less", 2},
  {"Equal", 3},
  {"LessEqual", 4},
  {"Greater", 5},
  {"NotEqual", 6},
  {"GreaterEqual", 7},
  {"Always", 8},
  {"", 0}
};

const EffectStateValue blendValues[] = {
  {"Zero", 1},
  {"One", 2},
  {"SrcColor", 3},
  {"InvSrcColor", 4},
  {"SrcAlpha", 5},
  {"InvSrcAlpha", 6},
  {"DestAlpha", 7},
  {"InvDestAlpha", 8},
  {"DestColor", 9},
  {"InvDestColor", 10},
  {"SrcAlphaSat", 11},
  {"BothSrcAlpha", 12},
  {"BothInvSrcAlpha", 13},
  {"BlendFactor", 14},
  {"InvBlendFactor", 15},
  {"SrcColor2", 16},
  {"InvSrcColor2", 17},
  {"", 0}
};

const EffectStateValue blendOpValues[] {
  {"Add", 1},
  {"Subtract", 2},
  {"RevSubtract", 3},
  {"Min", 4},
  {"Max", 5},
  {"", 0}
};  

const EffectStateValue fillModeValues[] = {
  {"Point", 1},
  {"Wireframe", 2},
  {"Solid", 3},
  {"", 0}
};

const EffectStateValue stencilOpValues[] = {
  {"Keep", 1},
  {"Zero", 2},
  {"Replace", 3},
  {"IncrSat", 4},
  {"DecrSat", 5},
  {"Invert", 6},
  {"Incr", 7},
  {"Decr", 8},
  {"", 0}
};

// These are flags.
const EffectStateValue colorMaskValues[] = {
  {"False", 0},
  {"Red",   1<<0},
  {"Green", 1<<1},
  {"Blue",  1<<2},
  {"Alpha", 1<<3},
  {"X", 1<<0},
  {"Y", 1<<1},
  {"Z", 1<<2},
  {"W", 1<<3},
  {"", 0}
};

const EffectStateValue integerValues[] = {
  {"", 0}
};

const EffectStateValue floatValues[] = {
  {"", 0}
};

const EffectState samplerStates[] = {
  {"AddressU", 1, textureAddressingValues},
  {"AddressV", 2, textureAddressingValues},
  {"AddressW", 3, textureAddressingValues},
  // "BorderColor", 4, D3DCOLOR
  {"MagFilter", 5, textureFilteringValues},
  {"MinFilter", 6, textureFilteringValues},
  {"MipFilter", 7, textureFilteringValues},
  {"MipMapLodBias", 8, floatValues},
  {"MaxMipLevel", 9, integerValues},
  {"MaxAnisotropy", 10, integerValues},
  {"sRGBTexture", 11, booleanValues},    
};

const EffectState effectStates[] = {
  {"VertexShader", 0, nullptr},
  {"PixelShader", 0, nullptr},
  {"AlphaBlendEnable", 27, booleanValues},
  {"SrcBlend", 19, blendValues},
  {"DestBlend", 20, blendValues},
  {"BlendOp", 171, blendOpValues},
  {"SeparateAlphaBlendEanble", 206, booleanValues},
  {"SrcBlendAlpha", 207, blendValues},
  {"DestBlendAlpha", 208, blendValues},
  {"BlendOpAlpha", 209, blendOpValues},
  {"AlphaTestEnable", 15, booleanValues},
  {"AlphaRef", 24, integerValues},
  {"AlphaFunc", 25, cmpValues},
  {"CullMode", 22, cullValues},
  {"ZEnable", 7, booleanValues},
  {"ZWriteEnable", 14, booleanValues},
  {"ZFunc", 23, cmpValues},
  {"StencilEnable", 52, booleanValues},
  {"StencilFail", 53, stencilOpValues},
  {"StencilZFail", 54, stencilOpValues},
  {"StencilPass", 55, stencilOpValues},
  {"StencilFunc", 56, cmpValues},
  {"StencilRef", 57, integerValues},
  {"StencilMask", 58, integerValues},
  {"StencilWriteMask", 59, integerValues},
  {"TwoSidedStencilMode", 185, booleanValues},
  {"CCW_StencilFail", 186, stencilOpValues},
  {"CCW_StencilZFail", 187, stencilOpValues},
  {"CCW_StencilPass", 188, stencilOpValues},
  {"CCW_StencilFunc", 189, cmpValues},
  {"ColorWriteEnable", 168, colorMaskValues},
  {"FillMode", 8, fillModeValues},
  {"MultisampleAlias", 161, booleanValues},
  {"MultisampleMask", 162, integerValues},
  {"ScissorTestEnable", 174, booleanValues},
  {"SlopeScaleDepthBias", 175, floatValues},
  {"DepthBias", 195, floatValues}
};

const EffectStateValue witnessCullModeValues[] = {
  {"None", 0},
  {"Back", 1},
  {"Front", 2},
  {"", 0}
};

const EffectStateValue witnessFillModeValues[] = {
  {"Solid", 0},
  {"Wireframe", 1},
  {"", 0}
};

const EffectStateValue witnessBlendModeValues[] = {
  {"Disabled", 0},
  {"AlphaBlend", 1},          // src * a + dst * (1-a)
  {"Add", 2},                 // src + dst
  {"Mixed", 3},               // src + dst * (1-a)
  {"Multiply", 4},            // src * dst
  {"Multiply2", 5},           // 2 * src * dst
  {"", 0}
};

const EffectStateValue witnessDepthFuncValues[] = {
  {"LessEqual", 0},
  {"Less", 1},
  {"Equal", 2},
  {"Greater", 3},
  {"Always", 4},
  {"", 0}
};

const EffectStateValue witnessStencilModeValues[] = {
  {"Disabled", 0},
  {"Set", 1},
  {"Test", 2},
  {"", 0}
};

const EffectStateValue witnessFilterModeValues[] = {
  {"Point", 0},
  {"Linear", 1},
  {"Mipmap_Nearest", 2},
  {"Mipmap_Best", 3},     // Quality of mipmap filtering depends on render settings.
  {"Anisotropic", 4},     // Aniso without mipmaps for reflection maps.
  {"", 0}
};

const EffectStateValue witnessWrapModeValues[] = {
  {"Repeat", 0},
  {"Clamp", 1},
  {"ClampToBorder", 2},
  {"", 0}
};

const EffectState pipelineStates[] = {
  {"VertexShader", 0, nullptr},
  {"PixelShader", 0, nullptr},

  // Depth_Stencil_State
  {"DepthWrite", 0, booleanValues},
  {"DepthEnable", 0, booleanValues},
  {"DepthFunc", 0, witnessDepthFuncValues},
  {"StencilMode", 0, witnessStencilModeValues},

  // Raster_State
  {"CullMode", 0, witnessCullModeValues},
  {"FillMode", 0, witnessFillModeValues},
  {"MultisampleEnable", 0, booleanValues},
  {"PolygonOffset", 0, booleanValues},

  // Blend_State
  {"BlendMode", 0, witnessBlendModeValues},
  {"ColorWrite", 0, booleanValues},
  {"AlphaWrite", 0, booleanValues},
  {"AlphaTest", 0, booleanValues},       // This is really alpha to coverage.
};

const EffectState* getEffectState(const std::string_view& name,
    bool isSamplerState, bool isPipeline) {
  const EffectState* states = isSamplerState ? samplerStates :
      isPipeline ? pipelineStates : effectStates;
  const size_t count = sizeof(states) / sizeof(states[0]);
  for (size_t i = 0; i < count; ++i) {
    const std::string_view sname = states[i].name;
    if (util::stringEqualCaseInsensitive(name, sname)) {
      return &states[i];
    }
  }
  return nullptr;
}

const EffectStateValue* getEffectStateValue(const std::string_view& name,
    const EffectState* state) {
  for (int i = 0; ; ++i) {
    const EffectStateValue* value = &state->values[i];
    if (value->name == "") {
      break;
    }
    if (util::stringEqualCaseInsensitive(name, value->name)) {
      return value;
    }
  }
  return nullptr;
}

} // namespace hlsl
} // namespace reader
