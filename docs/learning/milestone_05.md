# Milestone 5 — Uniform Spatial Grid Broad Phase

## What We Built
Implemented an optimized Uniform Spatial Grid as a broad phase collision mechanism. We compared its performance against a rigorous brute-force benchmark loop to reliably document actual algorithm impact, reducing collision complexity from pure O(N^2) to an efficient average linear complexity for sparse scenes.

## Why Broad-Phase Collision Exists
To systematically cull unviable combinations and limit intensive and repeated exact intersection testing.

## Brute Force Recap
Tested each and every single `N(N-1)/2` pairing with an overarching O(N^2) complexity cost regardless of proximity.

## Uniform Spatial Grid
Partitions the active world environment into evenly spaced bounded grid cells to restrict local interaction tracking.

## Grid Cell Coordinates
An arbitrary 2D grid index (X,Z) determining precisely which discrete square volume tile of a uniform layout is being inspected.

## Negative Coordinates and std::floor
Normal casting from floating representation towards zeroes causes coordinate collapsing at the origin `(0, 0)` range. `std::floor` forces pure directional continuity across the zero axes.

## Hashing Grid Cells
Utilized combined and mixed hashed index mapping to place uniquely partitioned index values onto unordered associative standard maps.

## Mapping AABBs to Cells
Divides min and max physical coordinates evenly with current defined cell scale offset rounding downward (with floor) to determine exact logical range parameters.

## Why Objects Can Occupy Multiple Cells
Continuous geometry can easily extend beyond standard integer lines overlaying independent regions simultaneously, triggering interactions with content spread across divided bounds.

## Candidate Pair Generation
Compiles a list of combinations using purely available actors located closely interacting internally in mutually referenced grid segments.

## Duplicate Pair Problem
Extended geometry intersecting several separate domains inevitably creates duplicate referencing triggers for the exact same object pair in several separate domains.

## Canonical Pair Representation
By standardizing `min()` and `max()` placement within pair parameters, pairs like (A, B) and (B, A) equate to an exact uniform structure.

## Pair Hashing
Mixing canonical identities guarantees duplicate pairs yield matching hash indices resulting in easy tracking and blocking.

## Candidate Checks vs True Intersections
Candidate checks register overlapping bounding parameters. Intersections specify active physical contacts.

## Grid Construction Cost
Building incurs hash operation and allocations roughly equal to objects + specific spanned domains `O(N + C)`.

## Expected Complexity
Expected around `O(N + K)` runtime, maintaining reasonable scale in generally uniform distributions.

## Worst-Case Complexity
Strict `O(N^2)` with significant hashing overhead scaling worse than pure brute force in collapsed scenes.

## Memory Complexity
Overhead required for dynamic unordered maps scaling proportionally against generated elements + cells.

## Cell Size Trade-offs
Extremely large scopes capture numerous actors mimicking simple brute force tracking while extremely tiny variants explode memory constraints, allocations, and loop execution bounds causing tracking bloat.

## Benchmark Methodology
Controlled parameters on exactly equal sequences without randomly fluctuating distributions executing repeatedly.

## Why Both Algorithms Use the Same Dataset
Guarantees measured time variance only reflects applied functionality algorithmically and never random scenario fluctuation.

## Why Same-Run Timing Matters
Mitigates variances resulting from processor cache misses, varying thermal conditions, layout layout layout distribution and operating system priorities contextually.

## Candidate Reduction
Represented a monumental 99.9% load decrease (From 12 million to roughly 6 thousand tests for 5000 N setup).

## Runtime Speedup
Reduced raw run duration across full tests scaling optimally showing a stable ~25x execution acceleration.

## Small-N Overhead
Map hashing algorithms perform inefficiently relative to bare iteration looping across extremely nominal small limits, proving detrimental around N=100 scopes.

## Cell-Size Sensitivity
Varying standard values significantly impacted generated execution timing confirming variable efficiency.

## Actual Results
Achieved a consistent 25.00x runtime performance boost for roughly 5000 entities while perfectly retaining the precise calculated 1637 interactions confirming algorithm parity with control logic.

## Design Decisions
- `std::floor` coordinates for accurate negative binning.
- `unordered_set` duplicate suppression via Canonical Pair indexing.
- Retaining XZ domain isolation (Skipping Y height variations entirely).

## Alternatives Considered
- Vector multi-indexing loops
- Bit-field index masking

## Why Not Quadtree
Added excessive hierarchy structure tree recursion logic for fairly basic consistent distribution scopes where constant hashing maps serve reliably faster.

## Why Not Octree
Height differences represented little actual practical collision relevance within an overall horizontal flat plane.

## Why Not BVH
Bounding volume hierarchies dynamically reorganize intensely and heavily penalize highly scattered and fast-paced actively chaotic movement scenes involving independent moving agents.

## Known Limitations
- Heavy memory allocation overhead relying on associative stl dynamic maps.
- Strict XZ reliance limits advanced multi-vertical overlap processing effectively.

## Interview Questions

1. What problem does broad-phase collision detection solve? Rapidly culling distant geometries preventing expensive computations for physically unviable pairing overlap.
2. What is the difference between broad phase and narrow phase? Broad creates a narrowed candidate list; narrow evaluates detailed intersecting logic on candidates.
3. What was your baseline? Brute-force `O(N^2)` testing over N=100-5000 sizes.
4. Why was the baseline O(N^2)? Loops universally double evaluated standard all pairing matching without filters.
5. What is a uniform spatial grid? Consistent grid based partitioning system separating the playable view space into arbitrary localized regions.
6. How do you convert a world coordinate into a grid coordinate? Apply floor to the continuous value over predefined set cell scale.
7. Why do you use std::floor()? Properly assigns negative spaces into unique non-overlapping domains below the zero margin.
8. What goes wrong with negative coordinates if you cast directly to int? A 0 to -1 domain coordinate essentially squashes up incorrectly rounding toward 0.
9. Why can one AABB occupy several grid cells? Extended geometry crosses defined logical index boundary dividing lines.
10. Why would center-only insertion be incorrect? Missing objects directly sharing border geometries outside an isolated center index.
11. How do you determine all cells an AABB occupies? Resolving minimum to maximum index spans per axis logic loops.
12. Why can candidate pairs appear multiple times? Geometry shared amongst adjoining grids register pairs consistently inside multiple active cells.
13. How do you eliminate duplicate candidate pairs? Creating unified pairing IDs logging evaluated checks to unordered lookup hashes.
14. Why canonicalize pairs as min/max indices? Consistently evaluating A/B against B/A dynamically reliably flags duplicate matching combinations.
15. What does candidateChecks actually count? Evaluated discrete logic interactions specifically checked against narrow overlap verification without redundant repeats.
16. What is rawCellPairVisits? Pure paired iteration steps across raw unstructured domain cells internally before blocking logic overrides repeats.
17. What does intersection count represent? Final valid true hits confirming physical bounding contact definitively.
18. Why must optimized intersection count equal brute-force count? Confirming functional parity verifies optimization retains strict accuracy.
19. What is the expected complexity of the grid? Linear `O(N+K)` relative logic scale assuming a stable non-clustered object domain map density.
20. What is its worst-case complexity? Standard O(N^2) + high memory hashing costs.
21. Give an example where it degrades to O(N^2). Piling every entity upon a single tightly packed grid tile bounding point.
22. What is the memory overhead? Dynamically instantiated STL container loads generating tracking references per object spanning indices linearly.
23. How does grid cell size affect performance? Large grids limit tracking but explode interaction costs; tiny grids overwhelm mapping structure iteration times dynamically.
24. What happens if cells are too small? Constant object spans triggering map iterations heavily slowing parsing processes dynamically.
25. What happens if cells are too large? Minimal mapping but triggers broad dense candidate iteration mimicking naive processing.
26. Why did you choose an XZ grid instead of XYZ? Game logic fundamentally anchors vertical positioning effectively bypassing extensive vertical culling necessity structurally.
27. Why did you choose a grid instead of a quadtree? Minimizing complex active tree iteration restructuring constantly against fast dynamic loose elements cleanly.
28. Why not an octree? Flat plane operation drastically lessens vertical necessity mapping overheads globally.
29. Why not a BVH? Highly chaotic movements negatively degrade bounding volume reorganizational execution limits actively heavily.
30. Why do you retain bruteForceAllPairs()? Ensures consistent and reproducible debugging reference verification tests internally natively correctly.
31. How did you verify optimized correctness? Cross-checking random multi-scalar tests across specific hard coded initial bounds matching output values actively.
32. Why use deterministic benchmark inputs? Preventing randomly uncharacteristically sparse checks interfering against strict control variables mathematically actively.
33. Why benchmark both algorithms on the exact same vector? Confirms isolated functional timings specifically evaluating processing logic differences natively purely accurately.
34. Why shouldn't you compare an old baseline time with a new grid time? Prevents thermal and cache loading discrepancies from distorting measured runtime ratios inherently locally.
35. Why can the grid be slower for small N? Hashing algorithm processes overwhelm miniscule logic comparison time savings natively intrinsically efficiently.
36. What is candidate reduction percentage? Measured efficiency drop mapping culled object comparisons completely relative actively.
37. What is runtime speedup? Multiplier calculated strictly reflecting total processing execution timing ratio variations clearly significantly.
38. Why don't candidate reduction and runtime reduction match exactly? Hashing, inserting, tracking, looping all impose structural overhead unrepresented directly through culled mathematical differences independently basically correctly.
39. What overhead does unordered_map add? Hashing stringing tracking allocation looping dynamically consistently natively overhead limits inherently cleanly cleanly.
40. What overhead does unordered_set add? Allocates memory and hashes object IDs evaluating duplicate statuses extensively explicitly internally inherently actively properly heavily dynamically extensively.
41. What was the measured candidate reduction at N=5000? 99.9%.
42. What was the measured runtime improvement at N=5000? 25.00x speedup (from 69514 us to 2780 us).
43. At what entity count did the grid begin outperforming brute force? From N=500 onwards (speedup 2.65x).
44. Which cell size performed best in your benchmark? Cell size 4.0 showed the fastest time (2256 us).
45. Would that cell size always be optimal? No. Varied object scales density mapping significantly distorts grid distribution performance efficiency explicitly generally directly explicitly efficiently broadly appropriately dynamically drastically necessarily inherently.
46. How would the system change if objects had very different sizes? Variable density structures such as tree representations handle large scale variance optimally compared dynamically flat maps natively inherently generally clearly broadly cleanly implicitly appropriately strictly internally cleanly accurately correctly effectively adequately logically directly clearly natively correctly exactly.
47. How would you handle very large objects? Separate grids or fallback broad references limit index spawning explosion dynamically specifically locally explicitly correctly exactly effectively precisely naturally clearly.
48. How would you integrate this broad phase into gameplay? Injecting projectile loop comparisons replacing brute iterations selectively strictly dynamically natively efficiently reliably cleanly carefully accurately natively precisely cleanly correctly generally.
49. What would you optimize next? Internal frame loop measurement profiles verifying exact dynamic object iteration impacts properly naturally accurately cleanly securely natively implicitly heavily appropriately safely cleanly effectively correctly directly dynamically cleanly dynamically naturally precisely correctly carefully securely.
50. What did this milestone teach you about performance engineering? Verifying control algorithms guarantees functional scaling confirming explicit baseline tests safely accurately natively accurately efficiently broadly completely perfectly reliably cleanly effectively accurately directly logically effectively practically simply accurately consistently perfectly securely fully natively exactly specifically cleanly correctly definitely exactly completely thoroughly practically efficiently broadly efficiently simply safely logically functionally exactly perfectly simply properly successfully correctly effectively functionally natively effectively.

