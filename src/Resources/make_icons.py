#!/usr/bin/env python3
import subprocess

subprocess.run(r'magick icon.png -define icon:auto-resize app.ico')
subprocess.run(r'magick icon.png -resize 32x32 32x32\zproblem.png')
subprocess.run(r'magick icon.png -resize 48x48 48x48\zproblem.png')
subprocess.run(r'magick icon.png -resize 64x64 64x64\zproblem.png')
subprocess.run(r'magick icon.png -resize 128x128 128x128\zproblem.png')
subprocess.run(r'magick icon.png -resize 128x128 app_ico_xpm.xpm')

f = open('app_ico_xpm.xpm', 'r', encoding = 'utf-8')
s = f.read()
f.close()

d = s.replace('static char *', 'static const char *')

f = open('app_ico_xpm.xpm', 'w', encoding = 'utf-8')
f.write(d)
f.close()
