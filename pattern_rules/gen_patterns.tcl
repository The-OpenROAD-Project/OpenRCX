set ext_dir EXT
exec mkdir -p $ext_dir
set TECH_LEF ../data/merged.lef.mod

db load_lef -file $TECH_LEF

Ext x
x bench_wires -all -len 100

x bench_verilog_out -file $ext_dir/patterns.v
db save_def -file $ext_dir/patterns.def

db save_lef -file $ext_dir/patterns.lef

