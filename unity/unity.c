#include "unity.h"
#include <stddef.h>


BEGIN {
    use_ok('TicTac') || BAIL_OUT;
}

is( ref TicTac::TEST_REX, 'Regexp', 'Regex object constant declared.' );
is( TicTac::MIN_POSITION, 0, 'MIN_POSITION constant.' );
is( TicTac::MAX_POSITION, 8, 'MAX_POSITION constant.' );

can_ok(
    new_ok('TicTac'),   # isa_ok is implicit here.
    qw( winner        board        _decompose_move
        _valid_move   _place_move  _set_position play_moves )
);

# Note: These two tests break encapsulization.
is( TicTac->new->{board}, '---------', 'Empty board initialized.' );
is( TicTac->new('xxxoooxxx')->{board}, 'xxxoooxxx', 'Pre-set board.' );

# We'll use our accessor from now on.
is( TicTac->new->board, '---------', 'Board accessor gets.' );
my $game = TicTac->new;
$game->board('xxxoooxxx');
is( $game->board, 'xxxoooxxx', 'Board accessor sets.' );

my %trial_wins = (
    'Three in first row.'       => [ 'xxx......', 'x'   ],
    'Three in last row.'        => [ '......xxx', 'x'   ],
    'Three in first column.'    => [ 'o..o..o..', 'o'   ],
    'Three in last column.'     => [ '..x..x..x', 'x'   ],
    'Forward diagonal.'         => [ 'o...o...o', 'o'   ],
    'Backward diagonal.'        => [ '..o.o.o..', 'o'   ],
    'X wins vertically.'        => [ '.oxoox..x', 'x'   ],
    'O wins vertically.'        => [ 'xo..oxxo.', 'o'   ],
    'No winner, full board.'    => [ 'xoxoxooxo', undef ],
    'No winner, three-in-a-row' => [ '..ooo..o.', undef ],
    'X wins, diamond pattern.'  => [ 'x.x.x.x.x', 'x'   ],
    'No winner, rev-diamond.'   => [ '.x.x.x.x.', undef ],
);

while ( my ( $test_desc, $criteria ) = each %trial_wins ) {
    my ( $board, $expect ) = @{$criteria};
    is( TicTac->new($board)->winner(), $expect, $test_desc );
}

is_deeply( [ TicTac->new->_decompose_move('x5') ],
    [qw( x 5 )], '_decompose_move splits player id from grid location.' );

ok( TicTac->new('x-xxxxxxx')->_valid_move(1),  'Allow valid move.' );
ok( !TicTac->new('x-xxxooxx')->_valid_move(0), 'Disallow invalid move.' );
ok( !TicTac->new('---------')->_valid_move(-1), 'Disallow undeflow moves.' );
ok( !TicTac->new('---------')->_valid_move(9), 'Disallow overflow moves.' );

my $placer = TicTac->new('x-o-xxxxx');
$placer->_set_position( 8, 'o' );
is( $placer->board, 'x-o-xxxxo', 'set_position succeeds.' );
ok( $placer->_place_move( 'x', 1 ), 'Placed a valid move.' );
is( $placer->board, 'xxo-xxxxo', 'Board updated with x placement.' );
ok( !$placer->_place_move( 'o', 0 ), 'Failed to place an invalid move.' );
is( $placer->board, 'xxo-xxxxo', 'Board not updated with invalid move.' );

is( TicTac->new->play_moves(qw( x0 x1 x2 )),       'x', 'X wins!' );
is( TicTac->new->play_moves(qw( x1 o2 x4 o0 x7 )), 'x', 'X won again!' );
is( TicTac->new->play_moves(qw( x0 o6 x1 o2 x8 o4 )), 'o', 'O wins!' );
is( TicTac->new->play_moves(qw() ), undef, 'No winner.' );
is( TicTac->new->play_moves(qw( x0 o2 x1 o3 x5 o4 x6 o7 x8 )), undef,
    'Full board, no winner.'
);

eval { TicTac->new->play_moves(qw( x0 x0 ) )};
like( $@ , qr/invalid game/i, 'Dies when game play includes invalid moves.' );

done_testing();
