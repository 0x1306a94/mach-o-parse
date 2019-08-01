//
//  Demangle_CPP.hpp
//  mach-o-parse
//
//  Created by king on 2019/8/1.
//  Copyright © 2019 K&K. All rights reserved.
//

#ifndef Demangle_CPP_hpp
#define Demangle_CPP_hpp

#ifdef __cplusplus
extern "C" {
#endif

/**
 Demangle a C++ symbol.

 @param mangledSymbol The mangled symbol.
 @return A demangled symbol, or NULL if demangling failed.
 */
char *demangleCPP(const char *mangledSymbol);

#ifdef __cplusplus
}
#endif

#endif /* Demangle_CPP_hpp */
