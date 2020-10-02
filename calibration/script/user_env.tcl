############################
# DATA GENERATION SETTINGS 
############################

# Variables that point to the LEF files
set DIR ../../test/sky130
set TECH_LEF ${DIR}/sky130_tech.lef
set MACRO_LEF ${DIR}/sky130_std_cell.lef

# Technology Node 
set PROCESS_NODE 130

# Process corner
set CORNER RCmax

# The file name and location of the custom RC tech file
set extRules ../data/${CORNER}.rules
