Program TStacks;                  { Testing the unit STACKS }
Uses Simset, Stacks, Crt;

Type
     StackItemCP = ^StackItemC;   { Object pointers }
     StackItemIP = ^StackItemI;
     StackItemRP = ^StackItemR;

     StackItemC = Object(Item)    { Character stack item }
         C : Char;
         Procedure Show; Virtual;
     End;

     StackItemI = Object(Item)    { Integer stack item }
         I : Integer;
         Procedure Show; Virtual;
     End;

     StackItemR = Object(Item)    { Real stack item }
         R : Real;
         Procedure Show; Virtual;
     End;

     Procedure StackItemC.Show;
     Begin
        Write(C)
     End;

     Procedure StackItemI.Show;
     Begin
        Write(I:5)
     End;

     Procedure StackItemR.Show;
     Begin
        Write(R:8:2)
     End;

Var
   S     : ^Stack;
   SItem : ItemP;
   ItemC : StackItemCP;
   ItemI : StackItemIP;
   ItemR : StackItemRP;
   C  : Char;
   J  : Integer;
   X  : Real;

Begin
   ClrScr;
   New(S,Init);
   Writeln('Enter a sentence, finish by "."');
   Repeat
      Read(C);
      New(ItemC,Init);
      ItemC^.C := C;
      S^.Push(ItemC);
   Until C='.';
   Readln;
   Writeln('Stack size =',S^.Size:3);
   ItemC := StackItemCP(S^.Top);
   Writeln('Top item = ',ItemC^.C);

   Writeln('Enter integers, finish by "0"');
   Repeat
      Read(J);
      New(ItemI,Init);
      ItemI^.I := J;
      S^.Push(ItemI);
   Until J=0;
   Readln;
   Writeln('Stack size =',S^.Size:3);
   ItemI := StackItemIP(S^.Top);
   Writeln('Top item = ',ItemI^.I);

   Writeln('Enter reals, finish by "-1"');
   Repeat
      Read(X);
      New(ItemR,Init);
      ItemR^.R := X;
      S^.Push(ItemR);
   Until X=-1;
   Readln;
   Writeln('Stack size =',S^.Size:3);
   ItemR := StackItemRP(S^.Top);
   Writeln('Top item = ',ItemR^.R:8:2);

   Writeln('The whole sequence goes out:');

   Repeat
      SItem := S^.Pop;
      SItem^.Show;
      Dispose(SItem);
   Until S^.Empty;
   Dispose(S);
   Writeln;
   Readln;
End.