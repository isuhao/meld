meld fil       Q                                                                                         	       	                                   
       
                                                                                                                                                                                                                                                                                                                                           !       !       "       "       #       #       &       &       '       '       (       (       $       $       )       )       %       %       *       *       +       +       ,       ,       /       /       0       0       1       1       -       -       2       2       .       .       3       3       4       4       5       5       8       8       9       9       :       :       6       6       ;       ;       7       7       <       <       =       =       >       >       A       A       B       B       C       C       ?       ?       D       D       @       @       E       E       F       F       G       G       J       J       K       K       L       L       H       H       M       M       I       I       N       N       O       O       P       P        	                init -o axioms�   propagate-left(Nodes, Coords) -o test-and-send-down(Nodes, Coords), {L | !left(L), 
			L != host-id | propagate-left(Nodes, Coords)@L}.�   propagate-right(Nodes, Coords) -o test-and-send-down(Nodes, Coords), {R | !right(R), 
			R != host-id | propagate-right(Nodes, Coords)@R}.T   test-and-send-down(Nodes, Coords), !coord(X, Y) -o test-y(Y, Coords, Nodes, Coords).v   test-y(Y, MV3, Nodes, Coords), !coord(OX, OY), test-nil(MV3) -o test-diag-left(OX - 1, OY - 1, Coords, Nodes, Coords).�   test-y(MV24, MV25, MV26, MV27), MV29 = head(MV28), MV28 = tail(MV25), not(test-nil(MV25)), 
			not(test-nil(MV28)) -o (MV24 = MV29 -o 1), OR (RestCoords = tail(MV28), MV24 != MV29 -o test-y(MV24, RestCoords, MV26, MV27)).�   test-diag-left(X, Y, MV1, Nodes, Coords), !coord(OX, OY), (X < 0) || (Y < 0) -o test-diag-right(OX - 1, OY + 1, Coords, Nodes, Coords).5  test-diag-left(MV30, MV31, MV32, MV33, MV34), MV37 = head(MV36), MV36 = tail(MV32), MV35 = head(MV32), 
			not(test-nil(MV32)), not(test-nil(MV36)) -o (MV30 = MV35, MV31 = MV37 -o 1), OR (RestCoords = tail(MV36), (MV30 != MV35) || (MV31 != MV37) -o test-diag-left(MV30 - 1, MV31 - 1, RestCoords, MV33, MV34)).�   test-diag-right(X, Y, MV16, Nodes, Coords), !coord(OX, OY), (X < 0) || (Y >= 9), test-nil(MV16) -o 
			send-down(cons(host-id,Nodes), cons(OX,cons(OY,Coords))).7  test-diag-right(MV38, MV39, MV40, MV41, MV42), MV45 = head(MV44), MV44 = tail(MV40), MV43 = head(MV40), 
			not(test-nil(MV40)), not(test-nil(MV44)) -o (MV38 = MV43, MV39 = MV45 -o 1), OR (RestCoords = tail(MV44), (MV38 != MV43) || (MV39 != MV45) -o test-diag-right(MV38 - 1, MV39 + 1, RestCoords, MV41, MV42)).T   send-down(Nodes, Coords), !coord(MV23, MV2), MV23 = 8 -o final-state(Nodes, Coords).�   send-down(Nodes, Coords) -o {B | !down-right(B), B != host-id | 
			propagate-right(Nodes, Coords)@B}, {B | !down-left(B), B != host-id | 
			propagate-left(Nodes, Coords)@B}.           �                  _init                                                               set-priority                                                        setcolor                                                            setedgelabel                                                        write-string                                                        add-priority                                                         schedule-next                                                       setcolor2                                                            stop-program                                                        set-default-priority                                                 set-moving                                                           set-static                                                          set-affinity                                                        set-cpu                                                              remove-priority                                                      left                                                                 right                                                                down-right                                                           down-left                                                            coord                                                                propagate-left                                                       propagate-right                                                      test-and-send-down                                                    test-y                                                                test-diag-left                                                        test-diag-right                                                      send-down                                                            final-state                                                      ���������                                                                                                                                              �                   �   � 
�  Q      O   �   �     [  �  �  $  g  �  �  0  s  �  �  <    �    H  �  �    T  �  �    `  �  �  )  l  �  �  5	  x	  �	  �	  A
  �
  �
  
  M  �  �    Y  �  �  "  e  �  �  .  q  �  �  :  }  �    F  �  �    R  �  �    ^  �  �  '  j  �  �  3  v  �  �  .      �?�@p pw2                                         �  .      �?�2                                       �  .      �?�2                 	                     o  .      �?�2                 
                     ,  .      �?�2                                      �  .      �?�2                                      �  .      �?�2                                      c  .      �?�2                                         .      �?�2                                      �  .       @�2                 	       	       
       �  .       @�2                
       	              W  .       @�2                       
                .       @�2                                     �  .       @�2                                     �  .       @�2                                     K  .       @�2                                       .       @�2                                     �  .       @�2                                     �  .      @�2                                      ?  .      @�2                                     �  .      @�2                                     �  .      @�2                                      v  .      @�2         !                            3  .      @�2         "                            �  .      @�2         #                            �  .      @�2                                      j  .      @�2                !                     '  .      @�2          &                            �  .      @�2         '                            �  .      @�2         (       $                     ^  .      @�2         )       %                       .      @�2         *       &                      �  .      @�2         +       '              !       �  .      @�2         ,       (               "       R  .      @�2         "       )       !       #         .      @�2         #       *       "       #       �  .      @�2          /       $       $       %       �  .      @�2         0       %       $       &       F  .      @�2         1       -       %       '         .      @�2         2       .       &       (       �
  .      @�2         3       /       '       )       }
  .      @�2         4       0       (       *       :
  .      @�2         5       1       )       +       �	  .      @�2         +       2       *       ,       �	  .      @�2         ,       3       +       ,       q	  .      @�2          8       -       -       .       .	  .      @�2         9       .       -       /       �  .      @�2         :       6       .       0       �  .      @�2         ;       7       /       1       e  .      @�2         <       8       0       2       "  .      @�2         =       9       1       3       �  .      @�2         >       :       2       4       �  .      @�2         4       ;       3       5       Y  .      @�2         5       <       4       5         .      @�2          A       6       6       7       �  .      @�2         B       7       6       8       �  .      @�2         C       ?       7       9       M  .      @�2         D       @       8       :       
  .      @�2         E       A       9       ;       �  .      @�2         F       B       :       <       �  .      @�2         G       C       ;       =       A  .      @�2         =       D       <       >       �  .      @�2         >       E       =       >       �  .       @�2          J       ?       ?       @       x  .       @�2         K       @       ?       A       5  .       @�2         L       H       @       B       �  .       @�2         M       I       A       C       �  .       @�2         N       J       B       D       l  .       @�2         O       K       C       E       )  .       @�2         P       L       D       F       �  .       @�2         F       M       E       G       �  .       @�2         G       N       F       G       `  .      "@�2          H       H       H       I         .      "@�2         I       I       H       J       �  .      "@�2         J       J       I       K       �  .      "@�2         K       K       J       L       T  .      "@�2         L       L       K       M         .      "@�2         M       M       L       N       �   .      "@�2         N       N       M       O       �   .      "@�2         O       O       N       P       H   .      "@�2         P       P       O       P          #         � D  �  �  �  �  X  O  F  =        �  �  �  �  �  �  �  }  t  L  C  :  1  	     �  �  �  �  �  �  �  z  q  h  @  7  .  %  �  �  �  �  �  �  �  �  w  n  e  \  4  +  "    �  �  �  �  �  �  �  �  k  b  Y  P  (        �  �  �  �  �  �  �  �  _  V  M  D      
    �  �  �  �  �  �  �  {  S  J  A  8      �  �  �  �  �  �  �  �  x  o  G  >  5  ,    �  �  �  �  �  �  �  ~  u  l  c  ;  2  )     �  �  �  �  �  �  �  �  r  i  `  W  /  &      �  �  �  �  �  �  �  �  f  ]  T  K  #        �  �  �  �  �  �  �  �  Z  Q  H  ?        �
  �
  �
  �
  �
  �
  �
  
  v
  N
  E
  <
  3
  
  
  �	  �	  �	  �	  �	  �	  �	  |	  s	  j	  B	  9	  0	  '	  �  �  �  �  �  �  �  �  y  p  g  ^  6  -  $    �  �  �  �  �  �  �  �  m  d  [  R  *  !      �  �  �  �  �  �  �  �  a  X  O  F          �  �  �  �  �  �  �  }  U  L  C  :    	     �  �  �  �  �  �  �  z  q  I  @  7  .    �  �  �  �  �  �  �  �  w  n  e  =  4  +  "  �  �  �  �  �  �  �  �  t  k  b  Y  1  (      �  �  �  �  �  �  �  �       o                  i    @%   % wA              ;    " 78`   @%   % " �� �         o                  i    @%   % wA              ;    " 78`   @%   % " �� �         N                  H               2    @! % %  % w� �         o                  i              Q    @"    :& "   :&% % % w� �         P                  J   Z S"  ;`	   � �"  <`   Y' { �         �                  �    "      >"     >A`W              Q    @"    :& "   =&% % % w� �         �                  �   Z ST "  ;`   " ;`	   � �"  <" <A`4   Y"     :&  "    :& ' { �         �                  �   "      >" 	   ?A`M              G    @" 7_' " "_ " _ 'w� �         �   	               �   Z ST "  ;`   " ;`	   � �"  <" <A`4   Y"     :&  "    =& ' { �         J   
               D               .       @%   % w� �         �                  �    A              ;    " 78`   @%   % " �A              ;    " 78`   @%   % " �� �         