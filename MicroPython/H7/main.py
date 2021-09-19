from machine import UART
from uos import dupterm
uart = UART(3, 115200)
dupterm(None,2)