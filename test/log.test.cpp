#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test.hpp"

using namespace sage;

TEST_CASE ("Log") {
	SAGE_LOG_TRACE("This is some: TRACE");
	SAGE_LOG_DEBUG("This is some: DEBUG");
	SAGE_LOG_INFO("This is some: INFO");
	SAGE_LOG_WARN("This is some: WARN");
	SAGE_LOG_ERROR("This is some: ERROR");
	SAGE_LOG_CRITICAL("This is some: CRITICAL");
}
