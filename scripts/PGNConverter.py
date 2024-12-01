def pgn_to_moves(pgn):
    moves = [move for move in pgn.split() if move[0].isalpha()]
    return moves

def filter_info_data(info):
    info_data_raw = info.split('\n')
    info_map = dict()
    for info_line in info_data_raw:
        info_line = info_line.strip('[]')
        splited_info = info_line.split(' ', 1)
        info_map[splited_info[0]] = splited_info[1].strip('"')
    return info_map

def simplify_pgn_data(input_file, output_file):
    with open(input_file, 'r') as f:
        content = f.read()

    lines = content.strip().split('\n\n')
    gameInfoPairs = [(lines[i], lines[i + 1]) for i in range(0, len(lines) - 1, 2)]
    formatted_games = []

    for game in gameInfoPairs:
        info = game[0]
        if '[Termination "Normal"]' not in info:
            continue
        moves = pgn_to_moves(game[1])
        formatted_moves = ', '.join(f'"{m}"' for m in moves)
        formatted_games.append((info, f'{{ {formatted_moves} }}'))

    with open(output_file, 'w') as f:
        for game in formatted_games:
            f.write(game[0] + ' : ' + game[1] + '\n')

def get_win_and_pgn_data(input_file, output_file):
    with open(input_file, 'r') as f:
        content = f.read()

    lines = content.strip().split('\n\n')
    gameInfoPairs = [(lines[i], lines[i + 1]) for i in range(0, len(lines) - 1, 2)]
    formatted_games = []

    for game in gameInfoPairs:
        info = filter_info_data(game[0])
        result = info['Result'].strip('"')
        if 'Normal' not in info['Termination'] :
            continue
        moves = pgn_to_moves(game[1])
        formatted_moves = ' '.join(f'{m}' for m in moves)
        formatted_games.append((result, f'{formatted_moves}'))

    with open(output_file, 'w') as f:
        for game in formatted_games:
            f.write(game[0] + ' : ' + game[1] + '\n')

def pgn_as_list(input):
    words = input.split()
    quoted_words = ['"' + word + '"' for word in words]
    output_string = ",".join(quoted_words)
    return output_string

def main():
    # input_file = '../data/lichessDatabase/rawData/lichess_db_standard_rated_2013-01.pgn'
    # output_file = '../data/lichessDatabase/outData/lichess_db_standard.rated_2013-01.txt'
    # get_win_and_pgn_data(input_file, output_file)
    # simplify_pgn_data(input_file, output_file)
    print(pgn_as_list("d4 d5 Bd2 Nc6 Nf3 Nf6 c4 dxc4 Nc3 Nxd4 Nxd4 Qxd4 e3 Qc5 Qa4  Bd7 Qxc4 Qxc4 Bxc4 e6 O-O O-O-O Be1 a6 e4 Bc6 f3 Bc5  Bf2 Nd7 Rad1 Bxf2  Kxf2 Nc5 Rxd8  Rxd8 Ke3 Bb5 Bxb5 axb5 Nxb5 Rd3  Ke2 Rd7 Rc1 b6 b4 Na6 Na7  Kb7 Nb5 Nxb4 a4 c6 Nc3 Ka6 Rb1 Ka5 h4 Nd3 g4 Nf4  Ke3 e5 h5 Rd3  Kf2 Rxc3 Rd1 Rc2  Kg3 Rg2  Kh4 f6 Rd6 h6 Rd7 g5"))

main()

# example 
#
# [Event "Rated Bullet tournament https://lichess.org/tournament/RCOPb6Bo"]
# [Site "https://lichess.org/aSWMBMRI"]
# [White "lunas"]
# [Black "FrancoValussi"]
# [Result "1-0"]
# [UTCDate "2015.02.28"]
# [UTCTime "23:00:02"]
# [WhiteElo "1767"]
# [BlackElo "1700"]
# [WhiteRatingDiff "+9"]
# [BlackRatingDiff "-14"]
# [ECO "A40"]
# [Opening "Modern Defense"]
# [TimeControl "60+0"]
# [Termination "Normal"]
# [LichessId "aSWMBMRI"]
#
# 1. d4 g6 2. c4 Bg7 3. Nc3 e6 4. e4 a6 5. Nf3 Ne7 6. Bg5 Nbc6 7. Qd2 h6 8. Be3 O-O 9. Bxh6 Bxh6 10. Qxh6 d5 11. Ng5 Nf5 12. Qh7# 1-0
#
# [Event "Rated Blitz game"]
# [Site "https://lichess.org/YmWK0unz"]
# [White "zivkoj"]
# [Black "Atlec"]
# [Result "0-1"]
# [UTCDate "2015.02.28"]
# [UTCTime "23:00:04"]
# [WhiteElo "1741"]
# [BlackElo "1409"]
# [WhiteRatingDiff "-19"]
# [BlackRatingDiff "+20"]
# [ECO "B20"]
# [Opening "Sicilian Defense: Bowdler Attack"]
# [TimeControl "300+0"]
# [Termination "Time forfeit"]
# [LichessId "YmWK0unz"]
#
# 1. e4 c5 2. Bc4 e6 3. a3 a6 4. d3 d5 5. exd5 exd5 6. Ba2 Nf6 7. h3 Bd6 8. Nf3 Nc6 9. O-O O-O 10. Bg5 h6 11. Bh4 Re8 12. Nbd2 Be7 13. Nh2 Nh7 14. Bxe7 Rxe7 15. c3 Bf5 16. Ndf3 Qd7 17. Nh4 Be6 18. d4 cxd4 19. cxd4 Rae8 20. Bb1 Nf6 21. Bc2 Qc7 22. Qd3 Nxd4 23. Rac1 Nxc2 24. Rxc2 Qd6 25. N2f3 Ne4 26. Nd4 Nc5 27. Qf3 Bd7 28. Nhf5 Bxf5 29. Nxf5 Qf6 30. Nxe7+ Qxe7 0-1
#
