# ptp4FreeRTOS
FreeRTOS port of linuxptp(ptp4l)
# Features
1. Supports linuxptp v1.6
2. Supports multi instance (currently tested with two ptp4l instances)
3. Uses LWiP. Changes are needed in LWIP to support timestamping. Included in this repo as a LWIP patch.
4. Tested with gPTP profile only
