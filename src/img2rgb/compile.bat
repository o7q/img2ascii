@echo off
set name=img2rgb
title Compiling %name%
color 7
pyinstaller %name%.py --onefile
title DONE!
echo.
pause