<?php

// counting stats for task expansion
$stat_loc;
$stat_com;
$stat_lab;
$stat_jum;

require_once("./aux.php");

function helper()
{  // print help
   echo "Usage: php7.3 parse.php \n";
   echo "       php7.3 parse.php --stats=[file] [--loc/--comments/--label/--jumps]\n";
   echo "       php7.3 parse.php --help \n";
   exit(0);
}
function loader()
{  // load input
   $input = fopen('php://stdin','r');
   $header = fgets($input);
   $header= remover($header);
   $header = trim($header);         // erase whitespaces
   $header = strtoupper($header);   // to uppercase
   if ($header != ".IPPCODE19")
   {  // check correct header
      fwrite(STDERR, "21\n");
      exit(21);
   }
   else
   {
      //echo "wrong header: $header\n";
   }

   while ($line = fgets($input))
   {
      $line =remover($line);
      yield $line;
   }
   fclose($input);
}

function remover($line)
{  // comment remove
   global $stat_com;
   $line = trim($line);         // erase whitespaces
   $pos = strpos($line,'#');
   if ( $pos === 0 )
   {  // whole line comment
      $line = "comment";
      $stat_com++;
   }
   else if ($pos !== false)
   {  // search line for # char
      $line = substr($line, 0, $pos);
      $stat_com++;
   }
   $line = trim($line);         // erase whitespaces
   if (strlen($line) == 0 )
   {  // empty lines marked as comments
      $line = "comment";
   }
   return $line;

}

function write_stats($file)
{  // count and show stats - task expansion
   global $options;
   global $stat_loc;
   global $stat_com;
   global $stat_lab;
   global $stat_jum;
   if ( array_key_exists("loc",$options))
   {
      $loc = "$stat_loc\n";
      if(!file_put_contents($file,$loc,FILE_APPEND))
      {
         fwrite(STDERR, "12\n");
         exit(12);
      }
   }
   if ( array_key_exists("comments",$options))
   {
      $com = "$stat_com\n";
      if(!file_put_contents($file,$com,FILE_APPEND))
      {
         fwrite(STDERR, "12\n");
         exit(12);
      }
   }
   if ( array_key_exists("labels",$options))
   {
      $lab = "$stat_lab\n";
      if(!file_put_contents($file,$lab,FILE_APPEND))
      {
         fwrite(STDERR, "12\n");
         exit(12);
      }
   }
   if ( array_key_exists("jumps",$options))
   {
      $jum = "$stat_jum\n";
      if(!file_put_contents($file,$jum,FILE_APPEND))
      {
         fwrite(STDERR, "12\n");
         exit(12);
      }
   }
}

$options = getopt("", array("help","stats:","loc","comments","labels","jumps"));

if (isset($options['help']))
{  // print help
   if ( !isset($options['stats']) && !isset($options['loc']) && !isset($options['comments']) && !isset($options['labels']) && !isset($options['jumps']))
   {
      helper();
   }
   else
   {  // print error
      fwrite(STDERR, "10\n");
      exit(10);
   }
}
if (!array_key_exists("stats",$options))
{  // stats params without STATS
   if ( array_key_exists("loc",$options) || array_key_exists("comments",$options) || array_key_exists("labels",$options) || array_key_exists("jumps",$options))
   {  // print error
      fwrite(STDERR, "10\n");
      exit(10);
   }
}


$main = new main_parser();
$seed = $main->make_array(Loader());
list($xml,$stat_loc,$stat_lab,$stat_jum) = $main->make_tree();
fwrite(STDOUT, $xml->saveXML());


if (array_key_exists("stats",$options))
{  // output stats
   $file = $options["stats"];
   if (file_exists($file))
   {  // delete file if exists
      if(!unlink($file))
      {  // print error
         fwrite(STDERR, "12\n");
         exit(12);  // cant rewrite file
      }
   }
   write_stats($file);
}

?>
