#pragma once
// 在windows 下面放一些预编译的头文件

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifdef PO_PLATFORM_WINDOWS
	#include <windows.h>
#endif