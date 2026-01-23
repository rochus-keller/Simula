Program TSimSet;             { Testing SimSet }
Uses SimSet;

Type
     Node1P = ^Node1;
     Node1 = object (Link)   { User defined integer list element }
       N : Integer;
     end;

     Node2P = ^Node2;
     Node2 = object (Link)   { User defined real list element }
       R : Real;
     end;

Var List : HeadP;
    Nod  : LinkP;
    N1   : Node1P;
    N2   : Node2P;
    i    : Integer;

Procedure DispList;          { Late binding not used }
Var Nod : LinkP;
begin
   Nod := List^.First;
   While Nod<>nil do begin
      If TypeOf(Nod^) = TypeOf(Node1) then begin
         N1 := Node1P(Nod);
         Write(N1^.N:3);
       end
      Else begin
         N2 := Node2P(Nod);
         Write(N2^.R:6:1);
      end{if};
      Nod := Nod^.Suc;
   end{while};
   Writeln
end;


Begin  { of the program body }
   New(List,Init);

{ Empty list: }
   Writeln('New list created');
   Write('Cardinal = ',List^.Cardinal);
   Writeln('   Empty = ',List^.Empty,'    Press Enter');
   Readln;

   For i := 1 to 5 do begin
     New(N1,Init);
     N1^.N := i;
     N1^.Into(List)
   end;

   Writeln('Now there are 5 integer items:');
   Displist;
   Write('Cardinal = ',List^.Cardinal);
   Writeln('   Empty = ',List^.Empty,'    Press Enter');
   Readln;

   For i := 1 to 5 do begin
     New(N2,Init);
     N2^.R := i*2;
     N2^.Into(List)
   end;

   Writeln('Now there are 5 integer and 5 real items:');
   Displist;
   Writeln('Cardinal = ',List^.Cardinal);
   Writeln;

{ Last goes second: }
   Write('And Now the last goes second: (Press Enter)');
   Readln;
   List^.Last^.Follow(List^.First);
   DispList; Writeln;

{ First goes last but two: }
   Write('And Now the first goes last but two: (Press Enter)');
   Readln;
   List^.First^.Precede(List^.Last^.Pred);
   DispList; Writeln;

{ Third removed and deleted: }
   Write('And Now the third will be deleted: (Press Enter)');
   Readln;
   Nod := List^.First^.Suc^.Suc;
   Nod^.Out;
   Dispose(Nod);
   Displist;
   Writeln('Cardinal = ',List^.Cardinal);
   Writeln;

{ Deleting the list: }
   Write('Now the list will be deleted: (Press Enter)');
   Readln;
   List^.Clear;
   DispList;
   Write('Cardinal = ',List^.Cardinal);
   Writeln('   Empty = ',List^.Empty);
   Write('Press Enter');
   Dispose(List);
   Readln;

End.

