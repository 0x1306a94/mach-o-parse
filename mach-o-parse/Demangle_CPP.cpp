//
//  Demangle_CPP.cpp
//  mach-o-parse
//
//  Created by king on 2019/8/1.
//  Copyright © 2019 K&K. All rights reserved.
//

#include <cxxabi.h>

#include "Demangle_CPP.hpp"

extern "C" char *demangleCPP(const char *mangledSymbol) {
	int status      = 0;
	char *demangled = __cxxabiv1::__cxa_demangle(mangledSymbol, NULL, NULL, &status);
	return status == 0 ? demangled : NULL;
}
