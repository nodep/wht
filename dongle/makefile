TARGET    = whtdngl
OBJPATH   = objs_dngl/

# set FLASH_SIZE_KB to either 16 or 32
FLASH_SIZE_KB = 16

ifeq ($(FLASH_SIZE_KB),32)
    CODE_SIZE = 0x8000
else ifeq ($(FLASH_SIZE_KB),16)
    CODE_SIZE = 0x4000
else
    $(error Please set FLASH_SIZE_KB to either 16 or 32)
endif

CFLAGS    = --model-large -I../tracker -I../mcu-lib -DNRF24LU1
CFLAGS   += -DDBG_MODE=2 -DPRINTF=printf_fast
CFLAGS   += -DSECOND_ADDR=0
LFLAGS    = --code-loc 0x0000 --code-size $(CODE_SIZE) --xram-loc 0x8000 --xram-size 0x800
ASFLAGS   = -plosgff
RELFILES  = $(addprefix $(OBJPATH), dongle.rel proc_packet.rel usb_desc.rel rf_dngl.rel usb.rel \
    reports.rel mdu.rel math_cordic.rel dongle_settings.rel rf_addr.rel nrfutils.rel nrfdbg.rel \
    nRF24L.rel crtxinit.rel)

VPATH    = ../mcu-lib:../tracker

$(TARGET).hex: $(RELFILES)
	sdcc $(CFLAGS) $(LFLAGS) $(RELFILES) -o $(TARGET).hex
	grep "ROM/EPROM" $(TARGET).mem

$(OBJPATH)%.rel: %.c makefile
	sdcc $(CFLAGS) -c $< -o $(OBJPATH)

$(OBJPATH)%.rel: %.asm
	sdas8051 $(ASFLAGS) $@ $<

clean:
	rm --force *.hex *.mem *.lk *.map $(addprefix $(OBJPATH), *.lst *.rel *.rst *.sym *.asm)

all: clean $(TARGET).hex

flash: $(TARGET).hex
	nrfburn -f $(FLASH_SIZE_KB) -w $(TARGET).hex
