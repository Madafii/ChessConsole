DO $$
DECLARE
    i INT;
    move_char CHAR(1);
    move_chars CHAR(1)[] := ARRAY['w'];
BEGIN
    FOREACH move_char IN ARRAY move_chars LOOP
        FOR i IN 0..500 LOOP
            -- Drop move table if it exists
            EXECUTE format('DROP TABLE IF EXISTS chess_move_D%s_%s CASCADE;', i, move_char);
            RAISE NOTICE 'Dropped Table: move_table_%_%', i, move_char;

            -- Drop connect table if it exists
            EXECUTE format('DROP TABLE IF EXISTS chess_moves_linker_D%s_%s CASCADE;', i, move_char);
            RAISE NOTICE 'Dropped Connect Table: linker_table_%_%', i, move_char;
        END LOOP;
    END LOOP;
END $$;

DO $$
DECLARE
    i INT;
    move_char CHAR(1);
    move_chars CHAR(1)[] := ARRAY['b'];
BEGIN
    FOREACH move_char IN ARRAY move_chars LOOP
        FOR i IN 0..500 LOOP
            -- Drop move table if it exists
            EXECUTE format('DROP TABLE IF EXISTS chess_move_D%s_%s CASCADE;', i, move_char);
            RAISE NOTICE 'Dropped Table: move_table_%_%', i, move_char;

            -- Drop connect table if it exists
            EXECUTE format('DROP TABLE IF EXISTS chess_moves_linker_D%s_%s CASCADE;', i, move_char);
            RAISE NOTICE 'Dropped Connect Table: linker_table_%_%', i, move_char;
        END LOOP;
    END LOOP;
END $$;
