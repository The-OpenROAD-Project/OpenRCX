# Copyright (c) 2020, OpenROAD
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

sta::define_cmd_args "define_process_corner" {
    [-ext_model_index index] filename
}

proc define_process_corner { args } {
  sta::parse_key_args "define_process_corner" args keys {-ext_model_index}
  sta::check_argc_eq1 "define_process_corner" $args

  set ext_model_index 0
  if { [info exists keys(-ext_model_index)] } {
    set ext_model_index $keys(-ext_model_index)
  }

  set filename $args

  rcx::define_process_corner $ext_model_index $filename
}

sta::define_cmd_args "extract_parasitics" {
    [-ext_model_file filename]
    [-corner_cnt count]
    [-max_res ohms]
    [-coupling_threshold fF]
    [-signal_table value]
}

proc extract_parasitics { args } {
  sta::parse_key_args "define_process_corner" args keys \
      { -ext_model_file
        -corner_cnt
        -max_res
        -coupling_threshold
        -signal_table }

  set ext_model_file ''
  if { [info exists keys(-ext_model_file)] } {
    set ext_model_file $keys(-ext_model_file)
  }

  set corner_cnt 1
  if { [info exists keys(-corner_cnt)] } {
    set corner_cnt $keys(-corner_cnt)
  }

  set max_res 50.0
  if { [info exists keys(-max_res)] } {
    set max_res $keys(-max_res)
  }

  set coupling_threshold 0.1
  if { [info exists keys(-coupling_threshold)] } {
    set coupling_threshold $keys(-coupling_threshold)
  }

  set signal_table 3
  if { [info exists keys(-signal_table)] } {
    set signal_table $keys(-signal_table)
  }

  rcx::extract $ext_model_file $corner_cnt $max_res \
      $coupling_threshold $signal_table
}

sta::define_cmd_args "write_spef" { filename }

proc write_spef { args } {
  sta::check_argc_eq1 "write_spef" $args
  rcx::write_spef $args
}

sta::define_cmd_args "adjust_rc" {
    [-res_factor res]
    [-cc_factor cc]
    [-gndc_factor gndc]
}

proc adjust_rc { args } {
  sta::parse_key_args "adjust_rc" args keys \
      { -res_factor
        -cc_factor
        -gndc_factor }

  set res_factor 1.0
  if { [info exists keys(-res_factor)] } {
    set res_factor $keys(-res_factor)
  }

  set cc_factor 1.0
  if { [info exists keys(-cc_factor)] } {
    set cc_factor $keys(-cc_factor)
  }

  set gndc_factor 1.0
  if { [info exists keys(-gndc_factor)] } {
    set gndc_factor $keys(-gndc_factor)
  }

   rcx::adjust_rc $res_factor $cc_factor $gndc_factor
}
