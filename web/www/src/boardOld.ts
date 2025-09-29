/// build an 8x8 chessboard and place standard starting pieces.
// Pieces can be dragged to another square (HTML5 drag & drop).

type Side = 'white' | 'black';
type PieceName = 'king' | 'queen' | 'rook' | 'bishop' | 'knight' | 'pawn';

type Square = { side: Side; name: PieceName; symbol: string } | null;
type Pieces = Record<Side, Record<PieceName, string>>;

// Unicode chess pieces
const PIECES: Pieces = {
    white: {
        // king: '<img src="./pics/king.png">',
        // queen: '<img src="./pics/queen.png">',
        // rook: '<img src="./pics/rook.png">',
        // bishop: '<img src="./pics/bishop.png">',
        // knight: '<img src="./pics/knight.png">',
        // pawn: '<img src="./pics/pawn.png">',
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

const SVR_PIECES: Record<string, string> = {
    'k': PIECES.white.king,
    'q': PIECES.white.queen,
    'r': PIECES.white.rook,
    'b': PIECES.white.bishop,
    'n': PIECES.white.knight,
    'p': PIECES.white.pawn,
    'K': PIECES.black.king,
    'Q': PIECES.black.queen,
    'R': PIECES.black.rook,
    'B': PIECES.black.bishop,
    'N': PIECES.black.knight,
    'P': PIECES.black.pawn,
};

const makePiece = (side: Side, name: PieceName): Square => ({
    side,
    name,
    symbol: PIECES[side][name],
});

const BACK_RANK: PieceName[] = [
    'rook',
    'knight',
    'bishop',
    'queen',
    'king',
    'bishop',
    'knight',
    'rook',
];

function startingPosition(): Square[][] {
    const board: Square[][] = new Array(8).fill(null).map(() => new Array<null | Square>(8).fill(null));

    // rank 8 (index 0): black back rank
    board[0] = BACK_RANK.map((n) => makePiece('black', n));

    // rank 7 (index 1): black pawns
    board[1] = new Array(8).fill(null).map(() => makePiece('black', 'pawn'));

    // ranks 6-3 (indexes 2..5): empty already

    // rank 2 (index 6): white pawns
    board[6] = new Array(8).fill(null).map(() => makePiece('white', 'pawn'));

    // rank 1 (index 7): white back rank
    board[7] = BACK_RANK.map((n) => makePiece('white', n));

    return board;
}

function coordToIndices(coord: string) {
    const rank = parseInt(coord[1], 10);
    const f = coord[0].charCodeAt(0) - 'a'.charCodeAt(0);
    const r = 8 - rank;
    return { r, f };
}

let white: boolean = true;
let dragFrom: HTMLElement | null = null;
let possMoves: Set<string> = new Set<string>;
let Board: Square[][] = startingPosition();

function dropHandler(ev: DragEvent) {
    const toSquare = (ev.target as HTMLElement).closest('.square');
    if (!toSquare) return;
    const dataSquare = toSquare.getAttribute('data-square');
    if (!dataSquare || !possMoves.has(dataSquare)) return;

    if (!ev.dataTransfer) return;
    const from = ev.dataTransfer.getData('text/plain');
    const to = toSquare.getAttribute('data-square')?.toString();
    if (!to || from === to) return;

    const { r: fr, f: ff } = coordToIndices(from);
    const { r: tr, f: tf } = coordToIndices(to);

    const movingPiece = Board[fr][ff];
    if (!movingPiece) return;

    Board[fr][ff] = null;
    Board[tr][tf] = movingPiece;

    // remove piece span at from position
    dragFrom?.querySelector('.piece')?.remove();

    const existingPiece = toSquare.querySelector('.piece');
    if (existingPiece) existingPiece.remove();

    // Create and append moved piece
    const newSpan = document.createElement('span');
    newSpan.classList.add('piece', movingPiece.side);
    newSpan.innerHTML = movingPiece.symbol;
    addPieceDragHandlers(newSpan);
    toSquare.appendChild(newSpan);

    // make the move on server
    fetch('/drag-end', {
        method: 'POST',
        headers: { 'Content-Type': 'text/plain' },
        body: from + ":" + to,
    }).then(res => {
        if (!res.ok) throw new Error(res.statusText);
        return res.text();
    }).then(text => {
        if (!text) {
            console.log('no res text');
        }
        console.log(text);
        syncBoard(text);
    }).catch(err => {
        console.log(err);
    })


    white = !white;
    document.querySelectorAll('.piece').forEach(piece => {
        const whiteStr: string = white ? 'white' : 'black';
        if (piece.classList[1] === whiteStr) {
            piece.setAttribute('draggable', 'true');
        } else {
            piece.removeAttribute('draggable');
        }
    });
}

function dragOverHandler(ev: DragEvent) {
    const dataSquare = (ev.target as HTMLElement).closest('.square')?.getAttribute('data-square');
    if (dataSquare && possMoves.has(dataSquare)) {
        ev.preventDefault();
        ev.dataTransfer!.dropEffect = 'move';
    }
}

function dragEnterHandler(ev: DragEvent) {
    const dataSquare = (ev.target as HTMLElement).closest('.square')?.getAttribute('data-square');
    if (dataSquare && possMoves.has(dataSquare)) ev.preventDefault();
}

function dragStartHandler(ev: DragEvent, span: HTMLSpanElement) {
    if (!span.hasAttribute('draggable')) return;
    const parentSquare = span.parentElement;
    if (!parentSquare) return;
    const from = parentSquare.getAttribute('data-square');
    if (!from) return;

    dragFrom = parentSquare;

    // set data of dragged piece
    if (ev.dataTransfer) {
        ev.dataTransfer.setData('text/plain', from);
        ev.dataTransfer.effectAllowed = 'move';
        // Optional: set a drag image
        if (ev.dataTransfer.setDragImage) {
            // small offset so glyph is visible under cursor
            ev.dataTransfer.setDragImage(span, 16, 16);
        }
    }

    // briefly hide original while dragging (optional)
    setTimeout(() => span.style.opacity = '0.4', 0);

    fetch('/drag-start', {
        method: 'POST',
        headers: { 'Content-Type': 'text/plain' },
        body: from
    }).then(res => {
        if (!res.ok) throw new Error(res.statusText);
        return res.text();
    }).then(text => {
        possMoves = new Set(text.split(' '));
        possMoves.forEach(possMove => {
            const possSquare = document.querySelector<HTMLDivElement>(`div[data-square="${CSS.escape(possMove)}"]`);
            possSquare?.setAttribute('hint', 'true');
        });
    }).catch(console.error);
}

function dragEndHandler(span: HTMLSpanElement) {
    dragFrom = null;
    possMoves.clear();
    span.style.opacity = '';

    document.querySelectorAll<HTMLDivElement>('.square[hint="true"]').forEach(sq => sq.removeAttribute('hint'));
}

function addPieceDragHandlers(span: HTMLSpanElement) {
    span.addEventListener('dragstart', (ev: DragEvent) => { dragStartHandler(ev, span) });
    span.addEventListener('dragend', () => { dragEndHandler(span) });
}

function addSquareDragHandlers(square: HTMLDivElement) {
    square.addEventListener('dragenter', (ev) => { dragEnterHandler(ev) });
    square.addEventListener('dragover', (ev) => { dragOverHandler(ev) });
    square.addEventListener('drop', (ev: DragEvent) => dropHandler(ev));
}

function createBoard(): void {
    const boardContainer = document.getElementById('board');
    if (!boardContainer) return;

    // Clear existing content
    boardContainer.innerHTML = '';

    for (let r = 0; r < 8; r++) {
        for (let f = 0; f < 8; f++) {
            const square = document.createElement('div');
            square.classList.add('square');

            // color square: (rank + file) even => light, odd => dark (with row 0 = rank 8)
            const isLight = ((r + f) % 2) === 0;
            square.classList.add(isLight ? 'light' : 'dark');

            // algebraic coordinate, e.g., a8 ... h1
            const fileChar = String.fromCharCode('a'.charCodeAt(0) + f);
            const rankChar = (8 - r).toString();
            const coord = fileChar + rankChar;
            square.setAttribute('data-square', coord);

            const piece = Board[r][f];
            if (piece) {
                const span = document.createElement('span');
                span.classList.add('piece');
                span.classList.add(piece.side);
                span.innerHTML = piece.symbol;
                if (piece.side === "white") {
                    span.setAttribute('draggable', 'true');
                }
                addPieceDragHandlers(span);
                square.appendChild(span);
            }
            addSquareDragHandlers(square);
            boardContainer.appendChild(square);
        }
    }
}

// sync js board with the one from server
function syncBoard(board: string) {
    // server and client board are reversed
    let i = 0;
    for (let r = 7; r > 0; r--) {
        let file = Board[r];
        for (let f = 0; f < 8; f++) {
            const piece = file[f];
            const boardChar: string = board.charAt(i);
            const charCode: number = boardChar.charCodeAt(0);
            // lowercase is a white piece
            const pieceSide: boolean = charCode >= 'a'.charCodeAt(0) && charCode <= 'z'.charCodeAt(0);
            const position = String.fromCharCode('a'.charCodeAt(0) + f) + (8 - r).toString();
            const selector = `div[data-square="${CSS.escape(position)}"]`
            const square = document.querySelector<HTMLDivElement>(selector);
            const span = square?.querySelector('.piece');
            if (!piece && boardChar != '_') {
                // server has piece but client does not
                if (!span) {
                    const span = document.createElement('span');
                    span.classList.add('piece');
                    span.classList.add(pieceSide ? 'white' : 'black');
                    span.innerHTML = SVR_PIECES[boardChar];
                    addPieceDragHandlers(span);
                    square?.appendChild(span);
                }
            } else if (piece && boardChar === '_') {
                span?.remove();
                file[f] = null;
            } else if (piece && piece.symbol != SVR_PIECES[boardChar]) {
                // both have a piece but it is not the same
                if (!span) {
                    console.log('should not happen');
                    return;
                }
                span.classList[1] = pieceSide ? 'white' : 'black';
                span.innerHTML = SVR_PIECES[boardChar];
            }
            i++;
        }
    }
}

document.addEventListener('DOMContentLoaded', () => {
    // reset game
    createBoard();
    fetch('/reset').then(res => {
        if (!res.ok) throw new Error(res.statusText);
    }).catch(err => {
        console.log(err);
    });
});
