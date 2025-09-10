#include "game.h"

int main() {
    struct Game game;

    
    if (game_new(&game)) {
        fprintf(stderr, "%s:%d: Error initializing game\n", __FILE__, __LINE__);
        return 1;
    }

    if (game_run(&game)) {
        fprintf(stderr, "%s:%d: Error running game\n", __FILE__, __LINE__);
        game_cleanup(&game);
        return 1;
    }

    game_cleanup(&game);
    return 0;
}
