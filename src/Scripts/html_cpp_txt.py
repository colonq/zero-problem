#!/usr/bin/env python3
import sys
import os
import subprocess

html_1 = r'''<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">'''

html_2 = r'''	<style type="text/css">
		body { margin: 0; padding: 0; background-color: #FAFAFA; font-size: 1vw }
		#buffer { display: none }
		.page { page-break-inside: avoid }
		.info  { width: 50vw; height: 3vw; margin: 1vw 0; font-size: 3vw; font-family: "Roboto", "Tahoma", sans-serif; white-space: pre }
		.two-canvases { width: 94vw; height: 50vw; padding-top: 1vw; margin: 0 auto; clear: left }
		.view { width: 45vw; height: 45vw; margin-right: 4vw; float: left }
		.view:nth-child(3n) { margin-right: 0 }
		.three-svgs { width: 60vh; height: 32vh; margin: 0 auto; clear: left }
		.diagram { width: 19.35vh; height: 25.8vh; margin-right: 0.975vh; float: left }
		.diagram:nth-child(4n) { margin-right: 0 }
		@media screen {
			.three-svgs { display:none; }
		}
		@media print {
			.info { width: 30vh; height: 1.8vh; margin: 0.5vh 0; font-size: 1.8vh }
			.two-canvases { display:none; }
		}
	</style>
	<script>'''

html_3 = r'''	</script>
</head>
<body>
</body>
</html>
'''

def cstr_write(f, name, s, tabs):
	f.write('static const char* const ' + name + '[] = {')
	lines = s.splitlines()
	last_line = lines.pop()
	for line in lines:
		f.write('\n\tR"*(' + tabs + line + ')*",')
	f.write('\n\tR"*(' + tabs + last_line + ')*"\n};\n\n')

def make_js_9(js_name):
	f19 = open(js_name, 'r', encoding='utf-8')
	lines = []
	for line in f19:
		lines.append(line)
	f19.close()
 
	f9 = open('9.js', 'w', encoding='utf-8')
	for line in lines:
		if line == '\tconst kBoardSize = 19;\n':
			line = '\tconst kBoardSize = 9;\n'
		f9.write(line)
	f9.close()

def compile_js(js_name):
	command = [r'C:\bin\openjdk-11.0.1_windows-x64_bin\jdk-11.0.1\bin\java.exe']
	command.extend(r'-jar C:\bin\closure-compiler\closure-compiler-v20181028.jar --compilation_level=ADVANCED'.split())
	command.append(js_name)
	return subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8').communicate()

def generate_html(h_name, js_name):
	make_js_9(js_name)
	(outstr, errstr) = compile_js(js_name)
	(outstr9, errstr9) = compile_js('9.js')
	os.remove('9.js')
 
	f = open(h_name, 'w', encoding='utf-8')
	if errstr:
		f.write(errstr)
		f.close()
		sys.exit()
	if errstr9:
		f.write(errstr9)
		f.close()
		sys.exit()	  

	cstr_write(f, 'html_1', html_1, '')
	cstr_write(f, 'html_2', html_2, '')
	cstr_write(f, 'html_3', html_3, '')
	cstr_write(f, 'js_load_problems_9', outstr9, '\t\t')
	cstr_write(f, 'js_load_problems_19', outstr, '\t\t')
 
	f.close()

generate_html('html_cpp.txt', 'load_problems.js')
