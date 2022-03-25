#CROSS_COMPILE := arm-fslc-linux-gnueabi
#C_FLAGS := -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard --sysroot=/opt/fslc-framebuffer/2.4.2/sysroots/armv7at2hf-neon-fslc-linux-gnueabi

CROSS_COMPILE := arm-poky-linux-gnueabi
C_FLAGS :=  -march=armv7-a -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=/opt/fsl-imx-fb/4.9.88-2.0.0/sysroots/cortexa9hf-neon-poky-linux-gnueabi

C_FLAGS += -Wall -O2

C_FLAGS +=  -I. 

LD_FLAGS := -Wl,-rpath,./ -L.  -lacx200t -lpthread


TAGET := acx200t_se_demo


CC := $(CROSS_COMPILE)-gcc
AR := $(CROSS_COMPILE)-ar
STRIP := $(CROSS_COMPILE)-strip

all:
	$(CC) $(C_FLAGS)  -o $(TAGET) acx200t_se_demo.c $(LD_FLAGS) 
	$(STRIP) $(TAGET)

clean:
	rm -f *.o
	rm -f $(TAGET)
