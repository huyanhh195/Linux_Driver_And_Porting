## Register View
000 = GPIO Pin 19 is an input
001 = GPIO Pin 19 is an output

The GPIO register base address is 0x7e200000

GPFSEL2 Register 
11:9 FSEL23: FSEL23 - Function Select 23 RW 0x0

GPSET0 Register

Description
    The output set registers are used to set a GPIO pin. The SETn field defines the respective GPIO pin to set, writing a “0”
    to the field has no effect. If the GPIO pin is being used as an input (by default) then the value in the SETn field is
    ignored. However, if the pin is subsequently defined as an output then the bit will be set according to the last set/clear
    operation. Separating the set and clear functions removes the need for read-modify-write operations