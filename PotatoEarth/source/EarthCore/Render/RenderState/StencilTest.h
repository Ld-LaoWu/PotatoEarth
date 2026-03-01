#pragma once
#include <glm/glm.hpp>

namespace PotatoEarth {

	enum class StencilOperation
	{
        Zero,
        Invert,
        Keep,
        Replace,
        Increment,
        Decrement,
        IncrementWrap,
        DecrementWrap
	};
    enum class StencilTestFunction
    {
        Never,
        Less,
        Equal,
        LessThanOrEqual,
        Greater,
        NotEqual,
        GreaterThanOrEqual,
        Always
    };

    struct StencilTestFace
    {
        int Mask = ~0;
        int ReferenceValue = 0;


        StencilOperation StencilFailOperation = StencilOperation::Keep;
        StencilOperation DepthFailStencilPassOperation = StencilOperation::Keep;
        StencilOperation DepthPassStencilPassOperation = StencilOperation::Keep;
        StencilTestFunction Function = StencilTestFunction::Always;
    };

    struct StencilTest
    {
        bool Enabled = false;
        StencilTestFace FrontFace;
        StencilTestFace BackFace;
    };
    
    struct ScissorTest
    {
        bool Enabled = false;
        glm::vec4 Rectangle = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    };

    struct PrimitiveRestart
    {
        bool Enabled = false;
        int Index = 0;
    };
}