{ $Id$ }

procedure machar (var ibeta , it , irnd , ngrd , machep , negep , iexp,
  minexp , maxexp : integer ; var eps , epsneg , xmin , xmax : real ) ;
var trapped:boolean;

procedure encaps(procedure p; procedure q(i:integer)); extern;
procedure trap(i:integer); extern;

procedure catch(i:integer);
const underflo=5;
begin if i=underflo then trapped:=true else trap(i) end;

procedure work;
var


{     This subroutine is intended to determine the characteristics
      of the floating-point arithmetic system that are specified
      below.  The first three are determined according to an
      algorithm due to M. Malcolm, CACM 15 (1972), pp. 949-951,
      incorporating some, but not all, of the improvements
      suggested by M. Gentleman and S. Marovich, CACM 17 (1974),
      pp. 276-277.  The version given here is for single precision.

      Latest revision - October 1, 1976.

      Author - W. J. Cody
               Argonne National Laboratory

      Revised for Pascal - R. A. Freak
                           University of Tasmania
                           Hobart
                           Tasmania

      ibeta    is the radix of the floating-point representation
      it       is the number of base ibeta digits in the floating-point
                  significand
      irnd     =  0 if the arithmetic chops,
                  1 if the arithmetic rounds
      ngrd     =  0 if  irnd=1, or if  irnd=0  and only  it  base ibeta
                    digits participate in the post normalization shift
                    of the floating-point significand in multiplication
                  1 if  irnd=0  and more than  it  base  ibeta  digits
                    participate in the post normalization shift of the
                    floating-point significand in multiplication
      machep   is the exponent on the smallest positive floating-point
                  number  eps such that  1.0+eps <> 1.0
      negeps   is the exponent on the smallest positive fl. pt. no.
                  negeps such that  1.0-negeps <> 1.0, except that
                  negeps is bounded below by  it-3
      iexp     is the number of bits (decimal places if ibeta = 10)
                  reserved for the representation of the exponent of
                  a floating-point number
      minexp   is the exponent of the smallest positive fl. pt. no.
                  xmin
      maxexp   is the exponent of the largest finite floating-point
                  number  xmax
      eps      is the smallest positive floating-point number such
                  that  1.0+eps <> 1.0. in particular,
                  eps = ibeta**machep
      epsneg   is the smallest positive floating-point number such
                  that  1.0-eps <> 1.0  (except that the exponent
                  negeps is bounded below by it-3).  in particular
                  epsneg = ibeta**negep
      xmin     is the smallest positive floating-point number.  in
                  particular,  xmin = ibeta ** minexp
      xmax     is the largest finite floating-point number.  in
                  particular   xmax = (1.0-epsneg) * ibeta ** maxexp
                  note - on some machines  xmax  will be only the
                  second, or perhaps third, largest number, being
                  too small by 1 or 2 units in the last digit of
                  the significand.

                                                                    }

   i , iz , j , k , mx : integer ;
   a , b , beta , betain , betam1 , one , y , z , zero : real ;

begin
   irnd := 1 ;
   one := ( irnd );
   a := one + one ;
   b := a ;
   zero := 0.0 ;
{
      determine ibeta,beta ala Malcolm
                                                                    }
   while ( ( ( a + one ) - a ) - one = zero ) do begin
      a := a + a ;
   end ;
   while ( ( a + b ) - a = zero ) do begin
      b := b + b ;
   end ;
   ibeta := trunc ( ( a + b ) - a );
   beta := ( ibeta );
   betam1 := beta - one ;
{
      determine irnd,ngrd,it
                                                                    }
   if ( ( a + betam1 ) - a = zero ) then irnd := 0 ;
   it := 0 ;
   a := one ;
   repeat begin
      it := it + 1 ;
      a := a * beta ;
   end until ( ( ( a + one ) - a ) - one <> zero ) ;
{
      determine negep, epsneg
                                                                    }
   negep := it + 3 ;
   a := one ;

   for i := 1 to negep do begin
      a := a / beta ;
   end ;

   while ( ( one - a ) - one = zero ) do begin
      a := a * beta ;
      negep := negep - 1 ;
   end ;
   negep := - negep ;
   epsneg := a ;
{
      determine machep, eps
                                                                    }
   machep := negep ;
   while ( ( one + a ) - one = zero ) do begin
      a := a * beta ;
      machep := machep + 1 ;
   end ;
   eps := a ;
{
      determine ngrd
                                                                    }
   ngrd := 0 ;
   if(( irnd = 0) and((( one + eps) * one - one) <> zero)) then
   ngrd := 1 ;
{
      determine iexp, minexp, xmin

      loop to determine largest i such that
          (1/beta) ** (2**(i))
      does not underflow
      exit from loop is signall by an underflow
                                                                    }
   i := 0 ;
   betain := one / beta ;
   z := betain ;
   trapped:=false;
   repeat begin
      y := z ;
      z := y * y ;
{
      check for underflow
                                                                    }
      i := i + 1 ;
   end until trapped;
   i := i - 1;
   k := 1 ;
{
      determine k such that (1/beta)**k does not underflow

      first set k = 2 ** i
                                                                    }

   for j := 1 to i do begin
      k := k + k ;
   end ;

   iexp := i + 1 ;
   mx := k + k ;
   if ( ibeta = 10 ) then begin
{
      for decimal machines only                                     }
      iexp := 2 ;
      iz := ibeta ;
      while ( k >= iz ) do begin
         iz := iz * ibeta ;
         iexp := iexp + 1 ;
      end ;
      mx := iz + iz - 1 ;
   end;
   trapped:=false;
   repeat begin
{
      loop to construct xmin
      exit from loop is signalled by an underflow
                                                                    }
      xmin := y ;
      y := y * betain ;
      k := k + 1 ;
   end until trapped;
   k := k - 1;
   minexp := - k ;
{  determine maxexp, xmax
                                                                    }
   if ( ( mx <= k + k - 3 ) and ( ibeta <> 10 ) ) then begin
      mx := mx + mx ;
      iexp := iexp + 1 ;
   end;
   maxexp := mx + minexp ;
{  adjust for machines with implicit leading
   bit in binary significand and machines with
   radix point at extreme right of significand
                                                                    }
   i := maxexp + minexp ;
   if ( ( ibeta = 2 ) and ( i = 0 ) ) then maxexp := maxexp - 1 ;
   if ( i > 20 ) then maxexp := maxexp - 3 ;
   xmax := one - epsneg ;
   if ( xmax * one <> xmax ) then xmax := one - beta * epsneg ;
   xmax := ( xmax * betain * betain * betain ) / xmin ;
   i := maxexp + minexp + 3 ;
   if  ( i > 0 ) then begin

      for j := 1 to i do begin
         xmax := xmax * beta ;
      end ;
   end;

end;

begin
  trapped:=false;
  encaps(work,catch);
end;
