#pragma once

namespace PotatoEarth {


	enum class DepthTestFunction
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

	struct DepthRange
	{
		double Near = 0.0;
		double Far = 1.0;
	};

	struct DepthTest
	{
		bool Enabled = true;
		DepthTestFunction Function = DepthTestFunction::Less;
	};
}