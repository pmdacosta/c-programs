#include "game.h"

int main() {
    struct Game* game = NULL;

    game = game_new();
    if (!game) {
        fprintf(stderr, "%s:%d: Error initializing game\n", __FILE__, __LINE__);
        return 1;
    }

    if (game_run(game)) {
        fprintf(stderr, "%s:%d: Error running game\n", __FILE__, __LINE__);
        game_free(game);
        return 1;
    }

    game_free(game);
    return 0;
}
