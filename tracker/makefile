TARGET    = tracker
OBJPATH   = objs/

CFLAGS    = --model-large -I../mcu-lib -DNRF24LE1
CFLAGS   += -DDBG_MODE=0 -DPRINTF=printf_tiny
CFLAGS   += -DSECOND_ADDR=0
CFLAGS   += -DKBD_MATRIX=1
LFLAGS    = --code-loc 0x0000 --code-size 0x4000 --xram-loc 0x0000 --xram-size 0x400
ASFLAGS   = -plosgff
RELFILES  = $(addprefix $(OBJPATH), tracker.rel i2c.rel sensors.rel sensor_dmp_firmware.rel \
  sleeping.rel tracker_settings.rel rf_addr.rel rf_tracker.rel auto_shutdown.rel nRF24L.rel \
  nrfdbg.rel nrfutils.rel crtxinit.rel)

VPATH     = ../mcu-lib

hex: $(TARGET).hex

$(TARGET).hex: $(RELFILES)
	sdcc $(CFLAGS) $(LFLAGS) $(RELFILES) -o $(TARGET).hex
	grep "ROM/EPROM" $(TARGET).mem

$(OBJPATH)%.rel: %.c makefile
	sdcc $(CFLAGS) -c $< -o $(OBJPATH)

$(OBJPATH)%.rel: %.asm
	sdas8051 $(ASFLAGS) $@ $< 

clean:
	rm --force *.hex *.mem *.lk *.map $(addprefix $(OBJPATH), *.lst *.rel *.rst *.sym *.asm)

all: clean hex

flash: clean hex
	nrfburn -f 16 -w $(TARGET).hex
