#!/usr/bin/env python3
import markdown
from mdx_gfm import GithubFlavoredMarkdownExtension

header1 = r'''<!DOCTYPE html>
<html lang="'''

header2 =r'''">
<head>
<meta charset="UTF-8">
<title>'''

header3 = r'''</title>
</head>
<body>
'''
footer = r'''
</body>
</html>
'''
md = markdown.Markdown(extensions=[GithubFlavoredMarkdownExtension()])

def write_html(name, lang, title):
	fin = open(name + ".md", "r", encoding="utf-8")
	source = fin.read()
	fin.close()
	html = md.convert(source)
	h = header1 + lang + header2 + title + header3 + html + footer
	fout = open(name + ".html", "w", encoding="utf-8")
	fout.write(h)
	fout.close()
	fdoc = open(r"../../docs/Htmls/" + name + ".html", "w", encoding="utf-8")
	fdoc.write(h)
	fdoc.close()

write_html("how_to_build", "en", "How to build")
write_html("user_guide", "en", "User guide")
write_html("user_guide_ja", "ja", "ユーザーガイド")
