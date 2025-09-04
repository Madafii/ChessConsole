-- drop all tables
DO $$
DECLARE
    i INT;
    move_char CHAR(1) := 'W';
BEGIN
    FOR i IN 0..200 LOOP
        -- Drop move table if it exists
        EXECUTE format('DROP TABLE IF EXISTS chess_move_D%s_W CASCADE;', i, move_char);
        RAISE NOTICE 'Dropped Table: chess_move_D%_%', i, move_char;

        -- Drop connect table if it exists
        EXECUTE format('DROP TABLE IF EXISTS chess_moves_linker_D%s_%s CASCADE;', i, move_char);
        RAISE NOTICE 'Dropped Connect Table: chess_moves_linker_D%_%', i, move_char;
    END LOOP;
END $$;

DO $$
DECLARE
    i INT;
    move_char CHAR(1) := 'B';
BEGIN
    FOR i IN 0..200 LOOP
        -- Drop move table if it exists
        EXECUTE format('DROP TABLE IF EXISTS chess_move_D%s_B CASCADE;', i, move_char);
        RAISE NOTICE 'Dropped Table: chess_move_D%_%', i, move_char;

        -- Drop connect table if it exists
        EXECUTE format('DROP TABLE IF EXISTS chess_moves_linker_D%s_%s CASCADE;', i, move_char);
        RAISE NOTICE 'Dropped Connect Table: chess_moves_linker_D%_%', i, move_char;
    END LOOP;
END $$;

-- first 10 moves should save with BIGINT after that smaller ones
DO $$
DECLARE
    i INT;
    move_char CHAR(1);
    move_chars CHAR(1)[] := ARRAY['W', 'B'];
BEGIN
    FOREACH move_char IN ARRAY move_chars LOOP
        FOR i IN 0..5 LOOP
            -- Create move table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_move_D%s_%s (id SERIAL PRIMARY KEY, moveData BYTEA, wins BIGINT, loses BIGINT, draws BIGINT);', i, move_char);
            RAISE NOTICE 'Created Table: move_table_%_%', i, move_char;
        END LOOP;
    END LOOP;
END $$;

DO $$
DECLARE
    i INT;
    move_char CHAR(1);
    move_chars CHAR(1)[] := ARRAY['W', 'B'];
BEGIN
    FOREACH move_char IN ARRAY move_chars LOOP
        FOR i IN 6..30 LOOP
            -- Create move table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_move_D%s_%s (id SERIAL PRIMARY KEY, moveData BYTEA, wins INT, loses INT, draws INT);', i, move_char);
            RAISE NOTICE 'Created Table: move_table_%_%', i, move_char;
        END LOOP;
    END LOOP;
END $$;


DO $$
DECLARE
    i INT;
    move_char CHAR(1);
    move_chars CHAR(1)[] := ARRAY['W', 'B'];
BEGIN
    FOREACH move_char IN ARRAY move_chars LOOP
        FOR i IN 31..200 LOOP
            -- Create move table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_move_D%s_%s (id SERIAL PRIMARY KEY, moveData BYTEA, wins SMALLINT, loses SMALLINT, draws SMALLINT);', i, move_char);
            RAISE NOTICE 'Created Table: move_table_%_%', i, move_char;
        END LOOP;
    END LOOP;
END $$;

-- Create connect tables
DO $$
DECLARE
    i INT;
    white_char CHAR(1) := 'W';
    black_char CHAR(1) := 'B';
BEGIN
    FOR i IN 0..199 LOOP
        -- Create connect table
        EXECUTE format('CREATE TABLE IF NOT EXISTS chess_moves_linker_D%s_%s (id SERIAL PRIMARY KEY, chess_move_id INT REFERENCES chess_move_D%s_%s(id), next_id INT REFERENCES chess_move_D%s_%s(id));', i, white_char, i, white_char, (i + 1), black_char);
        RAISE NOTICE 'Created Connect Table: chess_moves_linker_D%_%', i, white_char;
    END LOOP;
END $$;

DO $$
DECLARE
    i INT;
    white_char CHAR(1) := 'W';
    black_char CHAR(1) := 'B';
BEGIN
    FOR i IN 0..200 LOOP
        -- Create connect table
        EXECUTE format('CREATE TABLE IF NOT EXISTS chess_moves_linker_D%s_%s (id SERIAL PRIMARY KEY, chess_move_id INT REFERENCES chess_move_D%s_%s(id), next_id INT REFERENCES chess_move_D%s_%s(id));', i, black_char, i, black_char, i, white_char);
        RAISE NOTICE 'Created Connect Table: linker_table_%_%', i, black_char;
    END LOOP;
END $$;

-- base from from which the rest will go from
INSERT INTO chess_move_d0_b (movedata, wins, loses, draws) VALUES (E'\\x0000', 0, 0, 0);
