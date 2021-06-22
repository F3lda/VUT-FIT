<?php
//php7.4
/*
* @file test.php
* 
* @brief VUT FIT IPP projekt 2020/2021
* @date 2021-04-21
* @author F3lda
* @update 2021-04-21
*/

function printerr($err, $code = 99)
{
    fwrite(STDERR, 'ERROR - '.$err.'!'.PHP_EOL);
    exit($code);
}

$TEST_DIR = ".";
$TEST_DIRS_RECURSIVE = false;
$PARSE_FILE = "parse.php";
$PARSE_ONLY = false;
$INTERPRET_FILE = "interpret.py";
$INTERPRET_ONLY = false;
$JEXAM_XML_FILE = "/pub/courses/ipp/jexamxml/jexamxml.jar";
$JEXAM_CFG_FILE = "/pub/courses/ipp/jexamxml/options";

/***** PARSE ARGS *****/
$options = getopt("", ["help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only", "jexamxml:", "jexamcfg:"]);
//var_dump($options);
//var_dump($argv);

if (count($options) != count($argv)-1) {
    // 10  - chybějící parametr skriptu (je-li třeba) nebo použití zakázané kombinace parametrů;
    printerr("ARG PARSE: unknown args passed", 10);
}

// HELP
if (key_exists("help", $options) && count($options) == 1) {
    echo 'Testovaci ramec parseru a interpretu IPPcode21 (test.php)'.PHP_EOL;
    echo '---------------------------------------'.PHP_EOL;
    echo 'Automaticke testovani aplikace parse.php a interpret.py.'.PHP_EOL;
    echo ''.PHP_EOL;
    echo 'Tento skript pracuje s temito parametry:'.PHP_EOL;
    echo '--help - vypise na standardni vystup napovedu skriptu (tento parametr nelze kombinovat s zadnym dalsim parametrem)'.PHP_EOL;
    echo ''.PHP_EOL;
    echo 'Chybove navratove kody:'.PHP_EOL;
    echo '10 - chybejici parametr skriptu (je-li treba) nebo pouziti zakazane kombinace parametru;'.PHP_EOL;
    echo '41 - zadany adresar (path v parametru --directory) nebo zadany soubor (file v parametrech --parse-script, --int-script, --jexamxml a --jexamcfg) neexistuje ci neni pristupny.;'.PHP_EOL;
    echo '99 - interni chyba (neovlivnena vstupnimi soubory ci parametry prikazove radky; napr. chyba alokace pameti);'.PHP_EOL;
    echo ''.PHP_EOL;
    exit(0);
    
} else if (key_exists("help", $options)) {
    printerr("ARG PARSE: help cannot be combined with other args", 10);
}

// DIRECTORY
if (key_exists("directory", $options)) {
    $TEST_DIR = $options['directory'];
}

// RECURSIVE
$TEST_DIRS_RECURSIVE = key_exists("recursive", $options);

// PARSE FILE
if (key_exists("parse-script", $options)){
    $PARSE_FILE = $options['parse-script'];
}

// PARSE ONLY
$PARSE_ONLY = key_exists("parse-only", $options);

// INTERPRET FILE
if (key_exists("int-script", $options)) {
    $INTERPRET_FILE = $options['int-script'];
}

// INTERPRET ONLY
$INTERPRET_ONLY = key_exists("int-only", $options);

// JEXAM XML
if (key_exists("jexamxml", $options)) {
    $JEXAM_XML_FILE = $options['jexamxml'];
}

// JEXAM CFG
if (key_exists("jexamcfg", $options)) {
    $JEXAM_CFG_FILE = $options['jexamcfg'];
}


if ($PARSE_ONLY && $INTERPRET_ONLY) {
    printerr("ARG PARSE: parse-only and int-only cannot be combined", 10);
}

if ($PARSE_ONLY && key_exists("int-script", $options)) {
    printerr("ARG PARSE: parse-only and int-script cannot be combined", 10);
}

if (key_exists("parse-script", $options) && $INTERPRET_ONLY) {
    printerr("ARG PARSE: parse-script and int-only cannot be combined", 10);
}

if (!is_dir($TEST_DIR) || !is_file($PARSE_FILE) || !is_file($INTERPRET_FILE) || !is_file($JEXAM_XML_FILE) || !is_file($JEXAM_CFG_FILE)){
    // 41 - zadaný adresář (path v parametru--directory) nebo zadaný soubor (filev parametrech--parse-script,--int-script,--jexamxmla--jexamcfg) neexistuje či není přístupný.
    printerr("ARG PARSE: invalid path or file", 41);
}

/*
echo $TEST_DIR.PHP_EOL;
echo $TEST_DIRS_RECURSIVE.PHP_EOL;
echo $PARSE_FILE.PHP_EOL;
echo $PARSE_ONLY.PHP_EOL;
echo $INTERPRET_FILE.PHP_EOL;
echo $INTERPRET_ONLY.PHP_EOL;
echo $JEXAM_XML_FILE.PHP_EOL;
echo $JEXAM_CFG_FILE.PHP_EOL;
*/

/***** SCAN DIRS *****/
$files = [];
if ($TEST_DIRS_RECURSIVE) {
    $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($TEST_DIR));
    foreach ($rii as $file) {
        if (!$file->isDir()){ 
            $files[] = $file->getPathname(); 
        }
    }
} else {
    $filesAndDirs = array_diff(scandir($TEST_DIR), array('.', '..'));
    foreach($filesAndDirs as $item)
    {
        if (is_file($item)) {
            $files[] = $item;
        }
    }
}

/***** TESTS *****/
$tests_checked = [];
foreach($files as $file)
{
    $file_array = explode('.', $file);
    if ($file_array[1] == 'src') {
        if (!in_array($file_array[0], $tests_checked)){
            $tests_checked[] = $file_array[0];
            
            // CREATE MISSING FILES
            // in, out -> gen empty
            if (!in_array($file_array[0].'.in', $files)) {
                //echo "CREATE FILE in".PHP_EOL;
                file_put_contents($file_array[0].'.in', '');
            }
            if (!in_array($file_array[0].'.out', $files)) {
                //echo "CREATE FILE out".PHP_EOL;
                file_put_contents($file_array[0].'.out', '');
            }
            
            // rc -> gen 0
            if (!in_array($file_array[0].'.rc', $files)) {
                //echo "CREATE FILE rc".PHP_EOL;
                file_put_contents($file_array[0].'.rc', '0');
            }
            
            // TODO - exec file and compare results
            
            //echo $file.PHP_EOL;
            
        }
    }
}



//var_dump($files);
//var_dump($TEST_DIRS_RECURSIVE);



exit(0)
?>