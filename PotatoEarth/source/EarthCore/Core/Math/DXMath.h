#pragma once

// DirectX Math replacement for GLM
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace PTEarth {
    using float3 = DirectX::XMFLOAT3;
    using float4 = DirectX::XMFLOAT4;
    using mat4 = DirectX::XMFLOAT4X4;
    constexpr float PI = 3.14159265358979323846f;
}
