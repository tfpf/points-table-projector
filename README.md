# Points Table Projector
Determine the best outcomes for a team in an ongoing tournament.

![style](https://github.com/tfpf/points-table-projector/actions/workflows/style.yml/badge.svg)
![build](https://github.com/tfpf/points-table-projector/actions/workflows/build.yml/badge.svg)

## Compile
```shell
make -j
```

## Run
Specify the path to a file describing the tournament as the first argument. Some example files are included in this
repository. (For a detailed description of the syntax of the file and the resulting output, go to [`doc`](doc).)

### Examples
#### New Zealand, Men's Cricket World Cup 2023
As of 4 November 2023, New Zealand were not guaranteed to reach the top four. Their chances depended on how other teams
performed. However, it turns out that if they won their remaining matches, they'd make it.
```shell
./project examples/cricket_world_cup_2023.txt
```

There is a lot of output, so it may be difficult to understand.

#### Gujarat Giants, Women's Premier League 2024
As of 9 March 2024, Gujarat Giants had won only one match, but they could still mathematically make it to the top
three.
```shell
./project examples/womens_premier_league_2024.txt
```

The output here is small enough to comprehend easily.
