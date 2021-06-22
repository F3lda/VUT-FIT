<?php
//php7.4
/*
* @file parse.php
* 
* @brief VUT FIT IPP projekt 2020/2021
* @date 2021-03-01
* @author F3lda
* @update 2021-03-16
*/
//-------------------------
// everything to STDERR
ini_set('display_errors', 'stderr');
//fwrite(STDERR, "Some debug infos".PHP_EOL);



//-------------------------
// open files
// - 11 - error when opening input files (not found, permissions)
// - 12 - error when opening output files for writing (permissions, writing error)
if(fstat(STDIN) === false){
	fwrite(STDERR, 'INTERNAL ERROR - unable to read STDIN'.PHP_EOL);
	exit(99);
}
if(fstat(STDOUT) === false){
	fwrite(STDERR, 'INTERNAL ERROR - unable to write to STDOUT'.PHP_EOL);
	exit(99);
}
if(fstat(STDERR) === false){
	fwrite(STDERR, 'INTERNAL ERROR - unable to write to STDERR'.PHP_EOL);
	exit(99);
}



//-------------------------
// check arg --help -> print help and exit 0
// - 10 - missing parameter of script (if necessary) or forbidden combination of parameters
if($argc > 1){
	if($argc == 2 && $argv[1] == "--help"){
		echo 'Analyzator kodu v IPPcode21 (parse.php)'.PHP_EOL;
		echo '---------------------------------------'.PHP_EOL;
		echo 'Skript typu filtr (parse.php v jazyce PHP 7.4) nacte ze standardniho vstupu zdrojovy kod v IPP-code21, '.PHP_EOL;
		echo 'zkontroluje lexikalni a syntaktickou spravnost kodu a vypise na standardni vystup XML reprezentaci programu.'.PHP_EOL;
		echo ''.PHP_EOL;
		echo 'Tento skript pracuje s temito parametry:'.PHP_EOL;
		echo '--help - vypise na standardni vystup napovedu skriptu (tento parametr nelze kombinovat s zadnym dalsim parametrem)'.PHP_EOL;
		echo ''.PHP_EOL;
		echo 'Chybove navratove kody:'.PHP_EOL;
		echo '10 - chybejici parametr skriptu (je-li treba) nebo pouziti zakazane kombinace parametru;'.PHP_EOL;
		echo '11 - chyba pri otevirani vstupnich souboru (napr. neexistence, nedostatecne opravneni);'.PHP_EOL;
		echo '12 - chyba pri otevrení vystupnich souboru pro zapis (napr. nedostatecne opravneni, chyba pri zapisu);'.PHP_EOL;
		echo '21 - chybna nebo chybejici hlavicka ve zdrojovem kodu zapsanem v IPPcode21;'.PHP_EOL;
		echo '22 - neznamy nebo chybny operacni kod ve zdrojovem kodu zapsanem v IPPcode21;'.PHP_EOL;
		echo '23 - jina lexikalni nebo syntakticka chyba zdrojoveho kodu zapsaneho v IPPcode21;'.PHP_EOL;
		echo '99 - interni chyba (neovlivnena vstupnimi soubory ci parametry prikazove radky; napr. chyba alokace pameti);'.PHP_EOL;
		echo ''.PHP_EOL;
		exit(0);
	} else {
		fwrite(STDERR, 'ERROR - forbidden combination of parameters'.PHP_EOL);
		exit(10);
	}
}



//-------------------------
// read and trim() stdin line by line until:
// ".IPPcode21" is first non-empty line - case insensitive !!!
// - 21 - missing or wrong header in source code
$line_number = 1;
while(($line = fgets(STDIN, 1024)) !== false)
{
	// couldn't read whole line
	if(strpos($line, "\n") === false && strpos($line, "\r\n") === false && strpos($line, "\r") === false){
		fwrite(STDERR, 'INTERNAL ERROR - code line is too long (max 1024 chars) [line: '.$line_number.']'.PHP_EOL);
		exit(99);
	}
	
	// remove EOL chars and set str to UPPERCASE
	$line = strtoupper(trim($line));
	
	// header found OK
	if($line === strtoupper(".IPPcode21")){
		// add line number
		$line_number++;
		break;
	// wrong header
	} else if($line !== ""){
		fwrite(STDERR, 'ERROR - missing or wrong header in source code [line: '.$line_number.']'.PHP_EOL);
		exit(21);
	}
	
	// add line number
	$line_number++;
}
// check EOF
if(feof(STDIN)){
	fwrite(STDERR, 'ERROR - missing or wrong header in source code [line: '.$line_number.']'.PHP_EOL);
	exit(21);
}



//-------------------------
// arg types
define('ARG_TYPE_VAR', 1); // variable
define('ARG_TYPE_SYMB', 2); // constant or variable
define('ARG_TYPE_LABEL', 3); // label
define('ARG_TYPE_TYPE', 4); // var type

$INSTRUCTIONS = [
	'MOVE' => [ARG_TYPE_VAR, ARG_TYPE_SYMB],
	'CREATEFRAME' => [],
	'PUSHFRAME' => [],
	'POPFRAME' => [],
	'DEFVAR' => [ARG_TYPE_VAR],
	'CALL' => [ARG_TYPE_LABEL],
	'RETURN' => [],
	
	'PUSHS' => [ARG_TYPE_SYMB],
	'POPS' => [ARG_TYPE_VAR],
	
	'ADD' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'SUB' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'MUL' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'IDIV' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'LT' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'GT' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'EQ' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'AND' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'OR' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'NOT' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'INT2CHAR' => [ARG_TYPE_VAR, ARG_TYPE_SYMB],
	'STRI2INT' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	
	'READ' => [ARG_TYPE_VAR, ARG_TYPE_TYPE],
	'WRITE' => [ARG_TYPE_SYMB],
	
	'CONCAT' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'STRLEN' => [ARG_TYPE_VAR, ARG_TYPE_SYMB],
	'GETCHAR' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'SETCHAR' => [ARG_TYPE_VAR, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	
	'TYPE' => [ARG_TYPE_VAR, ARG_TYPE_SYMB],
	
	'LABEL' => [ARG_TYPE_LABEL],
	'JUMP' => [ARG_TYPE_LABEL],
	'JUMPIFEQ' => [ARG_TYPE_LABEL, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'JUMPIFNEQ' => [ARG_TYPE_LABEL, ARG_TYPE_SYMB, ARG_TYPE_SYMB],
	'EXIT' => [ARG_TYPE_SYMB],
	
	'DPRINT' => [ARG_TYPE_SYMB],
	'BREAK' => []
];



//-------------------------
// functions for arg type check
function is_var($arg)
{
	// check if no white chars
	if(preg_match_all('/\s+/', $arg, $matches) === 0){
		$parts = explode('@', $arg);
		// only one @ (at)
		if(count($parts) === 2){
			// left part -> LF or TF or GF
			if($parts[0] === 'LF' || $parts[0] === 'TF' || $parts[0] === 'GF'){
				// right part -> starting with a letter or special character and rest is sequence of alphanumeric and special characters (special characters: _, -, $, &, %, *, !, ?)
				if(preg_match_all('/^([a-zA-Z_\-$&%*!?])([a-zA-Z0-9_\-$&%*!?]*)$/', $parts[1], $matches) === 1){
					return true;
				}
			}
		}
	}
	return false;
}

function is_label($arg)
{
	// same as var right part -> starting with a letter or special character and rest is sequence of alphanumeric and special characters (special characters: _, -, $, &, %, *, !, ?)
	if(preg_match_all('/^([a-zA-Z_\-$&%*!?])([a-zA-Z0-9_\-$&%*!?]*)$/', $arg, $matches) === 1){
		return true;
	}
	return false;
}

function is_constant($arg)
{
	// check if no white chars
	if(preg_match_all('/\s+/', $arg, $matches) === 0){
		$parts = explode('@', $arg);
		// only one @ (at)
		if(count($parts) == 2 || $parts[0] === "string"){
			// left part is:
			if($parts[0] === "nil"){
				// nil@nil
				if($parts[1] === "nil"){
					return true;
				}
			} else if($parts[0] === "bool"){
				// bool@true or bool@false
				if($parts[1] === "true" || $parts[1] === "false"){
					return true;
				}
			} else if($parts[0] === "int"){
				// int@<number> (int@35)
				if(preg_match_all('/^([0-9]+)$/', $parts[1], $matches) === 1){
					return true;
				}
			} else if($parts[0] === "string"){
				// sequence of printable characters in UTF-8 (except white characters, `#` and `\`)
				// decadic codes \xyz -> \035, \000 - \999
				// if no forbidden char is found
				array_shift($parts); // remove left part
				$string = implode('@', $parts); // implode right part
				if(preg_match_all('/((\\\\(?!([0-9]{3})))|([#\s]))/', $string, $matches) === 0){
					return true;
				}
			}
		}
	}
	return false;
}

function is_symbol($arg)
{
	// constant or variable
	return (is_constant($arg) || is_var($arg));
}

function is_type($arg)
{
	// type
	return ($arg === "int" || $arg === "string" || $arg === "bool");
}



// PARSE
//-------------------------
// check line by line for commands -> explode(), trim();
//- 22 - unknown or wrong operation code in source code
//- 23 - other lexical or syntax errors in source code
echo '<?xml version="1.0" encoding="utf-8"?>'.PHP_EOL;
echo '<program language="IPPcode21">'.PHP_EOL;

$instruction_number = 1;
while(($line = fgets(STDIN, 1024)) !== false)
{
	// couldn't read whole line
	if(strpos($line, "\n") === false && strpos($line, "\r\n") === false && strpos($line, "\r") === false){
		fwrite(STDERR, 'INTERNAL ERROR - code line is too long (max 1024 chars) ['.substr($line, 0, 100).'...]'.PHP_EOL);
		exit(99);
	}
	
	// remove comment
	$comment = explode('#', $line);
	if(count($comment) > 1){
		$line = $comment[0];
	}
	
	// trim line
	$line = trim($line);
	
	// check if line is not empty
	if($line != ''){		
		// get words from line
		$words = preg_split('/\s+/', $line, 0, PREG_SPLIT_NO_EMPTY);
		
		// check instruction - instructions are case insensitive
		if(isset($INSTRUCTIONS[strtoupper($words[0])])) {
			
			echo '	<instruction order="'.$instruction_number.'" opcode="'.strtoupper($words[0]).'">'.PHP_EOL;
			
			// check args type
			$arg_count = 0;
			foreach($INSTRUCTIONS[strtoupper($words[0])] as $arg_id => $arg_type)
			{
				echo '		<arg'.($arg_count+1).' type="';//'">end</arg1>';
				
				// check arg type
				if($arg_type == ARG_TYPE_VAR){
					if(is_var($words[$arg_id+1]) === false){
						break;
					}
					echo 'var">';
					echo $words[$arg_id+1];
				} else if($arg_type == ARG_TYPE_SYMB){
					if(is_symbol($words[$arg_id+1]) === false){
						break;
					}
					if(is_constant($words[$arg_id+1])){
						$parts = explode('@', $words[$arg_id+1]);
						
						echo $parts[0].'">';
						
						// only one @ (at)
						if(count($parts) == 2){
							echo $parts[1];
						} else if($parts[0] === "string"){
							array_shift($parts); // remove left part
							$string = implode('@', $parts); // implode right part
							echo $string;
						}
					} else if(is_var($words[$arg_id+1])){
						echo 'var">';
						echo $words[$arg_id+1];
					}
				} else if($arg_type == ARG_TYPE_LABEL){
					if(is_label($words[$arg_id+1]) === false){
						break;
					}
					echo 'label">';
					echo $words[$arg_id+1];
				} else if($arg_type == ARG_TYPE_TYPE){
					if(is_type($words[$arg_id+1]) === false){
						break;
					}
					echo 'type">';
					echo $words[$arg_id+1];
				}
				
				// correct arg count
				$arg_count++;
				
				echo '</arg'.$arg_count.'>'.PHP_EOL;
			}
			//	correct count of args												and 	all args are right type
			if((count($words)-1) != count($INSTRUCTIONS[strtoupper($words[0])]) || $arg_count != count($INSTRUCTIONS[strtoupper($words[0])])) {
				// ERROR - wrong or missing args
				//- 23 - other lexical or syntax errors in source code
				fwrite(STDERR, PHP_EOL.'ERROR - wrong or missing args [line: '.$line_number.']'.PHP_EOL);
				exit(23);
			}
			// add instruction
			$instruction_number++;
			
			echo '	</instruction>'.PHP_EOL;
		} else {
			// ERROR - instruction doesn't exist!
			//- 22 - unknown or wrong operation code in source code
			fwrite(STDERR, 'ERROR - unknown or wrong operation code in source code [line: '.$line_number.']'.PHP_EOL);
			exit(22);
		}
	}
	
	// add line number
	$line_number++;
}
// check reading error
if(!feof(STDIN)){
	fwrite(STDERR, 'INTERNAL ERROR - unexpected fgets() fail'.PHP_EOL);
	exit(99);
}

echo '</program>'.PHP_EOL;



// everything OK
exit(0);
?>
