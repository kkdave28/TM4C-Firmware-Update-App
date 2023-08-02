import sys

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

def run():
    with open(sys.argv[1], "r") as patch_file:
        addr_low = int(patch_file.readline(), 16)
        patch_sz = int(patch_file.readline())
        patch_file.close()
        return address_calculator(addr_low, addr_low+patch_sz, patch_sz)

if __name__ == "__main__":
    run()
