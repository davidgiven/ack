(* This is a shim that (hopefully) allows building the extremely custom Pascal
dialect used by the aem compiler using FreePascal. *)

procedure enew(var ptr: pointer; words: integer);
begin
    getmem(ptr, words*szword);
end;
