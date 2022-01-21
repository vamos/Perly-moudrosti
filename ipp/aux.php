<?php

require_once("./instructions.php");

class main_parser
{
   protected $instruction_array = array();
   public function make_array($generated)
   {  // of instructions
      foreach ($generated as $line) 
      {
         if ( $line !== "comment" )
         { 
         $decoded = $this->decoder($line);
         array_push($this->instruction_array,$decoded);
         }
      }
   }
      
   //return new $instruction_array
   public function decoder($line)
   {  // get instruction name

      $pos = strpos($line,' ');
      if ($pos == false)
      {  // instruction has no arguments
         $instruction = $line;
      }
      else
      {
         $instruction = substr($line, 0, $pos);
      }
      $instruction = strtoupper($instruction);  // to uppercase
      switch ($instruction)
      {
         case "MOVE":
            return new Move($line);
         case "CREATEFRAME":
            return new Createframe($line);
         case "PUSHFRAME":
            return new PUSHFRAME($line);
         case "POPFRAME":
            return new POPFRAME($line);
         case "DEFVAR":
            return new Defvar($line);
         case "CALL":
            return new Call($line);
         case "RETURN":
            return new iReturn($line);
         case "PUSHS":
            return new Pushs($line);
         case "POPS":
            return new Pops($line);
         case "ADD":
            return new Add($line);
         case "SUB":
            return new Sub($line);
         case "MUL":
            return new Mul($line);
         case "IDIV":
            return new Idiv($line);
         case "LT":
            return new Lt($line);
         case "GT":
            return new Gt($line);
         case "EQ":
            return new Eq($line);
         case "AND":
            return new iAnd($line);
         case "OR":
            return new iOr($line);
         case "NOT":
            return new Not($line);
         case "INT2CHAR":
            return new Int2char($line);
         case "STRI2INT":
            return new Stri2int($line);
         case "READ":
            return new Read($line);
         case "WRITE":
            return new Write($line);
         case "CONCAT":
            return new Concat($line);
         case "STRLEN":
            return new iStrlen($line);
         case "GETCHAR":
            return new Getchar($line);
         case "SETCHAR":
            return new Setchar($line);
         case "TYPE":
            return new Type($line);
         case "LABEL":
            return new Label($line);
         case "JUMP":
            return new Jump($line);
         case "JUMPIFEQ":
            return new Jumpifeq($line);
         case "JUMPIFNEQ":
            return new Jumpifneq($line);
         case "EXIT":
            return new iExit($line);
         case "DPRINT":
            return new Dprint($line);
         case "BREAK":
            return new iBreak($line);

         default:
            fwrite(STDERR, "22\n");
            exit(22); 

      }
   }
   public function make_tree()
   {  // XML
      $dom = new DOMDocument('1.0', 'UTF-8');
      $dom->preserveWhiteSpace = false;
      $dom->formatOutput = true;
      $program = simplexml_load_string("<program></program>");
      $program = dom_import_simplexml($program);
      $program = $dom->importNode($program, true);
      $program = $dom->appendChild($program);
      $program->setAttribute("language","IPPcode19");
      
      $cnt = 0;         // instruction counter
      $cnt_lab = 0;     // label counter
      $cnt_jum = 0;     // jump counter
      
      foreach ($this->instruction_array as $input)
      {
         $cnt++;
         $instruction = simplexml_load_string("<instruction></instruction>");
         $instruction = dom_import_simplexml($instruction);
         $instruction = $dom->importNode($instruction, true);
         $instruction = $program->appendChild($instruction);
         $instruction->setAttribute("order",$cnt);
         $instruction->setAttribute("opcode",$input->id);
         if ( $input->id == "LABEL" )
         {  // stats
            $cnt_lab++;
         }
         if ( $input->id == "JUMP" || $input->id == "JUMPIFEQ" ||
         $intup->id == "JUMPIFNEQ" )
         {  // stats
            $cnt_jum++;
         }
         
         // argument generation
         if ( $input->type1 != NULL || $input->arg1 !=  NULL )
         {
               $input->arg1 = htmlspecialchars($input->arg1);
               $attribute = simplexml_load_string("<arg1>$input->arg1</arg1>");
               $attribute = dom_import_simplexml($attribute);
               $attribute->setAttribute("type",$input->type1);
               $attribute = $dom->importNode($attribute, true);
               $attribute = $instruction->appendChild($attribute);
         }
         if ( $input->type2 != NULL || $input->arg2 !=  NULL )
         {
               $input->arg2 = htmlspecialchars($input->arg2);
               $attribute = simplexml_load_string("<arg2>$input->arg2</arg2>");
               $attribute = dom_import_simplexml($attribute);
               $attribute->setAttribute("type",$input->type2);
               $attribute = $dom->importNode($attribute, true);
               $attribute = $instruction->appendChild($attribute);
         }
         if ( $input->type3 != NULL || $input->arg3 !=  NULL )
         {
               $input->arg3 = htmlspecialchars($input->arg3);
               $attribute = simplexml_load_string("<arg3>$input->arg3</arg3>");
               $attribute = dom_import_simplexml($attribute);
               $attribute->setAttribute("type",$input->type3);
               $attribute = $dom->importNode($attribute, true);
               $attribute = $instruction->appendChild($attribute);
         }
      }     
      return array($dom,$cnt,$cnt_lab,$cnt_jum);
   }
}


?>
