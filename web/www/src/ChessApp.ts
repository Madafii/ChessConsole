type Side = 'white' | 'black';
type PieceName = 'king' | 'queen' | 'rook' | 'bishop' | 'knight' | 'pawn';

type Piece = { side: Side; name: PieceName; symbol: string };
type Square = Piece | null;
type Board = Square[][];

const ENDPOINTS = {
    DRAG_START: '/drag-start',
    DRAG_END: '/drag-end',
    RESET: '/reset',
};

const PIECES: Record<Side, Record<PieceName, string>> = {
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

const BACK_RANK: ReadonlyArray<PieceName> = [
    'rook',
    'knight',
    'bishop',
    'queen',
    'king',
    'bishop',
    'knight',
    'rook',
];

const CHAR_TO_NAME: Record<string, PieceName> = {
    k: 'king',
    q: 'queen',
    r: 'rook',
    b: 'bishop',
    n: 'knight',
    p: 'pawn',
};

function makePiece(side: Side, name: PieceName): Piece {
    return { side, name, symbol: PIECES[side][name] };
}

function startingPosition(): Board {
    const board: Board = Array.from({ length: 8 }, () => Array<Square>(8).fill(null));
    board[0] = BACK_RANK.map((n) => makePiece('black', n));
    board[1] = Array.from({ length: 8 }, () => makePiece('black', 'pawn'));
    board[6] = Array.from({ length: 8 }, () => makePiece('white', 'pawn'));
    board[7] = BACK_RANK.map((n) => makePiece('white', n));
    return board;
}

function coordToIndices(coord: string): { r: number; f: number } {
    const file = coord[0];
    const rank = parseInt(coord[1], 10);
    const f = file.charCodeAt(0) - 'a'.charCodeAt(0);
    const r = 8 - rank;
    return { r, f };
}

function indicesToCoord(r: number, f: number): string {
    const fileChar = String.fromCharCode('a'.charCodeAt(0) + f);
    const rankChar = (8 - r).toString();
    return fileChar + rankChar;
}

function charToPiece(c: string): Square {
    if (!c || c === '_' || c === ' ') return null;
    const lower = c.toLowerCase();
    const name = CHAR_TO_NAME[lower];
    if (!name) return null;
    const side: Side = c === lower ? 'white' : 'black';
    return makePiece(side, name);
}

export class ChessApp {
    private boardEl: HTMLElement;
    private board: Board = startingPosition();
    private turn: Side = 'white';
    private dragFromSquare: HTMLDivElement | null = null;
    private possibleMoves: Set<string> = new Set();

    constructor(rootId = 'board') {
        const el = document.getElementById(rootId);
        if (!el) throw new Error(`#${rootId} not found`);
        this.boardEl = el;

        this.renderBoard();
        this.resetServer().catch(console.error);
        this.updateDraggableForTurn();
    }

    // Rendering

    private renderBoard(): void {
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

    private createSquareElement(r: number, f: number): HTMLDivElement {
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

    private createPieceElement(piece: Piece): HTMLSpanElement {
        const span = document.createElement('span');
        span.classList.add('piece', piece.side);
        span.dataset.side = piece.side;
        span.innerHTML = piece.symbol;

        if (piece.side === this.turn) span.setAttribute('draggable', 'true');

        span.addEventListener('dragstart', (ev: DragEvent) => this.onDragStart(ev, span));
        span.addEventListener('dragend', () => this.onDragEnd(span));

        return span;
    }

    private getSquareElement(coord: string): HTMLDivElement | null {
        return document.querySelector<HTMLDivElement>(`div[data-square="${CSS.escape(coord)}"]`);
    }

    private updateSquareDOM(r: number, f: number, piece: Square): void {
        const coord = indicesToCoord(r, f);
        const squareEl = this.getSquareElement(coord);
        if (!squareEl) return;

        const existing = squareEl.querySelector<HTMLSpanElement>('.piece');
        if (!piece) {
            existing?.remove();
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
        } else {
            existing.removeAttribute('draggable');
        }
    }

    private updateDraggableForTurn(): void {
        document.querySelectorAll<HTMLSpanElement>('.piece').forEach((el) => {
            const side = el.dataset.side as Side | undefined;
            if (side === this.turn) {
                el.setAttribute('draggable', 'true');
            } else {
                el.removeAttribute('draggable');
            }
        });
    }

    // Drag & Drop Handlers

    private onDragStart(ev: DragEvent, span: HTMLSpanElement): void {
        if (!span.hasAttribute('draggable')) return;

        const parentSquare = span.parentElement as HTMLDivElement | null;
        if (!parentSquare) return;

        const from = parentSquare.getAttribute('data-square');
        if (!from || !ev.dataTransfer) return;

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

    private onDragEnd(span: HTMLSpanElement): void {
        this.dragFromSquare = null;
        this.possibleMoves.clear();
        span.style.opacity = '';
        this.clearMoveHints();
    }

    private onDragEnter(ev: DragEvent): void {
        const coord = (ev.target as HTMLElement)?.closest('.square')?.getAttribute('data-square');
        if (coord && this.possibleMoves.has(coord)) ev.preventDefault();
    }

    private onDragOver(ev: DragEvent): void {
        const coord = (ev.target as HTMLElement)?.closest('.square')?.getAttribute('data-square');
        if (coord && this.possibleMoves.has(coord)) {
            ev.preventDefault();
            if (ev.dataTransfer) ev.dataTransfer.dropEffect = 'move';
        }
    }

    private onDrop(ev: DragEvent): void {
        const toSquareEl = (ev.target as HTMLElement)?.closest('.square') as HTMLDivElement | null;
        if (!toSquareEl || !ev.dataTransfer) return;

        const to = toSquareEl.getAttribute('data-square');
        const from = ev.dataTransfer.getData('text/plain');
        if (!to || !from || from === to) return;
        if (!this.possibleMoves.has(to)) return;

        const { r: fr, f: ff } = coordToIndices(from);
        const { r: tr, f: tf } = coordToIndices(to);
        const movingPiece = this.board[fr][ff];
        if (!movingPiece) return;

        this.board[fr][ff] = null;
        this.board[tr][tf] = movingPiece;

        this.dragFromSquare?.querySelector('.piece')?.remove();
        toSquareEl.querySelector('.piece')?.remove();
        toSquareEl.appendChild(this.createPieceElement(movingPiece));

        this.sendMove(from, to)
            .then((serverBoard) => this.syncBoardFromString(serverBoard))
            .catch(console.error);

        // playing against engine makes it always be whites turn
        // this.turn = this.turn === 'white' ? 'black' : 'white';
        this.updateDraggableForTurn();

        this.onDragEnd(toSquareEl.querySelector('.piece') as HTMLSpanElement);
    }

    // Move hints

    private showMoveHints(): void {
        this.possibleMoves.forEach((coord) => {
            const el = this.getSquareElement(coord);
            el?.setAttribute('hint', 'true');
        });
    }

    private clearMoveHints(): void {
        document.querySelectorAll<HTMLDivElement>('.square[hint="true"]').forEach((sq) => sq.removeAttribute('hint'));
    }

    // Server sync

    private async resetServer(): Promise<void> {
        const res = await fetch(ENDPOINTS.RESET);
        if (!res.ok) throw new Error(res.statusText);
    }

    private async fetchPossibleMoves(from: string): Promise<Set<string>> {
        const res = await fetch(ENDPOINTS.DRAG_START, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: from,
        });
        if (!res.ok) throw new Error(res.statusText);
        const text = await res.text();
        return new Set(text.trim().length ? text.trim().split(/\s+/) : []);
    }

    private async sendMove(from: string, to: string): Promise<string> {
        const res = await fetch(ENDPOINTS.DRAG_END, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: `${from}:${to}`,
        });
        if (!res.ok) throw new Error(res.statusText);
        return await res.text();
    }

    // Assumption: server string is 64 chars, ordered a1..h1, a2..h2, ..., a8..h8
    private syncBoardFromString(boardStr: string): void {
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

                const needsUpdate =
                    (!clientSquare && serverSquare) ||
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
