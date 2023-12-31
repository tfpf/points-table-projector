# Points Table Projector
Determine the best outcomes for a team in an ongoing tournament.

![style](https://github.com/tfpf/points-table-projector/actions/workflows/style.yml/badge.svg)
![build](https://github.com/tfpf/points-table-projector/actions/workflows/build.yml/badge.svg)

## Compile
```shell
make -j
```

## Run
Specify the path to a file describing the tournament as the first argument. For instance, to find out New Zealand's
best possible outcomes in the 2023 Cricket World Cup as of 4 November 2023, run the following command.
```shell
./project examples/cricket_world_cup_2023.txt
```

For a detailed description of the syntax of the file and the resulting output, go to [`doc`](doc). For more examples,
check [`examples`](examples).
