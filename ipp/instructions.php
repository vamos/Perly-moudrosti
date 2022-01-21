<?php

require_once("./arguments.php");

class worker
{  // instruction 
   public function parameter_check($line,$num_par)
   {
      // get rid of white spaces / all ws -> " "
      $tmp_pieces = preg_replace('/\s+/',' ', $line);
      $pieces = explode(" ", $tmp_pieces);
      if ( count($pieces)-1 == $num_par )
      {
         return $pieces;
      }
      else
         fwrite(STDERR, "23\n");
         exit(23);
   }
}

class Instruction
{
   public $id;
   public $arg1;
   public $arg2;
   public $arg3;
   public $type1;
   public $type2;
   public $type3;
   
   public function get_symb($arg_numb)  // takes argument index
   {  // aux func 4 parsing symbols
      $tmp_arg = "arg" . $arg_numb;
      $tmp_type = "type" . $arg_numb;
      $legit_var = new symb_check();
      $legit_var->work($this->$tmp_arg);
      $pieces = explode("@", $this->$tmp_arg); 
      if ( empty($pieces[1]) )
      {  // init of empty variable
         $pieces[1] == "neco";
      }
      if ( $pieces[0] != "LF" && $pieces[0] != "TF" && $pieces[0] != "GF" )
      {  // not var
         $symb_type = $pieces[0];
         $symb_value = $pieces[1];
         $this->$tmp_type = $symb_type;
         $this->$tmp_arg = $symb_value;
      }
      else
      {
         $this->$tmp_type = "var";
      }
   }
}

class Move extends Instruction // <var> <symb>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,2);
      $this->id="MOVE";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];

      // variable
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
   }
}

class Createframe extends Instruction
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,0);
      $this->id="CREATEFRAME";
   }
}

class Pushframe extends Instruction
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,0);
      $this->id="PUSHFRAME";
   }
}
class Popframe extends Instruction
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,0);
      $this->id="POPFRAME";

   }
}

class Defvar extends Instruction // <var>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="DEFVAR";
      $this->arg1=$parameters[1];

      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";
   }
}

class CALL extends Instruction // <label>
{
   public function __construct($line)
   {  
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="CALL";
      $this->arg1=$parameters[1];

      $legit_label = new name_check();
      $legit_label->work($this->arg1);
      $this->type1="label";
   }
}

class iReturn extends Instruction 
{
   public function __construct($line)
   {  
      $output = new worker();
      $parameters = $output->parameter_check($line,0);
      $this->id="RETURN";
   }
}

class PUSHS extends Instruction // <symb>
{
   public function __construct($line)
   {  
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="PUSHS";
      $this->arg1=$parameters[1];

      // symbol
      $this->get_symb(1); 
   }
}
class POPS extends Instruction // <var>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="POPS";
      $this->arg1=$parameters[1];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";
   }
}
class ADD extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="ADD";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class SUB extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="SUB";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class MUL extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="MUL";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class IDIV extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="IDIV";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class LT extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="LT";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class GT extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="GT";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class EQ extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="EQ";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class iAnd extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="AND";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class iOR extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="OR";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}
class NOT extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,2);
      $this->id="NOT";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
   }
}

class INT2CHAR extends Instruction // <var> <symb> 
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,2);
      $this->id="INT2CHAR";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
   }
}

class STRI2INT extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="STRI2INT";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class READ extends Instruction // <var> <type>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,2);
      $this->id="READ";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];

      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";
      $legit_type = new type_check();
      $legit_type->work($this->arg1);
      $this->type2 = "type";
      
   }
}
class WRITE extends Instruction // <symb>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="WRITE";
      $this->arg1=$parameters[1];

      // symbol
      $this->get_symb(1); 
   }
}

class CONCAT extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="CONCAT";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class iSTRLEN extends Instruction // <var> <symb> 
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,2);
      $this->id="STRLEN";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
   }
}

class GETCHAR extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="GETCHAR";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class SETCHAR extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="SETCHAR";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class TYPE extends Instruction // <var> <symb1> 
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,2);
      $this->id="TYPE";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      
      $legit_var = new var_check();
      $legit_var->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
   }
}

class LABEL extends Instruction // <label>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="LABEL";
      $this->arg1=$parameters[1];

      //echo "label: $this->arg1\n";
      // label
      $legit_label = new name_check();
      $legit_label->work($this->arg1);
      $this->type1="label";
   }
}

class JUMP extends Instruction // <jump>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="JUMP";
      $this->arg1=$parameters[1];

      // label
      $legit_name = new name_check();
      $legit_name->work($this->arg1);
      $this->type1="label";
   }
}

class JUMPIFEQ extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="JUMPIFEQ";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_label = new name_check();
      $legit_label->work($this->arg1);
      $this->type1="label";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}
class JUMPIFNEQ extends Instruction // <var> <symb1> <symb2>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,3);
      $this->id="JUMPIFNEQ";
      $this->arg1=$parameters[1];
      $this->arg2=$parameters[2];
      $this->arg3=$parameters[3];
      
      $legit_label = new name_check();
      $legit_label->work($this->arg1);
      $this->type1="var";

      // symbol
      $this->get_symb(2); 
      $this->get_symb(3); 
   }
}

class iEXIT extends Instruction // <symb>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="EXIT";
      $this->arg1=$parameters[1];

      // symbol 
      $this->get_symb(1); 
   }
}

class DPRINT extends Instruction // <symb>
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,1);
      $this->id="DPRINT";
      $this->arg1=$parameters[1];

      // symbol 
      $this->get_symb(1); 
   }
}

class iBreak extends Instruction
{
   public function __construct($line)
   {
      $output = new worker();
      $parameters = $output->parameter_check($line,0);
      $this->id="BREAK";
   }
}

?>
