/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020, OpenROAD
// All rights reserved.
//
// BSD 3-Clause License
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
///////////////////////////////////////////////////////////////////////////////

%module openrcx
%{
#include "openroad/OpenRoad.hh"
#include "ext.h"

namespace ord {
// Defined in OpenRoad.i
OpenRCX::Ext*
getOpenRCX();
}

using ord::getOpenRCX;
using OpenRCX::Ext;
%}

%inline %{

void
define_process_corner(int ext_model_index, const char* file)
{
  Ext* ext = getOpenRCX();
  ext->define_process_corner(ext_model_index, file);
}

void
extract(const char* ext_model_file,
        int corner_cnt,
        double max_res,
        float coupling_threshold,
        int signal_table)
{
  Ext* ext = getOpenRCX();
  Ext::ExtractOptions opts;

  opts.ext_model_file = ext_model_file;
  opts.corner_cnt = corner_cnt;
  opts.max_res = max_res;
  opts.coupling_threshold = coupling_threshold;
  opts.signal_table = signal_table;

  ext->extract(opts);
}

void
write_spef(const char* file)
{
  Ext* ext = getOpenRCX();
  Ext::SpefOptions opts;
  opts.file = file;
  ext->write_spef(opts);
}

void
adjust_rc(double res_factor,
          double cc_factor,
          double gndc_factor)
{
  Ext* ext = getOpenRCX();
  ext->adjust_rc(res_factor, cc_factor, gndc_factor);
}

%} // inline


