##############################################################
#
# Javascript wrapper class generation
#

package Jsclass;

use File::Basename;
use File::Path 'mkpath';

use strict;
use Utils;
use Parser;

our $out_dir;

our $use_es6_mod = 1;
our $js_nsname = "wrapper";

##########

sub genJsWrapper($)
{
  my $cls = shift;

  my $qifname = $cls->{"qifname"};
  my $qif_fname = $cls->{"file"};


  my ($in_base, $in_dir, $in_ext) = fileparse($qif_fname, '\.qif');
  $in_dir =  "" if ($in_dir eq "./");
  my $out_fname = "$in_dir${in_base}.js";

  if ($out_dir) {
    $out_fname = "$out_dir/${in_base}.js";
    if (!-d $out_dir) {
        mkpath($out_dir) or die "Cannot create dir $out_dir: $!";
    }
  }

  print("Output JS file: $out_fname\n");

  open(OUT, ">$out_fname") || die "$?:$!";
  set_building_file($out_fname);

  print OUT "/////////////////////////////////////\n";
  print OUT "//\n";
  print OUT "// Javascript wrapper class for $qifname\n";
  print OUT "//\n";
  print OUT "\n";
  my $js_clsname;
  # print OUT "var EXPORTED_SYMBOLS = [\"${js_clsname}\"];\n";
  if ($use_es6_mod) {
      $js_clsname = $qifname;
  
      print OUT "\n";
      print OUT "import { BaseWrapper } from '../base_wrapper';\n";
      print OUT "\n";
      print OUT "export class ${js_clsname} extends BaseWrapper {\n";
  }
  else {
      $js_clsname = $js_nsname."_".$qifname;
  
      print OUT "\n";
      print OUT "${js_clsname} = function ${qifname}_ctor(aWrapped, aUtil)\n";
      print OUT "{\n";
      print OUT "  this._wrapped = aWrapped;\n";
      print OUT "  this._utils = aUtil;\n";
      print OUT "}\n";
      print OUT "\n";
      print OUT "module.exports.constructor = ${js_clsname};\n";
      print OUT "\n";
  }

  genJsSupclsCodeImpl($js_clsname, $qifname);

  if ($use_es6_mod) {
      print OUT "\n";
      print OUT "}\n";
      print OUT "\n";
      genJsES6ImplData($js_clsname, $qifname);
      print OUT "\n";
  }
  
  close(OUT);
}
		   
sub genJsSupclsCodeImpl($$)
{
  my ($class_name, $supcls_name) = @_;
  my $supcls = $Parser::db{$supcls_name};

  my @extends = @{$supcls->{"extends"}} if ($supcls->{"extends"});
  foreach my $i (@extends) {
    genJsSupclsCodeImpl($class_name, $i);
  }

  print OUT "/////////////////////////////////////\n";
  print OUT "// Class $supcls_name\n";
  print OUT "//\n";
  print OUT "\n";

  if ($use_es6_mod) {
      ;
  }
  else {
      my $clskey = "\@implements_$supcls_name";
      print OUT "${class_name}[\"$clskey\"] = \"yes\";\n\n";
  }
  
  genJsPropCode($supcls, $class_name);
  genJsInvokeCode($supcls, $class_name);
}

sub genJsPropCode($$)
{
  my $cls = shift;
  my $clsname = shift;

  return unless ($cls->{"properties"});

  my %props = %{$cls->{"properties"}};

  foreach my $propnm (sort keys %props) {

    my $prop = $props{$propnm};
    my $type = $prop->{"type"};
    # debug("JS: prop: $propnm, type: $type\n");
    print OUT "// property: $propnm, type: $type\n";

    if ($type eq "object") {
      genJsObjPropCode($clsname, $propnm, $prop);
    }
    elsif ($type eq "enum") {
      genJsEnumPropCode($clsname, $propnm, $prop);
    }
    else {
      genJsBasicPropCode($clsname, $propnm, $prop);
    }
  }
}

sub genJsBasicPropCode($$$)
{
  my $classnm = shift;
  my $propnm = shift;
  my $prop = shift;

  if ($use_es6_mod) {
      print OUT "  get $propnm() {\n";
      print OUT "    return this.getProp(\'$propnm\');\n";
      print OUT "  }\n";
      print OUT "\n";
  }      
  else {
      print OUT "${classnm}.prototype.__defineGetter__(\"$propnm\", function()\n";
      print OUT "{\n";
      print OUT "  return this._wrapped.getProp(\"$propnm\");\n";
      print OUT "});\n";
      print OUT "\n";
  }
  
  return if (contains($prop->{"options"}, "readonly"));
      
  if ($use_es6_mod) {
      print OUT "  set $propnm(arg0) {\n";
      print OUT "    this.setProp(\'$propnm\', arg0);\n";
      print OUT "  }\n";
      print OUT "\n";
  }
  else {
      print OUT "${classnm}.prototype.__defineSetter__(\"$propnm\", function(arg0)\n";
      print OUT "{\n";
      print OUT "  this._wrapped.setProp(\"$propnm\", arg0);\n";
      print OUT "});\n";
      print OUT "\n";
  }

}

sub genJsObjPropCode($$$)
{
  my $classnm = shift;
  my $propnm = shift;
  my $prop = shift;

  my $propqif = $prop->{"qif"};

  if ($use_es6_mod) {
      print OUT "  get $propnm() {\n";
      print OUT "    const result = this.getProp(\'$propnm\');\n";
      print OUT "    return this.createWrapper(result);\n";
      print OUT "  }\n";
      print OUT "\n";
  }
  else {
      print OUT "${classnm}.prototype.__defineGetter__(\"$propnm\", function()\n";
      print OUT "{\n";
      print OUT "  return this._utils.createWrapper(this._wrapped.getProp(\"$propnm\"));\n";
      print OUT "});\n";
      print OUT "\n";
  }
  
  return if (contains($prop->{"options"}, "readonly"));

  if ($use_es6_mod) {
      print OUT "  set $propnm(arg0) {\n";
      print OUT "    this.setProp(\'$propnm\', arg0.wrapped);\n";
      print OUT "  }\n";
      print OUT "\n";
  }
  else {
      print OUT "${classnm}.prototype.__defineSetter__(\"$propnm\", function(arg0)\n";
      print OUT "{\n";
      print OUT "  this._wrapped.setProp(\"$propnm\", arg0._wrapped);\n";
      print OUT "});\n";
      print OUT "\n";
  }
}

sub genJsEnumPropCode($$$)
{
  my ($classnm, $propnm, $prop) = @_;

  genJsBasicPropCode($classnm, $propnm, $prop);

  defined($prop->{"enumdef"}) || die;

  my %enums = %{ $prop->{"enumdef"} };
  foreach my $defnm (sort keys %enums) {
    my $key = $propnm."_".uc($defnm);
    my $value = $enums{$defnm};

    if ($use_es6_mod) {
        print OUT "  get $key() {\n";
        print OUT "    return this.getEnumDef(\'$propnm\', \'$defnm\');\n";
        print OUT "  }\n";
        print OUT "\n";
    }
    else {
        print OUT "\n";
        print OUT "${classnm}.prototype.__defineGetter__(\"$key\", function()\n";
        print OUT "{\n";
        print OUT "  return this._wrapped.getEnumDef(\"$propnm\", \"$defnm\");\n";
        print OUT "});\n";
        print OUT "\n";
    }
  }	  
}

#####################

sub genJsInvokeCode($$)
{
  my $cls = shift;
  my $classnm = shift;

  return if (!$cls->{"methods"});

  my %mths = %{$cls->{"methods"}};

  foreach my $nm (sort keys %mths) {
    my $mth = $mths{$nm};
    my $nargs = int(@{$mth->{"args"}});
    my $rettype = $mth->{"rettype"};
    my $rval_typename = $rettype->{"type"};

    print OUT "// method: $nm\n";

    if ($use_es6_mod) {
        print OUT "  ${nm}(".makeMthSignt($mth).") {\n";
        if ($rval_typename ne "void") {
            print OUT "    const result = ";
        }
        else {
            print OUT "    ";
        }
        print OUT "this.invokeMethod(".makeMthArg($mth).");\n";
    }
    else {
        print OUT "${classnm}.prototype.${nm} = function(".makeMthSignt($mth).") {\n";
        if ($rval_typename ne "void") {
            print OUT "  var rval = ";
        }
        else {
            print OUT "  ";
        }
        print OUT "this._wrapped.invokeMethod(".makeMthArg($mth).")\n";
    }

    if ($use_es6_mod) {
        if ($rval_typename eq "object") {
            print OUT "    return this.createWrapper(result);\n";
        }
        elsif ($rval_typename eq "void") {
            # No return code
        }
        else {
            # basic types
            print OUT "    return result;\n";
        }
        print OUT "  };\n";
    }
    else {
        if ($rval_typename eq "object") {
            # my $rettype_qif = $rettype->{"qif"};
            print OUT "  return this._utils.createWrapper(rval);\n";
        }
        elsif ($rval_typename eq "void") {
            # No return code
        }
        else {
            # basic types
            print OUT "  return rval;\n";
        }
        print OUT "};\n";
    }
    print OUT "\n";
  }

  print OUT "\n";
}

sub makeMthSignt($)
{
  my $mth = shift;
  my $args = $mth->{"args"};

  my $ind = 0;
  my @rval;
  foreach my $arg (@{$args}) {
    push(@rval, "arg_$ind");
    ++$ind;
  }
  return join(", ", @rval);
}

sub makeMthArg($)
{
  my $mth = shift;
  my $args = $mth->{"args"};
  my $name = $mth->{"name"};

  my @rval = ("\"$name\"");

  my $ind = 0;
  foreach my $arg (@{$args}) {
    my $arg_type = $arg->{"type"};
    if ($arg_type eq "object") {
        if ($arg->{"qif"} eq "LScrCallBack") {
            # Callback (function) object
            # --> no conversion
            push(@rval, "arg_$ind");
        }
        else {
            if ($use_es6_mod) {
                push(@rval, "arg_${ind}.wrapped");
            }
            else {
                push(@rval, "arg_${ind}._wrapped");
            }
        }
    }
    else {
      push(@rval, "arg_$ind");
    }
    ++$ind;
  }
  return join(", ", @rval);
}

# sub makeMthArg2($)
# {
#   my $mth = shift;
#   my $args = $mth->{"args"};
#   my $name = $mth->{"name"};

#   my @rval;

#   my $ind = 0;
#   foreach my $arg (@{$args}) {
#     my $arg_type = $arg->{"type"};
#     if ($arg_type eq "object") {
#       push(@rval, "arg_${ind}._wrapped");
#     }
#     else {
#       push(@rval, "arg_$ind");
#     }
#     ++$ind;
#   }

#   return "\"$name\"" . ", [" . join(", ", @rval) . "]";
# }

sub checkCallbackReg($)
{
  my $mth = shift;
  my $args = $mth->{"args"};
  my $name = $mth->{"name"};

  return 0 if (!defined($args->[0]));
  my $arg = $args->[0];

  return 0 if ($arg->{"type"} ne "object");

  return 1 if ($arg->{"qif"} eq "LScrCallBack");
  return 0;
}

sub genJsES6ImplData($$)
{
  my ($class_name, $supcls_name) = @_;
  my $supcls = $Parser::db{$supcls_name};

  my @extends = @{$supcls->{"extends"}} if ($supcls->{"extends"});
  foreach my $i (@extends) {
      genJsES6ImplData($class_name, $i);
  }

  my $clskey = "\@implements_$supcls_name";
  print OUT "${class_name}.prototype[\'$clskey\'] = \'yes\';\n";
}

1;


