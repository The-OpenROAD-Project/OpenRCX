
set extRules patterns.rules
set blk all

db load_lef -file ../data/12LP_13M_3Mx_2Cx_4Kx_2Hx_2Gx_LB_7p5t_84cpp_tech.lef.mod
db rlog -tag end__LEF
db load_def -file EXT/patterns.def

db order_wires

Ext ext
ext extract -ext_model_file $extRules -max_res 0 -coupling_threshold 0.001
db rlog -tag end__Extract

ext write_spef -file $blk.spef
ext report_total_cap 

ext diff_spef -file EXT/patterns.spef -r_res


