import serial
import os
FW_FILE = "complex-example.bin"
MAX_BLOCK_SZ = 2048
def run(port, baudrate, timeout=1000, wtimeout=1000):
    bridge = serial.Serial(port, baudrate, timeout=timeout, write_timeout=wtimeout)
    fw_file = open(FW_FILE, "rb")
    current_app_sz = os.path.getsize(FW_FILE)
    current_block_sz = current_app_sz if current_app_sz < MAX_BLOCK_SZ else MAX_BLOCK_SZ
    while True:
        char_read = bridge.read().decode("utf-8")
        if(char_read == 'u'):
            print("hee hee")
            bridge.write(bytes('s', "utf-8"))
            sz_info = str(current_app_sz) + "\n"
            bridge.write(bytes(sz_info, "utf-8"))
        elif(char_read == 'f'):
            buf = fw_file.read(MAX_BLOCK_SZ)
            print("sending chunk of size = ", len(buf))
            if(len(buf) == 0):
                fw_file.close()
            bridge.write(buf)
        





if __name__ == "__main__":
    run("COM4", 115200)