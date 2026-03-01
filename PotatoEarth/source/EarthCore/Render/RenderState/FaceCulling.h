#pragma once

namespace PotatoEarth {
	enum class CullFace
	{
		Front,
		Back,
		FrontAndBack
	};

	enum class WindingOrder
	{
		Clockwise,
		CounterClockwise
	};

	struct FacetCulling
	{
		bool Enabled = true;
		CullFace Face = CullFace::Back;
		WindingOrder FrontFaceWindingOrder = WindingOrder::CounterClockwise;
	};
}