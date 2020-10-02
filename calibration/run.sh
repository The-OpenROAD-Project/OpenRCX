# Clean up directory
rm -rf work
mkdir -p work 
cd work

# Create a soft link to openroad executable
ln -s ../../../../build/src/openroad

# Step A: Generate Patterns Layout
./openroad ../script/generate_patterns.tcl

# Step B: Perform extraction using commercial extractor.
# The output file should located in the directory below.
cd ./EXT

################################
# Running Golden Extractor
# #Put Command below 
################################

################################
cd ..

# Step C: Generate OpenRCX tech file
./openroad ../script/generate_rules.tcl

# Step D: Benchmarking to check the accuracy
./openroad ../script/ext_patterns.tcl
