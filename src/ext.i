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

%} // inline


