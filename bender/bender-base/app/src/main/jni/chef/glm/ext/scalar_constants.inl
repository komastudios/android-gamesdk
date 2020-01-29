#include <limits>

namespace glm
{
	template<typename genType>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR genType epsilon()
	{
		GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'epsilon' only accepts floating-point inputs");
		return std::numeric_limits<genType>::epsilon();
	}

	template<typename genType>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR genType pi()
	{
		GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'pi' only accepts floating-point inputs");
		return static_cast<genType>(3.14159265358979323846264338327950288);
	}
} //namespace glm
