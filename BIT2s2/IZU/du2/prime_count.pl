/* prime count - pocet prvocisel */
/*********************************/

is_div(X, Y):- 0 is mod(X,Y).
% is_div(15,5).

is_empty(X) :- X == [].
% is_empty([a]).

is_prime(FROM, TO, NUMBER, POCET, COUNT) :-
    % if the number is greater than 1
    ( NUMBER > 1 ->
    	% check if number is dividible
        ( is_div(NUMBER,FROM) -> 
            COUNT is POCET
            % return false
        ;
        	% still numbers to check?
            ( FROM =< TO ->
                Y is FROM + 1,
                is_prime(Y, TO, NUMBER, POCET, COUNT)
            	% try next number
            ;
                COUNT is POCET + 1
                % return true 
            )
        )
    ;
    	COUNT is POCET
    	% return false
    ).

reseni(SEZNAM, POCET, RETURN_POCET) :- 
    [FIRST|TAIL]=SEZNAM,
    
    write(FIRST), write(" = "),
    is_prime(2,FIRST/2,FIRST, POCET, COUNT),
    (POCET \== COUNT -> write("YES"), nl ; write("NO"), nl ),
    
    ( is_empty(TAIL) ->
    	RETURN_POCET is COUNT
    ; 
    	reseni(TAIL, COUNT, POCET_NEW),
    	RETURN_POCET is POCET_NEW
    ).


pocet_prvocisel(SEZNAM, POCET) :-
    % SEZNAM je seznam čísel typu Integer z intervalu <1..20>
	% POCET je počet prvočísel v seznamu
    reseni(SEZNAM, 0, POCET).

% uloha29([1,2,3,4,5,6,7,8,9],POCET).
% uloha29([1,17,3,11,20,15,19,8,9],POCET).
