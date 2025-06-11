# Sequence Generation
By Naufal Ahmad (na867@drexel.edu)

## Usage

Change directory to the project folder and run the following command, where `n` indicates the value for NS1D0(n) and `filename` is the file to output the sequences to.
```sh
./bin/sequence [n] [filename]
```

## Questions

### 1. How did you use concurrency to solve the problem?

Firstly, I read [Sagols, F., & Colbourn, C. J. (2002). NS1D0 Sequences and Anti-Pasch Steiner Triple Systems. Ars Combinatoria, 62.](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=ebdbbf4d691ae27cb355acce71b702a7370a16cc), which discussed similarities between the NS1D0 sequences and the n-queens problem, and provided an algorithm for determining NS1D0 sequences using a modified n-queens approach.

An NS1D0 sequence has an associated **queen arrangement**, which can be found using the following procedure:

1. Take an NS1D0 sequence (e.g. `0, 5, 2, 1` | `n=7`) and split it into adjacent pairs:
    - `(0, 5)`
    - `(5, 2)`
    - `(2, 1)`
2. These pairs represent zero-indexed coordinates `(i, j)` on an `n`-by-`n` chessboard for a queen (see below).

![](img/image.png)

Here are the key differences between the NS1D0 problem and the typical n-queens problem:
1. `n` must be odd in NS1D0.
2. Only `floor(n/2)` queens need to be placed in NS1D0.
3. Queens do not attack squares on their respective positive diagonal (northeast and southwest)
4. No queens may be placed on rows `1` and `ceil(n/2)`, columns `0` and `ceil(n/2)`, and the main diagonal.
    - `Row 1`: If a queen was placed on this row, it would indicate that `1` appears in the middle of the sequence, which cannot happen.
    - `Row ceil(n/2)`: The value of `ceil(n/2)` cannot appear in the sequence.
    - `Column 0`: If a queen was placed on this column, it would indicate that `0` appears in the middle or end of the sequence, which cannot happen.
    - `Column ceil(n/2)`: See `Row ceil(n/2)`.
    - `Main diagonal`: If a queen is placed here, it would indicate that the sequence contains duplicate values, which cannot happen.
5. For every queen placed on row `i`, no queens may be placed on row `1 - i mod n`. Similarly, for every queen placed on row `j`, no queens may be placed on row `1 - j mod n`. This corresponds to **Rule 5** from the instructions.
6. Let diagonal `k = (j - i) mod n`. For every queen is placed on diagonal `k`, no queens may be placed on diagonal `-k mod n`. This corresponds to **Rule 6** from the instructions.

The algorithm described in the paper (and utilized in my implementation) is essentially a modified iterative n-queens algorithm but with modifications to meet the aforementioned requirements.

To implement concurrency, I modified the algorithm described in the paper such that, given an input `k`, the branch of the algorithm where the first queen is located at `(0,k)` is given to a worker thread using a thread pool.

### 2. How do your threads communicate? Why?

The threads do not need to communicate data, as the tasks they perform are independent of the other threads' tasks. Locks are used to ensure only one thread is writing to the sequence file at a time.

### 3. What is the largest value n your program runs in a reasonable time for? (Define *reasonable* yourself)

All trials performed on Tux:

| n | Execution Time |
| - | - |
| <=15 | <100ms |
| 17 | 186ms |
| 19 | 562ms |
| 21 | 4.064s |
| 23 | 31.958s |
| 25 | 5m22.034s |

| n | Number of Sequences | File Size |
| - | - | - |
| 15 | 304 | 7.75kB |
| 17 | 1636 | 48.3kB |
| 19 | 10872 | 358kB |
| 21 | 71292 | 2.64MB |
| 23 | 542354 | 22.2MB |
| 25 | 4581580 | 206MB |

In general, it seems that if `NS1D0(n)` takes `t` seconds, then `NS1DO(n+2)` will take approximately `8t` seconds. Therefore, extrapolating this data, running the program with `n = 27` will take **~45 minutes** and `n = 29` will take **~6 hours**. I wouldn't consider anything higher reasonable in terms of timing.

However, the more pressing issue is space, which grows super-exponentially as `n` increases. Therefore, larger input values will produce files that take up GBs of space, approaching TBs.