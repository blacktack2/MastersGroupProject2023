/**
 * @file   RendererConfigBase.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

namespace NCL::Rendering {
	enum class AlphaTestFunc {
		Never,
		Less,
		Equal,
		LessEqual,
		NotEqual,
		GreaterEqual,
		Always,
		Default = Always
	};
	constexpr float ALPHA_TEST_REF_DEFAULT = 0.0f;

	class RendererConfigBase {
	public:
		RendererConfigBase();

		virtual void SetAlphaTest(bool enabled, AlphaTestFunc func = AlphaTestFunc::Default, float ref = ALPHA_TEST_REF_DEFAULT) = 0;
		inline void ResetAlphaTest() {
			SetAlphaTest(false, AlphaTestFunc::Default, ALPHA_TEST_REF_DEFAULT);
		}
	};
}
