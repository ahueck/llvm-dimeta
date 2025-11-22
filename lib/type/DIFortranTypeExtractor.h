#ifndef LIB_TYPE_DIFORTRANTYPEEXTRACTOR
#define LIB_TYPE_DIFORTRANTYPEEXTRACTOR

#include <optional>

namespace llvm {
class DIType;
}

namespace dimeta::dataflow {
struct CallValuePath;
}

namespace dimeta::fortran {
std::optional<llvm::DIType*> extract(const dataflow::CallValuePath& call_path, std::optional<llvm::DIType*>);
}

#endif /* LIB_TYPE_DIFORTRANTYPEEXTRACTOR */
