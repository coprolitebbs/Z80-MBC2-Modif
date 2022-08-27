program pterm;
{$C-} {$I-} {$U-}

CONST
     VIDINT = $10;
     RETRACE = $3da;
     ASCII = 0;
     UK = 1;
     SPECIAL = 2;

const
     att_low_mask: byte = $6;
     att_underline: byte = $1;
     att_intencity: byte = $8;
     att_blink: byte = 80;
     special_chars : array[0..31] of byte = (
       32,4,176,9,12,13,10,248,241,18,11,217,191,218,192,197,
       196,196,196,196,196,195,180,193,194,179,243,242,227,216,156,7);


type BytePtr = ^byte;
type TSCS = record
    cursx, cursy: integer;
    GL: Byte;
    G0:byte;
    G1:byte;
    mode:boolean;
end;

type _string = string[255];

var
   GL: Byte;
   columns:byte;
   lines:byte;
   cursx,cursy:byte;
   mode:boolean;
   lastc:char;
   portin : boolean;
   comread:_string;

   oldchar:char;
   needClose:boolean;

procedure vtprintf(row,col:byte;rev:boolean;s:_string);
begin
gotoXY(row,col);
write(s);
end;


procedure ttinit;
begin
portin:=true;
needClose:=false;
end;

procedure vidinit;
begin
cursx:=1;
cursy:=1;
mode:=false;
columns:=80;
lines:=24;
end;

procedure keyinit;
begin
oldchar:=#0;
end;



procedure SetCurs(col,row:byte);
begin
    if (col = 0)or(col > columns) then col:=cursx;

    if (row=0)or(row > lines) then row:=cursy;

      cursx:=col;
      cursy:=row;
      gotoxy(cursx,cursy);
end;


procedure vtinit;
begin
clrscr;
SetCurs(1,1);
lastc:=#0;
end;


procedure com_read;
var cnt:byte;
chh:char;
de:boolean;
begin
cnt:=0;
de:=false;
comread:='';
repeat
  read(AUX,chh);
  if(chh = #0){or(chh=#13)or(chh = #10)} then de:=true;
  cnt:=cnt+1;
  if chh<>#0 then comread := comread + chh;
  if cnt>=127 then de:=true;
until (de=true);
{write('readed:');
writeln(comread,' : ',length(comread));}
write(comread);
end;


function readkey:char;
var chh:char;
begin
chh := #0;
{readkey := #0;}
if keypressed then begin
    read(Kbd,chh);
    if (chh <> oldchar) then begin
       readkey:=chh;
       oldchar:=chh;
    end else begin
       if (chh = #27)and(oldchar=#27) then needClose := true;
       readkey:=#0;
    end;

end;
{if chh<>#0 then writeln('k:',ord(chh));}
if chh > #0 then begin
   write(AUX,chh);
end;
end;


function readSecEsc:boolean;
var chh:char;
begin
readSecEsc:=false;
if keypressed then
   read(Kbd,chh);
end;



procedure terminal;
var thru:boolean;
    k:char;
begin
write(AUX,#13);
writeln('To exit terminal hit ESC ESC ESC...');
writeln;

thru:=false;
while not thru do begin
   com_read;
   k:=readkey;

   if needClose then thru:=true;
end;
end;


begin
     ttinit;
     keyinit;
     vtinit;

     terminal;

     gotoxy(1,24);
     writeln('TERMINAL CLOSED');
end.