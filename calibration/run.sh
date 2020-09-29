rm -rf work
mkdir -p work 
cd work
ln -s ../../../../build/src/openroad

# Generate Patterns Layout
./openroad ../script/generate_patterns.tcl
cd ./EXT
################################
# Running Golden Extractor
# #Put Command below 
################################

################################
cd ..

# Generate OpenRCX tech file
./openroad ../script/generate_rules.tcl

# Benchmarking to check the accuracy
./openroad ../script/ext_patterns.tcl
