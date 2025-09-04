DO $$
DECLARE
    i INTEGER;
    color CHAR(1);
    linker_table_name TEXT;
    move_table_name TEXT;
BEGIN
    -- Schleife durch alle Zahlen von 0 bis 500
    FOR i IN 0..199 LOOP
        -- Für beide Farben w (weiß) und b (schwarz)
        FOREACH color IN ARRAY ARRAY['w', 'b'] LOOP
            -- Tabellennamen generieren
            linker_table_name := 'chess_moves_linker_d' || i || '_' || color;
            move_table_name := 'chess_move_d' || i || '_' || color;
            -- Indizes für linker-Tabelle erstellen
            EXECUTE 'CREATE INDEX IF NOT EXISTS idx_' || linker_table_name || '_chess_move_id ON ' || linker_table_name || '(chess_move_id);';
            EXECUTE 'CREATE INDEX IF NOT EXISTS idx_' || linker_table_name || '_next_id ON ' || linker_table_name || '(next_id);';
        END LOOP;
    END LOOP;
END $$;
