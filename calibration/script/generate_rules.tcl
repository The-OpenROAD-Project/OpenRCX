source ../script/user_env.tcl

set golden_spef ../../test/generate_pattern.spefok

read_lef $TECH_LEF

read_def -order_wires EXT/patterns.def
bench_read_spef $golden_spef

write_rules -file $extRules -db

exit
