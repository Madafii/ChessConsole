def pgn_to_moves(pgn):
    moves = [move for move in pgn.split() if move[0].isalpha()]
    return moves

def process_pgn_file(input_file, output_file):
    with open(input_file, 'r') as f:
        content = f.read()

    games = content.strip().split('\n\n')
    formatted_games = []

    for game in games:
        move_list = game.split('\n')[-1]
        moves = pgn_to_moves(move_list)
        formatted_moves = ', '.join(f'"{m}"' for m in moves)
        formatted_games.append(f'{{ {formatted_moves} }}')

    with open(output_file, 'w') as f:
        for game in formatted_games:
            f.write(game + '\n')

def main():
    input_file = 'data/rawPGN/50MovesDrawGame.pgn'
    output_file = 'data/outPGN/50MovesDrawGame.txt'
    process_pgn_file(input_file, output_file)

main()