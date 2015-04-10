# 
# Synthesis run script generated by Vivado
# 

  set_param gui.test TreeTableDev
set_msg_config -id {HDL 9-1061} -limit 100000
set_msg_config -id {HDL 9-1654} -limit 100000
set_msg_config -id {HDL-1065} -limit 10000
create_project -in_memory -part xc7a100tcsg324-1
set_property target_language Verilog [current_project]
set_param project.compositeFile.enableAutoGeneration 0
set_property default_lib xil_defaultlib [current_project]
set_property ip_repo_paths {
  d:/Xilinx_proj/ip_repo/ov7670_cam_1.0
  d:/Xilinx_proj/ip_repo/myip_1.0
  d:/Xilinx_proj/ip_repo/ov7670_1.0
  d:/xilinx_proj/ip_repo/ov7670_1.0
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0
  D:/Xilinx_proj/ip_repo/ov7670_cam_1.0
  D:/Xilinx_proj/ip_repo/myip_1.0
  D:/Xilinx_proj/ip_repo/ov7670_1.0
  D:/xilinx_proj/ip_repo/ov7670_1.0
  D:/xilinx_proj/ip_repo/ov7670_cam_1.0
  D:/xilinx_proj/ip_repo/ov7670_cam_1.0
  D:/xilinx_proj/ip_repo/ov7670_cam_1.0
  D:/xilinx_proj/ip_repo/ov7670_cam_1.0
} [current_fileset]
read_verilog -library xil_defaultlib {
  D:/Xilinx_proj/ip_repo/ov7670_cam_1.0/src/I2C_OV7670_RGB444_Config.v
  D:/Xilinx_proj/ip_repo/ov7670_cam_1.0/src/I2C_Controller.v
  D:/Xilinx_proj/ip_repo/ov7670_cam_1.0/src/ov7670_capture.v
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0/ov7670_cam_v1_0_project/ov7670_cam_v1_0_project.srcs/sources_1/new/I2C_AV_Config.v
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0/ov7670_cam_v1_0_project/ov7670_cam_v1_0_project.srcs/sources_1/new/debounce.v
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0/hdl/ov7670_cam_v1_0_S01_AXI.v
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0/hdl/ov7670_cam_v1_0_M00_AXI.v
  d:/xilinx_proj/ip_repo/ov7670_cam_1.0/hdl/ov7670_cam_v1_0.v
}
set_param synth.vivado.isSynthRun true
set_property webtalk.parent_dir d:/xilinx_proj/ip_repo/ov7670_cam_1.0/ov7670_cam_v1_0_project/ov7670_cam_v1_0_project.cache/wt [current_project]
set_property parent.project_dir d:/xilinx_proj/ip_repo/ov7670_cam_1.0/ov7670_cam_v1_0_project [current_project]
synth_design -top ov7670_cam_v1_0 -part xc7a100tcsg324-1
write_checkpoint ov7670_cam_v1_0.dcp
report_utilization -file ov7670_cam_v1_0_utilization_synth.rpt -pb ov7670_cam_v1_0_utilization_synth.pb