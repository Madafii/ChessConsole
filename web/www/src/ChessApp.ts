const PIECES: Readonly<Record<string, string>> = {
    k: '♔', q: '♕', r: '♖', b: '♗', n: '♘', p: '♙',
    K: '♚', Q: '♛', R: '♜', B: '♝', N: '♞', P: '♟',
};

const STATUS_MESSAGES: Readonly<Record<string, string>> = {
    playing: 'White to move.',
    draw: 'Draw.',
    'white-won': 'You win!',
    'black-won': 'The opponent wins.',
};

type Drag = { from: string; piece: HTMLSpanElement; destinations: Set<string> };

export class ChessApp {
    private readonly board = document.querySelector<HTMLElement>('#board')!;
    private readonly status = document.querySelector<HTMLElement>('#game-status')!;
    private readonly reset = document.querySelector<HTMLButtonElement>('#reset-game')!;
    private readonly squares = new Map<string, HTMLDivElement>();
    private position = '';
    private gameStatus = '';
    private revision = '';
    private drag?: Drag;
    private promotion?: HTMLDivElement;
    private busy = false;

    constructor() {
        for (let rank = 8; rank >= 1; rank--) {
            for (let file = 0; file < 8; file++) {
                const coordinate = 'abcdefgh'[file] + rank;
                const square = document.createElement('div');
                square.classList.add('square', (rank + file) % 2 === 0 ? 'light' : 'dark');
                square.dataset.square = coordinate;
                square.addEventListener('dragover', (event) => {
                    if (!this.drag?.destinations.has(coordinate)) return;
                    event.preventDefault();
                    if (event.dataTransfer) event.dataTransfer.dropEffect = 'move';
                });
                square.addEventListener('drop', (event) => {
                    const drag = this.drag;
                    if (!drag?.destinations.has(coordinate) || event.dataTransfer?.getData('text/plain') !== drag.from) return;
                    event.preventDefault();
                    this.clearDrag();
                    const move = `${drag.from}:${coordinate}`;
                    if (drag.piece.textContent === PIECES.p && rank === 8) {
                        this.choosePromotion(square, move);
                    } else {
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

    private render(): void {
        for (const [coordinate, square] of this.squares) {
            const index = (Number(coordinate[1]) - 1) * 8 + 'abcdefgh'.indexOf(coordinate[0]);
            const piece = this.position[index];
            square.replaceChildren();
            if (!piece || piece === '_') continue;

            const element = document.createElement('span');
            const white = piece === piece.toLowerCase();
            element.classList.add('piece', white ? 'white' : 'black');
            element.textContent = PIECES[piece];
            element.draggable = white && this.gameStatus === 'playing';
            element.addEventListener('dragstart', (event) => this.startDrag(event, coordinate, element));
            element.addEventListener('dragend', () => {
                if (this.drag?.piece === element) this.clearDrag();
            });
            square.append(element);
        }
    }

    private startDrag(event: DragEvent, from: string, piece: HTMLSpanElement): void {
        if (this.busy || this.promotion || !piece.draggable || !event.dataTransfer) {
            event.preventDefault();
            return;
        }

        this.clearDrag();
        const drag: Drag = { from, piece, destinations: new Set() };
        this.drag = drag;
        event.dataTransfer.setData('text/plain', from);
        event.dataTransfer.effectAllowed = 'move';
        void this.loadHints(drag);
    }

    private async loadHints(drag: Drag): Promise<void> {
        try {
            const response = await this.request('/drag-start', drag.from);
            const destinations = (await response.text()).trim().split(/\s+/);
            if (this.drag !== drag) return;
            for (const to of destinations) {
                const square = this.squares.get(to);
                if (!square) continue;
                drag.destinations.add(to);
                square.setAttribute('hint', 'true');
            }
        } catch {
            if (this.drag === drag) void this.update('/board');
        }
    }

    private clearDrag(): void {
        for (const to of this.drag?.destinations ?? []) this.squares.get(to)?.removeAttribute('hint');
        this.drag = undefined;
    }

    private choosePromotion(square: HTMLDivElement, move: string): void {
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
        selection.querySelector('button')?.focus();
        this.status.textContent = 'Choose a promotion piece.';
    }

    private async update(path: string, body?: string): Promise<void> {
        if (this.busy) return;
        this.busy = true;
        this.reset.disabled = true;
        this.clearDrag();
        this.promotion?.remove();
        this.promotion = undefined;
        try {
            await this.readBoard(await this.request(path, body));
            this.status.textContent = STATUS_MESSAGES[this.gameStatus];
        } catch (error) {
            const message = error instanceof Error ? error.message : 'The request failed.';
            if (path !== '/board') {
                try {
                    await this.readBoard(await this.request('/board'));
                } catch {
                    this.gameStatus = '';
                    this.render();
                }
            }
            this.status.textContent = message;
        } finally {
            this.reset.disabled = false;
            this.busy = false;
        }
    }

    private async readBoard(response: Response): Promise<void> {
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

    private async request(path: string, body?: string): Promise<Response> {
        const response = await fetch(path, body === undefined ? {} : {
            method: 'POST',
            headers: path === '/reset'
                ? { 'Content-Type': 'text/plain' }
                : { 'Content-Type': 'text/plain', 'X-Board-Revision': this.revision },
            body,
        });
        if (!response.ok) throw new Error((await response.text()) || `Request failed (${response.status}).`);
        return response;
    }
}
