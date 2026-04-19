# Ipponboard Domain Language Glossary

Version: 1.0  
Status: Draft  
Source: IJF Sport and Organisation Rules + DJB/WKO adaptations

## Purpose

This glossary defines the canonical domain language for Ipponboard.  
It separates **domain concepts** from **UI/presentation terms** and ensures consistency across code, APIs, and documentation.

## Core Principles

- Use **IJF terminology** as baseline
- Separate:
  - identity (Athlete)
  - role (ContestSide/call order)
  - presentation (DisplayPosition, Designation)
- Avoid mixing UI language with domain concepts
- National extensions (e.g. DJB scoring) must be modeled as **derived logic**, not core language

## Core Domain Terms

| Term              | Definition                                                            | Notes                                                                                        | Terms to Avoid                                                    |
| ----------------- | --------------------------------------------------------------------- | -------------------------------------------------------------------------------------------- | ----------------------------------------------------------------- |
| Competition       | An organized sporting event consisting of contests across categories. | Synonym: Event (context-dependent)                                                           | Tournament                                                        |
| Contest           | A single bout between two contest sides.                              | Synonym: Bout                                                                                | Match (not IJF canonical)                                         |
| Athlete           | A person registered to compete in a competition.                      | Synonym: Competitor                                                                          | Fighter                                                           |
| Team              | A group of athletes competing collectively in a team competition.     |                                                                                              |
| ContestSide       | A role/slot within a contest assigned to an athlete or team entry.    | - Represents participation in a specific contest <br/>- Independent of visual representation | Fighter, White/Blue as structural concept, Left/Right as identity |
| WeightCategory    | A classification of athletes based on body weight.                    |                                                                                              | Weight class (prefer consistency)                                 |
| AgeGroup          | A classification of athletes based on age.                            |                                                                                              |                                                                   |
| Draw              | The structured arrangement of contests in a competition.              | Includes: brackets, pools, repechage                                                         | List                                                              |
| CompetitionSystem | The format used to organize contests.                                 | Examples: Direct Knockout, Double Repechage, Round Robin                                     |
| ContestOrder      | The sequence of contests on a tatami.                                 |                                                                                              |
CallOrder | The sequential order in which contest sides are announced before a contest begins. | Values: `1` = first called athlete, `2` = second called athlete; Allowed synonyms: First called / Second called | Avoid: first competitor, second competitor, primary / secondary fighter

## Participation & Structure

| Term               | Definition                                           | Notes                                               | Terms to Avoid                                         |
| ------------------ | ---------------------------------------------------- | --------------------------------------------------- | ------------------------------------------------------ |
| Designation        | Visual identifier used to distinguish contest sides. | Examples: white, blue, red belt, white belt, custom | Important: Not stable → must not define identity       |
| DisplayPosition    | Visual placement on a display.                       | Values: left, right or first/second                 | Important: Purely presentation → must not affect logic |
| IdentificationType | Mechanism used to distinguish participants visually. | Examples: judogi color, belt color, sash            |

## Team Competition Terms

Term | Definition
-- | --
TeamContest | A contest between two teams consisting of multiple individual contests.
IndividualContest | A single bout within a team contest.
LineUp | Assignment of athletes to weight categories in a team contest.
TeamSheet | Official listing of team members and assignments.

## Weigh-in & Eligibility

Term | Definition
-- | --
OfficialWeighIn | Formal process to verify athlete eligibility.
UnofficialWeighIn | Preliminary weight check.
RandomWeighIn | Post-draw random weight control.
Entry | Registration of an athlete into a competition or category.

## Results & Scoring

Term | Definition
-- | --  
Result | Outcome of a contest. 
VictoryType | How a contest was won. Values: ippon, waza-ari, hansoku-make, fusen-gachi, decision
GoldenScore | Overtime phase to determine winner.
Penalty | Types: shido, hansoku-make

## Ranking & Progression

Term | Definition
-- | --  
Ranking | Ordered list of participants based on results.
Placement | Final position in a competition.
Seeding | Initial placement in the draw based on ranking.
PoolRanking | Ranking within a round robin group. _Note:_ Tie-break rules may vary (e.g. DJB SP/WP system)
DirectComparison | Head-to-head result used as tie-breaker.

## Derived / National Concepts (DJB)

These are **not canonical IJF terms**, but supported:

Term | Definition
-- | --  
Siegpunkt (SP) | Win counter used in pool ranking.
Wertungspunkt (WP) | Score-based ranking metric.


## Deprecated Terms (Do Not Use in Domain)

| Term           | Reason                      |
| -------------- | --------------------------- |
| fighter        | non-standard, ambiguous     |
| first fighter  | call order 1                |
| second fighter | call order 2                |
| left fighter   | contest side A              |
| right fighter  | contest side B              |
| white fighter  | mixes role and presentation |
| blue fighter   | mixes role and presentation |
| list           | unclear, replace with Draw  |

## Mapping Legacy → Canonical

| Legacy term        | Replace with    |
| ------------------ | --------------- |
| fighter (person)   | Athlete         |
| fighter (position) | ContestSide     |
| blue/white (role)  | Designation     |
| left/right         | DisplayPosition |
| list               | Draw            |

## Minimal Domain Model

```yaml
Competition
weightCategories[]
draw

Draw
competitionSystem
contests[]

Contest
sides[2]
result

ContestSide
athlete
designation
displayPosition

Result
winner
victoryType
penalties
goldenScore
```

---

## Notes

- Always separate **domain logic** from **visual representation**
- Avoid encoding federation-specific rules into core models
- Extend via configuration where possible
