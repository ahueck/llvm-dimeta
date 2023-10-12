//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_LOGGER_H

#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#ifndef DIMETA_LOG_LEVEL
/*
 * Usually set at compile time: -DDIMETA_LOG_LEVEL=<N>, N in [0, 3] for output
 * 3 being most verbose
 */
#define DIMETA_LOG_LEVEL 3
#endif

#ifndef DIMETA_LOG_BASENAME
#define DIMETA_LOG_BASENAME __FILE__
#endif

#ifndef DIMETA_MPI_LOGGER
#define DIMETA_MPI_LOGGER 0
#endif

namespace dimeta::log {
class LogContext {
 private:
  llvm::Module* context{nullptr};
  LogContext(llvm::Module* mod = nullptr) : context{mod} {
  }

 public:
  void setModule(llvm::Module* mod) {
    context = mod;
  }

  llvm::Module* getModule() {
    return context;
  }

  static LogContext& get() {
    static LogContext context;
    return context;
  }
};

inline std::string ditype_str(const llvm::Metadata* type) {
  if (type == nullptr) {
    return "";
  }
  auto* module = LogContext::get().getModule();
  std::string logging_message;
  llvm::raw_string_ostream rso(logging_message);
  type->print(rso, module);
  return rso.str();
}

inline void dimeta_log(const std::string& msg) {
  llvm::dbgs() << msg;
}
}  // namespace dimeta::log

#define DIMETA_LOG_LEVEL_MSG(LEVEL_NUM, LEVEL, MSG)                                                                 \
  if ((LEVEL_NUM) <= DIMETA_LOG_LEVEL) {                                                                            \
    std::string logging_message;                                                                                    \
    llvm::raw_string_ostream rso(logging_message);                                                                  \
    rso << (LEVEL) << DIMETA_LOG_BASENAME << ":" << __func__ << ":" << __LINE__ << ":" << MSG << "\n"; /* NOLINT */ \
    dimeta::log::dimeta_log(rso.str());                                                                             \
  }

#define DIMETA_LOG_LEVEL_MSG_BARE(LEVEL_NUM, LEVEL, MSG) \
  if ((LEVEL_NUM) <= DIMETA_LOG_LEVEL) {                 \
    std::string logging_message;                         \
    llvm::raw_string_ostream rso(logging_message);       \
    rso << (LEVEL) << " " << MSG << "\n"; /* NOLINT */   \
    dimeta::log::dimeta_log(rso.str());                  \
  }

#define LOG_TRACE(MSG) DIMETA_LOG_LEVEL_MSG_BARE(3, "[Trace]", MSG)
#define LOG_DEBUG(MSG) DIMETA_LOG_LEVEL_MSG(3, "[Debug]", MSG)
#define LOG_INFO(MSG) DIMETA_LOG_LEVEL_MSG(2, "[Info]", MSG)
#define LOG_WARNING(MSG) DIMETA_LOG_LEVEL_MSG(1, "[Warning]", MSG)
#define LOG_ERROR(MSG) DIMETA_LOG_LEVEL_MSG(1, "[Error]", MSG)
#define LOG_FATAL(MSG) DIMETA_LOG_LEVEL_MSG(0, "[Fatal]", MSG)
#define LOG_MSG(MSG) DIMETA_LOG_LEVEL_MSG_BARE(0, "", MSG)

#endif  // DIMETA_LOGGER_H
