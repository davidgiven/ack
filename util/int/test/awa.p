{ $Id$ }

program ArrayWithoutArray(input, output);
{	We simulate a (read-only) array by constructing a mapping
	function   map(n)   which yields the n-th element.
	We demonstrate its existence by first printing the length
	of the array and then its contents.
	This technique was first introduced by F.E.J. Kruseman-Aretz,
	in the early sixties.
}

procedure Action(n: integer; function map(n: integer): integer);
	{	Action is called when the construction of the virtual
		array is finished.  Actually, all elements now reside
		on the stack.
		n:	the length of the array,
		map:	the mapping function.
	}
	var i: integer;
	begin	{ show that the whole array is still there }
		writeln('#elems:', n);
		write('elems:');
		for i:= 1 to n do
			write(map(i))
	end {Action};

procedure Construct(n: integer; function oldmap(n: integer): integer);
	{	For each value read, Construct will store that value and
		declare a new map function, composed of the old one
		augmented by the new value.
		It then calls itself recursively for the next value.
		
		n:	element number on this level
		oldmap:	map for 1 .. n-1
	}
	var x: integer;		{ the value stored at level n }
	
	function newmap(i: integer): integer;
		{ yields elements stored so far }
		begin
			if	{ the i-th element is kept on this level}
				i = n
			then	{ yield it }
				newmap := x
			else	{ try lower down the road }
				newmap := oldmap(i)
		end {newmap};
	
	begin
		read(x);
		if	{ it is a valid value }
			x >= 0
		then	{ we continue reading values and constructing maps }
			Construct(n + 1, newmap)
		else	{ we stop reading and pass the info on to Action }
			Action(n - 1, newmap)
	end {Construct};

function EmptyMap(n: integer): integer;
	begin
		writeln('Illegal index', n, '; 0 yielded.');
		EmptyMap := 0
	end {EmptyMap};

begin
	Construct(1, EmptyMap)
end.
