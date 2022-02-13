# Custom firmware deployent for AMH-ESP-LEGO-UART
import argparse
from time import sleep
import serial
from esptool import *

parser = argparse.ArgumentParser(description="Micropython flash/configuration tool for AMH ESP8266")
parser.add_argument("--port","-p",help="Serial port e.g. COM3, /dev/ttyUSB0, etc.")
parser.add_argument("--detect-port","-d",help="Show detected port",action="store_true")
parser.add_argument("--flash","-f",metavar='<micropython.bin>',help="Erase flash and write binary file <micropython.bin> to esp device.")
parser.add_argument("--webrepl",metavar="<password>",help="Configure webrepl using password <password>")
parser.add_argument("--wifi",metavar='<ssid>,<pasword>',help="Configure wifi connection <ssids> and password <password>")
parser.add_argument("--getip","-i",help="show IP address for AP and STA mode",action="store_true")

args = parser.parse_args()

if args.webrepl:
    print(args.webrepl)

cur_mode='user'
cur_serial=None
serial_port=None

def set_mode(mode):
    global cur_mode
    if cur_mode!=mode:
        if mode=="boot":
            input("\nPut esp in boot mode by pressing the reset button while keeping the boot button pressed.\nHit <Enter> when done.")
            cur_mode="boot"
        elif mode=="user":
            input("\nPress the reset button on the esp module.\nHit <Enter> when done.")
            cur_mode="user"

# Install webrepl
def read_check_exec(exec, check=""):
    sleep(0.5)
    r = cur_serial.read(cur_serial.in_waiting)
    #print(r.decode())
    if check:
        if not (r[-len(check):] == bytes(check, "UTF-8")):
            print(r[-len(check):], "is not", bytes(check, "UTF-8"))
            return r
    cur_serial.write(bytes(exec+"\r\n","UTF-8"))
    return r.decode()

def config_wifi(ssid,wifipw):
    read_check_exec("\r\n\r\n")
    read_check_exec("import network", check=">>> ")
    read_check_exec("wlan = network.WLAN(network.STA_IF)", check=">>> ")
    read_check_exec("wlan.active(True)", check=">>> ")
    read_check_exec("wlan.connect('%s', '%s')"%(ssid,wifipw), check=">>> ")
    print("[*] Waiting for wifi to connect...")
    sleep(5)
    read_check_exec("wlan.ifconfig()", check=">>> ")
    q=read_check_exec("\r\n\r\n")
    print("wifi config:",q)

def get_sta_ip_address():
    read_check_exec("\r\n")
    read_check_exec("import network", check=">>> ")
    read_check_exec("wlan = network.WLAN(network.STA_IF)", check=">>> ")
    read_check_exec("wlan.ifconfig()", check=">>> ")
    q=read_check_exec("\r\n")
    ip_address=q.split('\r\n')[1][1:-1].split(',')[0][1:-1]
    return ip_address

def get_ap_ip_address():
    read_check_exec("\r\n")
    read_check_exec("import network", check=">>> ")
    read_check_exec("wlan = network.WLAN(network.AP_IF)", check=">>> ")
    read_check_exec("wlan.ifconfig()", check=">>> ")
    q=read_check_exec("\r\n")
    ip_address=q.split('\r\n')[1][1:-1].split(',')[0][1:-1]
    return ip_address

def webrepl(password):
    read_check_exec("\r\n\r\n")
    read_check_exec("import webrepl_setup", check=">>> ")
    read_check_exec("E", check="(Empty line to quit)\r\n> ")
    read_check_exec(password, check="New password (4-9 chars): ")
    read_check_exec(password, check="Confirm password: ")
    read_check_exec("y", check="reboot now? (y/n) ") 
    read_check_exec("\r\n\r\n")


def detect_port():
    set_mode('boot')
    ser_list = get_port_list() 
    esp=get_default_connected_device(ser_list,port=None, connect_attempts=7,initial_baud=115200,chip='auto')
    serial_port=esp.serial_port
    esp._port.close()
    return serial_port

def erase(port=None):
    cmd=["erase_flash"]
    if port:
        cmd.append("--port")
        cmd.append(port)
    main(cmd)

def flash(binfile,port=None):
    cmd=["--baud","460800","write_flash","--flash_size=detect","0",binfile]
    if port:
        cmd.append("--port")
        cmd.append(port)
    main(cmd)
 

if args.detect_port:
    serial_port=detect_port()

if args.flash:
    set_mode("boot")
    print('[*] erasing flash')
    erase(port=args.port)
    print('[*] writing flash')
    flash(args.flash,port=args.port)
    print('[*] flashing done')
    set_mode("user")
    
if args.getip or args.wifi or args.webrepl:
    if not args.port:
        if not serial_port:
            serial_port=detect_port()
    else:
        serial_port=args.port
    if args.wifi:
        set_mode("user")
        print("[*] configuring wifi STA") 
        cur_serial = serial.Serial(serial_port, 115200)
        ssid,wifipw=args.wifi.split(",")
        config_wifi(ssid,wifipw)
        print("[*] Wifi configured. User --getip for checking current IP address.") 
        
        cur_serial.close()

    if args.getip:
        set_mode("user")
        print("[*] checking IP address") 
        cur_serial = serial.Serial(serial_port, 115200)
        print("[+] IP address STA =",get_sta_ip_address())
        print("[+] IP address AP =",get_ap_ip_address())   
        cur_serial.close()

    if args.webrepl:
        set_mode("user")
        print('[*] configuring webrepl with password:',args.webrepl)
        cur_serial = serial.Serial(serial_port, 115200)
        webrepl(args.webrepl)
        cur_serial.close()
        print('[*] webrepl configured')


