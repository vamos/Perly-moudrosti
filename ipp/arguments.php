<?php

class name_check
{  // label & variable names check
   public function work($name)
   {
      if ( preg_match("/^[_\-$&%*!?a-zA-Z][0-9a-zA-Z_\-$&%*!?]*$/",$name))
      {
         return true;
      }
      else
      {
         fwrite(STDERR, "23\n");
         exit(23);
      }
   }
}

class int_check 
{  
   public function work($name)
   {
      if ( preg_match("/^[\x2B\x2D]?[0-9]*$/",$name))
      {
         return true;
      }
      else
      {
         fwrite(STDERR, "23\n");
         exit(23);  
      }
   }
}

class bool_check
{
   public function work($name)
   {
      if ( $name == "false" || $name == "true" )
      {
         return true;
      }
      else
      {
         fwrite(STDERR, "23\n");
         exit(23); 
      }
   }
}
class nil_check
{
   public function work($name)
   {
      if ( $name == "nil" )
      {
         return true;
      }
      else
      {
         fwrite(STDERR, "23\n");
         exit(23); 
      }
   }
}

class string_check
{  
   public function work($name)
   {
      if (preg_match("/^\[0-9][0-9][0-9]*|[a-zA-Z\x{21}-\x{22}^\x{23}\x{24}-\x{5B}\x{5D}-\x{FF}]*$/",$name))
      {
        return true;
      }
      else
      {
         fwrite(STDERR, "23\n");
         exit(23); 
      }
   }
}

class type_check
{
   protected $islegit;
   
   public function work($argu)
   {
      if ( $argu == "int" || $argu == "bool" || $argu == "string" )
      {
         return true;
      } 
   }
}
class var_check
{  // and label_check
   protected $islegit;

   public function work($argu)
   {
      $pieces = explode("@", $argu);
      if ( count($pieces) < 2 )
      {
         // bad variable  definition
         fwrite(STDERR, "23\n");
         exit(23);  
      }
      else
      {
         if( $pieces[0] == "GF" || $pieces[0] == "LF" || 
         $pieces[0] == "TF")
         {
            //echo "1st half ok";
         }
         else
         {
            fwrite(STDERR, "23\n");
            exit(23);  
         }
         $legit = new name_check();
         $this->$islegit = $legit->work($pieces[1]); 
      }
   } 
}

class symb_check  
{
   protected $islegit;

   public function work($argu)
   {
      $pieces = explode("@", $argu);
      if ( count($pieces) < 2 )
      {
         // bad var/symb definition
         fwrite(STDERR, "23\n");
         exit(23);  
      }
      else
      {
         if( $pieces[0] == "GF" || $pieces[0] == "LF" || 
         $pieces[0] == "TF")
         {
            $legit = new name_check();
            $this->$islegit = $legit->work($pieces[1]);
         }
         else if ( $pieces[0] == "int" )
         {
            $legit = new int_check();
            $this->$islegit = $legit->work($pieces[1]);
         } 
         else if ( $pieces[0] == "bool" )
         {
            $legit = new bool_check();
            $this->$islegit = $legit->work($pieces[1]);
         } 
         else if ( $pieces[0] == "string" )
         {
            $legit = new string_check();
            $this->$islegit = $legit->work($pieces[1]); //[0]
         } 
         else if ( $pieces[0] == "nil" )
         {
            $legit = new nil_check();
            $this->$islegit = $legit->work($pieces[1]);
         } 
         else
         {
            fwrite(STDERR, "23\n");
            exit(23); 
         }
      }
      
   }
}

?>
