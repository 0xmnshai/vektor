



#pragma once

#include "../dna/DNA_windowmanager_enums.h"
#include "../vklib/VKE_enum_flags.hh"

namespace vektor {


enum wmOperatorStatus {
  OPERATOR_RUNNING_MODAL = (1 << 0),
  OPERATOR_CANCELLED = (1 << 1),
  OPERATOR_FINISHED = (1 << 2),

  OPERATOR_PASS_THROUGH = (1 << 3),

  OPERATOR_HANDLED = (1 << 4),

  OPERATOR_INTERFACE = (1 << 5),
};
ENUM_OPERATORS(wmOperatorStatus);

#define OPERATOR_FLAGS_ALL \
  (OPERATOR_RUNNING_MODAL | OPERATOR_CANCELLED | OPERATOR_FINISHED | OPERATOR_PASS_THROUGH | \
   OPERATOR_HANDLED | OPERATOR_INTERFACE | 0)


#define OPERATOR_RETVAL_CHECK(ret) \
  { \
    CHECK_TYPE(ret, wmOperatorStatus); \
    BLI_assert(ret != 0 && (ret & OPERATOR_FLAGS_ALL) == ret); \
  } \
  ((void)0)


enum {

  OP_IS_INVOKE = (1 << 0),

  OP_IS_REPEAT = (1 << 1),

  OP_IS_REPEAT_LAST = (1 << 2),


  OP_IS_MODAL_GRAB_CURSOR = (1 << 3),


  OP_IS_MODAL_CURSOR_REGION = (1 << 4),
};

}
