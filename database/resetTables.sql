-- drop all tables
DO $$
DECLARE
    i INT;
    move_char CHAR(1);
BEGIN
    FOR i IN 0..500 LOOP
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
    move_char CHAR(1);
BEGIN
    FOR i IN 0..500 LOOP
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
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_move_D%s_%s (id SERIAL PRIMARY KEY, moveData INT, wins BIGINT, loses BIGINT, draws BIGINT);', i, move_char);
            RAISE NOTICE 'Created Table: move_table_%_%', i, move_char;

            -- Create connect table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_moves_linker_D%s_%s (id SERIAL PRIMARY KEY, chess_move_id INT REFERENCES chess_move_D%s_%s(id), next_id INT REFERENCES chess_moves_linker_D%s_%s(id));',
                           i, move_char, i, move_char, i, move_char);
            RAISE NOTICE 'Created Connect Table: linker_table_%_%', i, move_char;
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
        FOR i IN 5..30 LOOP
            -- Create move table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_move_D%s_%s (id SERIAL PRIMARY KEY, moveData INT, wins INT, loses INT, draws INT);', i, move_char);
            RAISE NOTICE 'Created Table: move_table_%_%', i, move_char;

            -- Create connect table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_moves_linker_D%s_%s (id SERIAL PRIMARY KEY, chess_move_id INT REFERENCES chess_move_D%s_%s(id), next_id INT REFERENCES chess_moves_linker_D%s_%s(id));',
                           i, move_char, i, move_char, i, move_char);
            RAISE NOTICE 'Created Connect Table: linker_table_%_%', i, move_char;
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
        FOR i IN 30..500 LOOP
            -- Create move table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_move_D%s_%s (id SERIAL PRIMARY KEY, moveData INT, wins SMALLINT, loses SMALLINT, draws SMALLINT);', i, move_char);
            RAISE NOTICE 'Created Table: move_table_%_%', i, move_char;

            -- Create connect table
            EXECUTE format('CREATE TABLE IF NOT EXISTS chess_moves_linker_D%s_%s (id SERIAL PRIMARY KEY, chess_move_id INT REFERENCES chess_move_D%s_%s(id), next_id INT REFERENCES chess_moves_linker_D%s_%s(id));',
                           i, move_char, i, move_char, i, move_char);
            RAISE NOTICE 'Created Connect Table: linker_table_%_%', i, move_char;
        END LOOP;
    END LOOP;
END $$;
