"use strict";
// Entry point: build an 8x8 chessboard and place standard starting pieces.
// Uses Unicode chess glyphs for pieces.
// Unicode chess pieces
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
// Starting position as 8x8 array (row 0 = rank 8, row 7 = rank 1)
const STARTING_POSITION = [
    // rank 8
    [PIECES.black.rook, PIECES.black.knight, PIECES.black.bishop, PIECES.black.queen, PIECES.black.king, PIECES.black.bishop, PIECES.black.knight, PIECES.black.rook],
    // rank 7
    Array(8).fill(PIECES.black.pawn),
    // ranks 6-3 empty
    Array(8).fill(null),
    Array(8).fill(null),
    Array(8).fill(null),
    Array(8).fill(null),
    // rank 2
    Array(8).fill(PIECES.white.pawn),
    // rank 1
    [PIECES.white.rook, PIECES.white.knight, PIECES.white.bishop, PIECES.white.queen, PIECES.white.king, PIECES.white.bishop, PIECES.white.knight, PIECES.white.rook],
];
function createBoard() {
    const boardContainer = document.getElementById('board');
    if (!boardContainer)
        return;
    // Clear existing content
    boardContainer.innerHTML = '';
    for (let r = 0; r < 8; r++) {
        for (let f = 0; f < 8; f++) {
            const square = document.createElement('div');
            square.classList.add('square');
            // color: (rank + file) even => light, odd => dark (with row 0 = rank 8)
            const isLight = ((r + f) % 2) === 0;
            square.classList.add(isLight ? 'light' : 'dark');
            // algebraic coordinate, e.g., a8 ... h1
            const fileChar = String.fromCharCode('a'.charCodeAt(0) + f);
            const rankChar = (8 - r).toString();
            square.setAttribute('data-square', fileChar + rankChar);
            const piece = STARTING_POSITION[r][f];
            if (piece) {
                const span = document.createElement('span');
                span.classList.add('piece');
                span.textContent = piece;
                square.appendChild(span);
            }
            // Optional: click handler to log coords (simple demonstration)
            square.addEventListener('click', () => {
                console.log('Clicked square', square.getAttribute('data-square'));
                // Visual feedback: briefly outline
                square.style.outline = '3px solid rgba(255,0,0,0.6)';
                setTimeout(() => (square.style.outline = ''), 250);
            });
            boardContainer.appendChild(square);
        }
    }
}
document.addEventListener('DOMContentLoaded', createBoard);
