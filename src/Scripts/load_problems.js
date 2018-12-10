(function () {
	'use strict';

	let problems;
	window['LoadProblems'] = function(pr) { problems = pr; };

	const kMode = 2;
	const kBoardSize = 19;
	const kFourWay = [ -(kBoardSize + 2), -1, 1, kBoardSize + 2 ];
	const eEmpty = 0;
	const eBlack = 1;
	const eWhite = 2;
	const eOffBoard = 4;
	const nsSvg = 'http://www.w3.org/2000/svg';

	const fonts = [];
	const boards = [];

	function append_canvas_buffer() {
		const canvas =  document.createElement('canvas');
		canvas.setAttribute('id', 'buffer');
		document.body.appendChild(canvas);
	}
	function append_div_info(parent, n) {
		const div = document.createElement('div');
		div.setAttribute('class', 'info');
		div.setAttribute('id', 'i' + n);
		parent.appendChild(div);
	}
	function append_canvas_view(parent, n) {
		const canvas =  document.createElement('canvas');
		canvas.setAttribute('class', 'view');
		canvas.setAttribute('id', 'c' + n);
		parent.appendChild(canvas);		
	}
	function append_div_two_canvases(n) {
		const div = document.createElement('div');
		div.setAttribute('class', 'two-canvases');
		append_div_info(div, n * 2);
		append_canvas_view(div, n * 2);
		append_canvas_view(div, n * 2 + 1);
		document.body.appendChild(div);
	}
	function append_svg_diagram(parent, n) {
		const svg = document.createElementNS(nsSvg, 'svg');
		svg.setAttribute('class', 'diagram');
		svg.setAttribute('id', 's' + n);
		svg.setAttribute('xmlns', nsSvg);
		svg.setAttribute('version', '1.1');
		svg.setAttribute('viewBox', '0 0 2280 3040');
		parent.appendChild(svg);
	}
	function append_page_three_svgs(parent, n) {
		const div = document.createElement('div');
		div.setAttribute('class', 'three-svgs');
		append_div_info(div, n * 2 + 1);
		append_svg_diagram(div, n * 3);
		append_svg_diagram(div, n * 3 + 1);
		append_svg_diagram(div, n * 3 + 2);
		parent.appendChild(div);
	}
	function append_div_page(n, len) {
		const div = document.createElement('div');
		div.setAttribute('class', 'page');
		if (n < len) {
			append_page_three_svgs(div, n);
		}
		if (n + 1 < len) {
			append_page_three_svgs(div, n + 1);
		}
		if (n + 2 < len) {
			append_page_three_svgs(div, n + 2);
		}
		document.body.appendChild(div);
	}
	function createBody() {
		append_canvas_buffer();
		const len = problems.length;
		for (let i = 0; i < len; ++i) {
			append_div_two_canvases(i);
		}
		for (let j = 0; j < len; j += 3) {
			append_div_page(j, len);
		}
	}
	function exists(sansSerif) {
		const px = '72px ';
		const canvas = document.getElementById('c0');
		const context = canvas.getContext('2d');
		const text = [ 'WWWW', '!!!!' ];
		for (let i = 0; i < 2; ++i) {
			context.font = px + sansSerif + ', monospace, serif';
			const w1 = context.measureText(text[i]);
			context.font = px + 'monospace, serif';
			const w2 = context.measureText(text[i]);
			if (w1.width != w2.width) {
				return true;
			}
		}
		return false;
	}
	function createFonts() {
		if (exists('Roboto') && exists('Roboto Condensed')) {
			fonts[0] = { name: 'Roboto', center: 728 / 2048, centerS: 541 / 2048 };
			fonts[1] = { name: 'Roboto Condensed', center: 728 / 2048, centerS: 541 / 2048, pl: 643 / 2048, pr: 652 / 2048, di: 1011 / 2048 };
		} else if (exists('Verdana') && exists('Tahoma')) {
			fonts[0] = { name: 'Verdana', center: 744 / 2048, centerS: 558.5 / 2048 };
			fonts[1] = { name: 'Tahoma', center: 744 / 2048, centerS: 558.5 / 2048, pl: 784 / 2048, pr: 784 / 2048, di: 1118 / 2048 };
		} else {
			fonts[0] = { name: 'sans-serif', center: 744 / 2048, centerS: 558.5 / 2048 };
			fonts[1] = { name: 'sans-serif', center: 744 / 2048, centerS: 558.5 / 2048, pl: 784 / 2048, pr: 784 / 2048, di: 1118 / 2048 };
		}
	}
	function opponentColor(color) {
		return color == eBlack ? eWhite : eBlack;
	}
	function toColorName(color) {
		return color === eBlack ? 'black' : 'white';
	}
	function toOpponentColorName(color) {
		return color === eBlack ? 'white' : 'black';
	}
	function coordinate(ix, iy) {
		return (iy + 1) * (kBoardSize + 2) + (ix + 1);
	}
	function toIx(coord) {
		return (coord % (kBoardSize + 2)) - 1;
	}
	function toIy(coord) {
		return Math.floor(coord / (kBoardSize + 2)) - 1;
	}
	function toNumberArray(s) {
		const numbers = [];
		const len = s.length;
		let c, d;
		for (let i = 0; i < len; ++i) {
			c = s.charCodeAt(i) - 64;
			d = c % 3;
			numbers.push(d);
			c = (c - d) / 3;
			d = c % 3;
			numbers.push(d);
			c = (c - d) / 3;
			d = c % 3;
			numbers.push(d);
		}
		return numbers;
	}
	function toStoneArray(numbers) {
		const stones = [];
		let i = 0;
		for (let x = 0; x < kBoardSize + 2; ++x) {
			for (let y = 0; y < kBoardSize + 2; ++y) {
				if (x === 0 || x === kBoardSize + 1 || y === 0 || y === kBoardSize + 1) {
					stones.push(eOffBoard);
				} else {
					stones.push(numbers[i]);
					++i;
				}
			}
		}
		return stones;
	}
	function toCoordinate(v, i) {
		if (v.charAt(i) === '-') {
			return null;
		}
		const x = v.charCodeAt(i) - 96;
		const y = v.charCodeAt(i + 1) - 96;
		return x + y * (kBoardSize + 2);
	}
	function toCoordinateArray(v) {
		const coords = [];
		const len = v.length;
		for (let i = 0; i < len; i += 2) {
			coords.push(toCoordinate(v, i));
		}
		return coords;
	}
	function expandProblem(n)
	{
		const p = problems[n];
		p['st'] = toStoneArray(toNumberArray(p['st']));
		p['lm'] = toCoordinate(p['lm'], 0);
		p['cl'] = p['cl'] === 'B' ? eBlack : eWhite;
		p['va'][0] = toCoordinateArray(p['va'][0]);
		p['va'][1] = toCoordinateArray(p['va'][1]);
	}
	function expandAllProblems() {
		const len = problems.length;
		for (let n = 0; n < len; ++n) {
			expandProblem(n);
		}
	}

	class Board {
		constructor(n, moveNumber, forView = true) {
			const p = problems[forView === true ? Math.floor(n / 2) : Math.floor(n / 3)];
			const s = p['st'];
			this.stones = new Int8Array(s.length);
			const len = s.length;
			for (let i = 0; i < len; ++i) {
				this.stones[i] = s[i];
			}
			this.labels = new Int16Array(this.stones.length);
			this.lostLabels = [];
			this.last = p['lm'];
			if (forView === true) {
				this.sequence = n % 2 === 0 ? p['va'][0] : p['va'][1];
			} else {
				const v1 = p['va'][0];
				const v2 = p['va'][1];
				if (n % 3 > 0) {
					this.sequence = (n + 1) % 2 === 0 ? v1 : v2;
				} else {
					this.sequence = [];
					if (v1.length > 0 && v1[0]) {
						this.labels[v1[0]] = -65; // A
					}
					if (v2.length > 0 && v2[0]) {
						this.labels[v2[0]] = -66; // B
					}
				}					
			}
			this.color = p['cl'];
			this.moveNumber = 0;
			const num = moveNumber < 0 ? this.sequence.length : moveNumber;
			for (let j = 0; j < num; ++j) {
				this.play();
			}
		}
		removeStones(deadStones) {
			const stones = this.stones;
			const len = deadStones.length;
			for (let i = 0; i < len; ++i) {
				stones[deadStones[i]] = eEmpty;
			}
		}
		removeDeadStones(coord)
		{
			const stones = this.stones;
			const color = this.color;
			if (stones[coord] !== color) {
				return;
			}
			const stack = [ coord ];
			const visited = new Int8Array(stones.length);
			const deadStones = [];
			while (stack.length > 0) {
				const i = stack.pop();
				visited[i] = 1;
				if (stones[i] === eEmpty) {
					return;
				} else if (stones[i] === color) {
					deadStones.push(i);
					for (const d of kFourWay) {
						const j = i + d;
						if (! visited[j]) {
							stack.push(j);
						}
					}
				}
			}
			this.removeStones(deadStones);
		}
		play() {
			const seq = this.sequence;
			const i = this.moveNumber;
			if (i >= seq.length) {
				return;
			}
			const coord = seq[i];
			++this.moveNumber;
			this.last = coord;
			if (coord === null || this.stones[coord] !== eEmpty) {
				this.color = opponentColor(this.color);
				return;
			}
			this.stones[coord] = this.color;
			this.color = opponentColor(this.color);
			for (const d of kFourWay) {
				this.removeDeadStones(coord + d);
			}
			const label = this.labels[coord];
			const moveNumber = this.moveNumber;
			this.labels[coord] = moveNumber;
			if (label) {
				const lostLabels = this.lostLabels;
				if (lostLabels[coord] === undefined) {
					lostLabels[coord] = [];
				}
				lostLabels[coord].push(label);
			}
		}
	}

	class View {
		constructor(n) {
			const buffer = document.getElementById('buffer');
			this.context = buffer.getContext('2d');
			this.board = boards[n];
			this.cw = buffer.width;
			this.ch = buffer.height;
			this.ew = Math.floor((this.cw / (kBoardSize + 0.5)) * 0.5 / 2);
			this.step = Math.floor((this.cw - this.ew * 2) / kBoardSize);
			this.w = this.step * kBoardSize + this.ew * 2;
			this.h = this.w;
			this.offset = this.ew + Math.floor((this.step - 1) / 2) + 0.5;
		}
		fillBoard() {
			const context = this.context;
			context.clearRect(0, 0, this.cw, this.ch);
			context.fillStyle = '#F9BF6D';
			context.fillRect(0, 0, this.w, this.h);
		}
		drawGrid() {
			const context = this.context;
			context.strokeStyle = 'black';
			context.lineWidth = this.w / 1000;
			const offset = this.offset;
			const step = this.step;
			const start = offset;
			const lineLength = step * (kBoardSize - 1);
			const end = offset + step * (kBoardSize - 1);
			for (let iy = 1; iy < kBoardSize - 1; ++iy) {
				const y = offset + step * iy;
				context.beginPath();
				context.moveTo(start, y);
				context.lineTo(end, y);
				context.stroke();
			}
			for (let ix = 1; ix < kBoardSize - 1; ++ix) {
				const x = offset + step * ix;
				context.beginPath();
				context.moveTo(x, start);
				context.lineTo(x, end);
				context.stroke();
			}
			context.beginPath();
			context.rect(start, start, lineLength, lineLength);
			context.stroke();
		}
		drawCircle(ix, iy, r, offset) {
			const context = this.context;
			const step = this.step;
			const x = offset + step * ix;
			const y = offset + step * iy;
			context.beginPath();
			context.arc(x, y, r, 0, Math.PI * 2, false);
			context.fill();
		}
		drawAllStoneShades() {
			this.context.fillStyle = '#7C5F36';
			const offset = this.offset + ((this.step / 2) - (this.step / 2.3));
			const stones = this.board.stones;
			const r = this.step / 2.1;
			for (let iy = 0; iy < kBoardSize; ++iy) {
				for (let ix = 0; ix < kBoardSize; ++ix) {
					const i = coordinate(ix, iy);
					if (stones[i] === eBlack || stones[i] === eWhite) {
						this.drawCircle(ix, iy, r, offset);
					}
				}
			}
		}
		drawStar(ix, iy) {
			const context = this.context;
			const step = this.step;
			const offset = this.offset;
			const x = offset + step * ix;
			const y = offset + step * iy;
			context.beginPath();
			context.arc(x, y, step / 12, 0, Math.PI * 2, false);
			context.fill();
		}
		drawAllStars() {
			const context = this.context;
			if (kBoardSize === 19) {
				context.fillStyle = 'black';
				for (let iy = 0; iy < 3; ++iy) {
					for (let ix = 0; ix < 3; ++ix) {
						this.drawStar(3 + 6 * ix, 3 + 6 * iy);
					}
				}
			}
		}
		drawStone(ix, iy, r, color) {
			const context = this.context;
			context.fillStyle = color;
			this.drawCircle(ix, iy, r, this.offset);
		}
		drawAllStones() {
			const stones = this.board.stones;
			const r = this.step / 2.1;
			for (let iy = 0; iy < kBoardSize; ++iy) {
				for (let ix = 0; ix < kBoardSize; ++ix) {
					const i = coordinate(ix, iy);
					if (stones[i] === eBlack) {
						this.drawStone(ix, iy, r, 'black');
					} else if (stones[i] === eWhite) {
						this.drawStone(ix, iy, r, 'white');
					}
				}
			}
		}
		drawLastMove() {
			if (this.board.moveNumber !== 0) {
				return;
			}
			const last = this.board.last;
			if (last !== null) {
				const ix = toIx(last);
				const iy = toIy(last);
				this.context.fillStyle = toColorName(this.board.color);
				this.drawCircle(ix, iy, this.step / 10, this.offset);
			}
		}
		drawLabel(coord, font, text) {
			const context = this.context;
			const px = this.step * 0.6;
			const x = this.offset + this.step * toIx(coord);
			const y = this.offset + this.step * toIy(coord) + px * font.center;
			context.font = px + 'px ' + font.name;
			context.fillStyle = toOpponentColorName(this.board.stones[coord]);
			context.textAlign = 'center';
			context.textBaseline = 'alphabetic';
			context.fillText(text, x, y);
		}
		drawAllLabels() {
			const labels = this.board.labels;
			const stones = this.board.stones;
			const len = labels.length;
			for (let i = 0; i < len; ++i) {
				if (labels[i] && stones[i] !== eEmpty) {
					const s = String(labels[i]);
					this.drawLabel(i, s.length <= 1 ? fonts[0] : fonts[1], s);
				}
			}
		}
		drawBoard() {
			this.fillBoard();
			this.drawAllStoneShades();
			this.drawGrid();
			this.drawAllStars();
			this.drawAllStones();
			this.drawLastMove();
			this.drawAllLabels();
		}
	}

	class Diagram {
		constructor(n) {
			this.element = document.getElementById('s' + n);
			this.board = new Board(n, -1, false);
			this.w = 2280;// ==40*19*3
			this.h = 3040;// ==40*19*4
			this.step = this.w / kBoardSize;
			this.offset = this.step / 2;
			this.lw = this.step / 40;
			this.llx = 0.5;
			this.lly = kBoardSize + 0.4;
		}
		drawRect(x, y, width, height, w) {
			const rect = document.createElementNS(nsSvg, 'rect');
			rect.setAttribute('x', x);
			rect.setAttribute('y', y);
			rect.setAttribute('width', width);
			rect.setAttribute('height', height);
			rect.setAttribute('fill', 'none');
			rect.setAttribute('stroke', 'black');
			rect.setAttribute('stroke-width', w);
			this.element.appendChild(rect);
		}
		drawLine(x1, y1, x2, y2, w) {
			const line = document.createElementNS(nsSvg, 'line');
			line.setAttribute('x1', x1);
			line.setAttribute('y1', y1);
			line.setAttribute('x2', x2);
			line.setAttribute('y2', y2);
			line.setAttribute('stroke', 'black');
			line.setAttribute('stroke-width', w);
			line.setAttribute('stroke-linecap', 'butt');
			this.element.appendChild(line);
		}
		drawCircle(x, y, r, w, color) {
			const circle = document.createElementNS(nsSvg, 'circle');
			circle.setAttribute('cx', x);
			circle.setAttribute('cy', y);
			circle.setAttribute('r', r);
			circle.setAttribute('fill', color);
			circle.setAttribute('stroke', 'black');
			circle.setAttribute('stroke-width', w);
			this.element.appendChild(circle);
		}
		drawGrid() {
			const offset = this.offset;
			const step = this.step;
			const lw = this.lw;
			const start = offset;
			const end = offset + step * (kBoardSize - 1);
			for (let iy = 1; iy < kBoardSize - 1; ++iy) {
				const y = offset + step * iy;
				this.drawLine(start, y, end, y, lw);
			}
			for (let ix = 1; ix < kBoardSize - 1; ++ix) {
				const x = offset + step * ix;
				this.drawLine(x, start, x, end, lw);
			}
			this.drawRect(start, start, end - offset, end - offset, lw * 3);
		}
		drawStar(ix, iy) {
			const offset = this.offset;
			const step = this.step;
			const x = offset + step * ix;
			const y = offset + step * iy;
			const lw = this.lw;
			const r = lw * 4;
			this.drawCircle(x, y, r, lw, 'black');
		}
		drawAllStars() {
			if (kBoardSize === 19) {
				for (let iy = 0; iy < 3; ++iy) {
					for (let ix = 0; ix < 3; ++ix) {
						this.drawStar(3 + 6 * ix, 3 + 6 * iy);
					}
				}
			}
		}
		drawSpace(x, y, width, height) {
			const rect = document.createElementNS(nsSvg, 'rect');
			rect.setAttribute('x', x);
			rect.setAttribute('y', y);
			rect.setAttribute('width', width);
			rect.setAttribute('height', height);
			rect.setAttribute('fill', 'white');
			this.element.appendChild(rect);	
		}
		drawStone(ix, iy, color) {
			const offset = this.offset;
			const step = this.step;
			const x = offset + step * ix;
			const y = offset + step * iy;
			const r = step / 2 - this.lw;
			this.drawCircle(x, y, r, this.lw * 1.5, color);
		}
		drawAllStones() {
			const stones = this.board.stones;
			const d = this.lw * 3;
			const d2 = d * 2;
			const offset = this.offset;
			const step = this.step;
			for (let iy = 0; iy < kBoardSize; ++iy) {
				for (let ix = 0; ix < kBoardSize; ++ix) {
					const i = coordinate(ix, iy);
					const s = stones[i];
					if (s === eBlack || s === eWhite) {
						const sr = stones[i + 1];
						if (sr === eBlack || sr === eWhite) {
							const x = offset + step * (ix + 0.5) - d;
							const y = offset + step * iy - d;
							this.drawSpace(x, y, d2, d2);
						}
						const sb = stones[i + kBoardSize + 2];
						if (sb === eBlack || sb === eWhite) {
							const x = offset + step * ix - d;
							const y = offset + step * (iy + 0.5) - d;
							this.drawSpace(x, y, d2, d2);
						}
						this.drawStone(ix, iy, toColorName(s));
					}
				}
			}
		}
		drawTriangle(ix, iy, r, lw, color) {
			const offset = this.offset;
			const step = this.step;
			const cx = offset + step * ix;
			const cy = offset + step * iy;
			const x1 = cx - r * Math.sqrt(3) / 2;
			const x2 = cx;
			const x3 = cx + r * Math.sqrt(3) / 2;
			const y1 = cy + r / 2;
			const y2 = cy - r;
			const y3 = cy + r / 2;
			const d = 'M' + x1 + ' ' + y1 + ' L ' + x2 + ' ' + y2 + ' L ' + x3 + ' ' + y3 + ' Z';
			const path = document.createElementNS(nsSvg, 'path');
			path.setAttribute('d', d);
			path.setAttribute('fill', 'none');
			path.setAttribute('stroke', color);
			path.setAttribute('stroke-width', lw);
			this.element.appendChild(path);
		}
		drawLastMove() {
			if (this.board.moveNumber !== 0) {
				return;
			}
			const last = this.board.last;
			if (last !== null) {
				const ix = toIx(last);
				const iy = toIy(last);
				const lw = this.lw * 2;
				const r = (this.step / 2 - this.lw / 2) - lw;
				this.drawTriangle(ix, iy, r, lw, toColorName(this.board.color));
			}
		}
		drawText(x, y, fn, fs, color, text) {
			const t = document.createElementNS(nsSvg, 'text');
			if (text.length > 2) {
				t.setAttribute('x', 0);
				t.setAttribute('y', 0);
				t.setAttribute('transform', 'translate(' + x + ',' + y + ')scale(0.7,1)');
			} else {
				t.setAttribute('x', x);
				t.setAttribute('y', y);
			}
			t.setAttribute('font-family', fn);
			t.setAttribute('font-weight', 'bold');
			t.setAttribute('font-size', fs);
			t.setAttribute('text-anchor', 'middle');
			t.setAttribute('stroke', 'none');
			t.setAttribute('fill', color);
			t.textContent = text;
			this.element.appendChild(t);
		}
		drawLabel(ix, iy, font, color, text) {
			const fn = font.name;
			const cc = text.charCodeAt(0);
			const isNumber = cc >= 48 && cc < 58;
			const isLower = cc >= 97 && cc < 123;
			const fs = this.step * (isNumber ? 0.7 : 0.9);
			const dx = isNumber ? -this.lw / 2 : 0;
			const offset = this.offset;
			const step = this.step;
			const x = step * ix;
			const y = step * iy;
			const fx = offset + x + dx;
			const fy = offset + y + fs * (isLower ? font.centerS : font.center);
			if (! isNumber) {
				this.drawSpace(x, y, step, step);
			}
			this.drawText(fx, fy, fn, fs, color, text);
		}
		drawAllLabels() {
			const labels = this.board.labels;
			const len = labels.length;
			for (let i = 0; i < len; ++i) {
				const l = labels[i];
				if (l) {
					let s;
					if (l > 0) {
						s = String(l);
					} else  {
						s = kMode === 2 ? 'Z' : String.fromCharCode(-l);
					}
					const color = toOpponentColorName(this.board.stones[i]);
					this.drawLabel(toIx(i), toIy(i), s.length <= 1 ? fonts[0] : fonts[1], color, s);
				}
			}
		}
		drawLabeledStone(number, color) {
			let ix = this.llx;
			let iy = this.lly;
			const s = String(number);
			if (ix > kBoardSize - 1.5) {
				ix = 0.5;
				iy += kBoardSize === 9 ? 1 : 1.2;
				this.llx = ix;
				this.lly = iy;
			}
			if (iy > kBoardSize * 1.33 - 1.5) {
				return;
			}
			this.drawStone(ix, iy, toColorName(color));
			this.drawLabel(ix, iy, s.length <= 1 ? fonts[0] : fonts[1], toOpponentColorName(color), s);
			this.llx += 1.1;
		}
		getComputedTextLength(font, text) {
			let em = 0;
			const len = text.length;
			for (let i = 0; i < len; ++i) {
				const c = text.charAt(i);
				if (c === '(') {
					em += font.pr;
				} else if (c === ')') {
					em += font.pl;
				} else {
					em += font.di;
				}
			}
			return em;
		}
		drawCurrentLabel(text) {
			let ix = this.llx;
			let iy = this.lly;
			const s = '(' + text + ')';
			const offset = this.offset;
			const font = fonts[1];
			const step = this.step;
			const fs = step - this.lw * 2;
			const dx = -this.lw / 2;
			const dy = offset + fs * font.center;
			const t = document.createElementNS(nsSvg,'text');
			const iw = this.getComputedTextLength(font, s) * fs / step;
			if (ix > kBoardSize - iw - 0.5) {
				ix = 0.5;
				iy += kBoardSize === 9 ? 1 : 1.2;
				this.llx = ix;
				this.lly = iy;
			}
			if (iy > kBoardSize * 1.33 - 1.5) {
				return;
			}
			t.setAttribute('x', ix * step + dx);
			t.setAttribute('y', iy * step + dy);
			t.setAttribute('font-family', font.name);
			t.setAttribute('font-size', fs);
			t.setAttribute('stroke', 'none');
			t.setAttribute('fill', 'black');
			t.textContent = s;
			this.element.appendChild(t);
			this.llx += iw + 0.4;
		}
		drawLostLabels(labels) {
			const c = this.board.color;
			const color = this.board.moveNumber % 2 ? [c, opponentColor(c)] : [opponentColor(c), c];
			const len = labels.length;
			for (let i = 0; i < len - 1; ++i) {
				const number = labels[i];
				this.drawLabeledStone(number, color[number % 2]);
			}
			this.drawCurrentLabel(labels[len - 1]);
		}
		drawAllLostLabels() {
			const labels = this.board.labels;
			const lostLabels = this.board.lostLabels;
			const sortedLabels = [];
			const len = lostLabels.length;
			for (let i = 0; i < len; ++i) {
				const ll = lostLabels[i];
				if (ll !== undefined && ll.length > 0) {
					sortedLabels[ll[0]] = ll.concat(labels[i]);
				}
			}
			for (const sl of sortedLabels) {
				if (sl !== undefined) {
					this.drawLostLabels(sl);
				}
			}
		}
		drawBoard() {
			this.drawGrid();
			this.drawAllStars();
			this.drawAllStones();
			this.drawLastMove();
			this.drawAllLabels();
			this.drawAllLostLabels();
		}
	}

	function createInfo(n) {
		const halfN = Math.floor(n / 2);
		const p = problems[halfN];
		const text = '#' + (halfN + 1) + '  ' + p['wr'][0] + '%/' + p['wr'][1] + '%';
		const textNode = document.createTextNode(text);
		const info = document.getElementById('i' + n);
		info.appendChild(textNode);
	}
	function createAllInfos() {
		const len = problems.length * 2;
		for (let n = 0; n < len; ++n) {
			createInfo(n);
		}
	}
	function resizeBuffer() {
		const buffer = document.getElementById('buffer');
		const canvas = document.getElementById('c0');
		const dpr = window.devicePixelRatio;
		let w = canvas.clientWidth;
		let h = canvas.clientHeight;
		if (dpr > 1) {
			w = Math.floor(w * dpr);
			h = Math.floor(h * dpr);
		}
		if (buffer.width != w || buffer.height != h) {
			buffer.width = w;
			buffer.height = h;
		}
	}
	function resizeCanvas(n) {
		const buffer = document.getElementById('buffer');
		const canvas = document.getElementById('c' + n);
		if (canvas.width != buffer.width || canvas.height != buffer.height) {
			canvas.width = buffer.width;
			canvas.height = buffer.height;
		}
	}
	function drawBoard(n) {
		resizeCanvas(n);
		const view = new View(n);
		view.drawBoard();
		const canvas = document.getElementById('c' + n);
		const context = canvas.getContext('2d');
		const buffer = document.getElementById('buffer');
		context.drawImage(buffer, 0, 0);
	}
	function drawAllBoards() {
		resizeBuffer();
		const len = problems.length * 2;
		for (let n = 0; n < len; ++n) {
			drawBoard(n);
		}
	}
	function createAllBoards() {
		expandAllProblems();
		const len = problems.length * 2;
		for (let n = 0; n < len; ++n) {
			boards.push(new Board(n, 0));
		}
		drawAllBoards();
	}
	function createSvg(n) {
		const diagram = new Diagram(n);
		diagram.drawBoard();
	}
	function createAllSvgs() {
		const len = problems.length * 3;
		for (let n = 0; n < len; ++n) {
			createSvg(n);
		}
	}

	function goForward(n) {
		const b = boards[n];
		if (b.moveNumber < b.sequence.length) {
			boards[n] = new Board(n, b.moveNumber + 1);
			return true;
		}
		return false;
	}
	function goBackward(n) {
		const b = boards[n];
		if (b.moveNumber > 0) {
			boards[n] = new Board(n, b.moveNumber - 1);
			return true;
		}
		return false;
	}
	function addWheelEventListener(n) {
		const canvas = document.getElementById('c' + n);
		canvas.addEventListener('wheel', function(e) {
			const canvas = document.getElementById('c' + n);
			const view = new View(n);
			if (e.offsetX) {
				const w = (view.w / view.cw) * canvas.clientWidth;
				const margin = canvas.clientWidth / 40;
				if (e.offsetX < margin || e.offsetX > w - margin) {
					return;
				}
			}
			if (e.deltaY >= 0) {
				goForward(n);
			} else {
				goBackward(n);
			}
			drawBoard(n);
			e.preventDefault();
		});
	}
	function addAllWheelEventListeners() {
		const len = problems.length * 2;
		for (let n = 0; n < len; ++n) {
			addWheelEventListener(n);
		}
	}
	const requestIds = [];
	const startTimes = [];
	function addClickEventListener(n) {
		const canvas = document.getElementById('c' + n);
		function animate(n, now) {
			const elapsedTime = startTimes[n] === undefined ? 10000 : now - startTimes[n];
			if (elapsedTime > 82) {
				startTimes[n] = now;
				if (goForward(n)) {
					requestIds[n] = window.requestAnimationFrame(function(time) { animate(n, time); });
					drawBoard(n);
				} else {
					requestIds[n] = undefined;
				}
			} else {
				requestIds[n] = window.requestAnimationFrame(function(time) { animate(n, time); });
			}
		}
		canvas.addEventListener('click', function() {
			if (boards[n].moveNumber > 0) {
				window.cancelAnimationFrame(requestIds[n]);
				boards[n] = new Board(n, 0);
				drawBoard(n);
			} else {
				animate(n, 0);
			}
		});
	}
	function addAllClickEventListeners() {
		const len = problems.length * 2;
		for (let n = 0; n < len; ++n) {
			addClickEventListener(n);
		}
	}
	function addResizeEventListener() {
		let resizeTimerId = undefined;
		window.addEventListener('resize', function() {
			clearTimeout(resizeTimerId);
			resizeTimerId = setTimeout(function() {
				drawAllBoards();
			}, 300);
		});
	}

	window.addEventListener('DOMContentLoaded', function() {
		createBody();
		createFonts();
		createAllInfos();
		createAllBoards();
		createAllSvgs();
		addAllWheelEventListeners();
		addAllClickEventListeners();
		addResizeEventListener();
	});

})();

