#pragma once 
#include <glm/vec4.hpp>
namespace PotatoEarth { 

	
	enum class SouceBlendingFactor {
		Zero,                         
		One,                          
		SourceAlpha,                  
		OneMinusSourceAlpha,          
		DestinationAlpha,             
		OneMinusDestinationAlpha,     
		DestinationColor,             
		OneMinusDestinationColor,     
		SourceAlphaSaturate,          
		ConstantColor,                
		OneMinusConstantColor,        
		ConstantAlpha,                
		OneMinusConstantAlpha         
	};

	enum class DestinationBlendingFactor {
		Zero,                       
		One,                        
		SourceColor,                
		OneMinusSourceColor,        
		SourceAlpha,                
		OneMinusSourceAlpha,        
		DestinationAlpha,           
		OneMinusDestinationAlpha,   
		DestinationColor,           
		OneMinusDestinationColor,   
		ConstantColor,              
		OneMinusConstantColor,      
		ConstantAlpha,              
		OneMinusConstantAlpha       
	};

	enum class BlendFunction {
		Add,                   
		Minimum,               
		Maximum,               
		Subtract,              
		ReverseSubtract,       
	};

	class Blending {
	public:
		Blending() = default; 

	private:
		bool m_Enabled = false;                       
		SouceBlendingFactor m_SourceRGBFactor = SouceBlendingFactor::One; 
		SouceBlendingFactor m_SourceAlphaFactor = SouceBlendingFactor::One; 
		DestinationBlendingFactor m_DestinationRGBFactor = DestinationBlendingFactor::Zero; 
		DestinationBlendingFactor m_DestinationAlphaFactor = DestinationBlendingFactor::Zero; 
		BlendFunction m_RGBEquation = BlendFunction::Add;        
		BlendFunction m_AlphaEquation = BlendFunction::Add;      

		glm::vec4 m_Color = glm::vec4(0.0f); 
	};
}
