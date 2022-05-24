% Zadání è. 8:
% Napište program øešící úkol daný predikátem u8(LIN,VIN), kde LIN je vstupní 
% èíselný seznam s nejménì dvìma prvky a VIN je vstupní promìnná, jejíž 
% hodnotou musí být kladné èíslo. Predikát je pravdivý (má hodnotu true), pokud 
% absolutní hodnoty rozdílù mezi všemi sousedními èísly v seznamu LIN jsou menší
% než èíslo v promìnné VIN, jinak je predikát nepravdivý (má hodnotu false). 

% Testovací predikáty: 	                               	  
u8_1:- u8([5,6.2,4,2.1,-1,1],4).                         %  true
u8_2:- u8([5,-6.2,4,2,-1,1],3).                          %  false
u8_3:- u8([5.1,6,4,2,-1,1.2],2).                         %  false
u8_r:- write('Zadej LIN: '),read(LIN),
       write('Zadej VIN: '),read(VIN),
       u8(LIN,VIN).

u8(LIN,VIN):- loop(LIN, VIN).



loop([_], _):-
    %write("END"),
    !.
loop(LIN, VIN) :-
    [H1, H2|T] = LIN,
    %write("abs("), write(H1), write("-"), write(H2), write(")"),
    
    N is (H1 - H2),
    absN(N, R),
    R < VIN,
    
    loop([H2|T], VIN).


absN(N, R) :-
    N >= 0,
    %write("(P) = "), write(N), write("; "),
    R is N,
    !.
absN(N, R) :-
    NN is (N*(-1)),
    %write("(N) = "), write(NN), write("; "),
    R is NN,
    !.
