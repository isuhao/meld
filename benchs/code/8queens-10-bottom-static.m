meld fil       d                                                                                         
       
                                                                                                                                                                               	       	                                                                                                                                                                   !       !       "       "                     #       #                     $       $       %       %       &       &       '       '       *       *       +       +       ,       ,       (       (       -       -       )       )       .       .       /       /       0       0       1       1       4       4       5       5       6       6       2       2       7       7       3       3       8       8       9       9       :       :       ;       ;       >       >       ?       ?       @       @       <       <       A       A       =       =       B       B       C       C       D       D       E       E       H       H       I       I       J       J       F       F       K       K       G       G       L       L       M       M       N       N       O       O       R       R       S       S       T       T       P       P       U       U       Q       Q       V       V       W       W       X       X       Y       Y       \       \       ]       ]       ^       ^       Z       Z       _       _       [       [       `       `       a       a       b       b       c       c        	                init -o axioms�   propagate-left(Nodes, Coords) -o test-and-send-down(Nodes, Coords), {L | !left(L), 
			L != host-id | propagate-left(Nodes, Coords)@L}.�   propagate-right(Nodes, Coords) -o test-and-send-down(Nodes, Coords), {R | !right(R), 
			R != host-id | propagate-right(Nodes, Coords)@R}.T   test-and-send-down(Nodes, Coords), !coord(X, Y) -o test-y(Y, Coords, Nodes, Coords).v   test-y(Y, MV3, Nodes, Coords), !coord(OX, OY), test-nil(MV3) -o test-diag-left(OX - 1, OY - 1, Coords, Nodes, Coords).�   test-y(MV24, MV25, MV26, MV27), MV29 = head(MV28), MV28 = tail(MV25), not(test-nil(MV25)), 
			not(test-nil(MV28)) -o (MV24 = MV29 -o 1), OR (RestCoords = tail(MV28), MV24 != MV29 -o test-y(MV24, RestCoords, MV26, MV27)).�   test-diag-left(X, Y, MV1, Nodes, Coords), !coord(OX, OY), (X < 0) || (Y < 0) -o test-diag-right(OX - 1, OY + 1, Coords, Nodes, Coords).5  test-diag-left(MV30, MV31, MV32, MV33, MV34), MV37 = head(MV36), MV36 = tail(MV32), MV35 = head(MV32), 
			not(test-nil(MV32)), not(test-nil(MV36)) -o (MV30 = MV35, MV31 = MV37 -o 1), OR (RestCoords = tail(MV36), (MV30 != MV35) || (MV31 != MV37) -o test-diag-left(MV30 - 1, MV31 - 1, RestCoords, MV33, MV34)).�   test-diag-right(X, Y, MV16, Nodes, Coords), !coord(OX, OY), (X < 0) || (Y >= 10), test-nil(MV16) -o 
			send-down(cons(host-id,Nodes), cons(OX,cons(OY,Coords))).7  test-diag-right(MV38, MV39, MV40, MV41, MV42), MV45 = head(MV44), MV44 = tail(MV40), MV43 = head(MV40), 
			not(test-nil(MV40)), not(test-nil(MV44)) -o (MV38 = MV43, MV39 = MV45 -o 1), OR (RestCoords = tail(MV44), (MV38 != MV43) || (MV39 != MV45) -o test-diag-right(MV38 - 1, MV39 + 1, RestCoords, MV41, MV42)).T   send-down(Nodes, Coords), !coord(MV23, MV2), MV23 = 9 -o final-state(Nodes, Coords).�   send-down(Nodes, Coords) -o {B | !down-right(B), B != host-id | 
			propagate-right(Nodes, Coords)@B}, {B | !down-left(B), B != host-id | 
			propagate-left(Nodes, Coords)@B}.           �                  _init                                                               set-priority                                                        setcolor                                                            setedgelabel                                                        write-string                                                        add-priority                                                         schedule-next                                                       setcolor2                                                            stop-program                                                        set-default-priority                                                 set-moving                                                           set-static                                                          set-affinity                                                        set-cpu                                                              remove-priority                                                      left                                                                 right                                                                down-right                                                           down-left                                                            coord                                                                propagate-left                                                       propagate-right                                                      test-and-send-down                                                    test-y                                                                test-diag-left                                                        test-diag-right                                                      send-down                                                            final-state                                                      ���������                                                                                                                                              B)                   <)   � 
)  d      q   �   ;  �    j  �  4  �  �  c  �  -  �  �  \  �  &  �  �  U  �  	  �	  �	  N
  �
    }  �  G  �    v  �  @  �  
  o  �  9  �    h  �  2  �  �  a  �  +  �  �  Z  �  $  �  �  S  �    �  �  L  �    {  �  E  �    t  �  >  �    m  �  7  �     f   �   0!  �!  �!  _"  �"  )#  �#  �#  X$  �$  "%  �%  �%  Q&  �&  '          
   
    /�.      �?�@p pw2                                         '         
   
    /�.      �?�2                                       �&         
   
    /�.      �?�2                 
                     J&         
   
    /�.      �?�2                                      �%         
   
    /�.      �?�2                                      �%         
   
    /�.      �?�2                                      %         
   
    /�.      �?�2                                      �$         
   
    /�.      �?�2                                      Q$         
   
    /�.      �?�2                               	       �#      	   
   
    /�.      �?�2       	   	                     	       �#         
   
    /�.       @�2                 
       
              "#        
   
    /�.       @�2                       
              �"        
   
    /�.       @�2                                     X"        
   
    /�.       @�2                                     �!        
   
    /�.       @�2                                     �!        
   
    /�.       @�2                                     )!        
   
    /�.       @�2                                     �         
   
    /�.       @�2                                     _         
   
    /�.       @�2                                     �     	   
   
    /�.       @�2      	                               �         
   
    /�.      @�2                                       0        
   
    /�.      @�2         !                            �        
   
    /�.      @�2         "                            f        
   
    /�.      @�2         #                                    
   
    /�.      @�2         $                             �        
   
    /�.      @�2         %       !                     7        
   
    /�.      @�2         &       "                     �        
   
    /�.      @�2         '       #                     m        
   
    /�.      @�2                $                          	   
   
    /�.      @�2      	          %                     �         
   
    /�.      @�2          *                            >        
   
    /�.      @�2         +                             �        
   
    /�.      @�2         ,       (              !       t        
   
    /�.      @�2         -       )               "               
   
    /�.      @�2         .       *       !       #       �        
   
    /�.      @�2         /       +       "       $       E        
   
    /�.      @�2         0       ,       #       %       �        
   
    /�.      @�2         1       -       $       &       {        
   
    /�.      @�2         &       .       %       '            	   
   
    /�.      @�2      	   '       /       &       '       �         
   
    /�.      @�2          4       (       (       )       L        
   
    /�.      @�2         5       )       (       *       �        
   
    /�.      @�2         6       2       )       +       �        
   
    /�.      @�2         7       3       *       ,               
   
    /�.      @�2         8       4       +       -       �        
   
    /�.      @�2         9       5       ,       .       S        
   
    /�.      @�2         :       6       -       /       �        
   
    /�.      @�2         ;       7       .       0       �        
   
    /�.      @�2         0       8       /       1       $     	   
   
    /�.      @�2      	   1       9       0       1       �         
   
    /�.      @�2          >       2       2       3       Z        
   
    /�.      @�2         ?       3       2       4       �        
   
    /�.      @�2         @       <       3       5       �        
   
    /�.      @�2         A       =       4       6       +        
   
    /�.      @�2         B       >       5       7       �        
   
    /�.      @�2         C       ?       6       8       a        
   
    /�.      @�2         D       @       7       9       �        
   
    /�.      @�2         E       A       8       :       �        
   
    /�.      @�2         :       B       9       ;       2     	   
   
    /�.      @�2      	   ;       C       :       ;       �         
   
    /�.      @�2          H       <       <       =       h        
   
    /�.      @�2         I       =       <       >               
   
    /�.      @�2         J       F       =       ?       �        
   
    /�.      @�2         K       G       >       @       9        
   
    /�.      @�2         L       H       ?       A       �        
   
    /�.      @�2         M       I       @       B       o        
   
    /�.      @�2         N       J       A       C       
        
   
    /�.      @�2         O       K       B       D       �        
   
    /�.      @�2         D       L       C       E       @     	   
   
    /�.      @�2      	   E       M       D       E       �         
   
    /�.       @�2          R       F       F       G       v        
   
    /�.       @�2         S       G       F       H               
   
    /�.       @�2         T       P       G       I       �
        
   
    /�.       @�2         U       Q       H       J       G
        
   
    /�.       @�2         V       R       I       K       �	        
   
    /�.       @�2         W       S       J       L       }	        
   
    /�.       @�2         X       T       K       M       	        
   
    /�.       @�2         Y       U       L       N       �        
   
    /�.       @�2         N       V       M       O       N     	   
   
    /�.       @�2      	   O       W       N       O       �         
   
    /�.      "@�2          \       P       P       Q       �        
   
    /�.      "@�2         ]       Q       P       R               
   
    /�.      "@�2         ^       Z       Q       S       �        
   
    /�.      "@�2         _       [       R       T       U        
   
    /�.      "@�2         `       \       S       U       �        
   
    /�.      "@�2         a       ]       T       V       �        
   
    /�.      "@�2         b       ^       U       W       &        
   
    /�.      "@�2         c       _       V       X       �        
   
    /�.      "@�2         X       `       W       Y       \     	   
   
    /�.      "@�2      	   Y       a       X       Y       �  	       
   
    /�.      $@�2   	       Z       Z       Z       [       �  	      
   
    /�.      $@�2   	      [       [       Z       \       -  	      
   
    /�.      $@�2   	      \       \       [       ]       �  	      
   
    /�.      $@�2   	      ]       ]       \       ^       c  	      
   
    /�.      $@�2   	      ^       ^       ]       _       �  	      
   
    /�.      $@�2   	      _       _       ^       `       �  	      
   
    /�.      $@�2   	      `       `       _       a       4  	      
   
    /�.      $@�2   	      a       a       `       b       �   	      
   
    /�.      $@�2   	      b       b       a       c       j   	   	   
   
    /�.      $@�2   	   	   c       c       b       c          #         � �  ()  )  )  )  �(  �(  �(  �(  ^(  U(  L(  C(  �'  �'  �'  �'  �'  �'  �'  y'  /'  &'  '  '  �&  �&  �&  �&  e&  \&  S&  J&   &  �%  �%  �%  �%  �%  �%  �%  6%  -%  $%  %  �$  �$  �$  �$  l$  c$  Z$  Q$  $  �#  �#  �#  �#  �#  �#  �#  =#  4#  +#  "#  �"  �"  �"  �"  s"  j"  a"  X"  "  "  �!  �!  �!  �!  �!  �!  D!  ;!  2!  )!  �   �   �   �   z   q   h   _            �  �  �  �  �  K  B  9  0  �  �  �  �  �  x  o  f      
    �  �  �  �  R  I  @  7  �  �  �  �  �    v  m  #        �  �  �  �  Y  P  G  >  �  �  �  �  �  �  }  t  *  !      �  �  �  �  `  W  N  E  �  �  �  �  �  �  �  {  1  (      �  �  �  �  g  ^  U  L    �  �  �  �  �  �  �  8  /  &    �  �  �  �  n  e  \  S  	     �  �  �  �  �  �  ?  6  -  $  �  �  �  �  u  l  c  Z      �  �  �  �  �  �  F  =  4  +  �  �  �  �  |  s  j  a        �  �  �  �  �  M  D  ;  2  �  �  �  �  �  z  q  h          �  �  �  �  T  K  B  9  �  �  �  �  �  �  x  o  %      
  �  �  �  �  [  R  I  @  �  �  �  �  �  �    v  ,  #      �
  �
  �
  �
  b
  Y
  P
  G
  �	  �	  �	  �	  �	  �	  �	  }	  3	  *	  !	  	  �  �  �  �  i  `  W  N    �  �  �  �  �  �  �  :  1  (    �  �  �  �  p  g  ^  U      �  �  �  �  �  �  A  8  /  &  �  �  �  �  w  n  e  \    	     �  �  �  �  �  H  ?  6  -  �  �  �  �  ~  u  l  c        �       o                  i    @%   % wA              ;    " 78`   @%   % " �� �         o                  i    @%   % wA              ;    " 78`   @%   % " �� �         N                  H               2    @! % %  % w� �         o                  i              Q    @"    :& "   :&% % % w� �         P                  J   Z S"  ;`	   � �"  <`   Y' { �         �                  �    "      >"     >A`W              Q    @"    :& "   =&% % % w� �         �                  �   Z ST "  ;`   " ;`	   � �"  <" <A`4   Y"     :&  "    :& ' { �         �                  �   "      >" 
   ?A`M              G    @" 7_' " "_ " _ 'w� �         �   	               �   Z ST "  ;`   " ;`	   � �"  <" <A`4   Y"     :&  "    =& ' { �         J   
               D               .    	   @%   % w� �         �                  �    A              ;    " 78`   @%   % " �A              ;    " 78`   @%   % " �� �         