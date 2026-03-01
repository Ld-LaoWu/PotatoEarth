#pragma once

#include <string>
#include <sstream>
#include <functional>


namespace PotatoEarth{
	class ColorMask {
	public:

		ColorMask(bool red, bool green, bool blue, bool alpha):
			m_Red(red), m_Green(green), m_Blue(blue), m_Alpha(alpha) {}	
		
		bool GetRed() const { return m_Red; }
		bool GetGreen() const { return m_Green; }
		bool GetBlue() const { return m_Blue; }	
		bool GetAlpha() const { return m_Alpha; }


		bool Equals(const ColorMask& other) const {
			return m_Red == other.m_Red && 
				m_Green == other.m_Green && 
				m_Blue == other.m_Blue && 
				m_Alpha == other.m_Alpha;
		}

		bool operator == (const ColorMask& other) const {
			return this->Equals(other);
		}

		bool operator != (const ColorMask& other) const {
			return !this->Equals(other);
		}

		std::string ToString() {
			//

			std::stringstream ss;
			ss << "ColorMask( Red: " << m_Red << ", Green: " << m_Green << ", Blue: " << m_Blue << ", Alpha: " << m_Alpha << " )";
			return ss.str();
		}

		uint32_t getHash() const {
			//TODO: Implement hash function
			
			std::hash<bool> hasher;
			return hasher(m_Red) ^ hasher(m_Green) ^ hasher(m_Blue) ^ hasher(m_Alpha);
		
		}

	private:

		bool m_Red;
		bool m_Green;
		bool m_Blue;
		bool m_Alpha;
	};
	
}