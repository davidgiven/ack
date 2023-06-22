MODULE test;
FROM Test IMPORT fail, finished;

TYPE charset = SET OF CHAR;

PROCEDURE Space(c: CHAR): BOOLEAN;
BEGIN
  RETURN c IN charset{' ', 11C, 12C, 15C}
END Space;

BEGIN
  IF Space('a') THEN
    fail(1);
  END;
  IF NOT Space(' ') THEN
    fail(2);
  END;
  IF NOT Space(12C) THEN
    fail(3);
  END;
  finished;
END test.