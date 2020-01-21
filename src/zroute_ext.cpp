///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2019, Nefelus Inc
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <tcl.h>
//#include "logger.h"
#include <logger.h>

namespace OpenRCX {

#define FUNCTION_API(function)                                               \
  extern "C" int function(                                                   \
      ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* CONST objv[]) \
  {
FUNCTION_API(AthExtInit)
odb::notice(0, "Hello from extractor - ext_init\n");

return TCL_OK;
}  // namespace OpenRCX

FUNCTION_API(AthExtModel)
odb::notice(0, "Hello from extractor - ext_make_model\n");

return TCL_OK;
}

FUNCTION_API(AthExtExtract)
odb::notice(0, "Hello from extractor - ext_extract\n");

return TCL_OK;
}

FUNCTION_API(AthExtSpef)
odb::notice(0, "Hello from extractor - ext_write_spef\n");

return TCL_OK;
}
FUNCTION_API(AthExtClean)
odb::notice(0, "Hello from extractor - ext_clean\n");

return TCL_OK;
}

}  // namespace OpenRCX
