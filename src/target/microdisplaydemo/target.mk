NAME := microdisplaydemo
$(TARGET): build/$(TARGET)/$(TYPE)/$(NAME).bin
LISTINGS: build/$(TARGET)/$(TYPE)/$(NAME).elf.lst
