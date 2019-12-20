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
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef ADS_EXT_H
#define ADS_EXT_H

#ifndef ADS_TM_H
//#include "tm.h"
#include <tm.h>
#endif

#include "extRCap.h"
#include "exttree.h"

#define TCL_METHOD(name) int name( ZArgs * in, ZArgs * out )

BEGIN_NAMESPACE_ADS

class Ext : public ZTechModule<Ext>
{
  public:
	Ext( ZArgs * in, ZArgs * out );
	~Ext();
    
    TCL_METHOD ( load_model ) ;
    TCL_METHOD ( read_process ) ;
    TCL_METHOD ( rules_gen ) ;
    TCL_METHOD ( metal_rules_gen ) ;
    TCL_METHOD ( write_rules );
    TCL_METHOD ( get_ext_metal_count );
    TCL_METHOD ( bench_net ) ;
    TCL_METHOD ( run_solver ) ;
    TCL_METHOD ( bench_wires ) ;
    TCL_METHOD ( assembly ) ;
    TCL_METHOD ( write_spef_nets ) ;
    TCL_METHOD ( flatten ) ;
    TCL_METHOD ( extract ) ;
    TCL_METHOD ( define_process_corner ) ;
    TCL_METHOD ( define_derived_corner ) ;
    TCL_METHOD ( get_ext_db_corner );
    TCL_METHOD ( get_corners );
    TCL_METHOD ( delete_corners );
    TCL_METHOD ( clean ) ;
    TCL_METHOD ( adjust_rc ) ;
    TCL_METHOD ( init_incremental_spef ) ;
    TCL_METHOD ( write_spef ) ;
    TCL_METHOD ( independent_spef_corner ) ;
    TCL_METHOD ( read_spef ) ;
    TCL_METHOD ( diff_spef ) ;
    TCL_METHOD ( calibrate ) ;
    TCL_METHOD ( match ) ;
    TCL_METHOD ( set_block ) ;
    TCL_METHOD ( report_total_cap ) ;
    TCL_METHOD ( report_total_cc ) ;
    TCL_METHOD ( attach ) ;
    TCL_METHOD ( attach_gui ) ;
    TCL_METHOD ( export_sdb ) ;
    TCL_METHOD ( dump ) ;
    TCL_METHOD ( count ) ;
    TCL_METHOD ( read_qcap ) ;
    TCL_METHOD ( rc_tree ) ;
    TCL_METHOD ( net_stats ) ;

  private:
	extMain *_ext;
	extRcTree *_tree;
};

END_NAMESPACE_ADS
#endif
