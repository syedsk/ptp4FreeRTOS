# ptp4FreeRTOS
FreeRTOS port of linuxptp(ptp4l)
# Features
Supports linuxptp v1.6
Supports multi instance (currently tested with two ptp4l instances)
Uses LWiP. Changes are needed in LWIP to support timestamping. Included in this repo as a LWIP patch.
Tested with gPTP profile only
