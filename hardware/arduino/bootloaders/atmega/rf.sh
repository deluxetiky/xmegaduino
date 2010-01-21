#avrdude=/usr/local/CrossPack-AVR/bin/avrdude
#avrdude=/opt/local/bin/avrdude
avrdude=/Users/giuliano/Desktop/Arduino-0018rc1.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude
conf=/Users/giuliano/Desktop/Arduino-0018rc1.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf

#$avrdude -c avrisp2 -P usb -p m328p \
$avrdude -c avrisp2 -P usb -p m644p \
    -C $conf \
    -U flash:r:m328p.flash:i
