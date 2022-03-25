# acx200t-v2x-demo
This demo is a about how to use AisinoChip ACX200T library V2X APIs.

# Features

- OBU, RSU PDU Message to be Signed and Encode to the SPDU Message 
- SPDU Message to be Verified and Decode to the PDU message

# Build

This demo has been build successful by arm-poky-linux-gnueabi-gcc.

# Usage

```
 ./acx200t_v2x_demo verify means loop test the v2x verify api.
 ./acx200t_v2x_demo sign obu|rsu means loop test the v2x sign api.
 ./acx200t_v2x_demo verify | sign 15000000 means loop test the v2x verify or sign api under SPI CLK 15MHz.
```

# Notes

- Please copy the cert_list files to the directory of the demo.
- Please adjust the system date in the validity of the certificate.
- As SPI wires maybe interfered, please add the SPI CLK argument in the last, e.g. 15000000.
