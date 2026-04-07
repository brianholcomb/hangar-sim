extern crate rand;
use rand::prelude::*;

fn main() {
    const CARGO_TYPES: usize = 8;
    const MAX_CRATES: usize = 50;

    const STEPS: u64 = 10000000;

    let mut rng = rand::rng();

    let mut ss: [u32; CARGO_TYPES] = [0; 8];
    let mut ss_value: [u32; CARGO_TYPES] = [0; 8];
    let mut ss_crates: u32 = 0;
    let mut bs_crates: u32 = 0;

    let mut bonus: [[f32; MAX_CRATES + 1]; CARGO_TYPES] = [[0.0; MAX_CRATES + 1];CARGO_TYPES];
    for i in 0..CARGO_TYPES {
        for j in 0..MAX_CRATES + 1 {
            if i < 3 {
                if j < 25        {bonus[i][j] = 1.00;}
                else if j < 50   {bonus[i][j] = 1.35;}
                else             {bonus[i][j] = 1.70;}
            }
            else if i < 6 {
                if j < 10        {bonus[i][j] = 1.00;}
                else if j < 20   {bonus[i][j] = 1.12;}
                else if j < 30   {bonus[i][j] = 1.24;}
                else if j < 40   {bonus[i][j] = 1.36;}
                else if j < 50   {bonus[i][j] = 1.48;}
                else             {bonus[i][j] = 1.60;}
            }
            else {
                if j < 5         {bonus[i][j] = 1.00;}
                else if j < 10   {bonus[i][j] = 1.05;}
                else if j < 15   {bonus[i][j] = 1.10;}
                else if j < 20   {bonus[i][j] = 1.15;}
                else if j < 25   {bonus[i][j] = 1.20;}
                else if j < 30   {bonus[i][j] = 1.25;}
                else if j < 35   {bonus[i][j] = 1.30;}
                else if j < 40   {bonus[i][j] = 1.35;}
                else if j < 45   {bonus[i][j] = 1.40;}
                else if j < 50   {bonus[i][j] = 1.45;}
                else             {bonus[i][j] = 1.50;}
            }
        }
    }

    let mut step: u64 = 0;
    let mut small_profits: u64 = 0;
    let mut big_profits: u64 = 0;
    let mut small_sales: u64 = 0;
    let mut big_sales: u64 = 0;
    let mut total_cost: u64 = 0;

    while step < STEPS {
        step += 1;

        total_cost += 25000;
        let cur_crate: usize = rng.random_range(0..=7);
        let num_crates: u32 = if rng.random_range(1..=4) == 1 { 2 } else { 1 };
//        println!("sourced {} crates of {}", num_crates, cur_crate);
        let add_ss: u32 = if num_crates + ss_crates <= 50 { num_crates } else { 1 };
        let add_bs: u32 = if num_crates + bs_crates <= 50 { num_crates } else { 1 };
        ss[cur_crate] += add_ss;
        ss_value[cur_crate] = ((ss[cur_crate] as f32) * 30000.0 * 1.1 * bonus[cur_crate][ss[cur_crate] as usize]) as u32;
        ss_crates += add_ss;
        bs_crates += add_bs;

//        println!("SC: {} BC: {}", ss_crates, bs_crates);

        if ss_crates == MAX_CRATES as u32 {
            small_sales += 1;
            let mut best_slot: u32 = 0;
            let mut best_value: u32 = 0;
            for i in 0..CARGO_TYPES {
                if ss_value[i] > best_value {
                    best_slot = i as u32;
                    best_value = ss_value[i];
                }
            }
            small_profits += (best_value - 30000) as u64;
            ss_crates -= (ss[best_slot as usize]) as u32;
            ss[best_slot as usize] = 0;
            ss_value[best_slot as usize] = 0;
        }
        if bs_crates == MAX_CRATES as u32 {
            big_sales += 1;
            big_profits += 1620000;
            bs_crates = 0;
        }
    }
    let sales_ratio: f64 = small_sales as f64 / big_sales as f64;
    let profit_ratio: f64 = small_profits as f64 / big_profits as f64;
    println!("Cost: {} SS: {} ${} BS: {} ${}", total_cost, small_sales, small_profits, big_sales, big_profits);
    println!("Sales Ratio: {} Profit Ratio: {}", sales_ratio, profit_ratio);


    return();
}
