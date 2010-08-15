#! perl

undef $/;
if ($ARGV[0] =~ /\.override$/) {
  $override = shift @ARGV;
  open IN, "$override" or die "Can't open $override\n";
  $override = <IN>;
  for $section (split /^\%\%\s*/mg, $override) {
    @lines = split /\n/, $section;
    $secname = shift @lines;
    if ($secname eq 'ignore') {
      $ignores = "|" . join("|", @lines) . "|";
    } elsif ($secname eq 'prefixes') {
      for $line (@lines) { ($a, $b) = split /\t/, $line; $prefix{$a} = $b; }
    } elsif ($secname =~ /^function/) {
      ($junk, $name, $prefix, $package) = split /\t/, $secname;
      print "Found override function $name\n";
      push @{$funcs{$prefix}}, join("\n", @lines) . "\n\n";
    } else {
      warn "Unkown section: $secname\n";
    }
  } # each section
} # if have override file

if ($ARGV[0] =~ /(\w+)\.defs/) {
  $modname = "$1";
} else {
  $modname = "Glib";
}

$info = <>;
for $defn ($info =~ /\(define-(\w+\s+([^()]+|[(]([^()]*|[(][^)]*[)])*[)])+)\)/sg) {
  next if ($defn =~ /^\s*$/s);
  $defn =~ s/GQuark/int/g;
  if ($defn =~ /^(\w+)\s+(\w+)/) {  $type = $1;   $name = $2;  }
  else { warn "Unparsable input: $defn"; next; }
  if ($defn =~ /\(in-module\s+"(\w+)"\)/) { $module = $1; }
  else { $module = "" }
  if ($defn =~ /\(parent\s+"(\w+)"\)/) { $parent = $1; }
  else { $parent = "" }
  if ($defn =~ /\(c-name\s+"(\w+)"\)/) { $cname = $1; }
  else { $cname = "" }
  if ($defn =~ /\(gtype-id\s+"(\w+)"\)/) { $gtypeid = $1; }
  else { $gtypeid = "" }
  if ($defn =~ /\(return-type\s+"([\w-]+\s*\*?)"\)/)
  {
    $ret = $1;
    $ret =~ s/-/ /;
  } 
  else { $ret = "" }
  if ($defn =~ /\(is-constructor-of\s+"(\w+)"\)/) { $construct = $1; }
  else { $construct = "" }
  if ($defn =~ /\(of-object\s+"(\w+)"\)/) { $obj = $1; }
  else { $obj = "" }
  if ($defn =~ /\(parameters\s+([^()]+|[(][^)]*[)])+\)/s)
  {
    $paramList = $&;
    @acc = ();
    @pacc = ();
    for $param ($paramList =~ /\((.*?)\)/g)
    {
      $param =~ s/-/ /;
      $param =~ /"([\w *]+)"\s+"(\w+)"/;
      push @acc, "$1 $2";
      push @pacc, "\$$2";
    }
    $params = join(", ", @acc);
    $pparams = join(", ", @pacc);
  } else { $params = ""; $pparams = ""; }

  $ret = "void" if ($ret eq 'none' or $ret eq '');

  $package = '';
  $myprefix = '';
  for $prefix (keys %prefix) {
    if ($cname =~ /^$prefix(.+)$/) {
      $package = "::" . $prefix{$prefix};
      $myprefix = $prefix;
      last;
    }
  }

# FIXME: expand support for GEnum, GFlags, GBoxed
  if ($type eq 'object')
  { push @maps, "$gtypeid\t$cname\tGObject\t$modname"."::$name\n"; }
  if ($type eq 'function')
  {
    if ($ret eq 'GType')
    {
      print "Skipping function $name.\n";
      next;
    }
    if ($ignores =~ /\|$cname\|/)
    {
      print "Ignoring function $name.\n";
      next;
    }
    if ($construct) 
    {
      $ret =~ s/(\w+)/$1_noinc/;
      $pnames = $pparams;
      $pnames =~ s/\$//g;
      push @{$funcs{$myprefix}}, "## call as \$obj = new $modname$package"."::$name($pparams)\n" .
                   "$ret $cname (SV * class" .
                   ($params ? ", $params" : "") .
                   ")\n\tC_ARGS:\n\t\t$pnames\n\n";
    } else { 
      push @{$funcs{$myprefix}}, "## call as $modname$package"."::$name($pparams)\n" .
                   "$ret $cname ($params);\n\n";
    }
  }
  if ($type eq 'method')
  {
    if ($ignores =~ /\|$cname\|/)
    {
      print "Ignoring method $cname.\n";
      next;
    }
    push @{$funcs{$myprefix}}, "## call as \$obj->$name($pparams)\n" .
                 "$ret $cname ($obj"." *self" .
                 ($params ? ", $params" : "") .
                 ");\n\n";
  }
}

open OUT, ">$modname.maps" or die "Can't open >$modname.maps\n";
print OUT @maps;
close OUT;

open OUT, ">lib/$modname.xs" or die "Can't open >lib/$modname.xs\n";
print OUT <<"END";
#include <gperl.h>
#include <oscats.h>
#include "oscats-autogen.h"

MODULE = $modname PACKAGE = $modname

BOOT:
#include "register-oscats.xsh"

END

for $prefix (keys %funcs) {
  if ($prefix) {
    print OUT "MODULE = $modname PACKAGE = $modname" . "::$prefix{$prefix} PREFIX = $prefix\n\n";
  } else {
    print OUT "MODULE = $modname PACKAGE = $modname\n\n";
  }
  print OUT join('', @{$funcs{$prefix}});
}
close OUT;
