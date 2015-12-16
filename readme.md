# Zynq AMP Mode #

Note : This design refers to xapp1078 and xapp1079.

### Summery###
Simple AMP 

***Running Linux and Bare-Metal System on Both Zynq SoC Processors***

CPU0 runs linux ; CPU1 runs standalone code to blink a led. 

And every CPU can communicate with the other through shared memory.
### Lab info ###

Vivado : 2015.2

linux kernel : 2014.4

Rootfs :  linaro

u-boot : 2015.01

board : ZYBO


## Work flow##

###Build hardware project###

- Download the zip and extract it
- Open `vivado 2015.2` tcl console ,then go into `<your path>/hardware_prj`
- Run `source ./system_pro.tcl`
- Waitting for bitstream ...  then it's ok !

###SDK work###
- Open the implemented design (necessary to export bitfile to SDK)
- Export the design and bitstream to SDK and launch SDK. Once SDK opens, a hardware platform project will have been created automatically
- Select `Xilinx_Tools->Repositories`.Beside the `Local Repositories`, select `NewBrowse` to and select `<your path>/src/sdk_repo`, select OK
- Create the FSBL project
  + Select `File->New->Application_Project`
  + Enter the project name `amp_fsbl` and select Next 
  + Select the template `Zynq FSBL` then select Finish
  + Verify the FSBL BSP is using standalone from thelocal repository: After FSBL builds, right click on the `fsbl_bsp` project and select `Board Support Package Settings`In the Overview window, verify the OS Version is `5.19`. 
- Create the BSP for CPU1
  + Select `File->New->Board_Support_Package`
  + Enter the project name `app_cpu1_bsp`, change CPU to `ps7_cortexa9_1`. 
  + Select Finish
  + In the `Board Support Package Settings` Select `Overview->drivers->ps7_cortexa9_1` and change the    `extra_compiler_flags` value to contain `-g -DUSE_AMP=1`
  + Select OK
  + Open `app_cpu1_bsp/ps7_cortexa9_1/libsrc/standalone_v5_19/src/xil-crt0.s` delete this command 
    `bl XTime_SetTime`  
- Create the Application that will run on CPU1
  + Select `File->New->Application_Project`
  + Enter the project name `app_cpu1`
  + Change Processor to `ps7_cortexa9_1`
  + Change `Board Support Package` to `Use existing` `app_cpu1_bsp` and select Next
  + Select the template `Empty Application` then select Finish.
  + Import the C and linkerscript file for `app_cpu1`. Right click on `app_cpu1/src` and select `Import`,Select `General->File_System` then select `From directory`, browse to and select `<your path>/src/cpu1_app/src`,select both `app_cpu1.c` and `lscript.ld` then select Finish. Answer Yes to overwrite lscript.ld
- Create BOOT.BIN
  + From SDK, open a command window by selecting `Xilinx_Tools->Launch_Shell`
  + In the shell, cd to `<your path>/src/bootgen`
  + After all files added (amp_fsbl.elf,*.bit,u-boot.elf,app_cpu1.elf) Run bootgen by executing the batch file `createBoot.bat`
  + Note: You can use `Creat Zynq Bootimage` GUI to achieve that ,too. 

###Linux work###

ZYBO has 512MB of Ram so 384MB was chosen to be used by CPU0 to run Linux and the remaining 128MB was left for CPU1.  This means we need to modify the device tree source and u-boot file.

- Devicetree
  + Add `mem=384M maxcpus=1` in the bootargs
  + Modify the memory reg to `reg = <0x0 0x18000000>`
  + Compile the devicetree
- u-boot
  + Modify the source of u-boot. Open `u-boot/include/configs/zynq_zybo.h`, modify `CONFIG_SYS_SDRAM_SIZE` to
   `(384 * 1024 * 1024)`
  + Compile the u-boot

###Working with the design###

- Copy BOOT.bin,uImage,devicetree.dtb and `<your path>/src/tool/devmem` to SD card.
- Power up the board
  + Run `mount /dev/mmcblk0p1 /mnt/`when linux is up.
  + Run `cd /mnt/devmem`
  + Run `./devmem 0xfffffff0 w 0x18000000` after this command ,you can see the led is blinking
  + Run `./devmem 0xFFFF8000 w ` repeatedly, you can see the counter is bigger 

Congratulations!

*If you get some problems,please contact `xinyuc@xilinx.com`*

**Best Regards!**
