# GeneSim
The purpose of this program is to facilitate the development of story-based game designs.
* genesim - A comprehensive kinship and family relationship simulator. (in development)
* namegen - A simple commandline name simulator that reads first-, middle-, and lastname from historical Finnish CSV-files. And prints 1 name to the screen. It can choose the second name with a 50% probability.

<h1>Namegen – Historical Name Generator</h1>
Namegen is a command-line based application designed to generate authentic Finnish names from specific historical periods.

<h2>Key Features</h2>
<li>Historical Dataset: Reads first, middle and last names from multiple CSV-files, sorted by decade.</li>
<li>Period Selection: Support for command-line parameters (-p <number>) allows name generation from a selected historical period automatically.</li>
<li>Randomized Structure: Middle name usage is randomized (50% probability), creating realistic variation in names.</li>
<li>Flexible Output: Generates one (1) name at a time by default, but supports expansion to ten (10) names (future feature).</li>
<li>CLI Standards: Includes support for standard command line parameters such as -v (verbose), -h (help), and -V (version), ensuring ease of use.</li>

<h2>Technical Specifications</h2>
<li>Language: Pure C.</li>
<li>Compatibility: Designed to work on both Windows and Linux environments (using #ifdef constructs).</li>
<li>Version: 0.1.0 (Development stage).</li>
<li>Data is based on https://www.tuomas.salste.net/suku/kela-etunimet.html#M1 research. There are a lot of ready-made names.</li>
