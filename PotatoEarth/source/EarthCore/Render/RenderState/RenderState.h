#pragma once

#include "Blending.h"
#include "ColorMask.h"
#include "DepthTest.h"
#include "FaceCulling.h"
#include "StencilTest.h"

namespace PotatoEarth {
	enum class ProgramPointSize
	{
		Enabled,
		Disabled
	};
	enum class RasterizationMode
	{
		Point,
		Line,
		Fill
	};

	struct RenderState {
		PrimitiveRestart primitiveRestart;
		FacetCulling facetCulling;
		ProgramPointSize programPointSize = ProgramPointSize::Disabled;
		RasterizationMode rasterizationMode = RasterizationMode::Fill;
		ScissorTest scissorTest;
		StencilTest stencilTest;
		DepthTest depthTest;
		DepthRange depthRange;
		Blending blending;
		ColorMask colorMask = ColorMask(true, true, true, true);
		bool DepthMask = true;
	};
}