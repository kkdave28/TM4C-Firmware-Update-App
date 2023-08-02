start_addr = 0x08008800
end_addr = 0x0803FFFF
stride = 0x800
address_low = 0x08009cf4
address_high = 0x08009de2
start_page = 17
end_page = 0

if(address_low < start_addr or address_high > end_addr):
    print("INVALID ADDR")
else:
    while address_low > start_addr:
        if(address_low < start_addr + stride):
            print(hex(start_addr), start_page)
            print(address_low % stride)
            break
        else:
            start_addr += stride
            start_page += 1
    end_page = start_page
    while address_high > start_addr:
        if(address_high < start_addr + stride):
            print(hex(start_addr+stride), end_page)
            break
        else:
            start_addr += stride
            end_page += 1
