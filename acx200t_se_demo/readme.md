# acx200t-se-demo
This demo is a about how to use AisinoChip ACX200T library SE APIs.

# Features

- SM2, SM3, SM4 API's Usage 
- ACX200T SM2 Sign and Verify Speed Demo
- One SPI or Two SPI Interfaces Can Be Used

# Build

This demo has been build successful by arm-poky-linux-gnueabi-gcc.

# Usage

```
 ./acx200t_se_demo 0 | 1 means a sigle test SM2, SM3, SM4 all APIs by SPI0 or SPI1 device.
 ./acx200t_se_demo 0 | 1 15000000 means a sigle test SM2, SM3, SM4 all APIs by SPI0 or SPI1 device under SPI CLK 15MHz.
 ./acx200t_se_demo sign|verify 0|1 means loop test SM2 sign or verify by SPI0 or SPI1 device.
 ./acx200t_se_demo sign|verify 2 means loop test SM2 sign or verify by SPI0 and SPI1 devices, two SPI interfaces.
```

# Notes

- This demo used the spi devices are spidev1.0, spidev0.0 and gpio numbers are gpio101, gpio169. Please change them as your hardware design. 
- As SPI wires maybe interfered, please add the SPI CLK argument in the last, e.g. 15000000.
