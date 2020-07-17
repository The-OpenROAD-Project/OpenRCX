set blk patterns
set dir ../data

set TECH_LEF ../data/12LP_13M_3Mx_2Cx_4Kx_2Hx_2Gx_LB_7p5t_84cpp_tech.lef.mod

db load_lef -file $TECH_LEF
db load_def -file EXT/$blk.def

db order_wires

Ext ext
ext read_spef -file EXT/$blk.spef
ext write_rules -file $blk.rules -read_from_db 1
