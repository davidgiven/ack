MODULE test;
FROM InOut IMPORT WriteLn, WriteString;

TYPE charset = SET OF CHAR;

PROCEDURE Space(c: CHAR): BOOLEAN;
BEGIN
  RETURN c IN charset{' ', 11C, 12C, 15C}
END Space;

BEGIN
  IF Space('a') THEN
    WriteString("@@FAIL 1");
    WriteLn;
  END;
  IF NOT Space(' ') THEN
    WriteString("@@FAIL 2");
    WriteLn;
  END;
  IF NOT Space(12C) THEN
    WriteString("@@FAIL 3");
    WriteLn;
  END;
  WriteString("@@FINISHED");
  WriteLn;
END test.