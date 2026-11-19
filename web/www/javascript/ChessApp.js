const PIECES = {
    k: '♔', q: '♕', r: '♖', b: '♗', n: '♘', p: '♙',
    K: '♚', Q: '♛', R: '♜', B: '♝', N: '♞', P: '♟',
};
const STATUS_MESSAGES = {
    playing: 'White to move.',
    draw: 'Draw.',
    'white-won': 'You win!',
    'black-won': 'The opponent wins.',
};
export class ChessApp {
    constructor() {
        this.board = document.querySelector('#board');
        this.status = document.querySelector('#game-status');
        this.reset = document.querySelector('#reset-game');
        this.squares = new Map();
        this.position = '';
        this.gameStatus = '';
        this.revision = '';
        this.busy = false;
        for (let rank = 8; rank >= 1; rank--) {
            for (let file = 0; file < 8; file++) {
                const coordinate = 'abcdefgh'[file] + rank;
                const square = document.createElement('div');
                square.classList.add('square', (rank + file) % 2 === 0 ? 'light' : 'dark');
                square.dataset.square = coordinate;
                square.addEventListener('dragover', (event) => {
                    var _a;
                    if (!((_a = this.drag) === null || _a === void 0 ? void 0 : _a.destinations.has(coordinate)))
                        return;
                    event.preventDefault();
                    if (event.dataTransfer)
                        event.dataTransfer.dropEffect = 'move';
                });
                square.addEventListener('drop', (event) => {
                    var _a;
                    const drag = this.drag;
                    if (!(drag === null || drag === void 0 ? void 0 : drag.destinations.has(coordinate)) || ((_a = event.dataTransfer) === null || _a === void 0 ? void 0 : _a.getData('text/plain')) !== drag.from)
                        return;
                    event.preventDefault();
                    this.clearDrag();
                    const move = `${drag.from}:${coordinate}`;
                    if (drag.piece.textContent === PIECES.p && rank === 8) {
                        this.choosePromotion(square, move);
                    }
                    else {
                        void this.update('/drag-end', move);
                    }
                });
                this.squares.set(coordinate, square);
                this.board.append(square);
            }
        }
        this.reset.addEventListener('click', () => void this.update('/reset', ''));
        void this.update('/board');
    }
    render() {
        for (const [coordinate, square] of this.squares) {
            const index = (Number(coordinate[1]) - 1) * 8 + 'abcdefgh'.indexOf(coordinate[0]);
            const piece = this.position[index];
            square.replaceChildren();
            if (!piece || piece === '_')
                continue;
            const element = document.createElement('span');
            const white = piece === piece.toLowerCase();
            element.classList.add('piece', white ? 'white' : 'black');
            element.textContent = PIECES[piece];
            element.draggable = white && this.gameStatus === 'playing';
            element.addEventListener('dragstart', (event) => this.startDrag(event, coordinate, element));
            element.addEventListener('dragend', () => {
                var _a;
                if (((_a = this.drag) === null || _a === void 0 ? void 0 : _a.piece) === element)
                    this.clearDrag();
            });
            square.append(element);
        }
    }
    startDrag(event, from, piece) {
        if (this.busy || this.promotion || !piece.draggable || !event.dataTransfer) {
            event.preventDefault();
            return;
        }
        this.clearDrag();
        const drag = { from, piece, destinations: new Set() };
        this.drag = drag;
        event.dataTransfer.setData('text/plain', from);
        event.dataTransfer.effectAllowed = 'move';
        void this.loadHints(drag);
    }
    async loadHints(drag) {
        try {
            const response = await this.request('/drag-start', drag.from);
            const destinations = (await response.text()).trim().split(/\s+/);
            if (this.drag !== drag)
                return;
            for (const to of destinations) {
                const square = this.squares.get(to);
                if (!square)
                    continue;
                drag.destinations.add(to);
                square.setAttribute('hint', 'true');
            }
        }
        catch (_a) {
            if (this.drag === drag)
                void this.update('/board');
        }
    }
    clearDrag() {
        var _a, _b, _c;
        for (const to of (_b = (_a = this.drag) === null || _a === void 0 ? void 0 : _a.destinations) !== null && _b !== void 0 ? _b : [])
            (_c = this.squares.get(to)) === null || _c === void 0 ? void 0 : _c.removeAttribute('hint');
        this.drag = undefined;
    }
    choosePromotion(square, move) {
        var _a;
        const selection = document.createElement('div');
        selection.className = 'selection';
        const file = 'abcdefgh'.indexOf(move[3]);
        const offset = Math.min(4, Math.max(0, file - 1)) - file;
        selection.style.setProperty('--promotion-offset', `calc(${offset} * var(--size))`);
        const choices = { Q: 'Queen', R: 'Rook', B: 'Bishop', N: 'Knight' };
        for (const [piece, name] of Object.entries(choices)) {
            const button = document.createElement('button');
            button.type = 'button';
            button.textContent = PIECES[piece.toLowerCase()];
            button.setAttribute('aria-label', name);
            button.addEventListener('click', () => void this.update('/drag-end', `${move}=${piece}`));
            selection.append(button);
        }
        square.append(selection);
        this.promotion = selection;
        (_a = selection.querySelector('button')) === null || _a === void 0 ? void 0 : _a.focus();
        this.status.textContent = 'Choose a promotion piece.';
    }
    async update(path, body) {
        var _a;
        if (this.busy)
            return;
        this.busy = true;
        this.reset.disabled = true;
        this.clearDrag();
        (_a = this.promotion) === null || _a === void 0 ? void 0 : _a.remove();
        this.promotion = undefined;
        try {
            await this.readBoard(await this.request(path, body));
            this.status.textContent = STATUS_MESSAGES[this.gameStatus];
        }
        catch (error) {
            const message = error instanceof Error ? error.message : 'The request failed.';
            if (path !== '/board') {
                try {
                    await this.readBoard(await this.request('/board'));
                }
                catch (_b) {
                    this.gameStatus = '';
                    this.render();
                }
            }
            this.status.textContent = message;
        }
        finally {
            this.reset.disabled = false;
            this.busy = false;
        }
    }
    async readBoard(response) {
        const position = await response.text();
        const status = response.headers.get('X-Game-Status');
        const revision = response.headers.get('X-Board-Revision');
        if (revision === null || position.length !== 64 || /[^_kqrbnpKQRBNP]/.test(position) || !status || !Object.prototype.hasOwnProperty.call(STATUS_MESSAGES, status)) {
            throw new Error('The server returned an invalid board.');
        }
        this.revision = revision;
        this.position = position;
        this.gameStatus = status;
        this.render();
    }
    async request(path, body) {
        const response = await fetch(path, body === undefined ? {} : {
            method: 'POST',
            headers: path === '/reset'
                ? { 'Content-Type': 'text/plain' }
                : { 'Content-Type': 'text/plain', 'X-Board-Revision': this.revision },
            body,
        });
        if (!response.ok)
            throw new Error((await response.text()) || `Request failed (${response.status}).`);
        return response;
    }
}
