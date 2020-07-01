read_lef Nangate45.lef
read_def gcd.def

define_process_corner -ext_model_index 0 X
extract_parasitics -ext_model_file nominal.rules \
        -max_res 0 -coupling_threshold 0.0001
