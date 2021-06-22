/*    prolog test file    */
/**************************/

% Your program goes here (https://swish.swi-prolog.org/)
% merlin (swipl) 
% % exit: halt.
% % load db: ['db']. (file: db.pl)
% %
/**************************/
girl(susan).
girl(ann).
boy(john).
boy(franta).
boy(pepa).
% girl(Girl).

/**************************/
likes(_Everybody,water).
likes(karl,beer).
likes(chris,juice).
% likes(X,juice).
% likes(paul,water).
% likes(karl,Y).
% likes(X,Y).

/**************************/
/*
L=[a,b,c,d,1,2,3,4].

[H|T]=[a,b,c,d].
[a,b,c,d]=[H|T].

[a,b,c,d]=[H1,H2|T].

[a,b,c,d]=[_,H2|T].

T=[b,c,d],L=[a|T].
*/
/**************************/
/*
X is 2 + 2.

Y = 5 + 2,
X is Y.
*/
/**************************/
count(FROM, TO) :-
	write(FROM), nl,
	( FROM < TO -> Y is FROM + 1, count(Y, TO) ; write("Done."), nl, false ).
	% non-standard prolog
% count(5,10).
count_to(TO) :-
	count(0,TO).
% count_to(10).
count_to_half(TO) :-
	count(0,TO/2).
% count_to_half(10).

/**************************/
is_div(X, Y):- 0 is mod(X,Y).
% is_div(15,5).

is_empty(X) :- X == [].
% is_empty([a]).

%[FIRST|TAIL]=[a,b,c,d].

/**************************/
% https://martinpilat.com/cs/neproceduralni-programovani/prolog-seznamy-ii-aritmetika
delka([], 0).
delka([_|TAIL], RETURN_LENGTH) :- 
    delka(TAIL, LENGTH), 
    RETURN_LENGTH is LENGTH + 1.

/**************************/
test(TRUE) :- write("FALSE "), TRUE == true, write(" TRUE").
% test(true).
% test(false).

/**************************/
test2(TRUE, 1) :- TRUE == true, !.
test2(_FALSE, 0) :- !.
% test2(true, RETURN).
% test2(false, RETURN).
