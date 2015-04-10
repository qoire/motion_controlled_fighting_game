
o
Command: %s
53*	vivadotcl2G
3synth_design -top ov7670_top -part xc7a100tcsg324-12default:defaultZ4-113
/

Starting synthesis...

3*	vivadotclZ4-3
ñ
@Attempting to get a license for feature '%s' and/or device '%s'
308*common2
	Synthesis2default:default2
xc7a100t2default:defaultZ17-347
Ü
0Got license for feature '%s' and/or device '%s'
310*common2
	Synthesis2default:default2
xc7a100t2default:defaultZ17-349
ñ
%s*synth2Ü
rStarting Synthesize : Time (s): cpu = 00:00:03 ; elapsed = 00:00:03 . Memory (MB): peak = 228.402 ; gain = 99.594
2default:default
—
synthesizing module '%s'638*oasys2

ov7670_top2default:default2b
LC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/ov7670_top.v2default:default2
232default:default8@Z8-638
Õ
synthesizing module '%s'638*oasys2
debounce2default:default2`
JC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/debounce.v2default:default2
232default:default8@Z8-638
à
%done synthesizing module '%s' (%s#%s)256*oasys2
debounce2default:default2
12default:default2
12default:default2`
JC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/debounce.v2default:default2
232default:default8@Z8-256
Ÿ
synthesizing module '%s'638*oasys2"
ov7670_capture2default:default2f
PC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/ov7670_capture.v2default:default2
232default:default8@Z8-638
J
%s*synth2;
'	Parameter s_idle bound to: 6'b000000 
2default:default
I
%s*synth2:
&	Parameter s_fsf bound to: 6'b000001 
2default:default
I
%s*synth2:
&	Parameter s_fsn bound to: 6'b000010 
2default:default
J
%s*synth2;
'	Parameter s_eolf bound to: 6'b000011 
2default:default
J
%s*synth2;
'	Parameter s_eoln bound to: 6'b000100 
2default:default
L
%s*synth2=
)	Parameter s_framef bound to: 6'b000101 
2default:default
L
%s*synth2=
)	Parameter s_framen bound to: 6'b000110 
2default:default
î
%done synthesizing module '%s' (%s#%s)256*oasys2"
ov7670_capture2default:default2
22default:default2
12default:default2f
PC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/ov7670_capture.v2default:default2
232default:default8@Z8-256
◊
synthesizing module '%s'638*oasys2!
I2C_AV_Config2default:default2e
OC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/I2C_AV_Config.v2default:default2
162default:default8@Z8-638
V
%s*synth2G
3	Parameter LUT_SIZE bound to: 193 - type: integer 
2default:default
[
%s*synth2L
8	Parameter CLK_Freq bound to: 25000000 - type: integer 
2default:default
X
%s*synth2I
5	Parameter I2C_Freq bound to: 10000 - type: integer 
2default:default
Ì
synthesizing module '%s'638*oasys2,
I2C_OV7670_RGB444_Config2default:default2p
ZC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/I2C_OV7670_RGB444_Config.v2default:default2
172default:default8@Z8-638
U
%s*synth2F
2	Parameter Read_DATA bound to: 0 - type: integer 
2default:default
V
%s*synth2G
3	Parameter SET_OV7670 bound to: 2 - type: integer 
2default:default
®
%done synthesizing module '%s' (%s#%s)256*oasys2,
I2C_OV7670_RGB444_Config2default:default2
32default:default2
12default:default2p
ZC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/I2C_OV7670_RGB444_Config.v2default:default2
172default:default8@Z8-256
Ÿ
synthesizing module '%s'638*oasys2"
I2C_Controller2default:default2f
PC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/I2C_Controller.v2default:default2
162default:default8@Z8-638
î
%done synthesizing module '%s' (%s#%s)256*oasys2"
I2C_Controller2default:default2
42default:default2
12default:default2f
PC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/I2C_Controller.v2default:default2
162default:default8@Z8-256
 
-case statement is not full and has no default155*oasys2e
OC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/I2C_AV_Config.v2default:default2
1032default:default8@Z8-155
í
%done synthesizing module '%s' (%s#%s)256*oasys2!
I2C_AV_Config2default:default2
52default:default2
12default:default2e
OC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/I2C_AV_Config.v2default:default2
162default:default8@Z8-256
Ö
0Net %s in module/entity %s does not have driver.3422*oasys2
aclk2default:default2

ov7670_top2default:default2b
LC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/ov7670_top.v2default:default2
382default:default8@Z8-3848
å
%done synthesizing module '%s' (%s#%s)256*oasys2

ov7670_top2default:default2
62default:default2
12default:default2b
LC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/ov7670_top.v2default:default2
232default:default8@Z8-256
t
!design %s has unconnected port %s3331*oasys2

ov7670_top2default:default2
aclk2default:defaultZ8-3331
ï
+design %s has port %s driven by constant %s3447*oasys2

ov7670_top2default:default2
pwdn2default:default2
02default:defaultZ8-3917
ñ
+design %s has port %s driven by constant %s3447*oasys2

ov7670_top2default:default2
reset2default:default2
12default:defaultZ8-3917
ó
%s*synth2á
sFinished Synthesize : Time (s): cpu = 00:00:04 ; elapsed = 00:00:04 . Memory (MB): peak = 261.250 ; gain = 132.441
2default:default
±
%s*synth2°
åFinished Loading Part and Timing Information : Time (s): cpu = 00:00:04 ; elapsed = 00:00:05 . Memory (MB): peak = 261.250 ; gain = 132.441
2default:default
ù
%s*synth2ç
yFinished RTL Optimization : Time (s): cpu = 00:00:04 ; elapsed = 00:00:05 . Memory (MB): peak = 261.250 ; gain = 132.441
2default:default
â
3inferred FSM for state register '%s' in module '%s'802*oasys2
s_reg2default:default2"
ov7670_capture2default:defaultZ8-802
º
Gencoded FSM with state register '%s' using encoding '%s' in module '%s'3353*oasys2
s_reg2default:default2
one-hot2default:default2"
ov7670_capture2default:defaultZ8-3354
Ö
0Net %s in module/entity %s does not have driver.3422*oasys2
aclk2default:default2

ov7670_top2default:default2b
LC:/Xilinx_proj/ip_repo/ovtop/ovtop.srcs/sources_1/imports/ovtop/ov7670_top.v2default:default2
382default:default8@Z8-3848
<
%s*synth2-

Report RTL Partitions: 
2default:default
N
%s*synth2?
++-+--------------+------------+----------+
2default:default
N
%s*synth2?
+| |RTL Partition |Replication |Instances |
2default:default
N
%s*synth2?
++-+--------------+------------+----------+
2default:default
N
%s*synth2?
++-+--------------+------------+----------+
2default:default
B
%s*synth23
Detailed RTL Component Info : 
2default:default
1
%s*synth2"
+---Adders : 
2default:default
Q
%s*synth2B
.	   2 Input     32 Bit       Adders := 1     
2default:default
Q
%s*synth2B
.	   2 Input      8 Bit       Adders := 1     
2default:default
Q
%s*synth2B
.	   2 Input      6 Bit       Adders := 1     
2default:default
4
%s*synth2%
+---Registers : 
2default:default
Q
%s*synth2B
.	               32 Bit    Registers := 2     
2default:default
Q
%s*synth2B
.	               16 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                8 Bit    Registers := 2     
2default:default
Q
%s*synth2B
.	                6 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                2 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                1 Bit    Registers := 19    
2default:default
0
%s*synth2!
+---Muxes : 
2default:default
Q
%s*synth2B
.	   9 Input     32 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input     32 Bit        Muxes := 2     
2default:default
Q
%s*synth2B
.	   8 Input     32 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   8 Input     16 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input     16 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	  59 Input      8 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   6 Input      8 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input      8 Bit        Muxes := 11    
2default:default
Q
%s*synth2B
.	   7 Input      6 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input      6 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input      2 Bit        Muxes := 2     
2default:default
Q
%s*synth2B
.	   4 Input      2 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	  41 Input      1 Bit        Muxes := 15    
2default:default
Q
%s*synth2B
.	   8 Input      1 Bit        Muxes := 3     
2default:default
Q
%s*synth2B
.	   2 Input      1 Bit        Muxes := 45    
2default:default
Q
%s*synth2B
.	  59 Input      1 Bit        Muxes := 14    
2default:default
Q
%s*synth2B
.	   4 Input      1 Bit        Muxes := 5     
2default:default
F
%s*synth27
#Hierarchical RTL Component report 
2default:default
6
%s*synth2'
Module ov7670_top 
2default:default
B
%s*synth23
Detailed RTL Component Info : 
2default:default
4
%s*synth2%
Module debounce 
2default:default
B
%s*synth23
Detailed RTL Component Info : 
2default:default
4
%s*synth2%
+---Registers : 
2default:default
Q
%s*synth2B
.	                1 Bit    Registers := 1     
2default:default
0
%s*synth2!
+---Muxes : 
2default:default
Q
%s*synth2B
.	   2 Input      1 Bit        Muxes := 1     
2default:default
:
%s*synth2+
Module ov7670_capture 
2default:default
B
%s*synth23
Detailed RTL Component Info : 
2default:default
1
%s*synth2"
+---Adders : 
2default:default
Q
%s*synth2B
.	   2 Input     32 Bit       Adders := 1     
2default:default
4
%s*synth2%
+---Registers : 
2default:default
Q
%s*synth2B
.	               32 Bit    Registers := 2     
2default:default
Q
%s*synth2B
.	               16 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                1 Bit    Registers := 3     
2default:default
0
%s*synth2!
+---Muxes : 
2default:default
Q
%s*synth2B
.	   8 Input     32 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   9 Input     32 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input     32 Bit        Muxes := 2     
2default:default
Q
%s*synth2B
.	   8 Input     16 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input     16 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   6 Input      8 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input      8 Bit        Muxes := 8     
2default:default
Q
%s*synth2B
.	   7 Input      6 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   8 Input      1 Bit        Muxes := 3     
2default:default
D
%s*synth25
!Module I2C_OV7670_RGB444_Config 
2default:default
B
%s*synth23
Detailed RTL Component Info : 
2default:default
:
%s*synth2+
Module I2C_Controller 
2default:default
B
%s*synth23
Detailed RTL Component Info : 
2default:default
1
%s*synth2"
+---Adders : 
2default:default
Q
%s*synth2B
.	   2 Input      6 Bit       Adders := 1     
2default:default
4
%s*synth2%
+---Registers : 
2default:default
Q
%s*synth2B
.	                8 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                6 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                1 Bit    Registers := 9     
2default:default
0
%s*synth2!
+---Muxes : 
2default:default
Q
%s*synth2B
.	   2 Input      8 Bit        Muxes := 3     
2default:default
Q
%s*synth2B
.	  59 Input      8 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input      6 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input      1 Bit        Muxes := 41    
2default:default
Q
%s*synth2B
.	  41 Input      1 Bit        Muxes := 15    
2default:default
Q
%s*synth2B
.	  59 Input      1 Bit        Muxes := 14    
2default:default
9
%s*synth2*
Module I2C_AV_Config 
2default:default
B
%s*synth23
Detailed RTL Component Info : 
2default:default
1
%s*synth2"
+---Adders : 
2default:default
Q
%s*synth2B
.	   2 Input      8 Bit       Adders := 1     
2default:default
4
%s*synth2%
+---Registers : 
2default:default
Q
%s*synth2B
.	                8 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                2 Bit    Registers := 1     
2default:default
Q
%s*synth2B
.	                1 Bit    Registers := 6     
2default:default
0
%s*synth2!
+---Muxes : 
2default:default
Q
%s*synth2B
.	   2 Input      2 Bit        Muxes := 2     
2default:default
Q
%s*synth2B
.	   4 Input      2 Bit        Muxes := 1     
2default:default
Q
%s*synth2B
.	   2 Input      1 Bit        Muxes := 3     
2default:default
Q
%s*synth2B
.	   4 Input      1 Bit        Muxes := 5     
2default:default
õ
Loading clock regions from %s
13*device2d
PC:/Xilinx/Vivado/2014.1/data\parts/xilinx/artix7/artix7/xc7a100t/ClockRegion.xml2default:defaultZ21-13
ú
Loading clock buffers from %s
11*device2e
QC:/Xilinx/Vivado/2014.1/data\parts/xilinx/artix7/artix7/xc7a100t/ClockBuffers.xml2default:defaultZ21-11
ô
&Loading clock placement rules from %s
318*place2Y
EC:/Xilinx/Vivado/2014.1/data/parts/xilinx/artix7/ClockPlacerRules.xml2default:defaultZ30-318
ó
)Loading package pin functions from %s...
17*device2U
AC:/Xilinx/Vivado/2014.1/data\parts/xilinx/artix7/PinFunctions.xml2default:defaultZ21-17
ò
Loading package from %s
16*device2g
SC:/Xilinx/Vivado/2014.1/data\parts/xilinx/artix7/artix7/xc7a100t/csg324/Package.xml2default:defaultZ21-16
å
Loading io standards from %s
15*device2V
BC:/Xilinx/Vivado/2014.1/data\./parts/xilinx/artix7/IOStandards.xml2default:defaultZ21-15
ò
+Loading device configuration modes from %s
14*device2T
@C:/Xilinx/Vivado/2014.1/data\parts/xilinx/artix7/ConfigModes.xml2default:defaultZ21-14
z
%s*synth2k
WPart Resources:
DSPs: 240 (col length:80)
BRAMs: 270 (col length: RAMB18 80 RAMB36 40)
2default:default
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[15] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[14] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[13] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[12] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[11] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[10] 2default:default2

ov7670_top2default:defaultZ8-3332
∞
ESequential element (%s) is unused and will be removed from module %s.3332*oasys20
\capture/r_dat_latch_reg[9] 2default:default2

ov7670_top2default:defaultZ8-3332
∞
ESequential element (%s) is unused and will be removed from module %s.3332*oasys20
\capture/r_dat_latch_reg[8] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[7] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[6] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[5] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[4] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[3] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[2] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[1] 2default:default2

ov7670_top2default:defaultZ8-3332
ª
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2;
'\IIC/u_I2C_Controller/I2C_RDATA_reg[0] 2default:default2

ov7670_top2default:defaultZ8-3332
©
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2)
\IIC/Config_Done_reg 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[15] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[14] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[13] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[12] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[11] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/r_dat_latch_reg[10] 2default:default2

ov7670_top2default:defaultZ8-3332
∞
ESequential element (%s) is unused and will be removed from module %s.3332*oasys20
\capture/r_dat_latch_reg[9] 2default:default2

ov7670_top2default:defaultZ8-3332
∞
ESequential element (%s) is unused and will be removed from module %s.3332*oasys20
\capture/r_dat_latch_reg[8] 2default:default2

ov7670_top2default:defaultZ8-3332
t
!design %s has unconnected port %s3331*oasys2

ov7670_top2default:default2
aclk2default:defaultZ8-3331
ï
+design %s has port %s driven by constant %s3447*oasys2

ov7670_top2default:default2
pwdn2default:default2
02default:defaultZ8-3917
ñ
+design %s has port %s driven by constant %s3447*oasys2

ov7670_top2default:default2
reset2default:default2
12default:defaultZ8-3917
u
!design %s has unconnected port %s3331*oasys2

ov7670_top2default:default2
READY2default:defaultZ8-3331
©
%s*synth2ô
ÑFinished Cross Boundary Optimization : Time (s): cpu = 00:00:16 ; elapsed = 00:00:17 . Memory (MB): peak = 566.969 ; gain = 438.160
2default:default
¢
%s*synth2í
~---------------------------------------------------------------------------------
Start RAM, DSP and Shift Register Reporting
2default:default
u
%s*synth2f
R---------------------------------------------------------------------------------
2default:default
¶
%s*synth2ñ
Å---------------------------------------------------------------------------------
Finished RAM, DSP and Shift Register Reporting
2default:default
u
%s*synth2f
R---------------------------------------------------------------------------------
2default:default
ù
6propagating constant %s across sequential element (%s)3333*oasys2
02default:default25
!i_0/\capture/FSM_onehot_s_reg[7] 2default:defaultZ8-3333
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/FSM_onehot_s_reg[7] 2default:default2

ov7670_top2default:defaultZ8-3332
±
ESequential element (%s) is unused and will be removed from module %s.3332*oasys21
\capture/FSM_onehot_s_reg[0] 2default:default2

ov7670_top2default:defaultZ8-3332
õ
6propagating constant %s across sequential element (%s)3333*oasys2
02default:default23
i_10/\IIC/mI2C_CLK_DIV_reg[14] 2default:defaultZ8-3333
Æ
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2.
\IIC/mI2C_CLK_DIV_reg[13] 2default:default2

ov7670_top2default:defaultZ8-3332
Æ
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2.
\IIC/mI2C_CLK_DIV_reg[14] 2default:default2

ov7670_top2default:defaultZ8-3332
Æ
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2.
\IIC/mI2C_CLK_DIV_reg[15] 2default:default2

ov7670_top2default:defaultZ8-3332
û
%s*synth2é
zFinished Area Optimization : Time (s): cpu = 00:00:17 ; elapsed = 00:00:18 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
†
%s*synth2ê
|Finished Timing Optimization : Time (s): cpu = 00:00:17 ; elapsed = 00:00:18 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[31] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[30] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[29] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[28] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[27] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[26] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[25] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[24] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[23] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[22] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[21] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[20] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[19] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[18] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[17] 2default:default2

ov7670_top2default:defaultZ8-3332
¨
ESequential element (%s) is unused and will be removed from module %s.3332*oasys2,
\capture/r_dout_reg[16] 2default:default2

ov7670_top2default:defaultZ8-3332
ü
%s*synth2è
{Finished Technology Mapping : Time (s): cpu = 00:00:17 ; elapsed = 00:00:19 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
ô
%s*synth2â
uFinished IO Insertion : Time (s): cpu = 00:00:18 ; elapsed = 00:00:19 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
;
%s*synth2,

Report Check Netlist: 
2default:default
l
%s*synth2]
I+------+------------------+-------+---------+-------+------------------+
2default:default
l
%s*synth2]
I|      |Item              |Errors |Warnings |Status |Description       |
2default:default
l
%s*synth2]
I+------+------------------+-------+---------+-------+------------------+
2default:default
l
%s*synth2]
I|1     |multi_driven_nets |      0|        0|Passed |Multi driven nets |
2default:default
l
%s*synth2]
I+------+------------------+-------+---------+-------+------------------+
2default:default
™
%s*synth2ö
ÖFinished Renaming Generated Instances : Time (s): cpu = 00:00:18 ; elapsed = 00:00:19 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
ß
%s*synth2ó
ÇFinished Rebuilding User Hierarchy : Time (s): cpu = 00:00:18 ; elapsed = 00:00:19 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
¢
%s*synth2í
~---------------------------------------------------------------------------------
Start RAM, DSP and Shift Register Reporting
2default:default
u
%s*synth2f
R---------------------------------------------------------------------------------
2default:default
¶
%s*synth2ñ
Å---------------------------------------------------------------------------------
Finished RAM, DSP and Shift Register Reporting
2default:default
u
%s*synth2f
R---------------------------------------------------------------------------------
2default:default
8
%s*synth2)

Report BlackBoxes: 
2default:default
A
%s*synth22
+-+--------------+----------+
2default:default
A
%s*synth22
| |BlackBox name |Instances |
2default:default
A
%s*synth22
+-+--------------+----------+
2default:default
A
%s*synth22
+-+--------------+----------+
2default:default
8
%s*synth2)

Report Cell Usage: 
2default:default
;
%s*synth2,
+------+-------+------+
2default:default
;
%s*synth2,
|      |Cell   |Count |
2default:default
;
%s*synth2,
+------+-------+------+
2default:default
;
%s*synth2,
|1     |BUFG   |     2|
2default:default
;
%s*synth2,
|2     |CARRY4 |    17|
2default:default
;
%s*synth2,
|3     |INV    |     1|
2default:default
;
%s*synth2,
|4     |LUT1   |    57|
2default:default
;
%s*synth2,
|5     |LUT2   |    14|
2default:default
;
%s*synth2,
|6     |LUT3   |    10|
2default:default
;
%s*synth2,
|7     |LUT4   |    71|
2default:default
;
%s*synth2,
|8     |LUT5   |    36|
2default:default
;
%s*synth2,
|9     |LUT6   |   124|
2default:default
;
%s*synth2,
|10    |MUXF7  |     8|
2default:default
;
%s*synth2,
|11    |FDCE   |    43|
2default:default
;
%s*synth2,
|12    |FDPE   |     8|
2default:default
;
%s*synth2,
|13    |FDRE   |    90|
2default:default
;
%s*synth2,
|14    |IBUF   |    13|
2default:default
;
%s*synth2,
|15    |IOBUF  |     1|
2default:default
;
%s*synth2,
|16    |OBUF   |    39|
2default:default
;
%s*synth2,
|17    |OBUFT  |     1|
2default:default
;
%s*synth2,
+------+-------+------+
2default:default
<
%s*synth2-

Report Instance Areas: 
2default:default
m
%s*synth2^
J+------+-------------------------------+-------------------------+------+
2default:default
m
%s*synth2^
J|      |Instance                       |Module                   |Cells |
2default:default
m
%s*synth2^
J+------+-------------------------------+-------------------------+------+
2default:default
m
%s*synth2^
J|1     |top                            |                         |   535|
2default:default
m
%s*synth2^
J|2     |  IIC                          |I2C_AV_Config            |   220|
2default:default
m
%s*synth2^
J|3     |    u_I2C_Controller           |I2C_Controller           |   141|
2default:default
m
%s*synth2^
J|4     |    u_I2C_OV7725_RGB444_Config |I2C_OV7670_RGB444_Config |    10|
2default:default
m
%s*synth2^
J|5     |  btn_debounce                 |debounce                 |    62|
2default:default
m
%s*synth2^
J|6     |  capture                      |ov7670_capture           |   196|
2default:default
m
%s*synth2^
J+------+-------------------------------+-------------------------+------+
2default:default
¶
%s*synth2ñ
ÅFinished Writing Synthesis Report : Time (s): cpu = 00:00:18 ; elapsed = 00:00:19 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
j
%s*synth2[
GSynthesis finished with 0 errors, 0 critical warnings and 55 warnings.
2default:default
£
%s*synth2ì
Synthesis Optimization Complete : Time (s): cpu = 00:00:18 ; elapsed = 00:00:19 . Memory (MB): peak = 581.801 ; gain = 452.992
2default:default
]
-Analyzing %s Unisim elements for replacement
17*netlist2
142default:defaultZ29-17
a
2Unisim Transformation completed in %s CPU seconds
28*netlist2
02default:defaultZ29-28
^
1Inserted %s IBUFs to IO ports without IO buffers.100*opt2
02default:defaultZ31-140
^
1Inserted %s OBUFs to IO ports without IO buffers.101*opt2
02default:defaultZ31-141
C
Pushed %s inverter(s).
98*opt2
02default:defaultZ31-138
√
!Unisim Transformation Summary:
%s111*project2Ü
r  A total of 2 instances were transformed.
  INV => LUT1: 1 instances
  IOBUF => IOBUF (IBUF, OBUFT): 1 instances
2default:defaultZ1-111
L
Releasing license: %s
83*common2
	Synthesis2default:defaultZ17-83
æ
G%s Infos, %s Warnings, %s Critical Warnings and %s Errors encountered.
28*	vivadotcl2
252default:default2
552default:default2
02default:default2
02default:defaultZ4-41
U
%s completed successfully
29*	vivadotcl2 
synth_design2default:defaultZ4-42
¸
I%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s
268*common2"
synth_design: 2default:default2
00:00:182default:default2
00:00:272default:default2
581.8012default:default2
407.6562default:defaultZ17-268

sreport_utilization: Time (s): cpu = 00:00:00 ; elapsed = 00:00:00.032 . Memory (MB): peak = 581.801 ; gain = 0.000
*common
w
Exiting %s at %s...
206*common2
Vivado2default:default2,
Thu Feb 26 02:26:07 20152default:defaultZ17-206