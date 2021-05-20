
#include "loader.h"
#include <fmt/format.h>

#include <iostream>

using namespace loader;

extern "C" {
    CustomLibraryPtr the_python_library;
}

// note: intentially leaking the vector so that
// dtors on the loaded libraries do not get called.
// this module will unload after python so it is unsafe
// for destruct the loaded libraries then.
auto loaded = new std::vector<CustomLibraryPtr>;

typedef void (*dl_funcptr)(void);
extern "C" dl_funcptr _PyImport_FindSharedFuncptr(
    const char* prefix,
    const char* shortname,
    const char* pathname,
    FILE* fp) {
  std::cout << "CUSTOM LOAD SHARED LIBRARY " << pathname << "\n";
  auto lib = CustomLibrary::create(pathname);
  lib->add_search_library(SystemLibrary::create());
  lib->add_search_library(the_python_library);
  lib->load();
  auto init_name = fmt::format("{}_{}", prefix, shortname);
  auto result = (dl_funcptr)lib->sym(init_name.c_str()).value();
  loaded->emplace_back(std::move(lib));
  return result;
}