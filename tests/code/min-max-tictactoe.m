meld fil                            	                _init -o node-axioms.A  build-next-plays(MV8, MV9, MV10, MV11, MV12, MV13, MV14, MV15) -o (MV11 = MV10, MV13 = 0 -o score(minimax_points2(MV8, MV12), MV14)), OR (MV13 > 0, MV11 = MV10, MV12 = MV9 -o maximize(MV13, -9999, 0)), OR (MV13 > 0, MV9 != MV12, MV11 = MV10 -o minimize(MV13, 9999, 0)), 
			OR (array_get(MV8, MV10) = 0 -o build-next-plays(MV8, MV9, MV10 + 1, MV11, MV12, MV13 + 1, MV14, MV15), exists B. (play(array_set(MV8, MV10, MV9), if (MV9 = 1) then 2 else 1 end, MV10, MV12, MV15 + 1)@B, 
			back(host-id)@B)), OR ( -o build-next-plays(MV8, MV9, MV10 + 1, MV11, MV12, MV13, MV14, MV15)).  play(Game, NextPlayer, LastPlay, RootPlayer, Depth), Score := minimax_score2(Game, NextPlayer, RootPlayer) -o if Score != 0 then score(Score, LastPlay), 
			 otherwise build-next-plays(Game, NextPlayer, 0, array_size(Game), RootPlayer, 0, LastPlay, Depth) end.@   score(Score, BestPlay), back(B) -o new-score(Score, BestPlay)@B.�    -o (new-score(Score, Play), minimize(N, Current, BestPlay), Current > Score -o minimize(N - 1, Score, Play)), OR (new-score(Score, Play), minimize(N, Current, BestPlay), Current <= Score -o minimize(N - 1, Current, BestPlay)).�    -o (new-score(Score, Play), maximize(N, Current, BestPlay), Current < Score -o maximize(N - 1, Score, Play)), OR (new-score(Score, Play), maximize(N, Current, BestPlay), Current >= Score -o maximize(N - 1, Current, BestPlay)).B   minimize(MV6, Score, BestPlay), MV6 = 0 -o score(Score, BestPlay).B   maximize(MV7, Score, BestPlay), MV7 = 0 -o score(Score, BestPlay).           �                   _init                                                                setcolor                                                             setedgelabel                                                         write-string                                                         setcolor2                                                             stop-program                                                         set-priority                                                         add-priority                                                          schedule-next                                                        set-default-priority                                                  set-moving                                                            set-static                                                           set-affinity                                                         set-cpu                                                               remove-priority                                                      play                                                                  root                                                                 build-next-plays                                                     back                                                                 score                                                                new-score                                                            maximize                                                             minimize                                                         ��������   �                    �    � 
�         @$   _     _     _     _     _     _    _    _    _    j< '              w      #         �         �                 �   " " ;`9   "     ;`%   @"  " j? & ! w� �"     C`E   " " ;`3   " " ;`!   @!  ����    w� �"     C`E   " " <`3   " " ;`!   @!  '      w� �"  " j9    ;`�   @"  " " k: ' "    ;�         �      ! ! "    =&@( "    =& "    =& { �"    =& { �        �                  �    "  " " k>    <�   V   @& ! w�9   @%   !     "  i= &!     ! ! w� �        K                  E               /    @!   ! " �� �        �                  _               I    ""  C`'   "    :& !  ! {� �            U               ?    ""  B`   "    :& {� �        �                  _               I    ""  >`'   "    :& !  ! {� �            U               ?    ""  ?`   "    :& {� �        4                  .        @!  ! w� �        4                  .        @!  ! w� �        