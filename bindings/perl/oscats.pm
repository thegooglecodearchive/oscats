package oscats;
use Glib;
require DynaLoader;
our @ISA = qw(DynaLoader);
our $VERSION = '0.5';
sub dl_load_flags {0x01};
bootstrap oscats $VERSION;
1;
__END__
# put pod here
