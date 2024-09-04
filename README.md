# ps4-PKG-Backup

This is a simple open-source payload to Backup PS4 games, update & dlcs to usb drive. It reads the `config.ini` file present in the root of the usb drive & backup the game files to the usb drive mentioned in the config.

## config.ini
```config.ini
; ps4-PKG-Backup config.ini file. Place it in your USB drive root.
; to define titleId of game to copy:
; Mention the original title id format "CUSAXXXXX" (Where XXXXX 5 digits in title id unique to the game)
titleId=CUSA00001

; to define method:
; 1 - Copy Base Game Only
; 2 - Copy Game Update Only
; 3 - Copy Game DLCs Only
; 4 - Copy Base Game, Update and DLCs
method=4
```

## Notes
- Place the `config.ini` file to the root of the usb and set desired `titleId` and `method` before injecting the payload.
- `CUSAXXXXX.inprogress` file in the usb drive root indicates Backup is inprogress.
- `CUSAXXXXX.completed` file in the usb drive root indicates Backup is completed.

## Credits to
- [kmeps4](https://github.com/kmeps4)
- [Al-Azif](https://github.com/Al-Azif)
- [Scene Collective](https://github.com/Scene-Collective)
