#============================================================================
# Run: 
#    xtclsh test_ise.tcl  - creates XILINX ISE project file
#    ise test_project.ise - opens the project
#============================================================================
source "../../../../../base/xilinxise.tcl"

project_new "test_project"
project_set_props
puts "Adding source files"
puts "Adding simulation files"
xfile add "../../fpga/sim/._tb.vhd" -view Simulation
xfile add "../../fpga/sim/tb.vhd" -view Simulation
puts "Libraries"
project_set_isimscript "test_xsim.tcl"
project_set_top "fpga"
project_close
