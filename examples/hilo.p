(* $Source$
 * $State$
 * $Revision$
 *)

(*$U+ --- enables underscores in identifiers *)

program hilo(input, output);

type
	string = array [0..255] of char;
	
var
	playing : Boolean;
	seed : integer;
	
{ This version of Pascal seems to have no random number generator I can find,
  so we have to implement our own here. This is a hacked up and probably
  broken version of the C library generator. }

procedure randomise(s : integer);
	begin
		seed := s;
	end;
	
function random(range : integer) : integer;
	begin
		seed := (20077 * seed + 12345);
		random := seed mod range;
	end;

{ Pascal doesn't provide string input, so we interface to the _read() syscall
  and do it manually. }
  
function _read(fd : integer; var buffer : char; count : integer) : integer;
	extern;

function readchar : char;
	var
		c : char;
		dummy : integer;
		
	begin
		c := chr(0);
		dummy := _read(0, c, 1);
		readchar := c;
	end;
	
procedure readstring(var buffer : string; var length : integer);
	var
		finished : Boolean;
		c : char;
		
	begin
		write('> ');
		
		length := 0;
		finished := FALSE;
		seed := 0;
		while not finished do
			begin
				c := readchar;
				if (ord(c) = 10) then
					finished := true
				else
					begin
						buffer[length] := c;
						length := length + 1;
					end
			end;
	end;

procedure getname;
	var
		name : string;
		namelen : integer;
		i : integer;
		seed : integer;
		
	begin
		writeln;
		writeln('Hi there! Before we start, what is your name?');
		writeln;
		readstring(name, namelen);
		writeln;
		write('Hello, ');
		
		seed := 0;
		for i := 0 to (namelen-1) do
			begin
				write(name[i]);
				seed := seed + ord(name[i]);
			end;
			
		randomise(seed);
		write('! ');
	end;
	
procedure game;
	var
		Number : integer;
		Attempts : integer;
		guess : integer;
		
	begin
		writeln('See if you can guess my number.');
		Number := random(100);
		Attempts := 0;
		
		guess := -1;
		while guess <> Number do
			begin
				Attempts := Attempts + 1;
				write('> ');
				readln(guess);
			
				if guess < Number then
					begin
						writeln;
						writeln('Try a bit higher.');
					end;
			
				if guess > Number then
					begin
						writeln;
						writeln('Try a bit lower.');
					end;
			end;
			
		writeln;
		write('You got it right in only ', Attempts:0, ' ');
		if Attempts = 1 then
			write('go')
		else
			write('goes');
		writeln('!');
	end;
	
function question: Boolean;
	var
		response: char;
		
	begin
		write('> ');
		readln(response);
		
		question := not ((response = 'n') or (response = 'N')); 
	end;
 
begin
	getname;

	playing := TRUE;
	while playing do
		begin
			game;
			writeln;
			writeln('Would you like another go?');
			playing := question;
			
			writeln;
			if playing then
				write('Excellent! ')
			else
				writeln('Thanks for playing --- goodbye!');
		end;
end.
