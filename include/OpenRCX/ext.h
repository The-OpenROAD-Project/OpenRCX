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

#ifndef ADS_EXT_H
#define ADS_EXT_H

#include "extRCap.h"
#include "exttree.h"
#include "tm.h"

namespace OpenRCX {

class Ext : public odb::ZTechModule<Ext>
{
 public:
  Ext();
  ~Ext();

  bool load_model(const std::string& name,
                  bool               lef_rc,
                  const std::string& file,
                  int                setMin,
                  int                setTyp,
                  int                setMax);
  bool read_process(const std::string& name, const std::string& file);
  bool rules_gen(const std::string& name,
                 const std::string& dir,
                 const std::string& file,
                 bool               write_to_solver,
                 bool               read_from_solver,
                 bool               run_solver,
                 int                pattern,
                 bool               keep_file);
  bool metal_rules_gen(const std::string& name,
                       const std::string& dir,
                       const std::string& file,
                       bool               write_to_solver,
                       bool               read_from_solver,
                       bool               run_solver,
                       int                pattern,
                       bool               keep_file,
                       int                metal);
  bool write_rules(const std::string& name,
                   const std::string& dir,
                   const std::string& file,
                   int                pattern,
                   bool               read_from_solver);
  bool get_ext_metal_count(int& metal_count);
  bool bench_net(const std::string& dir,
                 int                net,
                 bool               write_to_solver,
                 bool               read_from_solver,
                 bool               run_solver,
                 int                max_track_count);
  bool run_solver(const std::string& dir, int net, int shape);
  bool bench_wires(const std::string& block,
                   int                over_dist,
                   int                under_dist,
                   int                met,
                   int                over_met,
                   int                under_met,
                   int                len,
                   int                cnt,
                   const std::string& w,
                   const std::string& s,
                   const std::string& th,
                   const std::string& d,
                   const std::string& w_list,
                   const std::string& s_list,
                   const std::string& th_list,
                   const std::string& grid_list,
                   bool               default_lef_rules,
                   bool               nondefault_lef_rules,
                   const std::string& dir,
                   bool               Over,
                   bool               ddd,
                   bool               multiple_widths,
                   bool               write_to_solver,
                   bool               read_from_solver,
                   bool               run_solver,
                   bool               diag);
  bool assembly(odb::dbBlock* block, odb::dbBlock* main_block);
  bool write_spef_nets(odb::dbObject* block,
                       bool           flatten,
                       bool           parallel,
                       int            corner);
  bool flatten(odb::dbBlock* block, bool spef);
  bool extract(bool               min,
               bool               max,
               bool               typ,
               int                set_min,
               int                set_typ,
               int                set_max,
               bool               litho,
               bool               wire_density,
               const std::string& cmp_file,
               const std::string& ext_model_file,
               const std::string& net,
               const std::string& bbox,
               const std::string& ibox,
               int                test,
               int                cc_band_tracks,
               int                signal_table,
               int                cc_up,
               uint               preserve_geom,
               int                corner_cnt,
               double             max_res,
               bool               no_merge_via_res,
               float              coupling_threshold,
               int                context_depth,
               int                cc_model,
               bool               over_cell,
               bool               remove_cc,
               bool               remove_ext,
               bool               unlink_ext,
               bool               eco,
               bool               no_gs,
               bool               re_run,
               bool               tile,
               int                tiling,
               bool               skip_m1_caps,
               bool               power_grid,
               const std::string& exclude_cells,
               bool               skip_power_stubs,
               const std::string& power_source_coords,
               bool               lef_rc,
               bool               rlog);
  bool define_process_corner(int ext_model_index, const std::string& name);
  bool define_derived_corner(const std::string& name,
                             const std::string& process_corner_name,
                             float              res_factor,
                             float              cc_factor,
                             float              gndc_factor);
  bool get_ext_db_corner(int& index, const std::string& name);
  bool get_corners(std::list<std::string>& corner_list);
  bool delete_corners();
  bool clean(bool all_models, bool ext_only);
  bool adjust_rc(float res_factor, float cc_factor, float gndc_factor);

  bool init_incremental_spef(const std::string& origp,
                             const std::string& newp,
                             const std::string& reader,
                             bool               no_backslash,
                             const std::string& exclude_cells);
  bool write_spef(const std::string& nets,
                  int                net_id,
                  const std::string& ext_corner_name,
                  int                corner,
                  int                debug,
                  bool               flatten,
                  bool               parallel,
                  bool               init,
                  bool               end,
                  bool               use_ids,
                  bool               no_name_map,
                  const std::string& N,
                  bool               term_junction_xy,
                  bool               single_pi,
                  const std::string& file,
                  bool               gz,
                  bool               stop_after_map,
                  bool               w_clock,
                  bool               w_conn,
                  bool               w_cap,
                  bool               w_cc_cap,
                  bool               w_res,
                  bool               no_c_num,
                  bool               prime_time,
                  bool               psta,
                  bool               no_backslash,
                  const std::string& exclude_cells,
                  const std::string& cap_units,
                  const std::string& res_units);
  bool independent_spef_corner();
  bool read_spef(const std::string& file,
                 const std::string& net,
                 bool               force,
                 bool               use_ids,
                 bool               keep_loaded_corner,
                 bool               stamp_wire,
                 int                test_parsing,
                 const std::string& N,
                 bool               r_conn,
                 bool               r_cap,
                 bool               r_cc_cap,
                 bool               r_res,
                 float              cc_threshold,
                 float              cc_ground_factor,
                 int                app_print_limit,
                 int                corner,
                 const std::string& db_corner_name,
                 const std::string& calibrate_base_corner,
                 int                spef_corner,
                 bool               m_map,
                 bool               more_to_read,
                 float              length_unit,
                 int                fix_loop,
                 bool               no_cap_num_collapse,
                 const std::string& cap_node_map_file,
                 bool               log);
  bool diff_spef(const std::string& net,
                 bool               use_ids,
                 bool               test_parsing,
                 const std::string& file,
                 const std::string& db_corner_name,
                 int                spef_corner,
                 const std::string& exclude_net_subword,
                 const std::string& net_subword,
                 const std::string& rc_stats_file,
                 bool               r_conn,
                 bool               r_cap,
                 bool               r_cc_cap,
                 bool               r_res,
                 int                ext_corner,
                 float              low_guard,
                 float              upper_guard,
                 bool               m_map,
                 bool               log);
  bool calibrate(const std::string& spef_file,
                 const std::string& db_corner_name,
                 int                corner,
                 int                spef_corner,
                 bool               m_map,
                 float              upper_limit,
                 float              lower_limit);
  bool match(const std::string& spef_file,
             const std::string& db_corner_name,
             int                corner,
             int                spef_corner,
             bool               m_map);
  bool set_block(const std::string& block_name,
                 odb::dbBlock*      block,
                 const std::string& inst_name,
                 odb::dbInst*       inst);
  bool report_total_cap(const std::string& file,
                        bool               res_only,
                        bool               cap_only,
                        float              ccmult,
                        const std::string& ref,
                        const std::string& read);
  bool report_total_cc(const std::string& file,
                       const std::string& ref,
                       const std::string& read);
  bool attach(odb::ZPtr<odb::IZgui>& gui);
  bool attach_gui(odb::ZPtr<odb::IZdcr> dcr);
  bool export_sdb(odb::ZPtr<odb::ISdb>& net_sdb, odb::ZPtr<odb::ISdb>& cc_sdb);
  bool dump(bool               open_tree_file,
            bool               close_tree_file,
            bool               cc_cap_geom,
            bool               cc_net_geom,
            bool               track_cnt,
            bool               signal,
            bool               power,
            int                layer,
            const std::string& file);
  bool count(bool signal_wire_seg, bool power_wire_seg);
  bool read_qcap(const std::string& file_name,
                 const std::string& cap_file,
                 bool               skip_bterms,
                 bool               no_qcap,
                 const std::string& design);
  bool rc_tree(float max_cap, uint test, int net, const std::string& print_tag);
  bool net_stats(std::list<int>&    net_ids,
                 const std::string& tcap,
                 const std::string& ccap,
                 const std::string& ratio_cap,
                 const std::string& res,
                 const std::string& len,
                 const std::string& met_cnt,
                 const std::string& wire_cnt,
                 const std::string& via_cnt,
                 const std::string& seg_cnt,
                 const std::string& term_cnt,
                 const std::string& bterm_cnt,
                 const std::string& file,
                 const std::string& bbox,
                 const std::string& branch_len);

 private:
  extMain*   _ext;
  extRcTree* _tree;
};

}  // namespace OpenRCX

#endif
