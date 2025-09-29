const ENDPOINTS = {
    DRAG_START: '/drag-start',
    DRAG_END: '/drag-end',
    RESET: '/reset',
};
const PIECES = {
    white: {
        king: '\u2654',
        queen: '\u2655',
        rook: '\u2656',
        bishop: '\u2657',
        knight: '\u2658',
        pawn: '\u2659',
    },
    black: {
        king: '\u265A',
        queen: '\u265B',
        rook: '\u265C',
        bishop: '\u265D',
        knight: '\u265E',
        pawn: '\u265F',
    },
};
const BACK_RANK = [
    'rook',
    'knight',
    'bishop',
    'queen',
    'king',
    'bishop',
    'knight',
    'rook',
];
const CHAR_TO_NAME = {
    k: 'king',
    q: 'queen',
    r: 'rook',
    b: 'bishop',
    n: 'knight',
    p: 'pawn',
};
function makePiece(side, name) {
    return { side, name, symbol: PIECES[side][name] };
}
function startingPosition() {
    const board = Array.from({ length: 8 }, () => Array(8).fill(null));
    board[0] = BACK_RANK.map((n) => makePiece('black', n));
    board[1] = Array.from({ length: 8 }, () => makePiece('black', 'pawn'));
    board[6] = Array.from({ length: 8 }, () => makePiece('white', 'pawn'));
    board[7] = BACK_RANK.map((n) => makePiece('white', n));
    return board;
}
function coordToIndices(coord) {
    const file = coord[0];
    const rank = parseInt(coord[1], 10);
    const f = file.charCodeAt(0) - 'a'.charCodeAt(0);
    const r = 8 - rank;
    return { r, f };
}
function indicesToCoord(r, f) {
    const fileChar = String.fromCharCode('a'.charCodeAt(0) + f);
    const rankChar = (8 - r).toString();
    return fileChar + rankChar;
}
function charToPiece(c) {
    if (!c || c === '_' || c === ' ')
        return null;
    const lower = c.toLowerCase();
    const name = CHAR_TO_NAME[lower];
    if (!name)
        return null;
    const side = c === lower ? 'white' : 'black';
    return makePiece(side, name);
}
export class ChessApp {
    constructor(rootId = 'board') {
        this.board = startingPosition();
        this.turn = 'white';
        this.dragFromSquare = null;
        this.possibleMoves = new Set();
        const el = document.getElementById(rootId);
        if (!el)
            throw new Error(`#${rootId} not found`);
        this.boardEl = el;
        this.renderBoard();
        this.resetServer().catch(console.error);
        this.updateDraggableForTurn();
    }
    // Rendering
    renderBoard() {
        this.boardEl.innerHTML = '';
        for (let r = 0; r < 8; r++) {
            for (let f = 0; f < 8; f++) {
                const squareEl = this.createSquareElement(r, f);
                const piece = this.board[r][f];
                if (piece) {
                    const pieceEl = this.createPieceElement(piece);
                    squareEl.appendChild(pieceEl);
                }
                this.boardEl.appendChild(squareEl);
            }
        }
    }
    createSquareElement(r, f) {
        const square = document.createElement('div');
        square.classList.add('square');
        const isLight = ((r + f) % 2) === 0;
        square.classList.add(isLight ? 'light' : 'dark');
        const coord = indicesToCoord(r, f);
        square.setAttribute('data-square', coord);
        square.addEventListener('dragenter', (ev) => this.onDragEnter(ev));
        square.addEventListener('dragover', (ev) => this.onDragOver(ev));
        square.addEventListener('drop', (ev) => this.onDrop(ev));
        return square;
    }
    createPieceElement(piece) {
        const span = document.createElement('span');
        span.classList.add('piece', piece.side);
        span.dataset.side = piece.side;
        span.innerHTML = piece.symbol;
        if (piece.side === this.turn)
            span.setAttribute('draggable', 'true');
        span.addEventListener('dragstart', (ev) => this.onDragStart(ev, span));
        span.addEventListener('dragend', () => this.onDragEnd(span));
        return span;
    }
    getSquareElement(coord) {
        return document.querySelector(`div[data-square="${CSS.escape(coord)}"]`);
    }
    updateSquareDOM(r, f, piece) {
        const coord = indicesToCoord(r, f);
        const squareEl = this.getSquareElement(coord);
        if (!squareEl)
            return;
        const existing = squareEl.querySelector('.piece');
        if (!piece) {
            existing === null || existing === void 0 ? void 0 : existing.remove();
            return;
        }
        if (!existing) {
            squareEl.appendChild(this.createPieceElement(piece));
            return;
        }
        existing.classList.toggle('white', piece.side === 'white');
        existing.classList.toggle('black', piece.side === 'black');
        existing.dataset.side = piece.side;
        existing.innerHTML = piece.symbol;
        if (piece.side === this.turn) {
            existing.setAttribute('draggable', 'true');
        }
        else {
            existing.removeAttribute('draggable');
        }
    }
    updateDraggableForTurn() {
        document.querySelectorAll('.piece').forEach((el) => {
            const side = el.dataset.side;
            if (side === this.turn) {
                el.setAttribute('draggable', 'true');
            }
            else {
                el.removeAttribute('draggable');
            }
        });
    }
    // Drag & Drop Handlers
    onDragStart(ev, span) {
        if (!span.hasAttribute('draggable'))
            return;
        const parentSquare = span.parentElement;
        if (!parentSquare)
            return;
        const from = parentSquare.getAttribute('data-square');
        if (!from || !ev.dataTransfer)
            return;
        this.dragFromSquare = parentSquare;
        ev.dataTransfer.setData('text/plain', from);
        ev.dataTransfer.effectAllowed = 'move';
        if (ev.dataTransfer.setDragImage) {
            ev.dataTransfer.setDragImage(span, 16, 16);
        }
        setTimeout(() => { span.style.opacity = '0.4'; }, 0);
        this.fetchPossibleMoves(from)
            .then((moves) => {
            this.possibleMoves = moves;
            this.showMoveHints();
        })
            .catch(console.error);
    }
    onDragEnd(span) {
        this.dragFromSquare = null;
        this.possibleMoves.clear();
        span.style.opacity = '';
        this.clearMoveHints();
    }
    onDragEnter(ev) {
        var _a, _b;
        const coord = (_b = (_a = ev.target) === null || _a === void 0 ? void 0 : _a.closest('.square')) === null || _b === void 0 ? void 0 : _b.getAttribute('data-square');
        if (coord && this.possibleMoves.has(coord))
            ev.preventDefault();
    }
    onDragOver(ev) {
        var _a, _b;
        const coord = (_b = (_a = ev.target) === null || _a === void 0 ? void 0 : _a.closest('.square')) === null || _b === void 0 ? void 0 : _b.getAttribute('data-square');
        if (coord && this.possibleMoves.has(coord)) {
            ev.preventDefault();
            if (ev.dataTransfer)
                ev.dataTransfer.dropEffect = 'move';
        }
    }
    onDrop(ev) {
        var _a, _b, _c, _d;
        const toSquareEl = (_a = ev.target) === null || _a === void 0 ? void 0 : _a.closest('.square');
        if (!toSquareEl || !ev.dataTransfer)
            return;
        const to = toSquareEl.getAttribute('data-square');
        const from = ev.dataTransfer.getData('text/plain');
        if (!to || !from || from === to)
            return;
        if (!this.possibleMoves.has(to))
            return;
        const { r: fr, f: ff } = coordToIndices(from);
        const { r: tr, f: tf } = coordToIndices(to);
        const movingPiece = this.board[fr][ff];
        if (!movingPiece)
            return;
        this.board[fr][ff] = null;
        this.board[tr][tf] = movingPiece;
        (_c = (_b = this.dragFromSquare) === null || _b === void 0 ? void 0 : _b.querySelector('.piece')) === null || _c === void 0 ? void 0 : _c.remove();
        (_d = toSquareEl.querySelector('.piece')) === null || _d === void 0 ? void 0 : _d.remove();
        toSquareEl.appendChild(this.createPieceElement(movingPiece));
        this.sendMove(from, to)
            .then((serverBoard) => this.syncBoardFromString(serverBoard))
            .catch(console.error);
        // playing against engine makes it always be whites turn
        // this.turn = this.turn === 'white' ? 'black' : 'white';
        this.updateDraggableForTurn();
        this.onDragEnd(toSquareEl.querySelector('.piece'));
    }
    // Move hints
    showMoveHints() {
        this.possibleMoves.forEach((coord) => {
            const el = this.getSquareElement(coord);
            el === null || el === void 0 ? void 0 : el.setAttribute('hint', 'true');
        });
    }
    clearMoveHints() {
        document.querySelectorAll('.square[hint="true"]').forEach((sq) => sq.removeAttribute('hint'));
    }
    // Server sync
    async resetServer() {
        const res = await fetch(ENDPOINTS.RESET);
        if (!res.ok)
            throw new Error(res.statusText);
    }
    async fetchPossibleMoves(from) {
        const res = await fetch(ENDPOINTS.DRAG_START, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: from,
        });
        if (!res.ok)
            throw new Error(res.statusText);
        const text = await res.text();
        return new Set(text.trim().length ? text.trim().split(/\s+/) : []);
    }
    async sendMove(from, to) {
        const res = await fetch(ENDPOINTS.DRAG_END, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: `${from}:${to}`,
        });
        if (!res.ok)
            throw new Error(res.statusText);
        return await res.text();
    }
    // Assumption: server string is 64 chars, ordered a1..h1, a2..h2, ..., a8..h8
    syncBoardFromString(boardStr) {
        if (!boardStr || boardStr.length < 64) {
            console.warn('Invalid server board string');
            return;
        }
        let i = 0;
        for (let r = 7; r >= 0; r--) {
            for (let f = 0; f < 8; f++) {
                const c = boardStr.charAt(i++);
                const serverSquare = charToPiece(c);
                const clientSquare = this.board[r][f];
                const needsUpdate = (!clientSquare && serverSquare) ||
                    (clientSquare && !serverSquare) ||
                    (clientSquare && serverSquare &&
                        (clientSquare.side !== serverSquare.side || clientSquare.name !== serverSquare.name));
                if (needsUpdate) {
                    this.board[r][f] = serverSquare;
                    this.updateSquareDOM(r, f, serverSquare);
                }
            }
        }
        this.updateDraggableForTurn();
    }
}
