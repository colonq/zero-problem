# User guide

## Overview

Zero Problem is a software which generates Go problems from your or someone's Go game records using [Leela Zero](https://zero.sjeng.org/). It works on Windows Vista and later.

## Installation

### Zero Problem

You have to download the latest ZIP file of Zero Problem for Windows from [GitHub](https://github.com/colonq/ZeroProblem/releases), and extract it. A smaller ZIP file only includes Zero Problem. Other ZIP files also include the files Zero Problem requires to work. If you have got a smaller one, you need to get Leela Zero, its network weights file, and Roboto fonts.

### Leela Zero and its network weights file

- Get the latest ZIP file of Leela Zero for Windows from [GitHub] (https://github.com/gcp/leela-zero/releases)

- Get a network weights file from the list of *Best Network Hash* on [Leela Zero](https://zero.sjeng.org/). If you want a small and quality network weights file, an [ELF OpenGo](https://research.fb.com/facebook-open-sources-elf-opengo/)'s network weights file is available from [here](http://zero.sjeng.org/networks/62b5417b64c46976795d10a6741801f15f857e5029681a42d02c9852097df4b9.gz), which is converted for Leela Zero.

### Fonts

- Get the latest *roboto-hinted.zip* from [GitHub](https://github.com/google/roboto/releases),  and extract it.

- Double-click each of the files of *Roboto-Regular.ttf*, *RobotoCondensed-Regular.ttf*, and *Roboto-Bold.ttf*. And install the fonts from a font viewer.

### Build from source

If you want to build Zero Problem from source, see [how_to_build.html](how_to_build.html).

## Usage

### Start

Double-click *ZeroProblem.exe* to run.

### Change some settings

Select *Settings* from the problem menu. The settings dialog contains the following properties:

- **Leela Zero**: The path to *leelaz.exe*.
- **Network weights**: The path to a network weights file.
- **Number of threads**: The number of threads to use for generating Go problems. It is typically the number of CPU cores or less.
- **Number of visits**: The quality of Go problems. If you increase this number, you will get higher quality output but the generation speed will be lower.
- **Number of problems**: The maximum number of Go problems generated from a Go game file. If the Go game recorded on a file is awesome or just short, this software will generate a less number of Go problems.
- **Search range**: The range of position numbers to use for reducing positions which Go problems are generated from.
- **Number of moves**: The maximum number of moves on each diagrams.
- **Character set**: The default character set. If Go game file don't have a value of the character set property, this software will be use this value instead.
- **Player**: The player who play next on the first position of each Go problem.
- **Player name**: The list of player names. It is only used when the value of the player property is set *By name*. The player names must be separated by a semicolon.

### Choose Go game files and generate Go problems

The main window contains the following buttons:

- **Append**: appends Go geme files into the list. You can also drop files on the list control instead.
- **Delete**: deletes Go geme files in the list.
- **Clear**: clears the list.
- **Generate**: opens a dialog to choose an output file name, and starts generating Go problems.
- **Stop**: stops generating Go problems. If some Go problems have been already generated when the generating stops, they are written on an output file.

Note: The time remaining would be displayed incorrectly.

### Use a generated HTML file

- Each Go problem contains a couple of diagrams arranged horizontally.
- Each Go problem have a caption. The caption contains an ID and a couple of estimated win rates, but each percentage may have a large error.
- A diagram on the left shows the best move sequence estimated by Leela Zero. A diagram on the right shows the next move and the best move sequence estimated by Leela Zero on the next position.
- You can click on a diagram to start animating, and click again to show the first diagram.
- When the mouse cursor is on a diagram, you can use the scroll wheel to show the next or previous position.
- A generated file is printed in grayscale from a web browser. The size of printed diagrams depends on your browser.

### Edit generated HTML files

The following features are available from the HTML menu:

- **Merge**: merges Several HTML files into a single file. If the input files have duplicate Go problems, they are ignored.
- **Separate**: separates each of Go problems contained in an HTML file into its own files.
- **Separate by color**: separates Go problems, which black and white to play, which contained in an HTML file, into two files.

## Tips

Zero Problem is a Go problem generator, is *not* a Go game analyzer. It is probably a wrong idea that you use generated problems only for comparing the moves the AI recommended to the moves you played. Unless you solve Go problems repeatedly and learn how to make good move sequences, you will not improve your Go skills.

## Licence
You may use this software for free. The source code of Zero Problem is released under the Creative Commons CC Zero License 1.0, but this software requires wxWidgets, Leela Zero, Roboto which are released under specific licenses. See [license.html](../Licenses/license.html) for details.
