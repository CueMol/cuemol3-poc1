##############################################################
#
# Javascript (for emscripten/ES6) wrapper class generation
#

package EmClass;

use File::Basename;
use File::Path 'mkpath';

use strict;
use Utils;
use Parser;

our $out_dir;
our $es6_mod = 1;
our $utils_module_path = "../utils_module";

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

  my $js_clsname = $qifname;

  print OUT "/////////////////////////////////////\n";
  print OUT "//\n";
  print OUT "// Javascript wrapper class for $qifname\n";
  print OUT "//\n";
  print OUT "\n";
  if ($es6_mod) {
      print OUT "import { BaseWrapper } from '../base_wrapper';\n";
      print OUT "\n";
      print OUT "export class ${js_clsname} extends BaseWrapper {\n";
  }
  else {
      print OUT "const utils = require(\'$utils_module_path\');\n";
      print OUT "\n";
      print OUT "class ${js_clsname} extends utils.BaseWrapper {\n";
      print OUT "\n";
      print OUT "  constructor(...args) {\n";
      print OUT "    super(...args);\n";
      print OUT "  }\n";
  }

  print OUT "\n";

  genJsSupclsCodeImpl($js_clsname, $qifname);

  print OUT "\n";
  print OUT "}\n";

  print OUT "\n";
  genJsImplData($js_clsname, $qifname);
  print OUT "\n";
  if ($es6_mod) {
  }
  else {
      print OUT "module.exports = ${js_clsname};\n";
  }
  print OUT "\n";

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

  print OUT "  //\n";
  print OUT "  // Class $supcls_name\n";
  print OUT "  //\n";
  print OUT "\n";

  # my $clskey = "\@implements_$supcls_name";
  # print OUT "${class_name}[\'$clskey\'] = \'yes\';\n\n";

  genJsPropCode($supcls, $class_name);
  genJsInvokeCode($supcls, $class_name);
}

sub genJsImplData($$)
{
  my ($class_name, $supcls_name) = @_;
  my $supcls = $Parser::db{$supcls_name};

  my @extends = @{$supcls->{"extends"}} if ($supcls->{"extends"});
  foreach my $i (@extends) {
      genJsImplData($class_name, $i);
  }

  my $clskey = "\@implements_$supcls_name";
  print OUT "${class_name}.prototype[\'$clskey\'] = \'yes\';\n";
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
    print OUT "  // property: $propnm, type: $type\n";

    if ($type eq "enum") {
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

  print OUT "  get $propnm() {\n";
  print OUT "    return this.getProp(\'$propnm\');\n";
  print OUT "  }\n";
  print OUT "\n";
      
  return if (contains($prop->{"options"}, "readonly"));

  print OUT "  set $propnm(arg0) {\n";
  print OUT "    this.setProp(\'$propnm\', arg0);\n";
  print OUT "  }\n";
  print OUT "\n";
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

    print OUT "  get $key() {\n";
    print OUT "    return this.getEnumDef(\'$propnm\', \'$defnm\');\n";
    print OUT "  }\n";
    print OUT "\n";
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

    print OUT "  // method: $nm\n";

    print OUT "  ${nm}(".makeMthSignt($mth).") {\n";
    print OUT "    return this.invokeMethod(".makeMthArg($mth).");\n";
    print OUT "  };\n";
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

  my @rval = ("\'$name\'");

  my $ind = 0;
  foreach my $arg (@{$args}) {
    my $arg_type = $arg->{"type"};
    push(@rval, "arg_${ind}");
    ++$ind;
  }
  return join(", ", @rval);
}

1;


