source ../script/user_env.tcl

set ext_dir EXT
exec mkdir -p $ext_dir

read_lef $TECH_LEF

bench_wires -len 100 -all

bench_verilog $ext_dir/patterns.v

write_def $ext_dir/patterns.def

exit
