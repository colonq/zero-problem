@echo off
cd /d %~dp0

md Tmp

xcopy /e ..\src\Locale Tmp\Locale\
del /S *.po
xcopy /e ..\src\Licenses Tmp\Licenses\
xcopy /t /e ..\src\Htmls Tmp\Htmls\
copy ..\src\Htmls\*.html Tmp\Htmls\
copy ..\src\Release\ZeroProblem.exe Tmp\
copy README.html Tmp\README.html

cd Tmp
"C:\Program Files\7-Zip\7z.exe" a -mx=9 -stl ..\ZeroProblem-0.9.zip
cd ..

md Tmp\leela-zero-network-weights
copy ..\..\Downloads\d351f06e446ba10697bfd2977b4be52c3de148032865eaaf9efc9796aea95a0c.gz Tmp\leela-zero-network-weights\157.gz

"C:\Program Files\7-Zip\7z.exe" e "..\..\Downloads\roboto-hinted.zip" -o"Tmp\roboto-hinted"
rd /s /q Tmp\roboto-hinted\roboto-hinted

"C:\Program Files\7-Zip\7z.exe" e "..\..\Downloads\leela-zero-0.16-cpuonly-win64.zip" -o"Tmp\leela-zero-0.16-cpuonly-win64"
"C:\Program Files\7-Zip\7z.exe" e "..\..\Downloads\leela-zero-0.16-win64.zip" -o"Tmp\leela-zero-0.16-win64"
rd Tmp\leela-zero-0.16-cpuonly-win64\leela-zero-0.16-cpuonly-win64
rd Tmp\leela-zero-0.16-win64\leela-zero-0.16-win64
copy settings64.txt Tmp\settings.txt

cd Tmp
"C:\Program Files\7-Zip\7z.exe" a -mx=9 -stl ..\ZeroProblem-0.9-win64.zip
cd ..

rd /s /q Tmp\leela-zero-0.16-cpuonly-win64
rd /s /q Tmp\leela-zero-0.16-win64
del Tmp\setting.txt

"C:\Program Files\7-Zip\7z.exe" e "..\..\Downloads\leela-zero-0.16-cpuonly-win32.zip" -o"Tmp\leela-zero-0.16-cpuonly-win32"
"C:\Program Files\7-Zip\7z.exe" e "..\..\Downloads\leela-zero-0.16-win32.zip" -o"Tmp\leela-zero-0.16-win32"
rd Tmp\leela-zero-0.16-cpuonly-win32\leela-zero-0.16-cpuonly-win32
rd Tmp\leela-zero-0.16-win32\leela-zero-0.16-win32
copy settings32.txt Tmp\settings.txt

cd Tmp
"C:\Program Files\7-Zip\7z.exe" a -mx=9 -stl ..\ZeroProblem-0.9-win32.zip
cd ..

rd /s /q Tmp
