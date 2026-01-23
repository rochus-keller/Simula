Unit Stacks;         { Stack Implementation based on SimSet }

{***********************************************************}

Interface

Uses SimSet;

Type
     StackP = ^Stack;
     ItemP = ^Item;

     Stack  = Object(Head)           { Stack as such }
        Procedure Push(X : ItemP);
        Function  Pop  : ItemP;
        Function  Size : Integer;
        Function  Top  : ItemP;
        Function  IsIn : String; Virtual;
      (*   Inherited methods to be used:
        Function  Empty : Boolean;
        Procedure Clear;   *)
     end;

     Item  = Object(Link)         { Stack item able to show itself.     }
        Procedure Show; Virtual;  { SHOW has to be declared in each     }
     end;                         { subclass according to the item type.}

{***********************************************************}

Implementation

  Procedure Stack.Push(X : ItemP);
  Begin
     X^.Follow(@Self);
  End;

  Function Stack.Pop : ItemP;
  Begin
     Pop := ItemP(First);
     First^.Out;
  End;

  Function Stack.Size : Integer;
  Begin
     Size := Cardinal;
  End;

  Function Stack.Top : ItemP;
  Begin
     Top := ItemP(First);
  End;

  Function Stack.IsIn : String;
  Begin
     IsIn := 'Stack';
  End;

  Procedure Item.Show;
  Begin
     Write('Do not use me !');
  End;

End.
