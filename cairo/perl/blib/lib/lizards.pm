# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.9
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.

package lizards;
use base qw(Exporter);
use base qw(DynaLoader);
package lizardsc;
bootstrap lizards;
package lizards;
@EXPORT = qw();

# ---------- BASE METHODS -------------

package lizards;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;
}

sub CLEAR { }

sub FIRSTKEY { }

sub NEXTKEY { }

sub FETCH {
    my ($self,$field) = @_;
    my $member_func = "swig_${field}_get";
    $self->$member_func();
}

sub STORE {
    my ($self,$field,$newval) = @_;
    my $member_func = "swig_${field}_set";
    $self->$member_func($newval);
}

sub this {
    my $ptr = shift;
    return tied(%$ptr);
}


# ------- FUNCTION WRAPPERS --------

package lizards;


############# Class : lizards::point_t ##############

package lizards::point_t;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "=" => sub { my $class = ref($_[0]); $class->new($_[0]) },
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *lizardsc::point_t_x_get;
*swig_x_set = *lizardsc::point_t_x_set;
*swig_y_get = *lizardsc::point_t_y_get;
*swig_y_set = *lizardsc::point_t_y_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_point_t(@_);
    bless $self, $pkg if defined($self);
}

*__eq__ = *lizardsc::point_t___eq__;
*__ne__ = *lizardsc::point_t___ne__;
*__iadd__ = *lizardsc::point_t___iadd__;
*__add__ = *lizardsc::point_t___add__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_point_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::fpoint_t ##############

package lizards::fpoint_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *lizardsc::fpoint_t_x_get;
*swig_x_set = *lizardsc::fpoint_t_x_set;
*swig_y_get = *lizardsc::fpoint_t_y_get;
*swig_y_set = *lizardsc::fpoint_t_y_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_fpoint_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_fpoint_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::fplace_t ##############

package lizards::fplace_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_center_get = *lizardsc::fplace_t_center_get;
*swig_center_set = *lizardsc::fplace_t_center_set;
*swig_bounds_get = *lizardsc::fplace_t_bounds_get;
*swig_bounds_set = *lizardsc::fplace_t_bounds_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_fplace_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_fplace_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::feature_t ##############

package lizards::feature_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*id = *lizardsc::feature_t_id;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_feature_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_feature_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::band_t ##############

package lizards::band_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards::feature_t lizards );
%OWNER = ();
%ITERATORS = ();
*set = *lizardsc::band_t_set;
*get = *lizardsc::band_t_get;
*size = *lizardsc::band_t_size;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_band_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_band_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::raft_t ##############

package lizards::raft_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards::feature_t lizards );
%OWNER = ();
%ITERATORS = ();
*swig_age_get = *lizardsc::raft_t_age_get;
*swig_age_set = *lizardsc::raft_t_age_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_raft_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_raft_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::hex_t ##############

package lizards::hex_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_owner_get = *lizardsc::hex_t_owner_get;
*swig_owner_set = *lizardsc::hex_t_owner_set;
*swig_active_get = *lizardsc::hex_t_active_get;
*swig_active_set = *lizardsc::hex_t_active_set;
*getid = *lizardsc::hex_t_getid;
*setid = *lizardsc::hex_t_setid;
*swig_xy_get = *lizardsc::hex_t_xy_get;
*swig_xy_set = *lizardsc::hex_t_xy_set;
*is_type = *lizardsc::hex_t_is_type;
*get = *lizardsc::hex_t_get;
*set = *lizardsc::hex_t_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_hex_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_hex_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::water_t ##############

package lizards::water_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards::hex_t lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_water_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_water_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::plains_t ##############

package lizards::plains_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards::hex_t lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_plains_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_plains_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::fertile_t ##############

package lizards::fertile_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards::hex_t lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_fertile_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_fertile_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::layout_t ##############

package lizards::layout_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_size_get = *lizardsc::layout_t_size_get;
*swig_size_set = *lizardsc::layout_t_size_set;
*realize = *lizardsc::layout_t_realize;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_layout_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_layout_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::layouts_t ##############

package lizards::layouts_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*size = *lizardsc::layouts_t_size;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_layouts_t(@_);
    bless $self, $pkg if defined($self);
}

*layout = *lizardsc::layouts_t_layout;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_layouts_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::template_t ##############

package lizards::template_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_template_t(@_);
    bless $self, $pkg if defined($self);
}

*cost = *lizardsc::template_t_cost;
*realize = *lizardsc::template_t_realize;
*save = *lizardsc::template_t_save;
*getactive = *lizardsc::template_t_getactive;
*setactive = *lizardsc::template_t_setactive;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_template_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::grid_t ##############

package lizards::grid_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_grid_t(@_);
    bless $self, $pkg if defined($self);
}

*lay_islands = *lizardsc::grid_t_lay_islands;
*generate_typical = *lizardsc::grid_t_generate_typical;
*__call__ = *lizardsc::grid_t___call__;
*swig_width_get = *lizardsc::grid_t_width_get;
*swig_width_set = *lizardsc::grid_t_width_set;
*swig_height_get = *lizardsc::grid_t_height_get;
*swig_height_set = *lizardsc::grid_t_height_set;
*move = *lizardsc::grid_t_move;
*replace = *lizardsc::grid_t_replace;
*place_template = *lizardsc::grid_t_place_template;
*output = *lizardsc::grid_t_output;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_grid_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::fbox_t ##############

package lizards::fbox_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_left_get = *lizardsc::fbox_t_left_get;
*swig_left_set = *lizardsc::fbox_t_left_set;
*swig_top_get = *lizardsc::fbox_t_top_get;
*swig_top_set = *lizardsc::fbox_t_top_set;
*swig_right_get = *lizardsc::fbox_t_right_get;
*swig_right_set = *lizardsc::fbox_t_right_set;
*swig_bottom_get = *lizardsc::fbox_t_bottom_get;
*swig_bottom_set = *lizardsc::fbox_t_bottom_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_fbox_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_fbox_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::fpoint_v ##############

package lizards::fpoint_v;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_fpoint_v(@_);
    bless $self, $pkg if defined($self);
}

*size = *lizardsc::fpoint_v_size;
*empty = *lizardsc::fpoint_v_empty;
*clear = *lizardsc::fpoint_v_clear;
*push = *lizardsc::fpoint_v_push;
*pop = *lizardsc::fpoint_v_pop;
*get = *lizardsc::fpoint_v_get;
*set = *lizardsc::fpoint_v_set;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_fpoint_v($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::single_t ##############

package lizards::single_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_id_get = *lizardsc::single_t_id_get;
*swig_id_set = *lizardsc::single_t_id_set;
*swig_file_get = *lizardsc::single_t_file_get;
*swig_file_set = *lizardsc::single_t_file_set;
*swig_center_get = *lizardsc::single_t_center_get;
*swig_center_set = *lizardsc::single_t_center_set;
*swig_bounds_get = *lizardsc::single_t_bounds_get;
*swig_bounds_set = *lizardsc::single_t_bounds_set;
*swig_path_get = *lizardsc::single_t_path_get;
*swig_path_set = *lizardsc::single_t_path_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_single_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_single_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::single_v ##############

package lizards::single_v;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_single_v(@_);
    bless $self, $pkg if defined($self);
}

*size = *lizardsc::single_v_size;
*empty = *lizardsc::single_v_empty;
*clear = *lizardsc::single_v_clear;
*push = *lizardsc::single_v_push;
*pop = *lizardsc::single_v_pop;
*get = *lizardsc::single_v_get;
*set = *lizardsc::single_v_set;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_single_v($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::singles_t ##############

package lizards::singles_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_bounds_get = *lizardsc::singles_t_bounds_get;
*swig_bounds_set = *lizardsc::singles_t_bounds_set;
*swig_singles_get = *lizardsc::singles_t_singles_get;
*swig_singles_set = *lizardsc::singles_t_singles_set;
*size = *lizardsc::singles_t_size;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_singles_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_singles_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::output_t ##############

package lizards::output_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_output_t(@_);
    bless $self, $pkg if defined($self);
}

*json = *lizardsc::output_t_json;
*single = *lizardsc::output_t_single;
*svg = *lizardsc::output_t_svg;
*hit = *lizardsc::output_t_hit;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_output_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::player_t ##############

package lizards::player_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
*swig_startup_get = *lizardsc::player_t_startup_get;
*swig_startup_set = *lizardsc::player_t_startup_set;
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_player_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_player_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::game_t ##############

package lizards::game_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_game_t(@_);
    bless $self, $pkg if defined($self);
}

*swig_nplayers_get = *lizardsc::game_t_nplayers_get;
*swig_nplayers_set = *lizardsc::game_t_nplayers_set;
*player = *lizardsc::game_t_player;
*bootstrap = *lizardsc::game_t_bootstrap;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_game_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::overlay_grid_t ##############

package lizards::overlay_grid_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_overlay_grid_t(@_);
    bless $self, $pkg if defined($self);
}

*mark = *lizardsc::overlay_grid_t_mark;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_overlay_grid_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : lizards::serial_t ##############

package lizards::serial_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( lizards );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = lizardsc::new_serial_t(@_);
    bless $self, $pkg if defined($self);
}

*to_file = *lizardsc::serial_t_to_file;
*from_file = *lizardsc::serial_t_from_file;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        lizardsc::delete_serial_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


# ------- VARIABLE STUBS --------

package lizards;

*DEN = *lizardsc::DEN;
*BAND = *lizardsc::BAND;
*PEAK = *lizardsc::PEAK;
*VOLCANO = *lizardsc::VOLCANO;
*RED = *lizardsc::RED;
*GREEN = *lizardsc::GREEN;
*GREY = *lizardsc::GREY;
*BLACK = *lizardsc::BLACK;
*YELLOW = *lizardsc::YELLOW;
*UNSETTLED = *lizardsc::UNSETTLED;
*FORMAT_FILE_TXT = *lizardsc::FORMAT_FILE_TXT;
*FORMAT_FILE_XML = *lizardsc::FORMAT_FILE_XML;
*FORMAT_FILE_JS = *lizardsc::FORMAT_FILE_JS;
*FORMAT_STRING_TXT = *lizardsc::FORMAT_STRING_TXT;
*FORMAT_STRING_XML = *lizardsc::FORMAT_STRING_XML;
*FORMAT_STRING_JS = *lizardsc::FORMAT_STRING_JS;
1;
