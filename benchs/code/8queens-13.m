meld fil       ©                                                                                                                                                                                                                                                                                             	       	                     
       
                                                                                                                                                     !       !       "       "       #       #       $       $       %       %       &       &       )       )       *       *       +       +       '       '       ,       ,       (       (       -       -       .       .       /       /       0       0       1       1       2       2       3       3       6       6       7       7       8       8       4       4       9       9       5       5       :       :       ;       ;       <       <       =       =       >       >       ?       ?       @       @       C       C       D       D       E       E       A       A       F       F       B       B       G       G       H       H       I       I       J       J       K       K       L       L       M       M       P       P       Q       Q       R       R       N       N       S       S       O       O       T       T       U       U       V       V       W       W       X       X       Y       Y       Z       Z       ]       ]       ^       ^       _       _       [       [       `       `       \       \       a       a       b       b       c       c       d       d       e       e       f       f       g       g       j       j       k       k       l       l       h       h       m       m       i       i       n       n       o       o       p       p       q       q       r       r       s       s       t       t       w       w       x       x       y       y       u       u       z       z       v       v       {       {       |       |       }       }       ~       ~                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ¡       ¡                     ¢       ¢       £       £       ¤       ¤       ¥       ¥       ¦       ¦       §       §       ¨       ¨        	                init -o axioms   propagate-left(Nodes, Coords) -o test-and-send-down(Nodes, Coords), {L | !left(L), 
			L != host-id | propagate-left(Nodes, Coords)@L}.   propagate-right(Nodes, Coords) -o test-and-send-down(Nodes, Coords), {R | !right(R), 
			R != host-id | propagate-right(Nodes, Coords)@R}.T   test-and-send-down(Nodes, Coords), !coord(X, Y) -o test-y(Y, Coords, Nodes, Coords).v   test-y(Y, MV3, Nodes, Coords), !coord(OX, OY), test-nil(MV3) -o test-diag-left(OX - 1, OY - 1, Coords, Nodes, Coords).Ý   test-y(MV24, MV25, MV26, MV27), MV29 = head(MV28), MV28 = tail(MV25), not(test-nil(MV25)), 
			not(test-nil(MV28)) -o (MV24 = MV29 -o 1), OR (RestCoords = tail(MV28), MV24 != MV29 -o test-y(MV24, RestCoords, MV26, MV27)).   test-diag-left(X, Y, MV1, Nodes, Coords), !coord(OX, OY), (X < 0) || (Y < 0) -o test-diag-right(OX - 1, OY + 1, Coords, Nodes, Coords).5  test-diag-left(MV30, MV31, MV32, MV33, MV34), MV37 = head(MV36), MV36 = tail(MV32), MV35 = head(MV32), 
			not(test-nil(MV32)), not(test-nil(MV36)) -o (MV30 = MV35, MV31 = MV37 -o 1), OR (RestCoords = tail(MV36), (MV30 != MV35) || (MV31 != MV37) -o test-diag-left(MV30 - 1, MV31 - 1, RestCoords, MV33, MV34)).¡   test-diag-right(X, Y, MV16, Nodes, Coords), !coord(OX, OY), (X < 0) || (Y >= 13), test-nil(MV16) -o 
			send-down(cons(host-id,Nodes), cons(OX,cons(OY,Coords))).7  test-diag-right(MV38, MV39, MV40, MV41, MV42), MV45 = head(MV44), MV44 = tail(MV40), MV43 = head(MV40), 
			not(test-nil(MV40)), not(test-nil(MV44)) -o (MV38 = MV43, MV39 = MV45 -o 1), OR (RestCoords = tail(MV44), (MV38 != MV43) || (MV39 != MV45) -o test-diag-right(MV38 - 1, MV39 + 1, RestCoords, MV41, MV42)).U   send-down(Nodes, Coords), !coord(MV23, MV2), MV23 = 12 -o final-state(Nodes, Coords).¯   send-down(Nodes, Coords) -o {B | !down-right(B), B != host-id | 
			propagate-right(Nodes, Coords)@B}, {B | !down-left(B), B != host-id | 
			propagate-left(Nodes, Coords)@B}.           ð                  _init                                                               set-priority                                                        setcolor                                                            setedgelabel                                                        write-string                                                        add-priority                                                         schedule-next                                                       setcolor2                                                            stop-program                                                        set-default-priority                                                 set-moving                                                           set-static                                                          set-affinity                                                        set-cpu                                                              remove-priority                                                      left                                                                 right                                                                down-right                                                           down-left                                                            coord                                                                propagate-left                                                       propagate-right                                                      test-and-send-down                                                    test-y                                                                test-diag-left                                                        test-diag-right                                                      send-down                                                            final-state                                                      ÿÿÿÿÿÿïÿÐ                                                                                                                                              £=                   =    
y=  ©      e   ¾     p  É  "  {  Ô  -    ß  8    ê  C    õ  N  §     Y  ²    d  ½  	  o	  È	  !
  z
  Ó
  ,    Þ  7    é  B    ô  M  ¦  ÿ  X  ±  
  c  ¼    n  Ç     y  Ò  +    Ý  6    è  A    ó  L  ¥  þ  W  °  	  b  »    m  Æ    x  Ñ  *    Ü  5    ç  @    ò  K  ¤  ý  V  ¯     a   º   !  l!  Å!  "  w"  Ð"  )#  #  Û#  4$  $  æ$  ?%  %  ñ%  J&  £&  ü&  U'  ®'  (  `(  ¹(  )  k)  Ä)  *  v*  Ï*  (+  +  Ú+  3,  ,  å,  >-  -  ð-  I.  ¢.  û.  T/  ­/  0  _0  ¸0  1  j1  Ã1  2  u2  Î2  '3  3  Ù3  24  4  ä4  =5  5  ï5  H6  ¡6  ú6  S7  ¬7  8  ^8  ·8  9  i9  Â9  :  t:                 /­@p pw2                                         m:                /­2                                       :                /­2                                      »9                /­2                                      b9                /­2                                      	9                /­2                                      °8                /­2                                      W8                /­2                                      þ7                /­2                               	       ¥7      	          /­2       	                        
       L7      
          /­2       
                 	              ó6                /­2                        
              6                /­2                                      A6                /­2                                      è5               /­2                                     5               /­2                                     65               /­2                                     Ý4               /­2                                      4               /­2         !                            +4               /­2         "                            Ò3               /­2         #                            y3               /­2         $                              3     	          /­2      	   %       !                     Ç2     
          /­2      
   &       "                     n2               /­2                #                     2               /­2                $                     ¼1                /­2          )                            c1               /­2         *                            
1               /­2         +       '                     ±0               /­2         ,       (                     X0               /­2         -       )                     ÿ/               /­2         .       *                      ¦/               /­2         /       +              !       M/               /­2         0       ,               "       ô.               /­2         1       -       !       #       .     	          /­2      	   2       .       "       $       B.     
          /­2      
   3       /       #       %       é-               /­2         %       0       $       &       -               /­2         &       1       %       &       7-                /­2          6       '       '       (       Þ,               /­2         7       (       '       )       ,               /­2         8       4       (       *       ,,               /­2         9       5       )       +       Ó+               /­2         :       6       *       ,       z+               /­2         ;       7       +       -       !+               /­2         <       8       ,       .       È*               /­2         =       9       -       /       o*               /­2         >       :       .       0       *     	          /­2      	   ?       ;       /       1       ½)     
          /­2      
   @       <       0       2       d)               /­2         2       =       1       3       )               /­2         3       >       2       3       ²(                /­2          C       4       4       5       Y(               /­2         D       5       4       6        (               /­2         E       A       5       7       §'               /­2         F       B       6       8       N'               /­2         G       C       7       9       õ&               /­2         H       D       8       :       &               /­2         I       E       9       ;       C&               /­2         J       F       :       <       ê%               /­2         K       G       ;       =       %     	          /­2      	   L       H       <       >       8%     
          /­2      
   M       I       =       ?       ß$               /­2         ?       J       >       @       $               /­2         @       K       ?       @       -$                /­2          P       A       A       B       Ô#               /­2         Q       B       A       C       {#               /­2         R       N       B       D       "#               /­2         S       O       C       E       É"               /­2         T       P       D       F       p"               /­2         U       Q       E       G       "               /­2         V       R       F       H       ¾!               /­2         W       S       G       I       e!               /­2         X       T       H       J       !     	          /­2      	   Y       U       I       K       ³      
          /­2      
   Z       V       J       L       Z                /­2         L       W       K       M                       /­2         M       X       L       M       ¨                /­2          ]       N       N       O       O               /­2         ^       O       N       P       ö               /­2         _       [       O       Q                      /­2         `       \       P       R       D               /­2         a       ]       Q       S       ë               /­2         b       ^       R       T                      /­2         c       _       S       U       9               /­2         d       `       T       V       à               /­2         e       a       U       W            	          /­2      	   f       b       V       X       .     
          /­2      
   g       c       W       Y       Õ               /­2         Y       d       X       Z       |               /­2         Z       e       Y       Z       #                /­2          j       [       [       \       Ê               /­2         k       \       [       ]       q               /­2         l       h       \       ^                      /­2         m       i       ]       _       ¿               /­2         n       j       ^       `       f               /­2         o       k       _       a                      /­2         p       l       `       b       ´               /­2         q       m       a       c       [               /­2         r       n       b       d            	          /­2      	   s       o       c       e       ©     
          /­2      
   t       p       d       f       P               /­2         f       q       e       g       ÷               /­2         g       r       f       g                       /­2          w       h       h       i       E               /­2         x       i       h       j       ì               /­2         y       u       i       k                      /­2         z       v       j       l       :               /­2         {       w       k       m       á               /­2         |       x       l       n                      /­2         }       y       m       o       /               /­2         ~       z       n       p       Ö               /­2                {       o       q       }     	          /­2      	          |       p       r       $     
          /­2      
          }       q       s       Ë               /­2         s       ~       r       t       r               /­2         t              s       t         	              /­2   	              u       u       v       À  	             /­2   	             v       u       w       g  	             /­2   	                    v       x         	             /­2   	                    w       y       µ  	             /­2   	                    x       z       \  	             /­2   	                    y       {         	             /­2   	                    z       |       ª  	             /­2   	                    {       }       Q  	             /­2   	                    |       ~       ø  	   	          /­2   	   	                 }                	   
          /­2   	   
                 ~              F  	             /­2   	                                  í  	             /­2   	                                    
              /­2   
                                   ;  
             /­2   
                                  â  
             /­2   
                                    
             /­2   
                                  0  
             /­2   
                                  ×  
             /­2   
                                  ~  
             /­2   
                                  %  
             /­2   
                                  Ì
  
             /­2   
                                  s
  
   	          /­2   
   	                               
  
   
          /­2   
   
                               Á	  
             /­2   
                                  h	  
             /­2   
                                  	                /­2                                      ¶               /­2                                     ]               /­2                                                     /­2         ¡                            «               /­2         ¢                            R               /­2         £                            ù               /­2         ¤                                             /­2         ¥       ¡                     G               /­2         ¦       ¢                     î     	          /­2      	   §       £                          
          /­2      
   ¨       ¤                     <               /­2                ¥                     ã               /­2                ¦                                     /­2                                      1               /­2                                     Ø               /­2                                                    /­2                                      &               /­2                                ¡       Í               /­2         ¡       ¡               ¢       t               /­2         ¢       ¢       ¡       £                      /­2         £       £       ¢       ¤       Â               /­2         ¤       ¤       £       ¥       i     	          /­2      	   ¥       ¥       ¤       ¦            
          /­2      
   ¦       ¦       ¥       §       ·                /­2         §       §       ¦       ¨       ^                /­2         ¨       ¨       §       ¨          #         ð ¤  =  =  w=  n=  0=  '=  =  =  ×<  Î<  Å<  ¼<  ~<  u<  l<  c<  %<  <  <  
<  Ì;  Ã;  º;  ±;  s;  j;  a;  X;  ;  ;  ;  ÿ:  Á:  ¸:  ¯:  ¦:  h:  _:  V:  M:  :  :  ý9  ô9  ¶9  ­9  ¤9  9  ]9  T9  K9  B9  9  û8  ò8  é8  «8  ¢8  8  8  R8  I8  @8  78  ù7  ð7  ç7  Þ7   7  7  7  7  G7  >7  57  ,7  î6  å6  Ü6  Ó6  6  6  6  z6  <6  36  *6  !6  ã5  Ú5  Ñ5  È5  5  5  x5  o5  15  (5  5  5  Ø4  Ï4  Æ4  ½4  4  v4  m4  d4  &4  4  4  4  Í3  Ä3  »3  ²3  t3  k3  b3  Y3  3  3  	3   3  Â2  ¹2  °2  §2  i2  `2  W2  N2  2  2  þ1  õ1  ·1  ®1  ¥1  1  ^1  U1  L1  C1  1  ü0  ó0  ê0  ¬0  £0  0  0  S0  J0  A0  80  ú/  ñ/  è/  ß/  ¡/  /  /  /  H/  ?/  6/  -/  ï.  æ.  Ý.  Ô.  .  .  .  {.  =.  4.  +.  ".  ä-  Û-  Ò-  É-  -  -  y-  p-  2-  )-   -  -  Ù,  Ð,  Ç,  ¾,  ,  w,  n,  e,  ',  ,  ,  ,  Î+  Å+  ¼+  ³+  u+  l+  c+  Z+  +  +  
+  +  Ã*  º*  ±*  ¨*  j*  a*  X*  O*  *  *  ÿ)  ö)  ¸)  ¯)  ¦)  )  _)  V)  M)  D)  )  ý(  ô(  ë(  ­(  ¤(  (  (  T(  K(  B(  9(  û'  ò'  é'  à'  ¢'  '  '  '  I'  @'  7'  .'  ð&  ç&  Þ&  Õ&  &  &  &  |&  >&  5&  ,&  #&  å%  Ü%  Ó%  Ê%  %  %  z%  q%  3%  *%  !%  %  Ú$  Ñ$  È$  ¿$  $  x$  o$  f$  ($  $  $  $  Ï#  Æ#  ½#  ´#  v#  m#  d#  [#  #  #  #  #  Ä"  »"  ²"  ©"  k"  b"  Y"  P"  "  	"   "  ÷!  ¹!  °!  §!  !  `!  W!  N!  E!  !  þ   õ   ì   ®   ¥         U   L   C   :   ü  ó  ê  á  £        J  A  8  /  ñ  è  ß  Ö        }  ?  6  -  $  æ  Ý  Ô  Ë      {  r  4  +  "    Û  Ò  É  À    y  p  g  )         Ð  Ç  ¾  µ  w  n  e  \          Å  ¼  ³  ª  l  c  Z  Q    
    ø  º  ±  ¨    a  X  O  F    ÿ  ö  í  ¯  ¦      V  M  D  ;  ý  ô  ë  â  ¤        K  B  9  0  ò  é  à  ×        ~  @  7  .  %  ç  Þ  Õ  Ì      |  s  5  ,  #    Ü  Ó  Ê  Á    z  q  h  *  !      Ñ  È  ¿  ¶  x  o  f  ]          Æ  ½  ´  «  m  d  [  R        ù  »  ²  ©     b  Y  P  G  	     ÷  î  °  §      W  N  E  <  þ  õ  ì  ã  ¥        L  C  :  1  ó  ê  á  Ø          A  8  /  &  è  ß  Ö  Í      }  t  6  -  $    Ý  Ô  Ë  Â    {  r  i  +  "      Ò  É  À  ·  y  p  g  ^           Ç
  ¾
  µ
  ¬
  n
  e
  \
  S
  
  
  
  ú	  ¼	  ³	  ª	  ¡	  c	  Z	  Q	  H	  
	  	  ø  ï  ±  ¨      X  O  F  =  ÿ  ö  í  ä  ¦        M  D  ;  2  ô  ë  â  Ù          B  9  0  '  é  à  ×  Î      ~  u  7  .  %    Þ  Õ  Ì  Ã    |  s  j  ,  #      Ó  Ê  Á  ¸  z  q  h  _  !             o                  i    @%   % wA              ;    " 78`   @%   % " ð ð         o                  i    @%   % wA              ;    " 78`   @%   % " ð ð         N                  H               2    @! % %  % w ð         o                  i              Q    @"    :& "   :&% % % w ð         P                  J   Z S"  ;`	    ð"  <`   Y' { ð                               "      >"     >A`W              Q    @"    :& "   =&% % % w ð                              Z ST "  ;`   " ;`	    ð"  <" <A`4   Y"     :&  "    :& ' { ð                              "      >"    ?A`M              G    @" 7_' " "_ " _ 'w ð            	                  Z ST "  ;`   " ;`	    ð"  <" <A`4   Y"     :&  "    =& ' { ð         J   
               D               .       @%   % w ð         ¡                      A              ;    " 78`   @%   % " ðA              ;    " 78`   @%   % " ð ð         