// This is generated file. Do not modify directly.

#ifndef MODULES_DESKTOP_CAPTURE_LINUX_
#define MODULES_DESKTOP_CAPTURE_LINUX_

#include <stdarg.h>
#include <map>
#include <string>
#include <vector>

#include "rtc_base/logging.h"

namespace modules_desktop_capture_linux {
// Individual module initializer functions.
bool IsPipewireInitialized();
void InitializePipewire(void* module);
void UninitializePipewire();

// Enum and typedef for umbrella initializer.
enum StubModules {
  kModulePipewire = 0,
  kNumStubModules
};

typedef std::map<StubModules, std::vector<std::string> > StubPathMap;

// Umbrella initializer for all the modules in this stub file.
bool InitializeStubs(const StubPathMap& path_map);
}  // namespace modules_desktop_capture_linux

#endif  // MODULES_DESKTOP_CAPTURE_LINUX_
