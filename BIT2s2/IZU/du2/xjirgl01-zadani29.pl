/*
*    Popis: IZU - projekt 2
*    Datum: 2021-04-01
*    Login: xjirgl01
*/

prime(2).
prime(3).
prime(5).
prime(7).
prime(11).
prime(13).
prime(17).
prime(19).

is_prime(PRIME_NUMBER, 1) :- prime(PRIME_NUMBER), !.
is_prime(_NOT_PRIME_NUMBER, 0) :- !.

prime_count([], CURRENT_POCET, RETURN_POCET) :- RETURN_POCET is CURRENT_POCET, !.
prime_count(SEZNAM, CURRENT_POCET, RETURN_POCET) :-
    [FIRST|TAIL] = SEZNAM,
    is_prime(FIRST, IS_PRIME),
    NEW_POCET is CURRENT_POCET + IS_PRIME,
    prime_count(TAIL, NEW_POCET, RETURN_POCET).

uloha29(SEZNAM, POCET) :-
    % SEZNAM je seznam cisel typu Integer z intervalu <1..20>
    % POCET je pocet prvocisel v seznamu
    prime_count(SEZNAM, 0, POCET).

% uloha29([1,2,3,4,5,6,7,8,9],POCET).
% uloha29([1,2,17,3,11,20,15,19,8,9],POCET).
