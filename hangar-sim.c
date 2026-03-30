// hangar-sim
// simulate and compare two sales strategies for the hangar in GTA Online
// assumes using rooster to source
// full explanation of how hangar works is beyond scope of this documentation
// strategy 1: small sales. when the hangar is full, sell the cargo type of highest value
// strategy 2: big sales. when the hangar is full, just sell everything

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CARGO_TYPES 8
#define MAX_CRATES 50

#define STEPS 1000000000


// good enough RNG
static uint64_t rng_state = 0;
static inline uint64_t xorshift64(void) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return rng_state;
}


int main(void) {
    // seed our good enough rand with actual rand
    srand(time(NULL));
    while(rng_state == 0)
        rng_state = rand();

    // ss[] = small sale hangar
    // ss_value[] = current total value of each cargo type with bonuses
    // ss_crates = total number of crates in hangar
    // note that since the big sale strategy foregoes bonuses
    // we don't need to track which crates it has, just total crates
    int ss[CARGO_TYPES] = {0};
    int ss_value[CARGO_TYPES] = {0};

    // each hangar has 0 total crates to start
    int ss_crates = 0, bs_crates = 0;

    // initialize values to track
    long long small_profits = 0;
    long long big_profits = 0;
    long long small_sales = 0;
    long long big_sales = 0;
    long long total_cost = 0;
    int max_crates_per_sale = 0;

    // bonus table: bonus[cargo_type][num_crates], indices 0..50
    // contains multiplier to use when selling single cargo type
    float bonus[CARGO_TYPES][MAX_CRATES + 1];

    for (int i = 0; i < CARGO_TYPES; i++) {
        for (int j = 0; j <= MAX_CRATES; j++) {
            if (i < 3) {
                if (j < 25)       bonus[i][j] = 1.0;
                else if (j < 50)  bonus[i][j] = 1.35;
                else              bonus[i][j] = 1.7;
            } else if (i < 6) {
                if (j < 10)       bonus[i][j] = 1.0;
                else if (j < 20)  bonus[i][j] = 1.12;
                else if (j < 30)  bonus[i][j] = 1.24;
                else if (j < 40)  bonus[i][j] = 1.36;
                else if (j < 50)  bonus[i][j] = 1.48;
                else              bonus[i][j] = 1.6;
            } else {
                if (j < 5)        bonus[i][j] = 1.0;
                else if (j < 10)  bonus[i][j] = 1.05;
                else if (j < 15)  bonus[i][j] = 1.1;
                else if (j < 20)  bonus[i][j] = 1.15;
                else if (j < 25)  bonus[i][j] = 1.20;
                else if (j < 30)  bonus[i][j] = 1.25;
                else if (j < 35)  bonus[i][j] = 1.3;
                else if (j < 40)  bonus[i][j] = 1.35;
                else if (j < 45)  bonus[i][j] = 1.40;
                else if (j < 50)  bonus[i][j] = 1.45;
                else              bonus[i][j] = 1.5;
            }
        }
    }

    // initialize loop-global variables
    long long step = 0;
    int countdown = STEPS;

    // run similuation. at each step, rooster sources crates. if either
    // hangar is full, it is sold using the appropriate strategy.
    while (1) {
        step++;


        // rooster has a 25% chance of sourcing 2 crates
        // costs 25k to source
        total_cost += 25000;
        // use a good enough random number
        uint64_t rng = xorshift64();
        // use bits 0-2 to select cargo type
        int crate = rng % 8;
        // use bits 3-4 for chance at 2 crates
        int num_crates = ((rng >> 3) & 3) == 0 ? 2 : 1;

        // if adding 2 crates will put us over 50, only add enough to go to 50
        int add_ss = num_crates == 1 ? 1 : (ss_crates < MAX_CRATES - 1 ? 2 : 1);
        int add_bs = num_crates == 1 ? 1 : (bs_crates < MAX_CRATES - 1 ? 2 : 1);

        // add crate
        ss[crate] += add_ss;
        // calculate new value for cargo type
        ss_value[crate] = (int)(ss[crate] * 30000 * 1.1 * bonus[crate][ss[crate]]);

        // update total crates
        ss_crates += add_ss;
        bs_crates += add_bs;

        // if hangar is full, sell
        if (ss_crates == MAX_CRATES) {
            small_sales++;
            // find best value
            int best_slot = 0;
            int best_value = 0;
            for (int i = 0; i < CARGO_TYPES; i++) {
                if (ss_value[i] > best_value) {
                    best_slot = i;
                    best_value = ss_value[i];
                }
            }
            if (ss[best_slot] > max_crates_per_sale) {
                max_crates_per_sale = ss[best_slot];
                printf("at sale %lld, max sold is %d\n", small_sales, max_crates_per_sale);
            }
            small_profits += best_value - 30000;
            ss_crates -= ss[best_slot];
            ss[best_slot] = 0;
            ss_value[best_slot] = 0;
        }

        if (bs_crates == MAX_CRATES) {
            big_sales++;
            big_profits += 1630000;
            bs_crates = 0;
        }

        // print out info every STEPS steps
        countdown--;
        if (countdown == 0) {
            countdown = STEPS;
            double sales_ratio = (double)small_sales / (double)big_sales;
            double earnings_ratio = (double)small_profits / (double)big_profits;
            printf("Step %lld: Cost: %lld Small: %lld sales, $%lld Large: %lld sales, $%lld\n", step, total_cost, small_sales, small_profits, big_sales, big_profits);
            printf("Step %lld: sales ratio: %f earnings ratio: %f max crates per small sale: %d\n", step, sales_ratio, earnings_ratio, max_crates_per_sale);
        }
    }

    return 0;
}
