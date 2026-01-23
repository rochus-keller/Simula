Unit SimSet;     { This unit models the Simula class SIMSET }

{***********************************************************}

Interface

Type LinkageP = ^Linkage;
     HeadP = ^Head;
     LinkP = ^Link;

     Linkage = object
              Constructor Init;
              Function IsIn: String; Virtual;
              Function Prev: LinkageP;
              Function Suc : LinkP;
              Function Pred: LinkP;
             Private
              Succc: LinkageP;             { Hidden pointers }
              Predd: LinkageP;
              Function IsInHidden: String; Virtual;
	    end{of Linkage};

     Head = object (Linkage)
              Constructor Init;
              Function IsIn: String; Virtual;
              Function First : LinkP;
              Function Last  : LinkP;
              Function Empty : Boolean;
              Function Cardinal : Integer; { Number of items }
              Procedure Clear;
             Private
              Function IsInHidden: String; Virtual;
	    end;

     Link = object (Linkage)
              Function IsIn: String; Virtual;
              Procedure Out;         { Removes this Link from a list }
              Procedure Follow (Ptr: LinkageP);
                                       { Inserts This Link after Ptr }
              Procedure Precede(Ptr: LinkageP);
                                      { Inserts This Link before Ptr }
              Procedure Into(S: HeadP);
                            { Inserts ThisLinkage to S at end - FIFO }
             Private
              Function IsInHidden: String; Virtual;
	    end;

{***********************************************************}

Implementation

Constructor Linkage.Init;
begin
   Succc := nil;
   Predd := nil
end;

{***********************************************************}

Function Linkage.IsIn: String;
begin
   IsIn := 'Linkage';
end;

{***********************************************************}

Function Linkage.IsInHidden: String;
begin
   IsInHidden := 'Linkage';
end;

{***********************************************************}

Function Linkage.Prev;
begin
   Prev := Predd;
end;

{***********************************************************}

Function Linkage.Suc : LinkP;
begin
   If Succc^.IsInHidden = 'Link' then  {Is the type Link ?}
      Suc := LinkP(Succc)
   Else
      Suc := nil;
end;

{***********************************************************}

Function Linkage.Pred: LinkP;
begin
   If Predd^.IsInHidden = 'Link' then  {Is the type Link ?}
      Pred := LinkP(Predd)
   Else
      Pred := nil;
end;

{***********************************************************}

Constructor Head.Init;
begin
   Succc := @Self;
   Predd := @Self;
end;

{***********************************************************}

Function Head.IsIn: String;
begin
   IsIn := 'Head';
end;

{***********************************************************}

Function Head.IsInHidden: String;
begin
   IsInHidden := 'Head';
end;

{***********************************************************}

Function Head.First : LinkP;
begin
   First := Suc
end;

{***********************************************************}

Function Head.Last  : LinkP;
begin
   Last := Pred
end;

{***********************************************************}

Function Head.Empty : Boolean;
begin
   Empty := (@Self=Succc)
end;

{***********************************************************}

Function Head.Cardinal : Integer;
Var i : Integer;
    p : LinkP;
begin
   p := First;
   i := 0;
   While p<>nil do begin
      Inc(i);
      p := p^.Suc;
   end{while};
   Cardinal := i
end;

{***********************************************************}

Procedure Head.Clear;
Var ptr : LinkP;
begin
   While First<>nil do begin
      ptr := First;
      ptr^.Out;
      Dispose(ptr);
   end{while}
end;

{***********************************************************}

Function Link.IsIn: String;
begin
   IsIn := 'Link';
end;

{***********************************************************}

Function Link.IsInHidden: String;
begin
   IsInHidden := 'Link';
end;

{***********************************************************}

Procedure Link.Out;
begin
   If Succc<>nil then begin
      Succc^.Predd := Predd;
      Predd^.Succc := Succc;
      Succc := nil;
      Predd := nil
   end{if}
end;

{***********************************************************}

Procedure Link.Follow (Ptr: LinkageP);
begin
   Out;
   If (Ptr<>nil) and (Ptr^.Succc<>nil) then begin
      Predd := Ptr;
      Succc := Ptr^.Succc;
      Succc^.Predd := @Self;
      Ptr^.Succc := @Self
   end{if}
end;

{***********************************************************}

Procedure Link.Precede(Ptr: LinkageP);
begin
   Out;
   If (Ptr<>nil) and (Ptr^.Predd<>nil) then begin
      Predd := Ptr^.Predd;
      Succc := Ptr;
      Predd^.Succc := @Self;
      Ptr^.Predd := @Self
   end{if}
end;

{***********************************************************}

Procedure Link.Into(S: HeadP);
begin
   Precede(S)
end;

{***********************************************************}

end.


