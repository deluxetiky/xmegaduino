#avrdude=/usr/local/CrossPack-AVR/bin/avrdude
#avrdude=/opt/local/bin/avrdude
avrdude=/Users/giuliano/Desktop/Arduino-0018rc1.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude
conf=/Users/giuliano/Desktop/Arduino-0018rc1.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf

#$avrdude -c avrisp2 -P usb -p m328p \
#$avrdude -c avrisp2 -P usb -p m644p \
$avrdude -c dragon_jtag -P usb -p x128a1 \
    -C $conf \
    -U boot:r:xplain.boot:i \
    -U application:r:xplain.appl:i \
    -U apptable:r:xplain.appt:i \
