import serial
import os
FW_FILE = "complex-example.bin"
DELTA_FW_FILE = "divide.patch"
MAX_BLOCK_SZ = 2048

def address_calculator(address_low, address_high, patch_sz):
    
    start_page = 17
    end_page = 0
    start_addr = 0x08008800
    end_addr = 0x0803FFFF
    stride = 0x800
    ret_list = []
    if(address_low < start_addr or address_high > end_addr):
        print("INVALID ADDRESS")
    else:
        while address_low > start_addr:
            if(address_low < start_addr + stride):
                ret_list.append(start_page)
                ret_list.append(address_low % stride)
                break
            else:
                start_addr += stride
                start_page += 1
        end_page = start_page
        while address_high > start_addr:
            if(address_high < start_addr + stride):
                ret_list.append(end_page)
                if(end_page != start_page):
                    ret_list.append(patch_sz - ((start_addr) - address_low))
                break
            else:
                start_addr += stride
                end_page += 1
        
        return ret_list

def run(port, baudrate, timeout=1000, wtimeout=1000):
    bridge = serial.Serial(port, baudrate, timeout=timeout, write_timeout=wtimeout)
    fw_file = open(FW_FILE, "rb")
    delta_fw = open(DELTA_FW_FILE, "r")
    current_app_sz = os.path.getsize(FW_FILE)
    current_block_sz = current_app_sz if current_app_sz < MAX_BLOCK_SZ else MAX_BLOCK_SZ
    while True:
        char_read = bridge.read().decode("utf-8")
        if(char_read == 'u'):
            bridge.write(bytes('s', "utf-8"))
            sz_info = str(current_app_sz) + "\n"
            bridge.write(bytes(sz_info, "utf-8"))
        elif(char_read == 'f'):
            buf = fw_file.read(MAX_BLOCK_SZ)
            print("sending chunk of size = ", len(buf))
            if(len(buf) == 0):
                fw_file.close()
            bridge.write(buf)
        elif (char_read == 'v'):
            a_low = int(delta_fw.readline(), 16)
            patch_sz = delta_fw.readline()
            ret_list = address_calculator(a_low, a_low + int(patch_sz), int(patch_sz))
            bridge.write(bytes('s', "utf-8"))
            bridge.write(bytes(str(ret_list[0]) + "\n", 'utf-8'))
            bridge.write(bytes(str(ret_list[1]) + "\n", 'utf-8'))
            bridge.write(bytes(patch_sz, 'utf-8'))
            byte_list = delta_fw.readlines()
            byte_list = [int(x.strip(), 16) for x in byte_list]
            buf = bytes(byte_list)
            print(len(buf), buf)
            bridge.write(buf)
            

        





if __name__ == "__main__":
    run("COM4", 115200)