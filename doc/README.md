# Invocation
```shell
./project [options] [file]
```

`[options]` can be `-r` (to display the output without decorations) or omitted (to display it with decorations).
`[file]` is the input file.

# Input File Format
The input file must contain several sections, with a blank line between consecutive sections. A section is defined as
a sequence of non-blank lines, the first of which contains a single word in square brackets <kbd>[</kbd><kbd>]</kbd>.

## Sections
### `[points]`
The number of points earned by a team in case of a win, loss and any other result. These must be on separate lines.
* If this section is not specified, the three values will default to 2, 0 and 1 respectively.
* If this section is specified,
  * it must be before `[table]`, `[completed]` or `[upcoming]`; and
  * the points in case of a win, loss and any other result need not all be specified (the above-mentioned default
    values will be used if necessary).

#### Examples
```
[points]
win 3
loss -1
```

```
[points]
win 3
loss 1
other 2
```

### `[team]`
The name of your favourite team (i.e. the team whose possible outcomes you'd like to see).

#### Examples
```
[team]
TokyoTacticians
```

```
[team]
Banana
```

### `[table]`
The points table. Each entry of the table must be on a separate line. An entry is described by the name of a team and
the points it has earned so far.
* If this section is not specified, it is assumed that no fixtures have been played yet.
* If this section is specified,
  * it must be before the `[upcoming]` section; and
  * the `[completed]` section must not be specified.

#### Examples
```
[table]
LondonLightbringers 2
TokyoTacticians 0
CairoChampions 2
ParisPursuers 2
```

```
[table]
Apple 6
Orange 2
Guava 1
Banana 1
Jackfruit 2
```

### `[completed]`
Tournament fixtures which have been completed. Each fixture must be on a separate line. A fixture is described by the
names of the two teams which played. If the fixture was tied or drawn or similar, an equals sign <kbd>=</kbd> must
separate them. Else, a comma <kbd>,</kbd> must separate them, and the winner must be written first.
* If this section is not specified, it is assumed that no fixtures have been played yet.
* If this section is specified,
  * it must be before the `[upcoming]` section; and
  * the `[table]` section must not be specified.

#### Examples
```
[completed]
LondonLightbringers,TokyoTacticians
CairoChampions,ParisPursuers
ParisPursuers,TokyoTacticians
```

```
[completed]
Apple,Orange
Jackfruit,Guava
Apple,Jackfruit
Banana=Guava
Orange,Guava
Apple,Banana
```

### `[upcoming]`
Tournament fixtures which are to be played. Each fixture must be on a separate line. A fixture is described by the
names of the two teams which will play. A comma <kbd>,</kbd> or an equals sign <kbd>=</kbd> must separate them.

#### Examples
```
[upcoming]
ParisPursuers,LondonLightbringers
TokyoTacticians,CairoChampions
```

```
[upcoming]
Orange,Banana
Apple,Guava
Orange,Jackfruit
Jackfruit,Banana
```

## Notes
* Team names are case-sensitive, and must not contain spaces.
* Spaces are not automatically stripped from anywhere. Not even before or after a comma <kbd>,</kbd> or an equals sign
  <kbd>=</kbd>.
  * Hence, avoid using spaces, except between a word and a number, as seen in the examples of the `[points]` and
    `[table]` sections above.
* The maximum number of teams supported is 1024.
  * This is not a hard limit. You can increase it simply by changing the number in the constructor of
    `PointsTableProjector`.
* All teams start with zero points.

## Sample Input File
Teams Apple, Banana, Guava, Jackfruit and Orange compete in a tournament. Our favourite team is Banana.

Six matches have been played.

1. Apple beat Orange;
1. Jackfruit beat Guava;
1. Apple beat Jackfruit;
1. Banana tied with Guava;
1. Orange beat Guava; and
1. Apple beat Banana.

Four matches remain.

1. Orange v/s Banana;
1. Apple v/s Guava;
1. Orange v/s Jackfruit; and
1. Jackfruit v/s Banana.

You could write the input file like this:

```
[team]
Banana

[completed]
Apple,Orange
Jackfruit,Guava
Apple,Jackfruit
Banana=Guava
Orange,Guava
Apple,Banana

[upcoming]
Orange,Banana
Apple,Guava
Orange,Jackfruit
Jackfruit,Banana
```

or like this.

```
[team]
Banana

[table]
Apple 6
Orange 2
Guava 1
Banana 1
Jackfruit 2

[upcoming]
Orange,Banana
Apple,Guava
Orange,Jackfruit
Jackfruit,Banana
```

The two ways are equivalent.

# Processing
## Favouritism
The program assumes that our favourite team will win all their remaining matches.

## Pruning
If any match is found to be inconsequential, the program picks its result randomly (instead of exploring two
possibilities, one in which the first team wins and the other in which the second team wins). A match is said to be
inconsequential if it is played between two inconsequential teams. A team is said to be inconsequential if,
irrespective of what happens, it always ends up with more or fewer points than our favourite team.

# Output Format
Consider the [tournament described above](#sample-input-file). Ordinarily, since there are four matches left, there
would have been 2<sup>4</sup> = 16 different scenarios. However, because of favouritism, it is assumed that Banana will
win both of their remaining matches. Also, note that Banana cannot earn more points than Apple, and Guava cannot earn
more points than Banana. This means that the match between Apple and Guava is inconsequential, so its result is picked
randomly. Hence, only 2<sup>1</sup> = 2 scenarios are output.

<details>

<summary>Click to view possible output.</summary>

```
2
├─[table]
│ Apple 6
│ Banana 5
│ Jackfruit 4
│ Guava 3
│ Orange 2
└─[upcoming]
  Banana,Orange
  Guava,Apple
  Jackfruit,Orange
  Banana,Jackfruit
2
├─[table]
│ Apple 6
│ Banana 5
│ Orange 4
│ Guava 3
│ Jackfruit 2
└─[upcoming]
  Banana,Orange
  Guava,Apple
  Orange,Jackfruit
  Banana,Jackfruit
```

</details>

In the first scenario, Banana finish at position 2.

The final points table is shown under `[table]`. Banana get 5 points in all. Apple top the table with 6 points.
Jackfruit, Guava and Orange end up with 4, 3 and 2 points respectively.

For the tournament to end like this, the events that must transpire are mentioned under `[upcoming]`. (The name of the
winning team is written first.)
* Banana must beat Orange and Jackfruit;
* Guava must beat Apple; and
* Jackfruit must beat Orange.

The match between Apple and Guava will be written in a duller colour to emphasise the fact that it is inconsequential
with respect to Banana. `table` and `upcoming` will be coloured green. (Use the `-r` option to display raw output, i.e.
without colours and the lines to the left of the text.)

If multiple teams had earned the same points as Banana, Banana would have been placed highest among them, because the
program is written to calculate the _best_ outcome for our favourite team.

The second scenario is interpreted similarly.
